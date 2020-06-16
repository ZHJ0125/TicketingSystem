#ifndef	__GLOBALAPI_H
#define	__GLOBALAPI_H

#include <sys/types.h>	/*基本的系统数据类型*/
#include <sys/socket.h> /*基本的套接字的定义*/
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/un.h>
#include <string.h>
#include <pthread.h>
#include <mysql/mysql.h>

/*服务器端使用的端口*/
#define	SERVER_PORT_NO	8889

/*客户端与服务器端使用的消息类型定义*/
#define	INITIAL_VALUE	65535

/*客户端使用的消息代码含义*/
#define	DISCONNECT		0
#define	BUY_TICKET 	1
#define	INQUIRE_ONE	2
#define	INQUIRE_ALL	3
#define ADD_TICKET  4
#define UPDATE_TICKET 5
#define DELETE_TICKET 6

/*服务器端使用的消息代码含义*/
#define	BUY_SUCCEED		255
#define	BUY_FAILED			256
#define	INQUIRE_SUCCEED	257
#define	UNKNOWN_CODE		258

/*服务器端的线程缓冲区的最大数量*/
#define	THREAD_BUFF_NUM 128

/*提示信息输出*/
#define INFO_NUM	10	
#define	INFO_OCCUPIED	1
#define	INFO_FREED	0

pthread_mutex_t 	info_mutex;

/***************************message是客户端与服务器之间的消息结构体*******************************/

/*服务器与客户端使用的消息结构定义，用来向服务器请求不同类型的信息*/
struct stMessage {
	//消息类型。客户端可以取值为DISCONNECT：断开连接，BUY_TICKET：购买机票，I	INQUIRE_ONE：查询特定航班机票，	INQUIRE_ALL：查询所有航班机票
	unsigned int 	msg_type;		// 消息类型，用来向服务器请求不同类型的信息
    unsigned int	flight_ID;      // 航班号
    unsigned int	ticket_num;     // 机票张数
    unsigned int	ticket_total_price;     //机票价钱
} message;

/* 将消息数据类型进行初始化 */
void init_message()
{
	message.msg_type=INITIAL_VALUE;
	message.flight_ID=0;
	message.ticket_num=0;
	message.ticket_total_price=0;
}

/***************************以下是有关消息输出缓冲区的操作函数*************************************/

struct info_t {
	int		status;			/*INFO_OCCUPIED or INFO_FREED 消息缓冲区的状态*/
	char	msg[512];		/*contents of message 服务器返回的具体消息内容*/
} info[INFO_NUM];

/*初始化界面输出信息缓冲区*/
void init_info(){
	int i;
	for (i=INFO_NUM;i>0;i--){
		info[i-1].status=INFO_FREED;
	}
	sprintf(info[i-1].msg," ");	
}

/*分配一个空闲的界面输出信息缓冲区，如果没有空闲的缓冲区则返回-1*/
// 最终会获取空闲的缓冲区下标
int get_free_info(){
	int i,ret;
	/*注意对互斥锁的操作，这些操作必须是成对的（加锁和解锁），否则会发生死锁的情况*/
	pthread_mutex_lock(&info_mutex);
	for(i=0;i<INFO_NUM; i++){	// 获取当前是空闲状态的缓冲区下标ret
		if(info[i].status==INFO_FREED) {
			ret=i;
			pthread_mutex_unlock(&info_mutex);
			break;
		}
	}
	if(i==INFO_NUM) {		// 分配空间失败
		ret=-1;
		pthread_mutex_unlock(&info_mutex);
	}
	return ret;
}

/*释放界面输出信息缓冲区，对info_status的访问同样需要使用互斥保护*/
// 释放以index为下标的缓冲区
void free_info(int index){
	pthread_mutex_lock(&info_mutex);
	if(info[index].status==INFO_OCCUPIED){
		info[index].status=INFO_FREED;
	}
	pthread_mutex_unlock(&info_mutex);
}

// 为消息缓冲区添加具体的消息内容src
void add_info(char *src)
{	int i;
	// 获取空闲状态的缓冲区下标
	while((i=get_free_info())==-1){
		sleep(1);
	}
	/*添加消息*/
	pthread_mutex_lock(&info_mutex);
	info[i].status=INFO_OCCUPIED;
	strcpy(info[i].msg, src);
	pthread_mutex_unlock(&info_mutex);
}

#endif

