#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include"mqtt.h"

#define PORT 9919
#define DEBUG	//test only

/*subscribe message*/
//#define TOPIC "topic_test"
#define TOPIC "topic_testjflajdfjalfjaodflajfajsdlfja0eoolflaj"
#define QOS 1
#define RETAIN 1
#define CLIENT_ID "sub_clientalkfj0a514"
#define DUP 0
#define MSGID 10

int main(void)
{
	int fd;
	int eno = -1;
	struct sockaddr_in addr;
	char buf[1024];
	struct mqtt client_mq;
	memset(buf, 0, sizeof(buf));
	char granted_qos;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == fd){
		printf("socket() fail !\n");
		return -1;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);
	eno = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	if(-1 == eno){
		printf("connect() fail !\n");
		close(fd);
		return -1;
	}
	if(!mqtt_client_connect_back(fd, &client_mq, CLIENT_ID, NULL, NULL, 10, true, 1, TOPIC, 1, 0)){
		printf("Connect to Service success!\n");
	}
	printf("sub_client: Connect to Service success!\n");

	client_mq.fd = fd;
	printf("--------------------\n");
	printf("--------------------\n");
	mqtt_client_subscribe(client_mq.fd, &client_mq, QOS, DUP, MSGID, TOPIC, 1, &granted_qos);
	#ifdef DEBUG
	printf("strlen(topic) = %d\n", strlen(TOPIC));
	printf("client_mq.fd = %d\n", client_mq.fd);
	printf("client_mq.qos = %d\n", client_mq.qos);
	printf("client_mq.dup = %d\n", client_mq.dup);
	printf("granted_qos = %d\n", granted_qos);
	#endif
	//printf("\nsub_client: subscribe topic to Service OK!\n");

	close(fd);
	return 0;
}

