#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>

#define PORT 9527

int main()
{
	int listen_fd, client_fd;
	int client_port;
	char client_id[BUFSIZ];
	int ret = 0, rb_fd = 0;
	struct sockaddr_in server_addr, client_addr;
	struct epoll_event ev, evs[128];
	socklen_t client_len;
	char BUF[BUFSIZ];

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(listen_fd == -1) fprintf(stderr, "socket error: %s\n", strerror(errno));
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9527);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(ret == -1) fprintf(stderr, "bind error: %s\n", strerror(errno));

	ret = listen(listen_fd, 128);
	if(ret == -1) fprintf(stderr, "listen error: %s\n", strerror(errno));
	
	rb_fd = epoll_create(128);
	if(rb_fd == -1) fprintf(stderr, "epoll_creat error: %s\n", strerror(errno));

	ev.data.fd = listen_fd;
	ev.events = EPOLLIN;

	ret = epoll_ctl(rb_fd, EPOLL_CTL_ADD, listen_fd, &ev);
	if(ret == -1) fprintf(stderr, "epoll_ctl error: %s\n", strerror(errno));
	
	while(1)
	{
		ret = epoll_wait(rb_fd, &evs, 128, -1);
  	if(ret == -1) fprintf(stderr, "epoll_wait error: %s\n", strerror(errno));

		if(ret > 0){
			for(int i = 0; i < ret; ++i){
			if(evs[i].events != EPOLLIN) continue;

			if(evs[i].data.fd == listen_fd){
				client_len = sizeof(client_addr);
				client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
				if(client_fd == -1) fprintf(stderr, "accept error: %s\n", strerror(errno));

				inet_ntop(AF_INET, (void *)&client_addr.sin_addr.s_addr, client_id, BUFSIZ);
				client_port = ntohs(client_addr.sin_port);
				fprintf(stdout, "client ip = %s, client port = %d is connecting\n",client_id, client_port);  

				ev.data.fd = client_fd;
				ev.events = EPOLLIN;
				int tmp = epoll_ctl(rb_fd, EPOLL_CTL_ADD, client_fd, &ev);
				if(tmp == -1) fprintf(stderr, "epoll_ctl_client error: %s\n", strerror(errno));
				continue;
			}else{
				int count = read (evs[i].data.fd, BUF, BUFSIZ);	
				if(count == 0){
					fprintf(stdout, "%d is closed...\n", evs[i].data.fd);
					close(evs[i].data.fd);
					epoll_ctl(rb_fd, EPOLL_CTL_DEL,evs[i].data.fd,NULL);
 				}else if(count == -1) {
					fprintf(stderr, "read error: %s\n", strerror(errno));
				}else{
					write(STDOUT_FILENO, BUF, count);
					for(int j = 0; j < count; ++j){
						BUF[j] = toupper(BUF[j]);
 					}
					write(evs[i].data.fd, BUF, count);
 				}
 				}
			}
		}
	}
	return 0;
}

