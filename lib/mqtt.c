#include"mqtt.h"
#include"handle.h"
#include"mqtt_send.h"

#define DEBUG
#define true 1
#define false 0

int mqtt_write_byte(struct mqtt_packet *packet, char len_byte)
{
	if(!packet){
		printf("mqtt_write_byte: packet is NULL, check please!\n");
		return -1;
	}
	#ifdef DEBUG
	printf("mqtt_write_byte: now, packet->pos = %d\n", packet->pos);
	printf("len_byte = %c\n", len_byte);//test only
	#endif
	packet->payload[packet->pos] = len_byte;
	#ifdef DEBUG
	printf("packet->payload[%d] = %c\n", packet->pos, packet->payload[packet->pos]);//test only
	#endif
	packet->pos++;

	return 0;
}

int mqtt_write_2byte(struct mqtt_packet *packet, unsigned short length)
{
	char len_byte1, len_byte2;
	if(!packet){
		printf("mqtt_write_2byte: packet is NULL, check please!\n");
		return -1;
	}
	if(!length){
		printf("mqtt_write_2byte: length is equal to 0.\n");
		return -1;
	}
	#ifdef DEBUG
	printf("length = %d\n", length);//test only
	#endif
	len_byte1 = (length&0xFF00) >> 8;//MSB
	len_byte2 = (char)(length&0x00FF);//LSB
	#ifdef DEBUG
	printf("len_byte1 = %d  %c\n", (int)len_byte1, len_byte1);
	printf("len_byte2 = %d  %c\n", (int)len_byte2, len_byte2);
	#endif
	mqtt_write_byte(packet, len_byte1);
	mqtt_write_byte(packet, len_byte2);
	
	return 0;
}

int mqtt_write_mul_byte(struct mqtt_packet *packet, const void *str, unsigned short length)
{
	if(!packet){
		printf("mqtt_write_2byte: packet is NULL, check please!\n");
		return -1;
	}
	memcpy(&(packet->payload[packet->pos]), str, length);
	packet->pos = packet->pos + length;
	return 0;

}

int mqtt_write_string(struct mqtt_packet *packet, const void *str, unsigned short length)
{
	if(!packet || !str){
		printf("mqtt_write_string: packet or topic is NULL, check please!\n");
		return -1;
	}
	mqtt_write_2byte(packet, length);
	mqtt_write_mul_byte(packet, str, length);

	return 0;
}

int  mqtt_write(struct mqtt *mq)
{
	#ifdef DEBUG
	printf("---------mqtt_write()----------\n");
	#endif
	if(!mq){
		printf("mqtt_write()fail!\n");
		return -1;
	}
	#ifdef DEBUG
	int i;
	for(i=0;i<(mq->packet_out->packet_length);i++){
		printf("%c", mq->packet_out->payload[i]);
	}
	printf("\n");
	printf("mq->packet_out->packet_length = %d\n", mq->packet_out->packet_length);
	printf("mq->packet_out->payload[2] = %d\n", (int)mq->packet_out->payload[2]);
	printf("mq->packet_out->payload[3] = %d\n", (int)mq->packet_out->payload[3]);
	#endif
	return write(mq->fd, mq->packet_out->payload, mq->packet_out->packet_length);
}

int mqtt_packet_write(struct mqtt *mq, struct mqtt_packet *packet)
{
	int write_to_sevice_length;
	if(!packet){
		printf("mqtt_packet_write: packet is NULL, check please!\n");
		return -1;
	}
	if(!mq){
		printf("mqtt_packet_write: mq is NULL, check please!\n");
		return -1;
	}
	packet->pos = 0;
	packet->next = NULL;
	mq->packet_out = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	mq->packet_out->payload = (char *)malloc(sizeof(char)*(packet->packet_length));
	if(!mq->packet_out || !mq->packet_out->payload){
		printf("mqtt_packet_write: memory malloc fail, memory lack!\n");
		return -1;
	}
	mq->packet_out = packet;
	if(packet->remaining_length>0){
		write_to_sevice_length = mqtt_write(mq);
	}
	#ifdef DEBUG
	printf("write_to_sevice(client)_length = %d\n", write_to_sevice_length);//test only
	#endif
	return write_to_sevice_length;
}

int mqtt_encode_remaininglength(struct mqtt_packet *packet, char *remaining_bytes)
{
	int remaining_length;
	char byte;
//	char remaining_bytes[5];
	if(!packet){
		printf("packet is NULL, check please!\n");
		return -1;
	}
	remaining_length = packet->remaining_length;
	packet->remaining_count = 0;

	do{
		byte = remaining_length % 128;
		remaining_length = remaining_length / 128;
		if(remaining_length > 0){
			byte = byte | 0x80;
		}
		remaining_bytes[packet->remaining_count] = byte;
		packet->remaining_count++;
	}while(remaining_length > 0);
	if(packet->remaining_count >= 5){
		printf("remaining length Up to 4 bytes of storage, error!\n");
		return -1;
	}
	packet->packet_length = packet->remaining_length + packet->remaining_count + 1;
	/*'packet->remaining_count + 1' is the fixed header length*/
	#ifdef DEBUG
		printf("remaining_length = %d\n", remaining_length);//test remaining_length' s value
		printf("packet->remaining_length = %d\n", packet->remaining_length);
		printf("packet->remaining_mul = %d\n", packet->remaining_mul);
		printf("packet->remaining_count = %d\n", packet->remaining_count);
	#endif
	
	return 0;
}

int mqtt_client_connect_back(int fd, struct mqtt *client_mq, char *clientid, char *username, char *password, unsigned short keepalive, char will_flag, char clean_session, char *will_topic, char will_retain, int will_qos)
{
	int username_flag = 0, password_flag = 0;
	struct mqtt_packet *packet = NULL;
	struct willmessage *will_msg = NULL;
	int i, byte = 0, packetlen = 0;
	char connect_flag, temp_byte;
	char remaining_bytes[5];
	char read_from_serv[5];

	#ifdef DEBUG
	printf("---------client_mq->protocol_name----------\n");
	#endif
	client_mq->protocol_name = (char *)malloc(6+1);
	if(client_mq->protocol_name){
		memcpy(client_mq->protocol_name, "MQIsdp", strlen("MQIsdp"));
		client_mq->protocol_name[6] = '\0';
	}
	client_mq->protocol_version = 0x03;
	client_mq->clean_session = clean_session;
	client_mq->keepalive = keepalive;
	packetlen = 2 + 6 + 1;
	if(username || password || clean_session || will_flag){
		connect_flag = true;
		packetlen += 1;
	}
	packetlen = packetlen + 2;// keepalive storage length, 2 bytes.

	if(!client_mq){
		client_mq = (struct mqtt *)malloc(sizeof(struct mqtt));
		if(!client_mq){
			printf("memory malloc fail, memory lack!\n");
			return -1;
		}
	}
	#ifdef DEBUG
	printf("==========1==========\n");
	#endif
	client_mq->fd = fd;
	if(username){
		username_flag = 1;
		client_mq->username = (char *)malloc(strlen(username)+1);
		memcpy(client_mq->username, username, strlen(username));
		client_mq->username[strlen(username)] = '\0';
		if(password){
			password_flag = 1;
			client_mq->password = (char *)malloc(strlen(password)+1);
			memcpy(client_mq->username, password, strlen(password));
			client_mq->username[strlen(password)] = '\0';
		}
		packetlen += strlen(username) + strlen(password);
	}
	#ifdef DEBUG
	printf("==========2==========\n");
	#endif
	if(will_flag){
		will_msg = (struct willmessage *)malloc(sizeof(struct willmessage));
		if(will_msg){
			#ifdef DEBUG
			printf("==========2.1==========\n");
			#endif
			will_msg->qos = (unsigned short)will_qos;
			will_msg->retain = will_retain;
			will_msg->topic = (char *)malloc(strlen(will_topic)+1);
			#ifdef DEBUG
			printf("==========2.2==========\n");
			#endif
			if(will_msg->topic){
				//will_msg->topic = will_topic;
				memcpy(will_msg->topic, will_topic, strlen(will_topic));
				packetlen += strlen(will_topic);
				will_msg->topic[strlen(will_topic)] = '\0';
				#ifdef DEBUG
				printf("strlen(will_topic) = %d\n", strlen(will_topic));
				printf("%s\n", will_msg->topic);
				#endif
			}
			else{
				printf("memory malloc fail, memory lack!\n");
				return -1;
			}
			#ifdef DEBUG
			printf("==========2.3==========\n");
			#endif
		}
		else{
			printf("memory malloc fail, memory lack!\n");
			return -1;
		}
	}
	#ifdef DEBUG
	printf("==========3==========\n");
	#endif
	if(clientid && (strlen(clientid) <= 23)){
		packetlen += strlen(clientid);
		client_mq->ClientID = (char *)malloc(strlen(clientid)+1);
		if(client_mq->ClientID){
			memcpy(client_mq->ClientID, clientid, strlen(clientid));
			client_mq->ClientID[strlen(clientid)] = '\0';
			#ifdef DEBUG
			printf("strlen(clientid) = %d\n", strlen(clientid));
			printf("%s\n", clientid);
			#endif
		}
	}
	else{
		printf("client id invalid!\n");
		return -1;
	}
	#ifdef DEBUG
	printf("packetlen = %d\n", packetlen);
	#endif
	packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	packet->remaining_length = packetlen;
	#ifdef DEBUG
	printf("==========4==========\n");
	#endif
	mqtt_encode_remaininglength(packet, remaining_bytes);
	#ifdef DEBUG
	printf("packet->remaining_count = %d\n", packet->remaining_count);
	printf("packet->remaining_length = %d\n", packet->remaining_length);
	printf("packet->packet_length = %d\n", packet->packet_length);
	#endif
	/*Send message to service, storage in packet->payload[]*/
	packet->payload = NULL;
	packet->payload = (char *)malloc((packet->packet_length+1)*sizeof(char));
	if(!packet->payload){
		printf("create payload space fail, memory lack!\n");
		return -1;
	}
	packet->command = CONNECT;//other field not used. like dup, qos
	packet->payload[0] = packet->command;//storage 1st byte, fixed header
	for(i=0;i<packet->remaining_count;i++){
		packet->payload[i+1] = remaining_bytes[i];//storage remaining length space
	}
	packet->pos = packet->remaining_count + 1;
	#ifdef DEBUG
	printf("%s\n", packet->payload);
	printf("===========================\n");
	printf("strlen(client_mq->protocol_name) = %d\n", strlen(client_mq->protocol_name));//test only
	#endif
	mqtt_write_string(packet, client_mq->protocol_name, strlen(client_mq->protocol_name));
	mqtt_write_byte(packet, client_mq->protocol_version);
	if(connect_flag){
		temp_byte = username_flag<<7 | password_flag<<6 | will_retain<<5 | will_qos<<4 | will_flag<<2 | clean_session<<1;
		mqtt_write_byte(packet, temp_byte);
	}
	mqtt_write_2byte(packet, client_mq->keepalive);
	mqtt_write_string(packet, client_mq->ClientID, strlen(client_mq->ClientID));
	mqtt_write_string(packet, will_msg->topic, strlen(will_msg->topic));
	if(username_flag){
		mqtt_write_string(packet, client_mq->username, strlen(client_mq->username));
		if(password_flag){
			mqtt_write_string(packet,client_mq->password, strlen(client_mq->password));
		}
	}

	if(mqtt_packet_write(client_mq, packet) < 0){//temp write
		printf("Connect Sevice fail!\n");
		return -1;
	}
	printf("Send CONNECT to Service success!\n");
	memset(read_from_serv, 0, sizeof(read_from_serv));
	lseek(client_mq->fd, 0, SEEK_SET);
	while(byte <= 0){
		byte = read(client_mq->fd, read_from_serv, 5);
	}
	#ifdef DEBUG
	printf("----------0---------\n");
	printf("fd = %d\n", client_mq->fd);
	printf("byte = %d\n", byte);
	printf("command: %d\n", read_from_serv[0]);
	#endif
	if(byte > 0){
		#ifdef DEBUG
		printf("----------1---------\n");
		#endif
		if(CONNACK == read_from_serv[0]){
			#ifdef DEBUG
			printf("return_code: %d\n", read_from_serv[3]);
			printf("----------2---------\n");
			#endif
			printf("Receive CONNACK from Service success!\n");
			return (int)read_from_serv[3];
		}	
		#ifdef DEBUG
		printf("----------3---------\n");
		#endif
	}
	#ifdef DEBUG
	printf("===mqtt_client_connect_back() end===\n");//test only
	#endif

	return 0;
}

int mqtt_client_send_pubrel(struct mqtt *mq, unsigned short msgid)
{
	#ifdef DEBUG
	printf("----------enter mqtt_client_send_pubrel()----------\n");//test only
	#endif
	if(!mq){
		printf("mq is NULL, check please!\n");
		return -1;
	}
	char remaining_bytes[5];
	int i;
	mq->packet_in = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(!mq->packet_in){
		printf("memory malloc fail, memory lack!\n");
		return -1;
	}
	mq->packet_in->payload = (char *)malloc(2+2+1);
	if(mq->packet_in->payload){
		mq->packet_in->payload[0] = PUBREL;
		mq->packet_in->remaining_length = 2;
		mqtt_encode_remaininglength(mq->packet_in, remaining_bytes);
		#ifdef DEBUG
		printf("mq->packet_in->remaining_count = %d\n", mq->packet_in->remaining_count);
		printf("mq->packet_in->remaining_length = %d\n", mq->packet_in->remaining_length);
		printf("mq->packet_in->packet_length = %d\n", mq->packet_in->packet_length);
		#endif
		for(i=0;i<mq->packet_in->remaining_count;i++){
			mq->packet_in->payload[i+1] = remaining_bytes[i];//storage remaining length space
		}
		mq->packet_in->pos = mq->packet_in->remaining_count + 1;
		if(mqtt_write_2byte(mq->packet_in, msgid) == -1){
			printf("mqtt_write_2byte()fail!\n");
			return -1;
		}
		#ifdef DEBUG
		printf("-----1-----\n");
		#endif
		if(mqtt_packet_write(mq, mq->packet_in) <= 0){
			printf("mqtt_packet_write()fail\n");
			return -1;
		}
		printf("Send PUBREL to Service success!\n");
	}
	#ifdef DEBUG
	printf("----------mqtt_client_send_pubrel() end!----------\n");
	#endif

	return 0;
}

int _mqtt_client_publish_back(struct mqtt *mq, int qos, unsigned short msgid)
{
	#ifdef DEBUG
	printf("----------_mqtt_client_publish_back()----------\n");
	#endif
	char read_from_serv[5];
	int byte = 0;
	#ifdef DEBUG
	printf("qos = %d\n", qos);
	#endif
	if(qos == 0){
		return 0;
	}
	if(!mq){
		printf("mq is NULL, check please!\n");
		return -1;
	}

	if(qos == 1){
		#ifdef DEBUG
		printf("----------qos = 1----------\n");
		#endif
		memset(read_from_serv, 0, sizeof(read_from_serv));
		lseek(mq->fd, 0, SEEK_SET);
		while(byte <= 1){
			byte = read(mq->fd, read_from_serv, 5);
		}
		#ifdef DEBUG
		printf("read_from_serv[0] = %d\n", read_from_serv[0]);//test only
		#endif
		if((byte > 0) && (PUBACK == read_from_serv[0])){
			if(msgid == (read_from_serv[2] << 8 | read_from_serv[3])){
				printf("Receive the PUBACK from Service!\n");
				return 0;
			}
			else{
				printf("message id not match!\n");
				return -1;
			}
		}
		else{
			printf("qos = 1 call error!\n");
			return -1;
		}
	}
	/*qos = 2, handle PUBREC*/
	byte = 0;
	if(qos == 2){
		#ifdef DEBUG
		printf("----------qos = 2----------\n");
		#endif
		memset(read_from_serv, 0, sizeof(read_from_serv));
		lseek(mq->fd, 0, SEEK_SET);
		while(byte <= 1){
			byte = read(mq->fd, read_from_serv, 5);
		}
		#ifdef DEBUG
		printf("fd = %d\n", mq->fd);
		printf("byte = %d\n", byte);
		printf("read_from_serv[0] = %d\n", read_from_serv[0]);	
		printf("msgid_from_serv = %d\n", (read_from_serv[2] << 8 | read_from_serv[3]));
		#endif
		if((byte > 0) && (PUBREC == read_from_serv[0])){
			#ifdef DEBUG
			printf("msgid = %d\n", msgid);
			printf("msgid_from_serv = %d\n", (read_from_serv[2] << 8 | read_from_serv[3]));
			#endif
			if(msgid == (read_from_serv[2] << 8 | read_from_serv[3])){
				printf("Receive the PUBREC from Service!\n");
				//send pubrel here.
				if(mqtt_client_send_pubrel(mq, msgid) == -1){
					printf("mqtt_client_send_pubrel()fali!\n");
					return -1;
				}
				/*handle PUBCOMP command as follow.*/
				byte = 0;
				memset(read_from_serv, 0, 5);
				lseek(mq->fd, 0, SEEK_SET);
				while(byte <= 0){
					byte = read(mq->fd, read_from_serv, 5);
				}
				if((byte > 0) && (PUBCOMP == read_from_serv[0])){
					if(msgid == (read_from_serv[2] << 8 | read_from_serv[3])){
						printf("Receive the PUBCOMP from Service!\n");
					}
				}
			}
			else{
				printf("message id not match!\n");
				return -1;
			}
		}
	}

	return 0;
}

int mqtt_client_publish(struct mqtt *mq, unsigned short msgid, const char *topic, void *payload, int payloadlen, int qos, char retain)
{
	#ifdef DEBUG
	printf("===enter mqtt_client_publish()===\n");//test only
	#endif
	int i, packetlen = 0;
	struct mqtt_packet *packet = NULL;
	char remaining_bytes[5];
	char dup = false;
	#ifdef DEBUG
	printf("payloadlen = %d, qos = %d.\n", payloadlen, qos);//test only
	#endif
	if(!topic || qos<0 || qos>2){
		printf("topic is NULL, or qos is invalid!\n");
		return -1;
	}
	if(payloadlen<0 || payloadlen>MQTT_MAX_PAYLOADLEN){
		printf("payload length invalid, check please!\n");
		return -1;
	}

	packetlen = 2 + strlen(topic) + payloadlen;/* 2 for topic length storage, not for fixed header, here*/
	packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(qos>0){
		#ifdef DEBUG
		printf("----------0.0----------\n");//test only
		#endif
		packetlen = packetlen + 2;		
		packet->msgid = msgid;//if qos >0, then message id exist.
		#ifdef DEBUG	
		printf("packet->msgid = %d\n", (int)packet->msgid);//test only
		#endif
	}
	#ifdef DEBUG
	printf("----------0.1----------\n");//test only
	#endif
	if(!packet){
		printf("malloc memory fail, memory lack!\n");
		return -1;
	}
	#ifdef DEBUG
	printf("packetlen(not add the fixed header length) = %d\n", packetlen);//test only
	#endif
	packet->pos = 0;
	#ifdef DEBUG
	printf("msgid = %d\n", msgid);//test only
	#endif
	packet->command = PUBLISH | ((dup&0x01)<<3) | ((qos&0x03)<<1) | (retain);//dup&0x01 << 3, 1st byte, fixed header
	#ifdef DEBUG
	printf("packet->command = %d\n", (int)packet->command);//test only
	#endif
	packet->remaining_length = packetlen;//2nd byte, fixed header
	#ifdef DEBUG
	printf("==========0.2==========\n");//test only
	#endif

	if(mqtt_encode_remaininglength(packet, remaining_bytes) == -1){//get the remaining length info
		printf("mqtt_encode_remaininglength() fail!\n");
		return -1;
	}
	#ifdef DEBUG
	printf("==========1==========\n");//test only
	printf("packet_length = %d\n", packet->packet_length);//test only
	#endif
	packet->payload = NULL;
	packet->payload = (char *)malloc((packet->packet_length+1)*sizeof(char));
	if(!packet->payload){
		printf("create payload space fail, memory lack!\n");
		return -1;
	}
	packet->payload[0] = packet->command;//storage 1st byte, fixed header
	for(i=0;i<packet->remaining_count;i++){
		packet->payload[i+1] = remaining_bytes[i];//storage remaining length space
	}
	packet->pos = packet->remaining_count + 1;
	#ifdef DEBUG
	printf("==========2==========\n");//test only
	#endif
	#ifdef DEBUG
	printf("packet->remaining_count = %d\n", packet->remaining_count);//test only
	for(i=0;i<packet->remaining_count;i++){
		printf("%c\n", remaining_bytes[i]);//test only
	}
	printf("\n");
	printf("test: print current payload information!\n");
	printf("strlen(packet->payload) = %d\n", strlen(packet->payload));
	printf("payload: %s\n", packet->payload);
	printf("\n");
	printf("==========2.1==========\n");
	#endif
	#ifdef DEBUG
	printf("strlen(topic) = %d\n", strlen(topic));//test only
	#endif
	mqtt_write_string(packet, topic, (unsigned short)strlen(topic));//write topic string, variable header
	if(qos >0){
		#ifdef DEBUG
		printf("==========2.2==========\n");
		#endif
		mqtt_write_2byte(packet, msgid);//message id, variable header
	}
	#ifdef DEBUG
	printf("==========3==========\n");//test only
	printf("now, test: print the payload info again!\n");
	printf("strlen(packet->payload) = %d\n", strlen(packet->payload));
	printf("payload: %s\n", packet->payload);
	printf("\n");
	printf("=========3.1===========\n");
	#endif
	if(payloadlen){
		mqtt_write_string(packet, payload, payloadlen);//payload
	}
	#ifdef DEBUG
	printf("==========4==========\n");//test only
	#endif
	if(mqtt_packet_write(mq, packet) < 0){//write message to service
		printf("Publish Topic to Sevice fail!\n");
		return -1;
	}
	printf("Send PUBLISH to Service success!\n");
	#ifdef DEBUG
	printf("------publish_back test------\n");//test only
	#endif
	if(_mqtt_client_publish_back(mq, qos, msgid) == -1){
		printf("_mqtt_client_publish_back() fail!\n");
		return -1;
	}
	#ifdef DEBUG
	printf("===mqtt_client_publish() end===\n");//test only
	#endif

	return 0;
}

/*sub_client send puback or pubrec, qos = 1 or 2*/
/*usage:
		when qos = 1 or 2, and receive info from the service publish command, 
		then subscribe client call mqtt_client_publish_back() function.
		subclient_mq: include socket fd, 
		msgid: message id,
		command: PUBACK or PUBREC or PUBCOMP.
*/
int mqtt_client_publish_back(struct mqtt *subclient_mq, unsigned short msgid, char command)
{
	struct mqtt_packet *subclient_packet = NULL;
	int i, packetlen = 0;
	char remaining_bytes[5];

	if(!subclient_mq){
		printf("subclient_mq is NULL, check please!\n");
		return -1;
	}
	if(0 == msgid){
		printf("Message ID invalid!\n");
		return -1;
	}
	subclient_packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(!subclient_packet){
		printf("malloc memory fail, memory lack!\n");
		return -1;
	}
	subclient_packet->pos = 0;
	subclient_packet->command = command;//dup, qos, retain all not used.
	subclient_packet->remaining_length = 2;
	subclient_packet->packet_length = subclient_packet->remaining_length + 2;
	mqtt_encode_remaininglength(subclient_packet, remaining_bytes);
	subclient_packet->payload = NULL;
	subclient_packet->payload = (char *)malloc((subclient_packet->packet_length)*sizeof(char));
	if(!subclient_packet->payload){
		printf("create payload space fail, memory lack!\n");
		return -1;
	}
	subclient_packet->payload[0] = subclient_packet->command;//storage 1st byte, fixed header
	for(i=0;i<subclient_packet->remaining_count;i++){
		subclient_packet->payload[i+1] = remaining_bytes[i];//storage remaining length space
	}
	subclient_packet->pos = subclient_packet->remaining_count + 1;
	if(mqtt_write_2byte(subclient_packet, msgid) == -1){//message id, variable header
		printf("mqtt_client_publish_back() fail!\n");
		return -1;
	}
	if(mqtt_packet_write(subclient_mq, subclient_packet) < 0){//write
		printf("Send PUBACK to service fail!\n");
		return -1;
	}

	return 0;
}

/*publish_client send pubrel*/

int mqtt_client_pubrel(struct mqtt *subclient_mq, unsigned short msgid)
{
	char dup = 0;
	int i;
	unsigned short qos = 1;
	char remaining_bytes[5];
	struct mqtt_packet *subclient_packet = NULL;

	if(!subclient_mq){
		printf("subclient_mq is NULL, check please!\n");
		return -1;
	}
	if(0 == msgid){
		printf("Message ID invalid!\n");
		return -1;
	}
	subclient_packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(!subclient_packet){
		printf("malloc memory fail, memory lack!\n");
		return -1;
	}

	subclient_packet->pos = 0;
	subclient_packet->command = PUBREL| ((dup&0x01)<<3) | (qos<<2);//dup&0x01 << 3, 1st byte, fixed header, retain flag not used.
	subclient_packet->remaining_length = 2;
	subclient_packet->packet_length = subclient_packet->remaining_length + 2;
	mqtt_encode_remaininglength(subclient_packet, remaining_bytes);
	subclient_packet->payload = NULL;
	subclient_packet->payload = (char *)malloc((subclient_packet->packet_length)*sizeof(char));
	if(!subclient_packet->payload){
		printf("create payload space fail, memory lack!\n");
		return -1;
	}
	subclient_packet->payload[0] = subclient_packet->command;//storage 1st byte, fixed header
	for(i=0;i<subclient_packet->remaining_count;i++){
		subclient_packet->payload[i+1] = remaining_bytes[i];//storage remaining length space
	}
	subclient_packet->pos = subclient_packet->remaining_count + 1;
	if(mqtt_write_2byte(subclient_packet, msgid) == -1){//message id, variable header
		printf("mqtt_client_puback() fail!\n");
		return -1;
	}
	if(mqtt_packet_write(subclient_mq, subclient_packet) < 0){//write
		printf("Send PUBACK to service fail!\n");
		return -1;
	}

	return 0;
}

int mqtt_client_pingreq(int fd, struct mqtt *client_mq)
{
	int byte = 0;
	char bytes[2];
	bytes[0] = PINGREQ;
	bytes[1] = 0;
	if(!client_mq){
		printf("client_mq is NULL, fail!\n");
		return -1;
	}
	client_mq->packet_out = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	client_mq->packet_out->payload = (char *)malloc(2+1);
	if(!client_mq->packet_out->payload){
		printf("memory malloc fail, memory lack!\n");
		return -1;
	}
	else{
		client_mq->packet_out->payload[0] = PINGREQ;
		client_mq->packet_out->payload[1] = 0;
	}
	
	if(write(fd, client_mq->packet_out->payload, 2) < 0){
		printf("write message to service fail!\n");
		return -1;
	}
	printf("Send PINGREQ to Service success!\n");
	byte = 0;
	memset(bytes, 0, 2);
	lseek(fd, 0, SEEK_SET);
	while(byte <= 1){
		byte = read(fd, bytes, 2);
	}
	#ifdef DEBUG
	printf("bytes[0] = %d\n", bytes[0]);
	#endif
	if((byte > 0) && (PINGRESP == (bytes[0]&0xFF))){//command  is negative, so command&0xFF
		printf("Receive PINGRESP from Service success!\n");
	}
	else{
		printf("Client have not received PINGRESP!\n");
		return -1;
	}
	return 0;
}

int _mqtt_client_subscribe_back(struct mqtt *mq, unsigned short msgid, char **granted_qos)
{
	char read_from_serv[16];
	int bytes = 0;
	if(!mq){
		printf("mq is NULL, check please!\n");
		return -1;
	}
	memset(read_from_serv, 0, 16);
	#ifdef DEBUG
	printf("mq->fd = %d\n", mq->fd);
	#endif
	lseek(mq->fd, 0, SEEK_SET);
	while(bytes <= 2){
		bytes = read(mq->fd, read_from_serv, 16);
	}
//	lseek(mq->fd, 0, SEEK_SET);//add by [hyl], modify read fd's position. 
/*	#ifdef DEBUG
	printf("----now, test lseek(mq->fd, 0, SEEK_SET)-----\n");
	int i = 0;
	int buf[4096];
	memset(buf, 0, 4096);
	i = read(mq->fd, buf, 4096);
	printf("read_bytes_from_serv = %d\n", i);
	printf("buf[0] = %d\n", buf[0]);
	printf("buf[1] = %d\n", buf[1]);
	printf("buf[2] = %d\n", buf[2]);
	printf("----test lseek(mq->fd, 0, SEEK_SET) end!-----\n");
	#endif
*/
	#ifdef DEBUG
	printf("bytes = %d\n", bytes);
	printf("read_from_serv[0] = %d\n", read_from_serv[0]);
	#endif
	if(bytes > 0 && SUBACK == (read_from_serv[0]&0xFF)){
		if(mq->qos > 0){
			if(msgid == (read_from_serv[2] << 8 | read_from_serv[3])){
				**granted_qos = read_from_serv[4];
				#ifdef DEBUG
				printf("--------------------\n");//test only
				#endif
			}
			**granted_qos = read_from_serv[2];
			
		}
		#ifdef DEBUG
		printf("mq->qos = %d\n", mq->qos);
		printf("read_from_serv[1] = %d\n", read_from_serv[1]);
		printf("read_from_serv[2] = %d\n", read_from_serv[2]);
		printf("read_from_serv[3] = %d\n", read_from_serv[3]);
		printf("read_from_serv[4] = %d\n", read_from_serv[4]);
		printf("granted_qos = %d\n", *granted_qos);
		#endif
		printf("Receive SUBACK from service success!\n");
//		return 0;

	}

	if(mqtt_read_and_print_subtopic_info(mq) == -1){
		printf("-----mqtt_read_and_print_subtopic_info()fail!-----\n");
		return -1;
	}	
	return 0;;
}

int mqtt_read_and_print_subtopic_info(struct mqtt *mq)
{
	#ifdef DEBUG
	printf("---enter mqtt_read_and_print_subtopic_info()---\n");
	printf("-----print subtopic and payload-----\n");
	#endif
	int bytes = 0;
	char buffer[4096];
	char *topic = NULL;
	unsigned short qos = 0;
	unsigned short msgid;
	int payloadlen = 0;
	char *payload = NULL;
	memset(buffer, 0, 4096);
	#ifdef DEBUG
	printf("mq->fd = %d\n", mq->fd);
	printf("------------------------------\n");
	printf("------------------------------\n");
	printf("------------------------------\n");
	printf("------------------------------\n");
	printf("------------------------------\n");
	printf("------------------------------\n");
	#endif
	lseek(mq->fd, 0, SEEK_SET);
	while(bytes <= 2){
		bytes = read(mq->fd, buffer, 4096);
	}
	#ifdef DEBUG
	printf("---------1-----------\n");
	printf("bytes = %d\n", bytes);
	#endif
	mqtt_decode_remaininglength(mq, buffer);
	if(mqtt_read_string(buffer, &(mq->packet_in->pos), &topic) == -1){//get topic name
		printf("get the topic name fail!\n");
		return -1;
	}
	qos = ((buffer[0])&0x06) >> 3;
	#ifdef DEBUG
	printf("---------2-----------\n");
	#endif
	if(qos > 0){//when qos > 0, Message ID exist.
		if(mqtt_read_2byte(buffer, &(mq->packet_in->pos), &msgid) == -1){//get message id
			printf("get the message id fail!\n");
			return -1;
		}
		#ifdef DEBUG
		printf("---------2.1-----------\n");
		#endif
		if(msgid== 0){
			printf("message id invalid.\n");
			return -1;
		}	
		mq->MsgID = msgid;
	}
	#ifdef DEBUG
	printf("---------3-----------\n");
	#endif
	payloadlen = mq->packet_in->remaining_length - mq->packet_in->pos + mq->packet_in->remaining_count + 1 - 2;//get the payloadlen
	#ifdef DEBUG
	printf("payloadlen = %d\n", payloadlen);//test only
	#endif
	if(payloadlen >= 0){//payloadlen = 0 valid also.
		payload = malloc(payloadlen+1);//malloc +1
		if(!payload){
			printf("malloc payload memery fail!\n");
			return -1;
		}
		memset(payload, 0, payloadlen+1);
		memcpy(payload, buffer+(mq->packet_in->pos)+2, payloadlen);//get the payload
	}
	//mqtt_read_string(buffer, &(mq->packet_in->pos), &payload);

	printf("Receive PUBLISH from Service success!\n");

	printf("-----print the result-----\n");
	printf("--------------------\n");
	printf("granted_qos = %d\n", qos);
	if(qos > 0){
		printf("message id = %d\n", msgid);
	}
	printf("topic = %s\n", topic);
	printf("strlen(payload) = %d\n", strlen(payload));
	printf("payload = %s\n", payload);
	printf("--------------------\n");


	return 0;
}

/**********************************************
	mqtt_client_subscribe:
					mq: mqtt struct;
					fixed_qos: this qos field in the fixed header, identify: there are more than one topic(qos = 1).
					dup: dup field, duplicate
					msgid: message id
					topic: subscribe topic name
					topic_qos: topic qos level,  storage 2 bit by 1 byte in LSB 
**********************************************/
int mqtt_client_subscribe(int fd, struct mqtt *mq, unsigned short fixed_qos, char dup, unsigned short msgid, char *subtopic, char topic_qos, char *granted_qos)
{
	#ifdef DEBUG
	printf("-----enter mqtt_client_subscribe()-----\n");
	#endif
	struct mqtt_packet *packet = NULL;
	int i, packetlen = 0;
	char remaining_bytes[5];
	if(!mq){
		printf("mq is NULL, check please!\n");
		return -1;
	}
	if(topic_qos >2){
		printf("value topic_qos invalid, connecting break off!\n");
		return -1;
	}
	mq = (struct mqtt *)malloc(sizeof(struct mqtt));
	if(!mq){
		printf("mq memory malloc fail, memory lack!\n");
		return -1;
	}
	mq->MsgType = SUBSCRIBE;
	mq->dup = dup;
	mq->qos = fixed_qos;
	mq->fd = fd;
	mq->MsgID = 0;
	packetlen = 2 + strlen(subtopic) + 1;// 1 for topic_qos storage.
	if(fixed_qos > 0){
		mq->MsgID = msgid;
		packetlen += 2;//message id length storage.
	}
	packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(!packet){
		printf("packet memory malloc fail, memory lack!\n");
		return -1;
	}
	packet->remaining_length = packetlen;
	mqtt_encode_remaininglength(packet,  remaining_bytes);
	packet->payload = (char *)malloc(packet->packet_length+1);
	if(!packet->payload){
		printf("packet->payload memory malloc fail, memory lack!\n");
		return -1;
	}
	packet->payload[0] = SUBSCRIBE | (dup&0x01)<<3 | (fixed_qos&0x03)<<1;
	for(i=0;i<packet->remaining_count;i++){
		packet->payload[i+1] = remaining_bytes[i];//storage remaining length space
	}
	packet->pos = packet->remaining_count + 1;
	#ifdef DEBUG
		printf("packet->payload[0] = %d\n", packet->payload[0]);
		printf("fixed_qos = %d\n", fixed_qos);
	#endif
	if(fixed_qos > 0){
		mqtt_write_2byte(packet, msgid);
	}
	mqtt_write_string(packet, subtopic, strlen(subtopic));//write subscribe topic
	mqtt_write_byte(packet, topic_qos);
	#ifdef DEBUG
	printf("packet->payload[1]: %d\n", packet->payload[1]);
	printf("packet->payload[2]: %d\n", packet->payload[2]);
	printf("packet->payload[3]: %d\n", packet->payload[3]);
	printf("packet->payload[4]: %d\n", packet->payload[4]);
	printf("subtopic: %s\n", subtopic);
	printf("strlen(subtopic) = %d\n", strlen(subtopic));
	printf("mq->fd = %d\n", mq->fd);
	#endif
	if(mqtt_packet_write(mq, packet) < 0){
		printf("Send subscribe infomation to Service fail!\n");
		return -1;
	}
	printf("Send SUBSCRIBE to Service success!\n");
	if(_mqtt_client_subscribe_back(mq, mq->MsgID, &granted_qos) == -1){
		printf("_mqtt_client_subscribe_back()fail!\n");
		return -1;
	}
	//here, handle PUBLISH command from service
	#ifdef DEBUG
	printf("granted_qos = %d\n", granted_qos);
	#endif
	//mq->qos = granted_qos;
/*	
	if(mqtt_read_and_print_subtopic_info(mq) == -1){
		printf("mqtt_read_and_print_subtopic_info() fail!\n");
		return -1;
	}
*/
	if(mqtt_client_publish_back(mq, mq->MsgID, PUBACK) == -1){//send PUBACK to service.
		printf("mqtt_client_publish_back() fail!\n");
		return -1;
	}
	printf("Send PUBACK to service success!\n");
	return 0;
}

int _mqtt_client_unsubscribe_back(struct mqtt *mq)
{
	int bytes = 0;
	char read_from_serv[5];
	if(!mq){
		printf("mq is NULL, check please!\n");
		return -1;
	}
	memset(read_from_serv, 0, 5);
	lseek(mq->fd, 0, SEEK_SET);
	while(bytes <= 1){
		bytes = read(mq->fd, read_from_serv, 4);
	}
	#ifdef DEBUG
	printf("--------------------\n");
	printf("bytes = %d\n", bytes);
	printf("read_from_serv[0] = %d\n", read_from_serv[0]);
	printf("msgid_from_serv = %d\n", (read_from_serv[2] << 8 | read_from_serv[3]));
	#endif
	if(bytes > 0 && (UNSUBACK == (read_from_serv[0]&0xFF))){
		if(mq->qos > 0){
			if(mq->MsgID == (read_from_serv[2] << 8 | read_from_serv[3])){
				printf("Receive UNSUBACK from Service success!\n");
				return 0;
			}
		}
		printf("Receive UNSUBACK from Service success!\n");
		return 0;
	}

	return -1;
}
int  mqtt_client_unsubscribe(int fd, struct mqtt *mq, unsigned short fixed_qos, char dup, unsigned short msgid, char *unsubtopic)
{
	printf("-----enter mqtt_client_unsubscribe()-----\n");
	struct mqtt_packet *packet = NULL;
	int i, packetlen = 0;
	char remaining_bytes[5];
	if(!mq){
		printf("mq is NULL, check please!\n");
		return -1;
	}
	mq = (struct mqtt *)malloc(sizeof(struct mqtt));
	if(!mq){
		printf("mq memory malloc fail, memory lack!\n");
		return -1;
	}
	mq->MsgType = UNSUBSCRIBE;
	mq->dup = dup;
	mq->qos = fixed_qos;
	mq->fd = fd;
	mq->MsgID = 0;
	packetlen = 2 + strlen(unsubtopic);
	if(fixed_qos > 0){
		mq->MsgID = msgid;
		packetlen += 2;//message id length storage.
	}
	packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(!packet){
		printf("packet memory malloc fail, memory lack!\n");
		return -1;
	}
	packet->remaining_length = packetlen;
	mqtt_encode_remaininglength(packet,  remaining_bytes);
	packet->payload = (char *)malloc(packet->packet_length+1);
	if(!packet->payload){
		printf("packet->payload memory malloc fail, memory lack!\n");
		return -1;
	}
	packet->payload[0] = UNSUBSCRIBE | (dup&0x01)<<3 | (fixed_qos&0x03)<<1;
	for(i=0;i<packet->remaining_count;i++){
		packet->payload[i+1] = remaining_bytes[i];//storage remaining length space
	}
	packet->pos = packet->remaining_count + 1;
	#ifdef DEBUG
		printf("packet->payload[0] = %d\n", packet->payload[0]);
		printf("fixed_qos = %d\n", fixed_qos);
	#endif
	if(fixed_qos > 0){
		mqtt_write_2byte(packet, msgid);
	}
	mqtt_write_string(packet, unsubtopic, strlen(unsubtopic));//write subscribe topic
	#ifdef DEBUG
	printf("packet->payload[1]: %d\n", packet->payload[1]);
	printf("packet->payload[2]: %d\n", packet->payload[2]);
	printf("packet->payload[3]: %d\n", packet->payload[3]);
	printf("packet->payload[4]: %d\n", packet->payload[4]);
	printf("subtopic: %s\n", unsubtopic);
	printf("strlen(subtopic) = %d\n", strlen(unsubtopic));
	#endif
	if(mqtt_packet_write(mq, packet) < 0){
		printf("Send unsubscribe infomation to Service fail!\n");
		return -1;
	}
	printf("Send UNSUBSCRIBE to Service success!\n");
	if(_mqtt_client_unsubscribe_back(mq) == -1){
		printf("_mqtt_client_unsubscribe_back()fail!\n");
		return -1;
	}

	return 0;
}
