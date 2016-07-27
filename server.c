#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<string.h>
#include<pthread.h>

static void usage(const char *proc)
{
	printf("usage:%s [ip] [port]\n",proc);
}

void *thread_run(void *arg)
{
	int fd = (int *)arg;
	printf("create a new thread\n");

    char buf[1024];
    while(1){
    	memset(buf,'\0',sizeof(buf));
    	ssize_t _s = read(fd,buf,sizeof(buf)-1);
    	if(_s > 0){
    		buf[_s] = '\0';
			printf("client say:  %s\n",buf);

            memset(buf,'\0',sizeof(buf));
			printf("Please enter: ");
			fflush(stdout);
			_s = read(0,buf,sizeof(buf));
		    if(_s > 0){
    			buf[_s-1] = '\0';
				write(fd,buf,strlen(buf));
			}
    	}else{
    		printf("read done....");
    		break;
    	}
	}
	return (void *)0;
}
int main(int argc,char *argv[])
{
	if(argc != 3){
		usage(argv[0]);
		exit(1);
	}
	int listen_sock = socket(AF_INET,SOCK_STREAM,0);
	if(listen_sock < 0){
		perror("socket\n");
		return 1;
	}

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(atoi(argv[2]));
	local.sin_addr.s_addr = inet_addr(argv[1]);

    if(bind(listen_sock,(struct sockaddr *)&local,sizeof(local)) < 0){
   		perror("bind\n");
   		return 2;
   	}
    
   	listen(listen_sock,5);
    
	struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    
	while(1){
		int fd = accept(listen_sock,(struct sockaddr*)&peer,&len);
    	if(fd < 0){
    		perror("accept");
    		return 3;
    	}
    
		pthread_t id;
		pthread_create(&id,NULL,thread_run,(void *)fd);
		pthread_detach(id);

	}
	return 0;
}
