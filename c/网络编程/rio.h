#include <stdio.h>
// read, write
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>

#define RIO_BUFSIZE 8192
#define LISTENQ 1024
#define MAXLINE 1024

typedef struct {
	int rio_fd;// 描述内部缓冲区
	ssize_t rio_cnt;// 未读的字节数内部缓冲区
	char *rio_bufptr;// 下一个未读的字节
	char rio_buf[RIO_BUFSIZE]; //内部缓冲区
}  rio_t;

/**
 * 从描述符fd的当前位置最多传送n个字节到内存位置usrbuf
 * @param fd
 * @param usrbuf
 * @param n
 * @return
 */
ssize_t rio_readn(int fd, void *usrbuf, size_t n);

/**
 * 从未usrbuf传送n个字节到描述符fd
 * @param fd
 * @param usrbuf
 * @param n
 * @return
 */
ssize_t rio_writen(int fd, void *usrbuf, size_t n);

/**
 * 将描述符fd和地址rp处的一个类型为rio_t的读缓冲区联系起来
 * @param rp
 * @param fd
 */
void rio_readinitb(rio_t *rp, int fd);

/**
 * 从读缓冲区复制n和rp->rio_cnt中的较小值个字节到用户缓冲区返回复制的字节数
 * 返回值 -1 表示出错
 * 返回值  0 表示EOF
 * @param rp
 * @param usrbuf
 * @param n
 * @return
 */
static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n);

/**
 * 从内部缓冲区复制一个文本行, 对缓冲区变空时, 会自动调用read重新填满缓冲区
 * @param rp
 * @param usrbuf
 * @param maxlen
 * @return
 */
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

/**
 * 读取即包含文本行也包含二进制数据的文件
 * @param rp
 * @param usrbuf
 * @param n
 * @return
 */
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);

int open_clientfd(char *hostname, char *port);

int open_listenfd(char *port);
