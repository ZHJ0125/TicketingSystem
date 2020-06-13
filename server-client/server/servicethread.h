/*servicethread.h*/
#ifndef	__SERVICE_THREAD_H
#define	__SERVICE_THREAD_H

#include "ticket.h"
#include "threadbuff.h"

/*thread_err: 服务线程的错误处理函数，由于服务器端使用的是多线程技术，服务线程发生错误时，不能像在多进程的情况下，简单地调用exit()终止进程。在多线程下，服务线程必须将使用的资源释放后，调用pthread_exit()退出，并且在需要进行线程间同步的情况下，还需要做一些线程同步的工作，才能退出。这个特点在多线程编程中是非常重要的。*/
static void thread_err(char *s, int index)
{
//	int i;
    char msg[512];
    /*获取空闲的界面输出信息缓冲区，如果没有空闲的,延迟一段时间后继续获取*/
    sprintf(msg,"线程 %d 发生致命错误：,%s\n",(unsigned short)pthread_self(),s);
    add_info(msg);
    //info_print(strmsg,serverwindow);
    /*释放线程使用的线程缓冲区*/
    free_buff(index);
    pthread_exit(NULL);
}

/*service_thread:服务线程的线程函数。服务线程根据函数的参数中获取自身使用的线程缓冲区的序号，而后根据这个序号从线程缓冲区中获取需要的参数*/
void * service_thread(void *p)
{
    int conn_fd;
    int buff_index;
    char	send_buf[1024],recv_buf[512];
    int		ret,i,cnt;
    uint16_t	nbyte;
    struct sockaddr_in	peer_name;
    socklen_t		peer_name_len;
    unsigned int	required_ticket_num;
    int		pos;
    thread_buff_struct *pstruct;
    char msg[512];
    /*获取线程使用的线程缓冲区的序号*/
    pstruct=(thread_buff_struct *)p;
    buff_index=pstruct->buff_index;
    pstruct->tid=pthread_self();

    /*从线程缓冲区中获取通信使用的套接字描述符*/
    conn_fd=pstruct->conn_fd;

    /*打印远端主机地址*/
    peer_name_len=sizeof(peer_name);
    ret=getpeername(conn_fd,(struct sockaddr*)&peer_name, &peer_name_len);
    if(ret==-1)
        thread_err("获取远端主机地址出错",buff_index);

    sprintf(msg,"新连接-->线程ID：%d, 连接ID：%d, 线程缓冲区索引号：%d, 远端地址：%s, 端口号：%d\n",(unsigned short)pstruct->tid,conn_fd, buff_index, inet_ntoa(peer_name.sin_addr), ntohs(peer_name.sin_port));
    add_info(msg);
    while(1) {
        /*从网络中获取数据记录*/
        ret=recv(conn_fd,recv_buf,sizeof(message),0);
        /*接收出错*/
        if(ret==-1) {
            sprintf(msg,"线程：%d 在连接：%d 接收出错。连接将关闭。\n",(unsigned short)pstruct->tid, conn_fd);
            add_info(msg);
            thread_err(msg, buff_index);
        }
        /*ret==0说明客户端连接已关闭*/
        if(ret==0) {
            sprintf(msg,"线程  %d  的连接( ID：%d ) 客户端已关闭。服务器端连接也将关闭。\n",(unsigned short)pstruct->tid, conn_fd);
            add_info(msg);
            close(conn_fd);
            free_buff(buff_index);
            pthread_exit(NULL);
        }

        /*ret为其他值说明接收到了客户端的请求消息*/
        init_message();
        memcpy(&message,recv_buf,sizeof(message));
        switch(message.msg_type) {
            case 	DISCONNECT:
                sprintf(msg,"线程 %d  的连接(ID： %d ) 客户端已关闭。服务器端连接也将关闭。\n",(unsigned short)pstruct->tid, conn_fd);
                add_info(msg);
                close(conn_fd);
                free_buff(buff_index);
                pthread_exit(NULL);
                break;
            case 	BUY_TICKET :
                for(i=0; i<FLIGHT_NUM; i++) {
                    pthread_mutex_lock(&ticket_list[i].ticket_mutex);
                    if(ticket_list[i].flight_ID==message.flight_ID) {
                        if(ticket_list[i].ticket_num>=message.ticket_num) {		//剩余票数大于请求票数
                            message.msg_type=BUY_SUCCEED;
                            message.ticket_total_price=message.ticket_num*ticket_list[i].ticket_price;
                            ticket_list[i].ticket_num-=message.ticket_num;
                            pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
                            sprintf(msg,"客户端 %s 购买机票成功！航班号：%d, 票数：%d, 总票价：%d\n",inet_ntoa(peer_name.sin_addr),message.flight_ID, message.ticket_num, message.ticket_total_price);
                            add_info(msg);
                            memcpy(send_buf,&message,sizeof(message));
                            ret=send(conn_fd, send_buf, sizeof(message), 0);
                            if(ret<0)
                                thread_err("发送数据出错\n", buff_index);
                            break;
                        } else {													//剩余票数不足，购买失败
                            message.msg_type=BUY_FAILED;
                            required_ticket_num=message.ticket_num;
                            message.ticket_num=ticket_list[i].ticket_num;
                            pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
                            sprintf(msg,"客户端 %s 购买机票失败！航班号：%d, 剩余票数：%d, 请求票数：%d\n",inet_ntoa(peer_name.sin_addr),message.flight_ID, message.ticket_num,required_ticket_num);
                            add_info(msg);
                            memcpy(send_buf,&message,sizeof(message));
                            ret=send(conn_fd, send_buf, sizeof(message), 0);
                            if(ret<0)
                                thread_err("发送数据出错\n", buff_index);
                            break;
                        }
                    }
                pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
            }
            break;
            case 	INQUIRE_ONE:
                for(i=0; i<FLIGHT_NUM; i++) {
                        pthread_mutex_lock(&ticket_list[i].ticket_mutex);
                        if(ticket_list[i].flight_ID==message.flight_ID) {
                                message.msg_type=INQUIRE_SUCCEED;
                                message.ticket_num=ticket_list[i].ticket_num;
                                message.ticket_total_price=ticket_list[i].ticket_price;
                                pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
                                sprintf(msg,"客户端 %s 查询航班号：%d 成功！\n",inet_ntoa(peer_name.sin_addr),message.flight_ID);
                                add_info(msg);
                                memcpy(send_buf,&message,sizeof(message));
                                ret=send(conn_fd, send_buf, sizeof(message), 0);
                                if(ret<0)
                                    thread_err("发送数据出错\n", buff_index);
                                break;
                            }
                    pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
                }
                break;
            case 	INQUIRE_ALL:
                pos=0;

                for(i=0; i<FLIGHT_NUM; i++) {
                    pthread_mutex_lock(&ticket_list[i].ticket_mutex);
                    message.msg_type=INQUIRE_SUCCEED;
                    message.flight_ID=ticket_list[i].flight_ID;
                    message.ticket_num=ticket_list[i].ticket_num;
                    message.ticket_total_price=ticket_list[i].ticket_price;
                    pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
                    memcpy(send_buf+pos,&message,sizeof(message));
                    pos+=sizeof(message);
                }
                    sprintf(msg,"客户端 %s 查询所有航班号成功！\n",inet_ntoa(peer_name.sin_addr));
                    add_info(msg);
                ret=send(conn_fd, send_buf, pos, 0);

                if(ret<0)
                    thread_err("发送数据出错\n", buff_index);
                break;
            default	:
                message.msg_type=UNKNOWN_CODE;
                memcpy(send_buf, &message,sizeof(message));
                ret=send(conn_fd, send_buf, sizeof(message), 0);
                if(ret<0)
                    thread_err("发送数据出错\n", buff_index);
        }
    }
}

#endif



