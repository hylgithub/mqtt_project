#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<pthread.h>

#include"./../lib/handle.h"
#include"./../lib/mqtt.h"
#include"./../lib/mqtt_send.h"

#define PORT 9919
#define MAX_LISTEN_NUM 10
//#define DEBUG

int mqtt_sock_listen(int *sfd)
{
//	int i;
	struct sockaddr_in serv_addr;

	*sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == *sfd){
		printf("socket() fail !\n");
		return -1;
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);
	if((bind(*sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == -1){
		printf("bind() fail !\n");
		close(*sfd);
		return -1;
	}

	listen(*sfd, MAX_LISTEN_NUM);//MAX_LISTEN_NUM is the max listeners;

	return 0;
}

int mqtt_while(int fd)
{
	pthread_t thread;
	int cfd, run = 1;
	while(run){
		cfd = accept(fd, NULL, 0);
		#ifdef DEBUG
		printf("cfd = %d\n", cfd);
		#endif
		if(-1 == cfd){
			printf("accept() fail!\n");
			close(fd);
			return cfd;
		}
//		mqtt_while_read_and_write_handle(&cfd);
		pthread_create(&thread, NULL, mqtt_while_read_and_write_handle, (void *)&cfd);
	}
	close(fd);
	return 0;
}

int main(int argc, char *argv[])
{
	int sfd, ret;
	ret = mqtt_sock_listen(&sfd);
	if(ret){
		printf("mqtt_sock_listen() error!\n");
		return ret;
	}
	mqtt_while(sfd);
	close(sfd);
	return 0;
}
