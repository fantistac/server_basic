#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/time.h>
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
	int ret = 0, max_fd = 1, count = 0;
	struct sockaddr_in server_addr, client_addr;
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
	
	fd_set r_set, m_set;
	max_fd = listen_fd;

	FD_ZERO(&r_set);
	FD_ZERO(&m_set);
	FD_SET(listen_fd, &r_set);
	while(1){
		m_set = r_set;

	 	ret  = select(max_fd + 1, &m_set, NULL, NULL, NULL);
		if(ret == -1) fprintf(stderr, "select error: %s\n", strerror(errno));
		if(FD_ISSET(listen_fd, &m_set)){
			socklen_t client_len = sizeof(client_addr);
			
			client_fd  = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
			if(client_fd == -1) fprintf(stderr, "accept error: %s\n", strerror(errno));
		
			inet_ntop(AF_INET, (void *) &client_addr.sin_addr.s_addr, client_id, BUFSIZ);
			client_port = ntohs(client_addr.sin_port);
			fprintf(stdout, "Client IP = %s, Client port = %d is connected\n", client_id, client_port);

			FD_SET(client_fd, &r_set);
			if(max_fd < client_fd) max_fd = client_fd;

			if(ret == 1) continue;
		}

		for(int i = listen_fd + 1; i < max_fd + 1; ++i){
			if(FD_ISSET(i, &m_set)){
				ret = read(i, BUF, BUFSIZ);
				if(ret == 0){
					fprintf(stderr, "%d is closed..\n", i);
					close(i);
					FD_CLR(i, &r_set);
				}else if(ret > 0){
					write(STDOUT_FILENO, BUF, ret);
					for(int j = 0; j < ret; ++j)
						BUF[j] = toupper(BUF[j]);
					write(i, BUF, ret);
				}else{
					fprintf(stderr, "read error %s\n", strerror(errno));
				}
			}
		}
	}
	close(listen_fd);
	return 0;
}

