# getaddrinfo

getaddrinfo函数将 \
主机名, 主机地址, 服务号, 端口号的字符串表示 => 套接字地址结构;

一个很好的方面是addrinfo中的字段不是透明的, 他们可以直接传递给套接字接口中的函数,应用程序无需做任何处理 \
ai_family, ai_socktype, ai_protocol可以直接传递给socket; \
ai_addr, ai_addrlen可以直接传递给connect和bind;

```
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int	getaddrinfo(const char *host,
                const char *service,
			    const struct addrinfo *hints,
			    struct addrinfo **result);

返回:如果成功则为0, 如果错误则为非0的错误代码;
- host 可以是域名也可以是点分十进制IP地址
- service: 可以是服务名,
- hints: addrinfo结构, 只能设置下列字段 ai_family, ai_socktype, ai_protocol, ai_flags,其他字段必须设置为0, 实际中会用memset将结构清0

struct addrinfo {
	int	ai_flags;	/* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
	int	ai_family;	/* PF_xxx */
	int	ai_socktype;	/* SOCK_xxx */
	int	ai_protocol;	/* 0 or IPPROTO_xxx for IPv4 and IPv6 */
	socklen_t ai_addrlen;	/* length of ai_addr */
	char	*ai_canonname;	/* canonical name for hostname */
	struct	sockaddr *ai_addr;	/* binary address */
	struct	addrinfo *ai_next;	/* next structure in linked list */
};
- ai_flags
    + AI_ADDRCONFIG: 查询配置的地址类型(IPv4或IPv6).
    + AI_ALL: 查找IPv4和IPv6地址(仅用于AI_V4MAPPED).
    + AI_CANONNAME: 需要一个规范名(而不是别名).
    + AI_NUMERICHOST: 以数字格式返回主机地址.
    + AI_NUMERICSERV: 以端口号返回服务.
    + AI_PASSIVE: socket地址用于监听绑定.
    + AI_V4MAPPED: 如果没有找到IPv6地址, 则返回映射到IPv6格式的IPv6地址.
- ai_family : AF_INET ipv4, AF_INET6 ipv6
- ai_socktype: SOCK_STREAM


void freeaddrinfo(struct addrinfo *result);
返回: 无

const char *gai_strerror(int errcode);
返回: 错误消息
```


# getnameinfo函数

getnameinfo 函数 \
将 套接字地址结构 -> 主机和服务名字符串 \

```
int getnameinfo(const struct sockaddr *sa,
              socklen_t salen,
              char *host,
              socklen_t hostlen,
              char *service,
              socklen_t servlen,
              int flags);
返回: 如果成功则为0, 如果错误则为非0的错误代码

```
- sa: 套接字地址结构
- host: 指向大小为hostlen字节的缓冲区
- service: 大小为servlen字节的缓冲区
- flags: 指定转换的控制方式. 可以有如下几种:
    + NI_DGRAM: 服务基于数据报而非基于流.
    + NI_NAMEREQD: 如果找不到主机名字, 将其作为一个错误对待.
    + NI_NOFQDN: 对于本地主机, 仅返回完全限定域名的节点名部分.
    + NI_NUMERICHOST: 以数字形式而非名字返回主机地址.
    + NI_NUMERICSERV: 以数字形式而非名字返回服务地址(即端口号).



