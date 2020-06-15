/*threadbuff.h*/
#ifndef	__THREAD_BUFF_H
#define	__THREAD_BUFF_H

#include "globalapi.h"

/*定义线程缓冲区的使用状态*/
#define		BUFF_OCCUPIED	1
#define		BUFF_FREED		0

/*线程缓冲区结构*/

typedef struct thread_buff_struct_t {
	/*线程缓冲区的索引号*/
	int buff_index;
	/*保存对应线程的线程号*/
	int 	tid;
	/*保存对应的客户机的IP地址*/
	unsigned long ip_addr;
	/*该线程使用的连接套接字描述符*/
	int	conn_fd;
	/*线程缓冲区的状态*/
	int	buff_status;
} thread_buff_struct;

thread_buff_struct thread_buff[THREAD_BUFF_NUM];
/* 用于线程缓冲区互斥使用的互斥锁*/
/*由于当主线程分配线程缓冲区时需要检测buff_status变量的值，而服务线程在退出前，需要将它使用的线程缓冲区释放，所谓释放就是需要修改buff_status变量的值，所以主线程和服务线程间需要对buff_status进行互斥，可以为每一个buff_status变量设置一个互斥锁，但这样需要较多的系统资源。这里只使用了一个互斥锁来对结构数组中的所有buff_status变量进行互斥保护。*/
pthread_mutex_t buff_mutex;

/*初始化线程缓冲区*/
void init_thread_buff()
{
	int index;
	for(index=0; index<THREAD_BUFF_NUM;index++) {
		thread_buff[index].tid=-1;
		thread_buff[index].buff_status=BUFF_FREED;
	}
}

/*分配一个空闲的线程缓冲区，如果没有空闲的缓冲区则返回－1*/
int get_free_buff()
{
	int i,ret;
	/*注意对互斥锁的操作，这些操作必须是成对的（加锁和解锁），否则会发生死锁的情况*/
	pthread_mutex_lock(&buff_mutex);
	for(i=0;i<THREAD_BUFF_NUM; i++)
		if(thread_buff[i].buff_status==BUFF_FREED) {
			ret=i;
			pthread_mutex_unlock(&buff_mutex);
			break;
		}
	if(i==THREAD_BUFF_NUM) {
		ret=-1;
		pthread_mutex_unlock(&buff_mutex);
	}
	return ret;
}

/*释放线程缓冲区，对buff_status的访问同样需要使用互斥保护*/
void free_buff(int index)
{
	pthread_mutex_lock(&buff_mutex);
	if(thread_buff[index].buff_status==BUFF_OCCUPIED)
		thread_buff[index].buff_status=BUFF_FREED;
	pthread_mutex_unlock(&buff_mutex);
}

/*检查线程缓冲区中是否有重复连接，因为可能客户端的通信进程终止后重新启动，此时应当终止原来它所对应的服务线程，再重新创建一个服务线程，并为这个新的服务线程分配线程缓冲区。*/
void check_connection(unsigned long ip_addr)
{
	int i;
	struct in_addr in;
	char msg[512];
	/*检查所有的线程缓冲区*/
	pthread_mutex_lock(&buff_mutex);
	for(i=0;i<THREAD_BUFF_NUM;i++) {
		/*发现重复连接*/
		if((thread_buff[i].buff_status!=BUFF_FREED)&&thread_buff[i].ip_addr==ip_addr) {
			in.s_addr=htonl(ip_addr);
			sprintf(msg,"重复连接：%s，旧连接将关闭！\n",inet_ntoa(in));
			add_info(msg);
			pthread_cancel(thread_buff[i].tid);

			pthread_join(thread_buff[i].tid,NULL);
			/*退出的线程不释放它的缓冲区，释放工作由主线程来处理*/
			thread_buff[i].tid=0;
			thread_buff[i].buff_status=BUFF_FREED;
			close(thread_buff[i].conn_fd);
		}
	}
	pthread_mutex_unlock(&buff_mutex);
}

#endif
