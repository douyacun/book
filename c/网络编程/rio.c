//
// Created by 刘宁 on 2018/8/8.
//

#include "rio.h"

ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char *bufp = usrbuf;

	while ( nleft > 0){
		if( (nread = read(fd, bufp, nleft)) < 0){
			// EINTR: 程序中的信号处理程序捕获到函数的执行被某个信号中断，信号处理程序然后按正常方式返回
			if(errno == EINTR){
				nread = 0;
			} else {
				return -1;
			}
		} else if( nread == 0){// EOF
			break;
		}

		nleft -= nread;
		bufp += nread;
	}

	return (n-nleft);
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
	size_t nleft  = n;
	char *bufp = usrbuf;
	ssize_t nwrite;

	while( nleft > 0 ){
		if((nwrite = write(fd, bufp, nleft)) <= 0){
			if(errno == EINTR){
				nwrite = 0 ;
			} else {
				return -1;
			}
		}
		nleft -= nwrite;
		bufp += nwrite;
	}
	return n;
}

void rio_readinitb(rio_t *rp, int fd)
{
	rp->rio_fd = fd;
	rp->rio_cnt = 0;
	rp->rio_bufptr = rp->rio_buf;
}

ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
	ssize_t cnt;

	while (rp->rio_cnt <= 0){// Refill if buf is empty
		rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
		if(rp->rio_cnt < 0){
			if(errno != EINTR){
				return -1;
			}
		} else if(rp->rio_cnt == 0){// EOF
			return 0;
		} else {
			rp->rio_bufptr = rp->rio_buf; // Reset buffer ptr
		}
	}

	// copy min(n, rp->rio_cnt) bytes from internal buf to user buf
	cnt = n;
	if(rp->rio_cnt < n){
		cnt = rp->rio_cnt;
	}

	memcpy(usrbuf, rp->rio_bufptr, cnt);
	rp->rio_bufptr += cnt;
	rp->rio_cnt -= cnt;
	return cnt;
}

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen){
	int n;
	ssize_t rc;
	char c, *bufp = usrbuf;

	for (n = 1; n < maxlen; ++n) {
		if( (rc = rio_read(rp, &c, 1)) == 1){
			*bufp++ = c;
			if(c == '\n'){
				n++;
				break;
			}
		} else if(rc == 0){// EOF
			if(n == 1){
				return 0;// EOF , no data read
			} else {
				break;
			}
		} else {
			return -1;// error
		}
	}
	*bufp = 0;
	return n - 1;
}

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char *bufp = usrbuf;

	while(nleft > 0)
	{
		if( (nread = rio_read(rp, bufp, n)) < 0){
			return -1;
		} else if (nread == 0 ){
			break;
		}

		nleft -= nread;
		bufp += nread;
	}

	return (n - nleft);
}

int open_clientfd(char *hostname, char *port)
{
	int clientfd, rc;
	struct addrinfo hints,  *listp, *p;
	struct sockaddr_in *sockp;
	char buf[MAXLINE];


	//get a list of potential server address
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	// 默认会检查 /etc/services 如果可能会返回服务号, 设置该标志会使该函数跳过检查, 简单的返回端口号
	hints.ai_flags = AI_NUMERICSERV;
	hints.ai_flags |= AI_ADDRCONFIG;
	if((rc = getaddrinfo(hostname, port, &hints, &listp)) != 0){
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
		exit(0);
	}

	for (p = listp; p; p = listp->ai_next) {

		if( (clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0){
			continue;
		}

		if(connect(clientfd, p->ai_addr, p->ai_addrlen) != -1){
			// success
			break;
		}

		close(clientfd);
	}

	// clean up
	freeaddrinfo(listp);
	if(!p){
		return -1;
	} else {
		return clientfd;
	}
}

int open_listenfd(char *port)
{
	struct addrinfo hints, *listp, *p;
	int rc, listenfd, optval = 1;
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_socktype = SOCK_STREAM;
	// AI_PASSIVE: 被动的，用于bind，通常用于server socket
	// AI_CANONNAME:
	// AI_ADDRCONFIG: 查询配置的地址类型(IPv4或IPv6).
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	hints.ai_flags |= AI_NUMERICSERV;

	if( (rc=getaddrinfo(NULL, port, &hints, &listp)) != 0 ){
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
		exit(0);
	}

	for (p = listp; p; p = listp->ai_next) {
		// socket描述符
		if( (listenfd = socket(p->ai_family, p->ai_addrlen, p->ai_protocol)) < 0){
			continue;
		}
		// eliminates(消除) "Address already in use" error from bind
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

		// 套接字和描述符绑定在一起
		if(bind(listenfd, p->ai_addr, p->ai_addrlen) == 0){
			// success
			break;
		}

		// bind failed, try next
		close(listenfd);
	}

	// clean up
	freeaddrinfo(listp);

	if(!p){
		return -1;
	}

	if(listen(listenfd, LISTENQ) < 0 ){
		close(listenfd);
		return -1;
	}

	return listenfd;
}