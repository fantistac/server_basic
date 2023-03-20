#include<stdio.h>
     #include<unistd.h>
     #include<sys/socket.h>
     #include<errno.h>
     #include<string.h>
     #include<arpa/inet.h>
     #include<sys/types.h>
     #include<sys/wait.h>
     #include<signal.h>
     #define PORT 9527
    
    void comeback(int T)
    {
      while( (waitpid(-1, NULL, WNOHANG) > 0 ));
      fprintf(stderr, "success back !\n");
    }
    
     int main()
     {
       int ret, k = 0;
       sigset_t set, oldset;
       char BUF[BUFSIZ], client_IP[BUFSIZ]; /**/
       int fd, fd1;
       pid_t pid = 0;
       struct sockaddr_in server_sock, server_sock1;
       struct sigaction act, old_act;
       socklen_t server_sock1_len;
    
       sigemptyset (&set);
       sigaddset(&set,SIGCHLD);
       ret = sigprocmask(SIG_BLOCK, &set, &oldset);
    
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
    
       while(1){
       server_sock1_len = sizeof(server_sock1);
       fd1 = accept(fd, (struct sockaddr *)&server_sock1, &server_sock1_len);
       if(fd1 == -1){
         fprintf(stderr, "accept error: %s\n", strerror(errno));
         continue;
       }
         else
       {
       fprintf(stdout, "client_IP = %s, client_port = %d\n", inet_ntop(AF_INET, (void *) &server_sock1.sin_addr.s_addr, client_IP, BUFSIZ), ntohs(server_sock1.sin_port));
       pid = fork();
       if(pid == 0)
         break;
       else{
       if(k == 0){
       act.sa_flags = 0;
       sigemptyset(&act.sa_mask);
       act.sa_handler = comeback;
       sigaction(SIGCHLD, &act, &old_act);
       ret = sigprocmask(SIG_UNBLOCK, &set, &oldset);
       ++k;
       }
       close(fd1);
       }
       }
       }
       if(pid == 0)
       {
       while(1)
       {
         close(fd);
         ret = read(fd1, BUF, BUFSIZ);
         if(ret == -1)
           fprintf(stderr, "read error: %s\n", strerror(errno));
         if(ret == 0)
         {
           close(fd1);
           return 0;
         }
       write(STDOUT_FILENO, BUF, ret);
    
       for(int i = 0; i < ret; ++i)
        BUF[i] = toupper(BUF[i]);
       write(fd1, BUF, ret);
       }
       }
       return 0;
     } 
