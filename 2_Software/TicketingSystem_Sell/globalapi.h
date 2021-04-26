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
#define	DISCONNECT	0
#define	BUY_TICKET 	1
#define	INQUIRE_ONE	2
#define	INQUIRE_ALL	3
#define ADD_TICKET  4
#define UPDATE_TICKET 5
#define DELETE_TICKET 6

/*服务器端使用的消息代码含义*/
#define	BUY_SUCCEED		255
#define	BUY_FAILED		256
#define	INQUIRE_SUCCEED	257
#define	UNKNOWN_CODE	258

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
    unsigned int msg_type;          // 用来向服务器请求不同类型的信息
    unsigned int flight_ID;         // 航班号
    unsigned int ticket_num;        // 机票张数
    unsigned int ticket_total_price;// 机票价钱
} message;

/* 将消息数据类型进行初始化 */
void init_message(){
	message.msg_type=INITIAL_VALUE;
	message.flight_ID=0;
	message.ticket_num=0;
	message.ticket_total_price=0;
}

#endif

