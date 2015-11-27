#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include"mqtt.h"

#define PORT 9919
#define DEBUG	//test only

/*publish message*/
//#define TOPIC "topic_test"
#define TOPIC "topic_testjflajdfjalfjaodflajfajsdlfja0eoolflaj"
#define PAYLOAD "Hello, this is a jfaldfjaodjfoadsjrpfjaosdjfjaodfjadjflajforrajfoandfoajdofjaofjaofjoajfoafoafomqtt testdfad"
#define QOS 2
#define RETAIN 1
#define CLIENT_ID "alkfj0a514client"
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
	printf("pub_client: Connect to Service success!\n");

	client_mq.fd = fd;
	printf("--------------------\n");
	if(!mqtt_client_connect_back(fd, &client_mq, CLIENT_ID, NULL, NULL, 10, true, 1, PAYLOAD, 1, 0)){
		printf("Connect to Service success!\n");
	}
	printf("--------------------\n");
	printf("fd = %d\n", fd);//test only
	printf("---------qos = %d test-----------\n", QOS);
	#ifdef DEBUG
	printf("strlen(topic) = %d\n", strlen(TOPIC));
	printf("strlen(payload) = %d\n", strlen(PAYLOAD));
	#endif
	if(mqtt_client_publish(&client_mq, MSGID, TOPIC, PAYLOAD, strlen(PAYLOAD), QOS, RETAIN) == -1){
		printf("publish message to sevice fail!\n");
		return -1;
	}
//	if(mqtt_client_pingreq(fd, &client_mq) == -1){
//		printf("PING fail!\n");
//		return -1;
//	}

/*	******************test***************************/
//	printf("send msg to service.\n");
//	write(fd, "Hello World!", sizeof("Hello World!"));
//	bytes = read(fd, buf, sizeof(buf));
//	printf("bytes = %d\n", bytes);
//	if(-1 == bytes){
//		printf("pub_client: read buf[] error!\n");
//		close(fd)
//		return -1;
//	}
//	for(i=0;i<bytes;i++){
//		printf("%c", buf[i]);
//	}

/*	******************end****************************/

	printf("\npub_client: publish to Service OK!\n");
	close(fd);
	return 0;
}

