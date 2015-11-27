#ifndef _MQTT_SEND_H_
#define _MQTT_SEND_H_

#include"handle.h"
#include"mqtt.h"

int _mqtt_send_puback(struct mqtt *mq);
int _mqtt_send_pubrec(struct mqtt *mq);
int _mqtt_send_pubrel(struct mqtt *mq);
int _mqtt_send_pubcomp(struct mqtt *mq);
int _mqtt_send_pingresp(struct mqtt *mq);
int _mqtt_send_unsuback(struct mqtt *mq);
int _mqtt_send_suback(struct mqtt *mq, char granted_qos);
int _mqtt_connack_back(struct mqtt **mq, char return_code);



#endif
