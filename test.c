#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<errno.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<pthread.h>
#define PORT 9527

struct message{
	struct sockaddr_in p_sock;
	int connid;
};
void *comeback(void *arg)
{
	char client_IP_TMP[BUFSIZ];
	char BUF_TMP[BUFSIZ];
	int ret_tmp = 0;
	struct message *mess = (struct message *) arg;
	fprintf(stdout, "client_IP = %s, client_port = %lu\n", inet_ntop(AF_INET, (void *) &mess->p_sock.sin_addr.s_addr, client_IP_TMP, BUFSIZ), ntohs(mess->p_sock.sin_port));

	while(1)
	{
		ret_tmp	= read(mess->connid, BUF_TMP, BUFSIZ);
		if(ret_tmp == -1)
			fprintf(stderr, "read error: %s\n", strerror(errno));
		if(ret_tmp == 0)
		{
			fprintf(stderr, "client thread %d is closed\n", pthread_self());
			break;
		}
		write(STDOUT_FILENO, BUF_TMP, ret_tmp);

		for(int i = 0; i < ret_tmp; ++i)
			BUF_TMP[i] = toupper(BUF_TMP[i]);
		write(mess->connid, BUF_TMP, ret_tmp);
	}

	close(mess->connid);
	pthread_exit(0);
}

int main()
{
	int ret, i = 0;
	int fd, fd1;
	pthread_t tid;
	struct message mess[128];
	struct sockaddr_in server_sock, server_sock1;
	socklen_t server_sock1_len;


	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1)
		fprintf(stderr, "socket error:%s\n", strerror(errno));


	server_sock.sin_family = AF_INET;
	server_sock.sin_port = htons(PORT);
	server_sock.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(fd, (struct sockaddr *)&server_sock, sizeof(server_sock));
	if(ret == -1)
		fprintf(stderr, "bind error:%s\n", strerror(errno));

	ret = listen(fd, 10);
	if(ret == -1)
		fprintf(stderr, "listen error:%s\n", strerror(errno));

		fprintf(stdout, "accepting client connect...\n");
	while(1){
		server_sock1_len = sizeof(server_sock1);
		fd1 = accept(fd, (struct sockaddr *)&server_sock1, &server_sock1_len);
		if(fd1 == -1){
			fprintf(stderr, "accept error: %s\n", strerror(errno));
			continue;
		}
		else
		{
			mess[i].connid = fd1;
			mess[i].p_sock = server_sock1;
			pthread_create(&tid, NULL, comeback, (void *)&mess[i]);
			pthread_detach(tid);
			++i;
		}
	}
	return 0;
} 
