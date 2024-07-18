#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<ctype.h>
#include<sys/epoll.h>
#include<iostream>
#include<sstream>
#include<string>
#include<vector>


#define backlog 5
#define MAXBUFF 4096
#define MAXNAMELEN 100
#define BUF_SIZE 10240






int main(int argc, char* argv[]) {
	int lfd, cfd;
	//pid_t pid
	lfd = socket(AF_INET, SOCK_STREAM, 0);//创建监听的套接字lfd
	if (lfd == -1) {
		perror("socket error");
		exit(1);
	}
	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//设置端口复用
	struct sockaddr_in addr;
	/*设置服务器基本信息*/
	memset(&addr, 0, sizeof(addr));//清空
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3666);
	addr.sin_addr.s_addr = INADDR_ANY;

	/*绑定服务器地址到套接字socket*/
	int ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == -1) {
		perror("bind error");
		exit(1);
	}
	/*启动监听*/
	ret = listen(lfd, 128);
	if (ret == -1) {
		perror("listen error");
		exit(1);
	}
	struct epoll_event ep, eps[2000];
	int epfd = epoll_create(1);
	if (epfd == -1) {
		fprintf(stderr, "epoll_create error:%s\n", strerror(epfd));
		exit(1);
	}
	ep.events = EPOLLIN;
	ep.data.fd = lfd;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ep);
	if (ret == -1) {
		fprintf(stderr, "epoll_ctl error:%s\n", strerror(ret));
		exit(1);
	}
	/*服务器建立连接和提供服务*/
	while (1) {
		int n = epoll_wait(epfd, eps, 2000, -1);
		if (n == -1) {
			fprintf(stderr, "epoll_wait error:%s\n", strerror(n));
			exit(1);
		}
		for (int i = 0; i < n; i++) {
			if (!(eps[i].events & EPOLLIN)) {
				continue;
			}
			if (eps[i].data.fd == lfd) {
				struct sockaddr_in caddr;
				socklen_t len = sizeof(caddr);
				/*接收来自客户端的套接字socket cfd*/
				cfd = accept(lfd, (struct sockaddr*)&caddr, &len);
				if (cfd == -1) {
					perror("accept error");
					exit(1);
				}
				char buf[10240];
				printf("Client is lianjie ip:%s  port:%d\n", inet_ntop(AF_INET, &caddr.sin_addr, buf, 10240), ntohs(caddr.sin_port));
				ep.events = EPOLLIN;
				ep.data.fd = cfd;
				int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ep);
				if (ret == -1) {
					fprintf(stderr, "epoll_ctl error:%s\n", strerror(ret));
					exit(1);
				}
			
			}

			else {//跟上面的if（eps[i].data.fd == lfd）对应
				//char rev[1024];
				char buf[10240];//缓冲区
			
				//int n = read(eps[i].data.fd, buf, 1024);//eps[i].data.fd为要读取的文件描述符，
				//buf为要接收数据的缓冲区地址,1024为要读取的数据字节数
				//read() 函数会从 fd 文件中读取 nbytes 个字节并保存到缓冲区 buf，成功则返回读取到的字节数（但遇到文件结尾则返回0），失败则返回 -1。
				while (1) {
					//memset(buf, 0, sizeof(buf));
					//int n为接收到的数据长度
					int n = recv(eps[i].data.fd, buf, 10240, 0);
					//n = n - 19;
					if (n < 0) {//读取失败
						perror("read error");
						//exit(1);
						// 将这个文件描述符从epoll模型中删除
						epoll_ctl(epfd, EPOLL_CTL_DEL, eps[i].data.fd, NULL);
						close(eps[i].data.fd);
						break;
					}
					
					else if (n == 0) {//客户端已关闭

						int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, eps[i].data.fd, NULL);
						if (ret == -1) {
							fprintf(stderr, "epoll_ctl error:%s\n", strerror(ret));
							exit(1);
						}
						close(eps[i].data.fd);
						printf("Client is duankai lianjie\n");
						break;
					}
					else {//n>0 客户端打开
					
						printf("%s", buf);
						
						//int bufsize = strlen(buf);
						printf("  bufsize:%d\n", n);
						write(STDOUT_FILENO, buf, n);
						//write(eps[i].data.fd, buf, n);
						send(cfd, buf, n, 0);
						//send(eps[i].data.fd, buf, n,0);
						//send(cfd, buf, n, 0);
				
						}
				
					}
				
				 
			}
		}
	}
	close(lfd);
	close(epfd);
	return 0;
}




