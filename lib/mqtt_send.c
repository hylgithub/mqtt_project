#include"mqtt_send.h"
//#include"mqtt.h"

//#define DEBUG

int _mqtt_send_suback(struct mqtt *mq, char granted_qos)
{
	#ifdef DEBUG
	printf("-----_mqtt_send_suback()-----\n");//test only
	#endif
	struct mqtt_packet *packet = NULL;
	int i;
	char remaining_bytes[5];
	if(!mq){
		printf("mq is NULL, check please!\n");
		return -1;
	}
	if(granted_qos < 0 || granted_qos > 2){
		printf("granted_qos invalid!\n");
		return -1;
	}
	packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(!packet){
		printf("_mqtt_send_suback: memory malloc fail, memory lack!\n");
		return -1;
	}
	packet->command = SUBACK;
	packet->remaining_length = 1;
	if(mq->MsgID){
		packet->remaining_length += 2;
	}
	mqtt_encode_remaininglength(packet, remaining_bytes);
	packet->payload = NULL;
	packet->payload = (char *)malloc(packet->packet_length+1);
	if(!packet->payload){
		printf("memory malloc fail, memory lack!\n");
		return -1;
	}
	packet->payload[0] = packet->command;
	for(i=0;i<packet->remaining_count;i++){
		packet->payload[i+1] = remaining_bytes[i];
	}
	packet->pos = packet->remaining_count + 1;
	if(mq->qos > 0){
		mqtt_write_2byte(packet, mq->MsgID);
	}
	mqtt_write_byte(packet, granted_qos);
	#ifdef DEBUG
	printf("mq->fd = %d\n", mq->fd);//test only
	#endif
	if(mqtt_packet_write(mq, packet) < 0){
		printf("write packet fail!\n");
		return -1;
	}
	if(mq->ClientID){
		printf("Send SUBACK to %s client success!\n", mq->ClientID);
	}
	else{
		printf("Send SUBACK to client success!\n");
	}
	
	return 0;
}

int _mqtt_connack_back(struct mqtt **mq, char return_code)
{
	if(!(*mq)){
		printf("_mqtt_connack_back()fail!\n");
		return -1;
	}
	int i;
	char remaining_bytes[5];
	//char temp_return = return_code;
	//here, test only, return_code is 0, always.
	return_code = 0;
	struct mqtt_packet *packet = NULL;
	packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(!packet){
		printf("malloc memory fail, memory lack!\n");
		return -1;
	}
	packet->remaining_length = 2+2;
	mqtt_encode_remaininglength(packet, remaining_bytes);
	packet->payload = NULL;
	packet->payload = (char *)malloc(1+packet->remaining_count+2+1);
	packet->command = CONNACK;
	packet->payload[0] = packet->command;//storage 1st byte, fixed header
	for(i=0;i<packet->remaining_count;i++){
		packet->payload[i+1] = remaining_bytes[i];//storage remaining length space
	}
	packet->pos = packet->remaining_count + 1;
	packet->pos += 1;//because the 1st byte reverse, variable header, so pos + 1.
	mqtt_write_byte(packet, return_code);
	#ifdef DEBUG
	printf("%s\n", packet->payload);//test only
	#endif
	if(mqtt_packet_write((*mq), packet) == -1){
		printf("_mqtt_connack_back()fail!\n");
		return -1;
	}
	if((*mq)->ClientID){
		printf("Send back CONNACK to %s success!\n", (*mq)->ClientID);
	}
	else{
		printf("Send back CONNACK to Client success!\n");
	}
	return 0;
}

int _mqtt_send_puback(struct mqtt *mq)
{
	if(!mq){
		printf("mq is empty, check please!\n");
		return -1;
	}
	if(!(mq->MsgID)){
		printf("message id invalid!\n");
		return -1;
	}
	mq->MsgType = PUBACK;
	struct mqtt_packet *packet;
	int i;
	char remaining_bytes[5];
	packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(packet){
		packet->remaining_length = 2;
		memset(remaining_bytes, 0, 5);
		mqtt_encode_remaininglength(packet, remaining_bytes);
		packet->payload = (char *)malloc(1+packet->remaining_count+2+1);
		packet->payload[0] = PUBACK;
		for(i=0;i<packet->remaining_count;i++){
			packet->payload[i+1] = remaining_bytes[i];
		}
		packet->pos = packet->remaining_count + 1;
		mqtt_write_2byte(packet, mq->MsgID);
		if(mqtt_packet_write(mq, packet) < 0){
			printf("_mqtt_send_puback: write message to client fail!\n");
			return -1;
		}
		if(mq->ClientID){
			printf("Send PUBACK to %s client success!\n", mq->ClientID);
		}
		else{
			printf("Send PUBACK to client success!\n");
		}
		return 0;
	}
	return -1;
}

int _mqtt_send_unsuback(struct mqtt *mq)
{
	#ifdef DEBUG
	printf("-----_mqtt_send_unsuback()-----\n");
	#endif
	if(!mq){
		printf("mq is empty, check please!\n");
		return -1;
	}
	#ifdef DEBUG
	printf("mq->msgid = %d\n", mq->MsgID);
	printf("mq->fd = %d\n", mq->fd);
	#endif
	if(!(mq->MsgID)){
		printf("message id invalid!\n");
		return -1;
	}
	mq->MsgType = UNSUBACK;
	struct mqtt_packet *packet;
	int i;
	char remaining_bytes[5];
	packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(packet){
		packet->remaining_length = 2;
		memset(remaining_bytes, 0, 5);
		mqtt_encode_remaininglength(packet, remaining_bytes);
		packet->payload = (char *)malloc(1+packet->remaining_count+2+1);
		packet->payload[0] = UNSUBACK;
		for(i=0;i<packet->remaining_count;i++){
			packet->payload[i+1] = remaining_bytes[i];
		}
		packet->pos = packet->remaining_count + 1;
		mqtt_write_2byte(packet, mq->MsgID);
		if(mqtt_packet_write(mq, packet) < 0){
			printf("_mqtt_send_unsuback: write message to client fail!\n");
			return -1;
		}
		if(mq->ClientID){
			printf("Send UNSUBACK to %s client success!\n", mq->ClientID);
		}
		else{
			printf("Send UNSUBACK to client success!\n");
		}
		return 0;
	}
	return -1;
}


int _mqtt_send_pubrec(struct mqtt *mq)//实现方式跟PUBACK差不多，上面
{
	#ifdef DEBUG
	printf("=====enter _mqtt_send_pubrec()=====\n");//test only
	#endif
	if(!mq){
		printf("mq is empty, check please!\n");
		return -1;
	}
	if(!(mq->MsgID)){
		printf("message id invalid!\n");
		return -1;
	}
	mq->MsgType = PUBREC;
	struct mqtt_packet *packet;
	int i;
	char remaining_bytes[5];
	packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(packet){
		packet->remaining_length = 2;
		memset(remaining_bytes, 0, 5);
		mqtt_encode_remaininglength(packet, remaining_bytes);
		packet->payload = (char *)malloc(1+packet->remaining_count+2+1);
		packet->payload[0] = PUBREC;
		for(i=0;i<packet->remaining_count;i++){
			packet->payload[i+1] = remaining_bytes[i];
		}
		packet->pos = packet->remaining_count + 1;
		mqtt_write_2byte(packet, mq->MsgID);
		#ifdef DEBUG
		printf("pubrec: msgid = %d\n", mq->MsgID);
		printf("packet->payload[0] = %d\n", packet->payload[0]);
		printf("packet->payload[1] = %d\n", packet->payload[1]);
		printf("packet->payload[2] = %d\n", packet->payload[2]);
		printf("packet->payload[3] = %d\n", packet->payload[3]);
		#endif
		//sleep(8);
		if(mqtt_packet_write(mq, packet) < 0){
			printf("_mqtt_send_puback: write message to client fail!\n");
			return -1;
		}
		if(mq->ClientID){
			printf("Send PUBREC to %s client success!\n", mq->ClientID);
		}
		else{
			printf("Send PUBREC to client success!\n");
		}
		return 0;
	}
	return -1;
}

int _mqtt_send_pubcomp(struct mqtt *mq)
{
	#ifdef DEBUG
	printf("----------enter _mqtt_send_pubcomp()----------\n");
	#endif
	if(!mq){
		printf("PUBCOMP: mq is empty, check please!\n");
		return -1;
	}
	int i, byte = 0;
	char remaining_bytes[5];
	struct mqtt_packet *packet;
	memset(remaining_bytes, 0, 5);
	lseek(mq->fd, 0, SEEK_SET);
	while(byte <= 0){
		byte = read(mq->fd, remaining_bytes, 5);
	}
	if((byte > 0) && (remaining_bytes[0] == PUBREL)){
		if(mq->MsgID == (remaining_bytes[2] << 8 | remaining_bytes[3])){
			printf("Receive the PUBREL form client success!\n");
		}
	}
	else{
		printf("_mqtt_send_pubrel()fail!\n");
		return -1;
	}

	packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(packet){
		packet->remaining_length = 2;
		memset(remaining_bytes, 0, 5);
		mqtt_encode_remaininglength(packet, remaining_bytes);
		packet->payload = (char *)malloc(1+packet->remaining_count+2+1);
		packet->payload[0] = PUBCOMP;
		for(i=0;i<packet->remaining_count;i++){
			packet->payload[i+1] = remaining_bytes[i];
		}
		packet->pos = packet->remaining_count + 1;
		mqtt_write_2byte(packet, mq->MsgID);
		if(mqtt_packet_write(mq, packet) < 0){
			printf("_mqtt_send_pubrel: write message to client fail!\n");
			return -1;
		}
		if(mq->ClientID){
			printf("Send PUBCOMP to %s client success!\n", mq->ClientID);
		}
		else{
			printf("Send PUBCOMP to Client success!\n");
		}
	}
	return 0;

}

int _mqtt_send_pubrel(struct mqtt *mq)
{
	if(!mq){
		printf("mq is empty, check please!\n");
		return -1;
	}
	if(mq->ClientID){
		printf("Send PUBREL command to %s! client success!\n", mq->ClientID);
	}
	else{
		printf("Send PUBREL command to Client success!\n");
	}
	//send PUBCOMP command function, here.
	return 0;
}

int _mqtt_send_pingresp(struct mqtt *mq)//temp
{
	char buf[3];
	buf[0] = PINGRESP;
	buf[1] = 0;
	if(write(mq->fd, buf, 2) < 0){
		printf("write()fail\n");
		return -1;
	}
	if(mq->ClientID){
		printf("Send PINGRESQ to %s client success!\n", mq->ClientID);
	}
	else{
		printf("Send PINGRESQ to Client success!\n");
	}
	
	return 0;
}

