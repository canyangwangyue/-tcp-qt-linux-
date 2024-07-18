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
	lfd = socket(AF_INET, SOCK_STREAM, 0);//�����������׽���lfd
	if (lfd == -1) {
		perror("socket error");
		exit(1);
	}
	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//���ö˿ڸ���
	struct sockaddr_in addr;
	/*���÷�����������Ϣ*/
	memset(&addr, 0, sizeof(addr));//���
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3666);
	addr.sin_addr.s_addr = INADDR_ANY;

	/*�󶨷�������ַ���׽���socket*/
	int ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == -1) {
		perror("bind error");
		exit(1);
	}
	/*��������*/
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
	/*�������������Ӻ��ṩ����*/
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
				/*�������Կͻ��˵��׽���socket cfd*/
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

			else {//�������if��eps[i].data.fd == lfd����Ӧ
				//char rev[1024];
				char buf[10240];//������
			
				//int n = read(eps[i].data.fd, buf, 1024);//eps[i].data.fdΪҪ��ȡ���ļ���������
				//bufΪҪ�������ݵĻ�������ַ,1024ΪҪ��ȡ�������ֽ���
				//read() ������� fd �ļ��ж�ȡ nbytes ���ֽڲ����浽������ buf���ɹ��򷵻ض�ȡ�����ֽ������������ļ���β�򷵻�0����ʧ���򷵻� -1��
				while (1) {
					//memset(buf, 0, sizeof(buf));
					//int nΪ���յ������ݳ���
					int n = recv(eps[i].data.fd, buf, 10240, 0);
					//n = n - 19;
					if (n < 0) {//��ȡʧ��
						perror("read error");
						//exit(1);
						// ������ļ���������epollģ����ɾ��
						epoll_ctl(epfd, EPOLL_CTL_DEL, eps[i].data.fd, NULL);
						close(eps[i].data.fd);
						break;
					}
					
					else if (n == 0) {//�ͻ����ѹر�

						int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, eps[i].data.fd, NULL);
						if (ret == -1) {
							fprintf(stderr, "epoll_ctl error:%s\n", strerror(ret));
							exit(1);
						}
						close(eps[i].data.fd);
						printf("Client is duankai lianjie\n");
						break;
					}
					else {//n>0 �ͻ��˴�
					
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




