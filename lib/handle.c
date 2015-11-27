#include"handle.h"
#include"mqtt_send.h"
#include"mqtt.h"

#define DEBUG
static struct mqtt *mq_head = NULL;//storage mqtt struct message head.
static struct mqtt *mq_tail = NULL;//point to linklist's tail, always.
//int not_match = 1;//0, match;1, not match.
//mqtt_message_storage(mq_global, source);

void mqtt_print_linklist_info(int length)
{
	#ifdef DEBUG
	printf("----------print linklist----------\n");
	#endif
	struct mqtt *p;
	p = (struct mqtt *)malloc(sizeof(struct mqtt)+length);
	p = mq_head->next;
	while(p){
		printf("*****************************************\n");
		printf("clean_session = %d\n", p->clean_session);
		printf("ClientID = %s\n", p->ClientID);
		printf("connect_flag = %d\n", p->connect_flag);
		printf("connect_return_code = %d\n", p->connect_return_code);
		printf("dup = %d\n", p->dup);
		printf("fd = %d\n", p->fd);
		printf("keepalive = %d\n", p->keepalive);
		printf("MsgID = %d\n", p->MsgID);
		printf("MsgType = %d\n", p->MsgType);
		if(p->password_flag){
			printf("password = %s\n", p->password);
			printf("password_flag = %d\n", p->password_flag);
		}
		if(p->username_flag){
			printf("username = %s\n", p->username);
			printf("username_flag = %d\n", p->username_flag);
		}
		if(p->topic){
			printf("topic = %s\n", p->topic);

		}
		if(p->payload){
			printf("payload = %s\n", p->payload);
		}
		printf("qos = %d\n", p->qos);
		printf("retain = %d\n", p->retain);
		printf("will_flag = %d\n", p->will);
		if(p->will){
			printf("will_payload = %s\n", p->will_payload);
			printf("will_payloadlen = %d\n", p->will_payloadlen);
			printf("will_qos = %d\n", p->will_qos);
			printf("will_retain = %d\n", p->will_retain);
			printf("will_topic = %s\n", p->will_topic);
		}
		printf("*****************************************\n\n");
		p = p->next;
	}
	printf("\n\n");
}

void mqtt_content_save(struct mqtt **dest_mqtt, const struct mqtt *source_mqtt)
{
	#ifdef SAVE_DEBUG
	printf("----------0----------\n");
	#endif
	if(!(*dest_mqtt)){
		*dest_mqtt = (struct mqtt *)malloc(sizeof(struct mqtt)+1024);
	}
	(*dest_mqtt)->clean_session = source_mqtt->clean_session;
	if(source_mqtt->ClientID){
		(*dest_mqtt)->ClientID = (char *)malloc(sizeof(char)*strlen(source_mqtt->ClientID));
		memcpy((*dest_mqtt)->ClientID, source_mqtt->ClientID, sizeof(char)*strlen(source_mqtt->ClientID));
		(*dest_mqtt)->ClientID[strlen(source_mqtt->ClientID)] = '\0';
	}
	#ifdef SAVE_DEBUG
	printf("----------1----------\n");
	#endif
	(*dest_mqtt)->connect_flag = source_mqtt->connect_flag;
	(*dest_mqtt)->connect_return_code = source_mqtt->connect_return_code;
	(*dest_mqtt)->dup = source_mqtt->dup;
	(*dest_mqtt)->fd = source_mqtt->fd;
	(*dest_mqtt)->keepalive = source_mqtt->keepalive;
	(*dest_mqtt)->MsgID = source_mqtt->MsgID;
	(*dest_mqtt)->MsgType = source_mqtt->MsgType;
	#ifdef SAVE_DEBUG
	printf("----------2----------\n");
	#endif
	if(source_mqtt->topic){
		(*dest_mqtt)->topic = (char *)malloc(sizeof(char)*strlen(source_mqtt->topic));
		memcpy((*dest_mqtt)->topic, source_mqtt->topic, strlen(source_mqtt->topic));
		(*dest_mqtt)->topic[strlen(source_mqtt->topic)] = '\0';
	}
	(*dest_mqtt)->password_flag = source_mqtt->password_flag;
	if(source_mqtt->password){
		(*dest_mqtt)->password = (char *)malloc(sizeof(char)*strlen(source_mqtt->password));
		memcpy((*dest_mqtt)->password, source_mqtt->password, strlen(source_mqtt->password));
		(*dest_mqtt)->password[strlen(source_mqtt->password)] = '\0';
	}
	#ifdef SAVE_DEBUG
	printf("----------3----------\n");
	#endif
	(*dest_mqtt)->username_flag = source_mqtt->username_flag;
	if(source_mqtt->username){
		(*dest_mqtt)->username = (char *)malloc(sizeof(char)*strlen(source_mqtt->username));
		memcpy((*dest_mqtt)->username, source_mqtt->username, strlen(source_mqtt->username));
		(*dest_mqtt)->username[strlen(source_mqtt->username)] = '\0';
	}
	#ifdef SAVE_DEBUG
	printf("----------4----------\n");
	#endif
	if(source_mqtt->payload){
		(*dest_mqtt)->payload = (char *)malloc(sizeof(char)*strlen(source_mqtt->payload));
		memcpy((*dest_mqtt)->payload, source_mqtt->payload, strlen(source_mqtt->payload));
		(*dest_mqtt)->payload[strlen(source_mqtt->payload)] = '\0';
	}
	#ifdef SAVE_DEBUG
	printf("----------5----------\n");
	#endif
	(*dest_mqtt)->qos = source_mqtt->qos;
	(*dest_mqtt)->remaining_length = source_mqtt->remaining_length;
	(*dest_mqtt)->retain = source_mqtt->retain;
	(*dest_mqtt)->state = source_mqtt->state;
	(*dest_mqtt)->will = source_mqtt->will;
	#ifdef SAVE_DEBUG
	printf("----------6----------\n");
	#endif
	if(source_mqtt->will_payload){
		(*dest_mqtt)->will_payload = (char *)malloc(sizeof(char)*strlen(source_mqtt->will_payload));
		memcpy((*dest_mqtt)->will_payload, source_mqtt->will_payload, strlen(source_mqtt->will_payload));
		(*dest_mqtt)->will_payload[strlen(source_mqtt->will_payload)] = '\0';
	}
	(*dest_mqtt)->will_payloadlen = source_mqtt->will_payloadlen;
	(*dest_mqtt)->will_qos = source_mqtt->will_qos;
	(*dest_mqtt)->will_retain = source_mqtt->will_retain;
	#ifdef SAVE_DEBUG
	printf("----------7----------\n");
	#endif
	if(source_mqtt->will_topic){
		(*dest_mqtt)->will_topic = (char *)malloc(sizeof(char)*strlen(source_mqtt->will_topic));
		memcpy((*dest_mqtt)->will_topic, source_mqtt->will_topic, strlen(source_mqtt->will_topic));
		(*dest_mqtt)->will_topic[strlen(source_mqtt->will_topic)] = '\0';
	}
	#ifdef SAVE_DEBUG
	printf("----------8----------\n");
	#endif
}

void mqtt_message_storage(/*struct mqtt *dest_mq, */const struct mqtt *source_mq)
{
	#ifdef DEBUG
	printf("-----enter mqtt_message_storage()-----\n");
	#endif
	struct mqtt *temp1 = NULL;
	struct mqtt *temp2 = NULL;
	if(!mq_head){
		#ifdef SAVE_DEBUG
		printf("----------only once----------\n");
		#endif
		mq_head = (struct mqtt *)malloc(sizeof(struct mqtt)+1024);
		mq_head->next = NULL;		
	}
	#ifdef SAVE_DEBUG
	printf("----------1----------\n");
	#endif
	temp1= (struct mqtt *)malloc(sizeof(struct mqtt)+1024);
	temp2= (struct mqtt *)malloc(sizeof(struct mqtt)+1024);
	#ifdef SAVE_DEBUG
	printf("----------2----------\n");
	#endif
	temp1 = mq_head->next;
	temp2 = mq_head;
	while(NULL != temp1){
		temp2= temp1;//temp2 is linklist tail.
		temp1 = temp1->next;
	}
	if(source_mq->state == mqtt_state_connecting){
		mqtt_content_save(&(temp2->next), source_mq);
	}
	else{
		mqtt_content_save(&temp2, source_mq);
	}
	

//	}
/*
//	else{//not malloc new point.
		temp1 = mq_head->next;
		while(NULL != temp1){
			temp2 = temp1;
			printf("----------2.1----------\n");
			temp1 = temp1->next;
		}	
		printf("source_mq->topic: %s\n", source_mq->topic);
		printf("source_mq->payload: %s\n", source_mq->payload);
		printf("temp2->connect_flag = %d\n", temp2->connect_flag);
		temp2 = source_mq;
//	}
*/
		#ifdef DEBUG
		/*print linklist info, test only*/
		mqtt_print_linklist_info(1024);
		#endif

}

int mqtt_read_byte(char *buf, int *pos, char *str)
{
	if(!buf){
		printf("arg 'buf' is NULL, check please!\n");
		return -1;
	}
	*str = buf[*pos];
	(*pos)++;
	return 0;
}

int mqtt_read_2byte(char *buf, int *pos, unsigned short *str)
{
	char msb, lsb;
	if(!buf){
		printf("arg 'buf' is NULL, check please!\n");
		return -1;
	}
	msb = buf[*pos];
	#ifdef DEBUG
	printf("msb = %d\n", msb);//test only
	#endif
	(*pos)++;
	lsb = buf[*pos];
	#ifdef DEBUG
	printf("lsb = %d\n", lsb);//test only
	#endif
	(*pos)++;
	*str = (msb << 8) + lsb;
	return 0;
}

int mqtt_read_string(char *buf, int *pos, char **str)
{
	unsigned short len;
	if(!buf){
		printf("arg 'buf' is NULL, check please!\n");
		return -1;
	}
	mqtt_read_2byte(buf, pos, &len);
	#ifdef DEBUG
	printf("mqtt_read_string: len = %d\n", (int)len);
	#endif
	*str = (char *)malloc(len*sizeof(char)+1);
	if(str){
		memcpy(*str, buf+(*pos), len);
		*pos = *pos + len;
	}
	#ifdef DEBUG
	int i;
	printf("---\n");
	for(i=0;i<len;i++){
		printf("%c", (*str)[i]);
	}
	printf("\n");
	#endif
	return 0;
}

int mqtt_connect_handle(struct mqtt **mq, char *buf)//CONNECT Command
{
	#ifdef DEBUG
	printf("---enter mqtt_connect_handle()---\n");
	#endif
	char *protocol_name = NULL;
	char protocol_version = 0x00;
	bool connect_flag = false;
	bool clean_session, will_flag, will_qos, will_retain;
	bool username_flag = false, password_flag = false;
	char *ClientID = NULL;
	char *will_payload = NULL;
	unsigned short will_payloadlen;
	unsigned short keepalive;
	char *will_topic = NULL;
	char *username = NULL;
	char *password = NULL;
	struct willmessage *will_message;

	//一个字段一个字段读取，并处理
	#ifdef DEBUG
	printf("mq->packet_in->pos = %d\n", (*mq)->packet_in->pos);
	#endif
	(*mq)->state = mqtt_state_connecting;
	if(mqtt_read_string(buf, &((*mq)->packet_in->pos), &protocol_name) == -1){//get protocol name, the fix header length is 2byte, so pos+2
		printf("get the protocol name fail!\n");
		close((*mq)->fd);
		return -1;
	}
	if(strcmp(protocol_name, "MQIsdp") != 0){//"MQIsdp", v3.1
		printf("protocol name invalid!\n");
		close((*mq)->fd);
		return -1;
	}
	#ifdef DEBUG
	printf("----------1----------\n");
	#endif
	if(mqtt_read_byte(buf, &((*mq)->packet_in->pos), &protocol_version)  == -1){//get protocol_version
		printf("get the protocol version fail!\n");
		return -1;
	}
	if((protocol_version & 0x7F) != 3){//3, v3.1
		printf("protocol version is not equal to 3, invalid!\n");
		return -1;
	}
	#ifdef DEBUG
	printf("----------2----------\n");
	#endif
	if(mqtt_read_byte(buf, &((*mq)->packet_in->pos), &connect_flag) == -1){//get connect_flag
		printf("get the connect flag fail!\n");
		return -1;
	}
	clean_session = (connect_flag & 0x02) >> 1;
	will_flag = (connect_flag & 0x04) >> 2;
	#ifdef DEBUG
	printf("clean_session = %d\n", (int)clean_session);
	printf("will_flag = %d\n", (int)will_flag);
	#endif
	#ifdef DEBUG
	printf("----------3----------\n");
	#endif
	if(mqtt_read_2byte(buf, &((*mq)->packet_in->pos), &keepalive) == -1){//get keepalive
		printf("get the keepalive fail!\n");
		return -1;
	}
	(*mq)->keepalive = keepalive;
	#ifdef DEBUG
	printf("----------4----------\n");
	#endif
	if(mqtt_read_string(buf, &((*mq)->packet_in->pos), &ClientID) == -1){//get ClientID
		printf("get the client ID fail!\n");
		return -1;
	}
	if(strlen(ClientID) > 23){//check the client id
		printf("client id invalid!\n");
		//_mqtt_send_connack(mq, CONNACK_REFUSED_IDENTIFIER_REJECTED);
		return -1;
	}
	(*mq)->ClientID = (char *)malloc(sizeof(char)*strlen(ClientID));
	if((*mq)->ClientID){
		(*mq)->ClientID = ClientID;
	}
	#ifdef DEBUG
	printf("----------5----------\n");
	printf("client id = %s\n", (*mq)->ClientID);//test only
	#endif
	if(will_flag){
		will_qos = (connect_flag & 0x18) >> 3;
		will_retain = (connect_flag & 0x20) >> 5;
		if(mqtt_read_string(buf, &((*mq)->packet_in->pos), &will_topic) == -1){//get will_topic
			printf("get the will topic fail!\n");
			return -1;
		}

		if(mqtt_read_2byte(buf, &((*mq)->packet_in->pos), &will_payloadlen) == -1){//get keepalive
			printf("get the will payloadlen fail!\n");
			return -1;
		}

		will_payload = malloc(will_payloadlen+1);
		if(!will_payload){
			printf("malloc 'will_payload' fail!\n");
			return -1;
		}
//		memset(will_payload, 0, will_payloadlen+1);
		memcpy(will_payload, buf+((*mq)->packet_in->pos), will_payloadlen);//get the will_payload
		will_payload[will_payloadlen] = '\0';
	}

	#ifdef DEBUG
	printf("----------6----------\n");
	printf("client id = %s\n", (*mq)->ClientID);//test only
	#endif
	if(username_flag){
		if(mqtt_read_string(buf, &((*mq)->packet_in->pos), &username) == 0){//get username
			(*mq)->username = username;
			if(password_flag){
				if(mqtt_read_string(buf, &((*mq)->packet_in->pos), &password)){//get password
					printf("get username or password fail!\n");
					return -1;
				}
				(*mq)->password = password;
			}
		}
	}
	(*mq)->connect_flag = connect_flag;	
	(*mq)->will_topic = will_topic;
	(*mq)->will_payload = will_payload;
	(*mq)->will_payloadlen = will_payloadlen;
	(*mq)->will = will_flag;
	(*mq)->will_qos = will_qos;
	(*mq)->will_retain = will_retain;
	(*mq)->clean_session = clean_session;
	if(will_flag){
		will_message = malloc(sizeof(struct willmessage));
		if(will_message){
			(*mq)->will_message = will_message;
			(*mq)->will_message->topic = will_topic;
			if(will_payload){
				(*mq)->will_message->payload = will_payload;
				(*mq)->will_message->payloadlen = will_payloadlen;
			}
			else{
				(*mq)->will_message->payload = NULL;
				(*mq)->will_message->payloadlen = 0;
			}
			(*mq)->will_message->qos = will_qos;
			(*mq)->will_message->retain = will_retain;
		}
	}
	#ifdef DEBUG
	printf("----------7----------\n");
	printf("client id = %s\n", (*mq)->ClientID);//test only
	#endif
	#ifdef DEBUG
	if(will_flag){
		printf("will_qos = %d\n", will_qos);
		printf("will_retain = %d\n", will_retain);
		printf("will_topic = %s\n", will_topic);
		printf("clean_session = %d\n", clean_session);
	}
	printf("keepalive = %d\n", keepalive);
	#endif
	#ifdef DEBUG
	printf("==========================\n");
	#endif
	if(will_flag){
		free(will_payload);
		free(will_topic);
		will_payload = NULL;
		will_topic = NULL;
	}
	if(username_flag){
		free(username);
		username = NULL;
		if(password_flag){
			free(password);
			password = NULL;
		}
	}
	printf("Receive CONNECT from Client success!\n");
	printf("client id = %s\n", (*mq)->ClientID);//test only
	mqtt_message_storage(*mq);
	if(_mqtt_connack_back(mq, 0) == -1){
		printf("Send CONNACK to Client fail!\n");
		return -1;
	}
	return 0;
}

int mqtt_connack_handle(struct mqtt *mq, char *buffer)//CONNACK Command
{
	/*Server will not take the initiative to connect to the client, 
	    So don't take it(CONNACK) into consideration.*/
	return 0;
}

int mqtt_publish_handle(struct mqtt *mq, char *buffer)//PUBLISH Command
{
	#ifdef DEBUG
	printf("----------mqtt_publish_handle()-----------\n");
	#endif
	char *topic = NULL;
	char dup, retain;
	int qos = 0;
	char *payload = NULL;
	unsigned int payloadlen;
	unsigned short MsgID;

	mq->state = mqtt_state_publish;
	if(!mq || !buffer){
		printf("parameter invalid, check please!\n");
		return -1;
	}
	dup = (mq->packet_in->command & 0x08) >> 3;
	qos = ((mq->packet_in->command & 0x04) >> 2)*2+((mq->packet_in->command & 0x02) >> 1);
	retain = (mq->packet_in->command & 0x01);
	//dup = (mq->packet_in->command>>3)&0x08;
	//qos = (mq->packet_in->command>>1)&0x06;
	//retain = (mq->packet_in->command)&0x01;
	#ifdef DEBUG
	printf("--------------------\n");
	printf("mq->packet_in->command = %d\n", (int)mq->packet_in->command);
	printf("dup = %d\n", dup);
	printf("qos = %d\n", qos);
	printf("retain = %d\n", retain);
	printf("--------------------\n");
	/*the 1st byte is parsed over, here*/
	#endif
	if(qos > 2){
		printf("qos value invalid, check please!\n");
		return -1;
	}
	if(mqtt_read_string(buffer, &(mq->packet_in->pos), &topic) == -1){//get topic name
		printf("get the topic name fail!\n");
		return -1;
	}
	if(topic){
		mq->packet_in->topic = topic;
	}
	#ifdef DEBUG
	printf("--------------------\n");
	printf("mq->packet_in->pos = %d\n", mq->packet_in->pos);
	printf("strlen(topic) = %d\n", (int)strlen(topic));
	printf("print the topic info:\n");
	printf("%s\n", topic);
	printf("%s\n", mq->packet_in->topic);
	printf("--------------------\n");
	#endif
	//check topic 获取到topic后应该检查topic是否符合协议要求，通配符是否正确等等，在这里暂时不check，默认不含通配符；
	if(qos > 0){//when qos > 0, Message ID exist.
		if(mqtt_read_2byte(buffer, &(mq->packet_in->pos), &MsgID) == -1){//get message id
			printf("get the message id fail!\n");
			return -1;
		}
		if(MsgID == 0){
			printf("message id invalid.\n");
			return -1;
		}	
		mq->MsgID = MsgID;
	}
	payloadlen = mq->packet_in->remaining_length - mq->packet_in->pos + mq->packet_in->remaining_count + 1;//get the payloadlen
	if(payloadlen >= 0){//payloadlen = 0 valid also.
		payload = malloc(payloadlen+1);//malloc +1
		if(!payload){
			printf("malloc payload memery fail!\n");
			return -1;
		}
		memset(payload, 0, payloadlen+1);
		memcpy(payload, buffer+(mq->packet_in->pos)+2, payloadlen);//get the payload
		#ifdef DEBUG
		printf("--------------------\n");
		printf("payloadlen = %d\n", payloadlen);
		printf("%s\n", payload);
		printf("--------------------\n");
		#endif
	}
	if(mq->ClientID){
		printf("Receive PUBLISH from %s client success!\n", mq->ClientID);
	}
	else{
		printf("Receive PUBLISH from client success!\n");
	}		
	mq->dup = dup;
	mq->qos = qos;
	mq->retain = retain;
	mq->topic = topic;
	mq->payload = payload;
	mq->packet_in->payload = payload;
	#ifdef DEBUG
	printf("%s\n", mq->packet_in->payload);//test only
	printf("==========\n");
	printf("qos = %d\n", qos);
	#endif
	mqtt_message_storage(mq);//add by [hyl]
	switch(qos){
		case 0:
			return 0;
			break;
		case 1:
			#ifdef DEBUG
			printf("--------qos = 1--------\n");
			#endif
			//将消息持久化并返回PUBACK消息给发送方，将消息打包，然后write
			_mqtt_send_puback(mq);
			break;
		case 2:
			//将消息持久化并返回PUBREC消息给发送方，将消息打包，然后write
			#ifdef DEBUG
			printf("--------qos = 2--------\n");
			#endif
			if(_mqtt_send_pubrec(mq) == 0){
				_mqtt_send_pubcomp(mq);
			}
			break;
		default:
			printf("qos value error!\n");
			return -1;
	}
	#ifdef DEBUG
	printf("=====mqtt_publish_handle()end!=====\n");
	#endif

	return 0;
}

int mqtt_puback_handle(struct mqtt *mq, unsigned short msgid)//PUBACK Command
{
	#ifdef DEBUG
	printf("===mqtt_puback_handle===\n");//test only
	#endif
	char read_from_serv[5];
	int byte = 0;
	#ifdef DEBUG
	printf("qos = %d\n", mq->qos);
	#endif
	if(!mq){
		printf("mq is NULL, check please!\n");
		return -1;
	}
	memset(read_from_serv, 0, sizeof(read_from_serv));
	lseek(mq->fd, 0, SEEK_SET);
	while(byte <= 0){
		byte = read(mq->fd, read_from_serv, 5);
	}
	#ifdef DEBUG
	printf("read_from_serv[0] = %d\n", read_from_serv[0]);//test only
	#endif
	if((byte > 0) && (PUBACK == read_from_serv[0])){
		if(msgid == (read_from_serv[2] << 8 | read_from_serv[3])){
			printf("Receive the PUBACK from Client!\n");
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
	return -1;
}

int mqtt_pubrec_handle(struct mqtt *mq, char *buffer)//PUBREC Command
{
	if(!buffer){
		printf("buffer is empty, check please!\n");
		return -1;
	}

	char qos, dup;
	char temp_byte = NULL;
	int remaining_length = 2;
	unsigned short MsgID = 0;
	if(mqtt_read_byte(buffer, &(mq->packet_in->pos), &temp_byte) == -1){
		printf("get the first byte fail!\n");
		return -1;
	}
	dup = (temp_byte & 0x08) >> 3;
	qos = (temp_byte & 0x06) >> 1;
	temp_byte = NULL;
	if(mqtt_read_byte(buffer, &(mq->packet_in->pos), &temp_byte) == -1){//get remaining length
		printf("get the remaining length fail!\n");
		return -1;
	}
	mq->remaining_length = (int)temp_byte;
	if(mq->remaining_length){
		if(mqtt_read_2byte(buffer, &(mq->packet_in->pos), &MsgID) == -1){
			printf("get message id fail!\n");
			return -1;
		}
		mq->MsgID = MsgID;
	}
	if(qos == 1)
		_mqtt_send_pubrel(mq);
	else
		return -1;

	return 0;
}

int mqtt_pubrel_handle(struct mqtt *mq, char *buffer)//PUBREL Command
{
//	int pos = 0;
	unsigned short MsgID;
	if(!buffer){
		printf("buffer is empty, check please!\n");
		return -1;
	}
	if(mqtt_read_2byte(buffer, &(mq->packet_in->pos), &MsgID) == -1){//get Message ID
		printf("get message id fail!\n");
		return -1;
	}
	mq->MsgID = MsgID;
	_mqtt_send_pubcomp(mq);
	return 0;
}

int mqtt_pubcomp_handle()//PUBCOMP Command
{
	//PUBCOMP command, free resource.
	printf("===mqtt_pubcomp_handle===\n");//test only
	return 0;
}

int mqtt_service_publish(struct mqtt *serv_mq, struct mqtt *dest_mq, char grant_qos)
{
	#ifdef DEBUG
	printf("-----mqtt_service_publish()-----\n");
	#endif
	if(!dest_mq){
		printf("mqtt_service_publish: dest_mq is NULL, check please!\n");
		return -1;
	}
	#ifdef DEBUG
	printf("===enter mqtt_service_publish()===\n");//test only
	#endif
	int i = 0, packetlen = 0;
	struct mqtt_packet *packet = NULL;
	char remaining_bytes[5];
	char dup = false;
	#ifdef DEBUG
	printf("payloadlen = %d, qos = %d.\n", strlen(dest_mq->payload), grant_qos);//test only
	#endif
	if(!dest_mq->topic){
		printf("topic is NULL!\n");
		return -1;
	}

	packetlen = 2 + strlen(dest_mq->topic) + strlen(dest_mq->payload);/* 2 for topic length storage, not for fixed header, here*/
	packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(dest_mq->qos>0){
		#ifdef DEBUG
		printf("----------0.0----------\n");//test only
		#endif
		packetlen = packetlen + 2;		
		packet->msgid = dest_mq->MsgID;//if qos >0, then message id exist.
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
	printf("msgid = %d\n", dest_mq->MsgID);//test only
	#endif
	packet->command = PUBLISH | ((dup&0x01)<<3) | ((grant_qos&0x03)<<1) | (dest_mq->retain);//dup&0x01 << 3, 1st byte, fixed header
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
	printf("strlen(topic) = %d\n", strlen(dest_mq->topic));//test only
	#endif
	mqtt_write_string(packet, dest_mq->topic, (unsigned short)strlen(dest_mq->topic));//write topic string, variable header
	if(grant_qos > 0){
		#ifdef DEBUG
		printf("==========2.2==========\n");
		#endif
		mqtt_write_2byte(packet, dest_mq->MsgID);//message id, variable header
	}
	#ifdef DEBUG
	printf("==========3==========\n");//test only
	printf("now, test: print the payload info again!\n");
	printf("strlen(packet->payload) = %d\n", strlen(packet->payload));
	printf("payload: %s\n", packet->payload);
	printf("\n");
	printf("=========3.1===========\n");
	#endif
	if(dest_mq->payload){
		mqtt_write_string(packet, dest_mq->payload, strlen(dest_mq->payload));//payload
	}
	#ifdef DEBUG
	printf("==========4==========\n");//test only
	printf("serv_mq->fd = %d\n", serv_mq->fd);//test only
	printf("dest_mq->fd = %d\n", dest_mq->fd);//test only
	#endif
	dest_mq->fd = serv_mq->fd;//modify fd value.serv_mq->fd is current service fd, but dest_mq->fd is the last service fd.
	if(mqtt_packet_write(dest_mq, packet) < 0){//write message to service
		printf("Publish Topic to Sub_client fail!\n");
		return -1;
	}
	printf("Send PUBLISH to Sub_client success!\n");
	#ifdef DEBUG
	printf("------publish_back(service) test------\n");//test only
	#endif
	mqtt_puback_handle(serv_mq, dest_mq->MsgID);
	#ifdef DEBUG
	printf("===mqtt_service_publish() end===\n");//test only
	#endif

	return 0;
}

/*mqtt_match_sub_topic():
	return code: 1, match fail
			     0, match success.
*/
int mqtt_match_sub_topic(struct mqtt *mq, char *subtopic, char granted_qos)
{
	#ifdef MATCH_DEBUG
	printf("===enter mqtt_match_sub_topic()===\n");//test only
	#endif
	if(!subtopic){
		printf("subscribe topic not exist, check please!\n");
		return -1;
	}
	#ifdef MATCH_DEBUG
	printf("-----1-----\n");
	#endif
	if(granted_qos > 2){
		printf("qos value is invalid!\n");
		return -1;
	}
	#ifdef MATCH_DEBUG
	printf("-----2-----\n");
	#endif
	struct mqtt *temp;
	temp = (struct mqtt *)malloc(sizeof(struct mqtt)+1024);//1024, temp;
	if(!temp){
		printf("memory malloc fail, memory lack!\n");
		return -1;
	}
	#ifdef MATCH_DEBUG
	printf("-----3-----\n");
	#endif
/*	if(!mq_head){
		printf("Not match subscribe topic and payload, Try again later!\n");
		return -1;
	}
*/
	if(!mq_head){
		printf("linklist is empty, No topic info!\n");
		free(temp);
		temp = NULL;
	}
	else{
		temp = mq_head->next;		
	}
	#ifdef MATCH_DEBUG
	printf("-----4-----\n");
	printf("subtopic = %s\n", subtopic);
	printf("strlen(subtopic) = %d\n", strlen(subtopic));
	#endif
	while(temp && temp->topic){
		#ifdef MATCH_DEBUG
		printf("--------------------\n");//test only
		printf("temp->topic: %s\n", temp->topic);//test only
		#endif
		if(strcmp(subtopic, temp->topic) == 0){
		//if(strcmp(subtopic, temp->topic, strlen(subtopic)) == 0){
			#ifdef MATCH_DEBUG
			printf("=====================\n");
			#endif
			printf("mqtt_match_sub_topic: subscribe topic match success!\n");
			/*handle match topic success*/
			if(mqtt_service_publish(mq, temp, granted_qos) == -1){
				printf("mqtt_service_publish()fail!\n");
				return -1;
			}
			return 0;
		}
		else{
			temp = temp->next;
		}
	}
	mqtt_struct_free(temp);
	return 1;
}

/*SUBSCRIBE command use the qos level must be 1*/
int mqtt_subscribe_handle(struct mqtt *mq, char *buf)//SUBSCRIBE Command
{
	char *sub_topic = NULL;
	char topic_qos;
	unsigned short fixed_qos, msgid = 0;
	char granted_qos, dup;
	int not_match = 1;

	#ifdef DEBUG
	printf("mq->packet_in->command = %d\n", mq->packet_in->command);
	printf("mq->packet_in->pos = %d\n", mq->packet_in->pos);
	#endif

	dup = (mq->packet_in->command&0x08)>>3;
	fixed_qos = (mq->packet_in->command&0x06)>>1;
	mq->qos = fixed_qos;

	#ifdef DEBUG
	printf("mq->qos = %d\n", mq->qos);
	#endif
	if(1 != fixed_qos){
		printf("qos value invalid!\n");
		return -1;
	}
	if(mqtt_read_2byte(buf, &(mq->packet_in->pos), &msgid) == -1){//get message id
		printf("get message id fail!\n");
		return -1;
	}
	mq->MsgID = msgid;
	#ifdef DEBUG
	printf("dup = %d\n", dup);
	printf("fixed_qos = %d\n", fixed_qos);
	printf("message id = %d\n", msgid);
	#endif
	
	if(mqtt_read_string(buf, &(mq->packet_in->pos), &sub_topic) == -1){//get the subscribe topic
		printf("get the subscribe topic fail!\n");
		return -1;
	}
	if(sub_topic){
		if(!strlen(sub_topic)){
			printf("subscribe topic is empty!\n");
			return -1;
		}
		#ifdef DEBUG
		printf("=====0=====\n");//test only
		#endif
		sub_topic[strlen(sub_topic)] = '\0';
	}
	/****************************************mqtt_check_topic()******************************************/
	/*其实应该先检查subscribe topic是否符合要求，同时处理含有通配符的问题，现在先不考虑，publish那边也是*/
	#ifdef DEBUG
	printf("=====1=====\n");//test only
	#endif
	if(mqtt_read_byte(buf, &(mq->packet_in->pos), &topic_qos) == -1){//get the topic qos
		printf("get the qos according to topic fail!\n");
		return -1;
	}
	if(topic_qos > 2 || topic_qos < 0){
		printf("invalid topic_qos value!\n");
		return -1;
	}
	granted_qos = topic_qos - 1;//grant qos is topic_qos - 1 default.
	if(topic_qos == 0){
		granted_qos = topic_qos;
	}
	#ifdef DEBUG
	printf("=====2=====\n");
	printf("strlen(sub_topic) = %d\n", strlen(sub_topic));
	printf("sub_topic: %s\n", sub_topic);
	printf("topic_qos = %d\n", (int)topic_qos);
	printf("granted_qos = %d\n", granted_qos);
	#endif
	if(mq->ClientID){
		printf("Receive SUBSCRIBE from %s client success!\n", mq->ClientID);
	}
	else{
		printf("Receive SUBSCRIBE from client success!\n");
	}	
	if(_mqtt_send_suback(mq, granted_qos) == -1){
		printf("_mqtt_send_suback()fail!\n");
		return -1;
	}
	#ifdef DEBUG
	printf("==========1==========\n");
	printf("subtopic = %s\n", sub_topic);
	printf("granted_qos = %d\n", granted_qos);
	#endif
	while(0 != not_match){	
		not_match = mqtt_match_sub_topic(mq, sub_topic, granted_qos);
	}

	#ifdef DEBUG
	printf("-----mqtt_subscribe_handle()end!-----\n");
	#endif
	return 0;
}

int mqtt_suback_handle(struct mqtt *mq, char *buf)//SUBACK Command
{
	#ifdef DEBUG
	printf("===mqtt_suback_handle===\n");//test only
	#endif
	printf("Receive SUBACK from Sub_client success!\n");
	return 0;
}

int mqtt_unsubscribe_handle(struct mqtt *mq, char *buf)//UNSUBSCRIBE Command
{
	#ifdef DEBUG
	printf("-----mqtt_unsubscribe_handle()-----\n");
	#endif
	char *unsubtopic = NULL;
	unsigned short msgid;

	if(!buf){
		printf("buffer from client is empty.\n");
		return -1;
	}
	#ifdef DEBUG
	printf("mq->packet_in->command = %d\n", mq->packet_in->command);
	#endif
	mq->qos= (mq->packet_in->command&0x06)>>1;
	#ifdef DEBUG
	printf("mq->qos = %d\n", mq->qos);//test only
	#endif
	if(mq->qos > 0){
		if(mqtt_read_2byte(buf, &(mq->packet_in->pos), &msgid) == -1){
			printf("get message id fail!\n");
			return -1;
		}
		mq->MsgID = msgid;
	}
	if(mqtt_read_string(buf, &(mq->packet_in->pos), &unsubtopic) == -1){//get unsubscribe topic
		printf("get unsubscribe topic fail!\n");
		return -1;
	}
	if(unsubtopic){
		if(!strlen(unsubtopic)){
			printf("unsubscribe topic is empty, check please!\n");
			return -1;
		}
	}
	#ifdef DEBUG
	printf("--------------------\n");//test only
	printf("msgid = %d\n", msgid);
	printf("unsubtopic = %s\n", unsubtopic);
	#endif
	printf("Receive UNSUBSCRIBE from client success!\n");
	if(_mqtt_send_unsuback(mq) == -1){
		printf("_mqtt_send_unsuback()fail!\n");
		return -1;
	}

	return 0;
}

int mqtt_unsuback_handle()//UNSUBACK Command
{
	/*Service don't have to handle UNSUBACK command, same as SUBACK.*/
	#ifdef DEBUG
	printf("===mqtt_unsuback_handle===");//test only
	#endif
	return 0;
}

int mqtt_pingreq_handle(struct mqtt *mq, char *buf)//PINGREQ Command
{
	#ifdef DEBUG
	printf("-----mqtt_pingreq_handle()-----\n");
	#endif
	if(!mq || !buf){
		printf("PINGREQ: mq or buffer is empty, check please!\n");
		return -1;
	}
	#ifdef DEBUG
	printf("buf[0] = %d\n", buf[0]);
	#endif
	if((buf[0]&0xFF) != PINGREQ){
		printf("comand error, check please!\n");
		return -1;
	}
	if(mq->ClientID){
		printf("Receive PINGREQ from %s success!\n", mq->ClientID);
	}
	else{
		//printf("-----+++++-----\n");//test only
		printf("Receive PINGREQ from Client success!\n");
	}
	if(_mqtt_send_pingresp(mq) == -1){
		printf("Send PINGRESP to Client fail!\n");
		return -1;
	}
	#ifdef DEBUG
	printf("----------+-----------\n");
	#endif
	return 0;
}

int mqtt_pingresp_handle(struct mqtt *mq)//PINGRESP Command
{
	/*Service don't have to handle PINGRESP command.*/
	printf("Receive the PINGRESP from Client!\n");
	return 0;
}

void mqtt_struct_free(struct mqtt *free_mq)
{
	free_mq->clean_session = 0;
	free_mq->ClientID = NULL;
	free_mq->connect_flag = 0;
	free_mq->connect_return_code = 0;
	free_mq->dup = 0;
	free_mq->fd = 0;
	free_mq->keepalive = 0;
	free_mq->MsgID = 0;
	free_mq->MsgType = 0;
	free_mq->next = NULL;
	if(free_mq->password){
		free(free_mq->password);
		free_mq->password = NULL;
	}
	free_mq->password_flag = 0;
	if(free_mq->username){
		free(free_mq->username);
		free_mq->username = NULL;
	}
	free_mq->username_flag = 0;
	free_mq->protocol_version = 0x00;
	free_mq->qos = 0;
	free_mq->remaining_length = 0;
	free_mq->retain = 0;
	if(free_mq->topic){
		free(free_mq->topic);
		free_mq->topic = NULL;
	}
	free_mq->will = 0;
	free_mq->will_message = NULL;
	if(free_mq->will_payload){
		free(free_mq->will_payload);
		free_mq->will_payload = NULL;
	}
	free_mq->will_payloadlen = 0;
	free_mq->will_qos = 0;
	free_mq->will_retain = 0;
	if(free_mq->will_topic){
		free(free_mq->will_topic);
		free_mq->will_topic = NULL;
	}
	if(free_mq->payload){
		free(free_mq->payload);
		free_mq->payload= NULL;
	}

}

int mqtt_disconnect_handle(struct mqtt *mq)//DISCONNECT Command
{
	if(!mq){
		printf("DISCONNECT: mq is empty, check please!\n");
		return -1;
	}
	mqtt_struct_free(mq);
	mqtt_packet_free(mq->packet_in);
	mqtt_packet_free(mq->packet_out);
	return 0;
}

void mqtt_decode_remaininglength(struct mqtt *mq, char *buf)
{
	#ifdef DEBUG
	printf("-----------mqtt_decode_remaininglength()------------\n");
	#endif
	char byte;
	struct mqtt_packet *serv_packet = NULL;
	serv_packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(!serv_packet){
		printf("mqtt_decode_remaininglength: memory malloc fail, memory lack!\n");
	}
	serv_packet->command = buf[0];
	serv_packet->remaining_count = 0;
	serv_packet->remaining_length = 0;
	serv_packet->remaining_mul = 1;
	serv_packet->pos = 1;
	if(serv_packet->remaining_count <= 0){
		do{
			#ifdef DEBUG
			printf("mq->pos = %d\n", serv_packet->pos);//test only
			#endif
			mqtt_read_byte(buf, &(serv_packet->pos), &byte);
			#ifdef DEBUG
			printf("byte(10) = %d\n", byte);//test only
			#endif
			if(byte){
				serv_packet->remaining_count--;
				serv_packet->remaining_length += (byte & 127)*serv_packet->remaining_mul;
				serv_packet->remaining_mul = serv_packet->remaining_mul * 128;
			}
		}while((byte & 128) != 0);
	
	}
	serv_packet->remaining_count *= -1;//recover positive num
	serv_packet->packet_length = serv_packet->remaining_length + serv_packet->remaining_count + 1;
	#ifdef DEBUG
		printf("--------------------------\n");
		printf("serv_packet->remaining_count = %d\n", serv_packet->remaining_count);//test only
		printf("serv_packet->remaining_length = %d\n", serv_packet->remaining_length);//test only
		printf("serv_packet->remaining_mul = %d\n", serv_packet->remaining_mul);//test only
		printf("serv_packet->packet_length = %d\n", serv_packet->packet_length);
		printf("--------------------------\n");
	#endif

	mq->packet_in = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(mq->packet_in){
		mq->packet_in = serv_packet;
	}

}

int mqtt_packet_handle(int fd, struct mqtt *serv_mqtt, char *buf)//handle the packet
{
//	char byte;
//	int read_bytes;
	int msgtype;
	int test;
	if(!buf){
		printf("buffer is empty, check please!\n");
		close(fd);
		return -1;
	}
	serv_mqtt = (struct mqtt *)malloc(sizeof(struct mqtt));
	if(!serv_mqtt){
		printf("mqtt_packet_handle: malloc memory fail, memory lack!\n");
		return -1;
	}
	buffer_msgtype_analysis(buf, &msgtype);//get Message Type
	serv_mqtt->MsgType = msgtype;
	serv_mqtt->fd = fd;
	#ifdef DEBUG
	printf("serv_mqtt->MsgTypte = %d.\n", serv_mqtt->MsgType);
	#endif
	if((0 >=((serv_mqtt->MsgType&0xF0) >> 4)) || (14 < ((serv_mqtt->MsgType&0xF0) >> 4))){
		printf("serv_mqtt->MsgType invalid!\n");
		close(serv_mqtt->fd);
		return -1;
	}
	/*here, decode the remaining length*/
	mqtt_decode_remaininglength(serv_mqtt, buf);
	#ifdef DEBUG
	printf("--------------------------\n");
	printf("----------switch(serv_mqtt->MsgType)----------\n");
	#endif
	#ifdef DEBUG
	printf("serv_mqtt->packet_in->command = %d\n", serv_mqtt->packet_in->command);
	#endif
	switch(serv_mqtt->MsgType){
		case CONNECT:
			test = mqtt_connect_handle(&serv_mqtt, buf);
			printf("-----------\n");
			printf("serv_mqtt->ClientID = %s\n", serv_mqtt->ClientID);
			break;
			//return test;
		case CONNACK:
			return mqtt_connack_handle(serv_mqtt, buf);
			//break;
		case PUBLISH:
			return mqtt_publish_handle(serv_mqtt, buf);
			//break;
		case PUBACK:
			return mqtt_puback_handle(serv_mqtt, 10);//test
		case PUBREC:
			return mqtt_pubrec_handle(serv_mqtt, buf);
		case PUBREL:
			return mqtt_pubrel_handle(serv_mqtt, buf);
		case PUBCOMP:
			return mqtt_pubcomp_handle(serv_mqtt, buf);
		case SUBSCRIBE:
			return mqtt_subscribe_handle(serv_mqtt, buf);
		case SUBACK:
			return mqtt_suback_handle(serv_mqtt, buf);
		case UNSUBSCRIBE:
			return mqtt_unsubscribe_handle(serv_mqtt, buf);
		case UNSUBACK:
			return mqtt_unsuback_handle(serv_mqtt, buf);
		case PINGREQ:
			//printf("*******************\n");
			return mqtt_pingreq_handle(serv_mqtt, buf);
			//return 0;
		case PINGRESP:
			return mqtt_pingresp_handle(serv_mqtt);
		case DISCONNECT:
			return mqtt_disconnect_handle(serv_mqtt);
		default:
			return -1;
	}
	printf("--------------------\n");
	printf("serv_mqtt->ClientID = %s\n", serv_mqtt->ClientID);
	printf("--------------------\n");
	return test;
}

void buffer_msgtype_analysis(char *buf, int *msgtype_temp)//get MsgType
{
	char temp = buf[0];
	#ifdef DEBUG
	printf("buf[0] = %c %d\n", temp, temp);//test only
	#endif
	*msgtype_temp = (temp & 0xF0);
	#ifdef DEBUG
	printf("MsgType = %d\n", *msgtype_temp);
	#endif
}

int _mqtt_read_byte(int fd, char *buf, int count)
{
	if(!buf){
		printf("buf is NULL, check please!\n");
		return -1;
	}
	lseek(fd, 0, SEEK_SET);
	return read(fd, buf, count);
}

void mqtt_init_mqtt(struct mqtt *init_mqtt)
{
	init_mqtt->clean_session = 0;
	init_mqtt->ClientID = NULL;
	init_mqtt->connect_flag = 0;
	init_mqtt->connect_return_code = 0;
	init_mqtt->dup = 0;
	init_mqtt->fd = 0;
	init_mqtt->keepalive = 0;
	init_mqtt->MsgID = 0;
	init_mqtt->MsgType = 0;
	init_mqtt->next = NULL;
	init_mqtt->password = NULL;
	init_mqtt->password_flag = 0;
	init_mqtt->username = NULL;
	init_mqtt->username_flag = 0;
	init_mqtt->protocol_version = 0x00;
	init_mqtt->qos = 0;
	init_mqtt->remaining_length = 0;
	init_mqtt->retain = 0;
	init_mqtt->topic = NULL;
	init_mqtt->will = 0;
	init_mqtt->will_message = NULL;
	init_mqtt->will_payload = NULL;
	init_mqtt->will_payloadlen = 0;
	init_mqtt->will_qos = 0;
	init_mqtt->will_retain = 0;
	init_mqtt->will_topic = NULL;
	init_mqtt->payload= NULL;
}

int mqtt_while_read_and_write_handle(void *fd)//loop read or write
{
	#ifdef DEBUG
	printf("enter mqtt_while_read_and_write_handle()!\n");//test only
	#endif
	int cfd  = *((int*)fd);
	int i, msgtype;
	struct mqtt *serv_mq = NULL;
//	struct mqtt_packet *serv_packet = NULL;
	int ret = -1;
	#ifdef DEBUG
		printf("mqtt_while_read_and_write_handle: cfd = %d\n", cfd);
	#endif
	char bytes[4096];
	lseek(cfd, 0, SEEK_SET);
	while(read(cfd, bytes, 4096) >= 2){
		#ifdef DEBUG
		printf("--------------------\n");//test only
		printf("\n");
		printf("bytes[0] = %d, bytes[1] = %d\n", bytes[0], bytes[1]);
		printf("bytes[2] = %d, bytes[3] = %d, bytes[4] = %d\n", bytes[2], bytes[3], bytes[4]);
		#endif
	
//		buffer_msgtype_analysis(bytes, &msgtype);
		serv_mq = (struct mqtt *)malloc(sizeof(struct mqtt));
		mqtt_init_mqtt(serv_mq);//init mqtt struct.
		if(!serv_mq){
			printf("mqtt_while_read_and_write_handle: malloc memory fail, memory lack!\n");
			return -1;
		}
//		serv_mq->fd = cfd;
//		serv_mq->MsgType = msgtype;
		ret = mqtt_packet_handle(cfd, serv_mq, bytes);
		if(ret){
			printf("mqtt_packet_handle() fail!\n");
			close(cfd);
			return -1;
		}
	}
/*	serv_mq = (struct mqtt *)malloc(sizeof(struct mqtt));
	if(!serv_mq){
		printf("mqtt_while_read_and_write_handle: malloc memory fail, memory lack!\n");
		return -1;
	}
	serv_packet = (struct mqtt_packet *)malloc(sizeof(struct mqtt_packet));
	if(!serv_packet){
		printf("mqtt_while_read_and_write_handle: malloc memory fail, memory lack!\n");
		return -1;
	}
	byte = _mqtt_read_byte(cfd, &byte, 1);
	if(byte == 1){
		serv_packet->command = byte;
		#ifdef DEBUG
		printf("serv_packet->command = %d\n", serv_packet->command);//test only
		#endif
	}
	serv_packet->remaining_count = 0;
	serv_packet->remaining_length = 0;
	serv_packet->remaining_mul = 1;
	if(serv_packet->remaining_count <= 0){
		do{
			byte = _mqtt_read_byte(cfd, &byte, 1);
			printf("byte(10) = %d\n", byte);//test only
			if(byte == 1){
				serv_packet->remaining_count--;
				serv_packet->remaining_length += (byte & 127)*serv_packet->remaining_mul;
				serv_packet->remaining_mul = serv_packet->remaining_mul * 128;
			}
		}while((byte & 128) != 0);

	}
	#ifdef DEBUG
	printf("---------------------\n");
	printf("serv_mq->packet_in->remaining_count = %d\n", serv_packet->remaining_count);//test only
	printf("serv_mq->packet_in->remaining_length = %d\n", serv_packet->remaining_length);//test only
	printf("serv_mq->packet_in->remaining_mul = %d\n", serv_packet->remaining_mul);//test only
	#endif
*/
//read fixed header info from client finish!
	
/*	char buf[4096];//read buffer
//	memset(buf, 0, sizeof(buf));
//	byte = read(cfd, buf, 4096);
	#ifdef DEBUG
		printf("byte = %d\n", byte);
	#endif
	if(-1 == byte){
		printf("read() fail or message is empty.\n");
		close(cfd);
	}

	#ifdef DEBUG
	int i;
	for(i=0;i<byte;i++){
		printf("%c", buf[i]);//test only
	}
	buf[byte] = '\0';
	printf("strlen(buf) = %d\n", strlen(buf));
	#endif
	buffer_msgtype_analysis(buf, &msgtype);
	mq->fd = cfd;
	mq->MsgType = msgtype;
	ret = mqtt_packet_handle(mq, buf);
	if(ret){
		printf("mqtt_packet_handle() fail!\n");
		close(cfd);
		return -1;
	}
*/
	#ifdef DEBUG
	printf("==========mqtt_while_read_and_write_handle() end!==========\n");
	#endif
	printf("\n\n");

	return 0;
}

void mqtt_packet_free(struct mqtt_packet *packet)
{
	packet->command = 0;
	packet->msgid = 0;
	packet->packet_length = 0;
	if(packet->payload){
		free(packet->payload);	
		packet->payload = NULL;
	}
	packet->pos = 0;
	packet->remaining_count = 0;
	packet->remaining_length = 0;
	packet->remaining_mul = 1;
	packet->topic = NULL;
	packet->next = NULL;
}
