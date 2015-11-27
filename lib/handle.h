#ifndef _HANDLE_H_
#define _HANDLE_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include"mqtt_send.h"

typedef char bool;
#define true 1
#define false 0


/*Connect return code*/
#define CONNACK_ACCEPTED 0
#define CONNACK_REFUSED_PROTOCOL_VERSION 1
#define CONNACK_REFUSED_IDENTIFIER_REJECTED 2
#define CONNACK_REFUSED_SERVER_UNAVAILABLE 3
#define CONNACK_REFUSED_BAD_USERNAME_PASSWORD 4
#define CONNACK_REFUSED_NOT_AUTHORIZED 5

/*mqtt->state*/
#define mqtt_state_connecting 	0
#define mqtt_state_publish		1

struct willmessage
{
	char *topic;
	void *payload;
	int payloadlen;
	unsigned short qos;
	bool retain;
	char dup;
	unsigned short MsgID;
};

struct mqtt
{
	struct mqtt *next;
	int fd;
	int state;
	int MsgType;//Message Type
	char dup;
	unsigned short qos;
	char retain;
	int remaining_length;//1-4byte
	char *protocol_name;
	char protocol_version;
	char connect_flag;
	unsigned short keepalive;
	char *payload;
	char *ClientID;
	char *will_topic;
	char *will_payload;
	unsigned short will_payloadlen;
	char username_flag;
	char password_flag;
	char *username;
	char *password;
	bool will;//Will flag
	char will_qos;//Will QoS
	bool will_retain;//Will Retain flag
	char clean_session;//Clean session flag
	int connect_return_code;
	char *topic;
	unsigned short MsgID;//Message ID
	struct	willmessage *will_message;
	struct mqtt_packet *packet_in;
	struct mqtt_packet *packet_out;
};

struct mqtt_packet
{
	char *topic;
	char *payload;
	int remaining_mul;
	int remaining_length;
	char remaining_count;
	unsigned short msgid;
	int pos;
	char command;
	int packet_length;
	struct mqtt_packet *next;
};

/*
struct mqtt *struct_create(struct mqtt *mq)
{
	struct mqtt *head, *p1, *p2;
	p2 = p1 = (struct mqtt *)malloc(sizeof(struct mqtt));
	head = NULL;
	head = p1;
	p2 = p1;
	p2->next = NULL;

	return head;
}
*/
int mqtt_read_byte(char *buf, int *pos, char *str);
int mqtt_read_2byte(char *buf, int *pos, unsigned short *str);
int mqtt_read_string(char *buf, int *pos, char **str);
int mqtt_connect_handle(struct mqtt **mq, char *buf);
int mqtt_connack_handle(struct mqtt *mq, char *buffer);
int mqtt_publish_handle(struct mqtt *mq, char *buffer);
int mqtt_puback_handle(struct mqtt *mq, unsigned short msgid);
int mqtt_pubrec_handle(struct mqtt *mq, char *buffer);
int mqtt_pubrel_handle(struct mqtt *mq, char *buffer);
int mqtt_pubcomp_handle();
int mqtt_subscribe_handle(struct mqtt *mq, char *buf);
int mqtt_suback_handle();
int mqtt_unsubscribe_handle(struct mqtt *mq, char *buf);
int mqtt_unsuback_handle();
int mqtt_pingreq_handle(struct mqtt *mq, char *buf);
int mqtt_pingresp_handle(struct mqtt *mq);
int mqtt_disconnect_handle(struct mqtt *mq);
int mqtt_topic_sub_match();
int mqtt_packet_handle(int fd, struct mqtt *serv_mqtt, char *buf);
void buffer_msgtype_analysis(char *buf, int *msgtype_temp);
int mqtt_while_read_and_write_handle(void *fd);
void mqtt_packet_free(struct mqtt_packet *packet);
void mqtt_struct_free(struct mqtt *free_mq);
void mqtt_print_linklist_info();

#endif
