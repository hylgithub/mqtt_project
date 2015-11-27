#ifndef _MQTT_H_
#define _MQTT_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<time.h>
#include"handle.h"

/*Message Type*/
#define CONNECT			0x10
#define CONNACK			0x20
#define PUBLISH			0x30
#define PUBACK			0x40
#define PUBREC			0x50
#define PUBREL			0x60
#define PUBCOMP			0x70
#define SUBSCRIBE		0x80
#define SUBACK			0x90
#define UNSUBSCRIBE		0xA0
#define UNSUBACK		0xB0
#define PINGREQ			0xC0
#define PINGRESP		0xD0
#define DISCONNECT		0xE0

/*max Payloadlen defien*/
#define MQTT_MAX_PAYLOADLEN 268435455

int mqtt_client_publish(struct mqtt *mq, unsigned short msgid, const char *topic, void *payload, int payloadlen, int qos, char retain);
int mqtt_client_connect_back(int fd, struct mqtt *client_mq, char *clientid, char *username, char *password, unsigned short keepalive, char will_flag, char clean_session, char *will_topic, char will_retain, int will_qos);
int _mqtt_client_publish_back(struct mqtt *mq, int qos, unsigned short msgid);
int mqtt_client_send_pubrel(struct mqtt *mq, unsigned short msgid);
int mqtt_client_pubrel(struct mqtt *subclient_mq, unsigned short msgid);
int mqtt_encode_remaininglength(struct mqtt_packet *packet, char *remaining_bytes);
int mqtt_packet_write(struct mqtt *mq, struct mqtt_packet *packet);
int  mqtt_write(struct mqtt *mq);
int mqtt_write_string(struct mqtt_packet *packet, const void *str, unsigned short length);
int mqtt_write_mul_byte(struct mqtt_packet *packet, const void *str, unsigned short length);
int mqtt_write_2byte(struct mqtt_packet *packet, unsigned short length);
int mqtt_write_byte(struct mqtt_packet *packet, char len_byte);

int _mqtt_client_subscribe_back(struct mqtt *mq, unsigned short msgid, char **granted_qos);
#endif
