/*ticket.h*/
#ifndef	__TICKET_H
#define	__TICKET_H
#include "globalapi.h"

#define	FLIGHT_NUM 10		//航班总数
int numRows = 0;

/* 机票的一个简单描述 */
typedef struct ticket_struct_t {
    unsigned int flight_ID;     // 航班号
    unsigned int ticket_num;    // 机票剩余票数
    unsigned int ticket_price;	// 票价
    // 多个线程操作时，必须对机票的剩余数量进行保护
    // 应当对每一个ticket_num使用不同的互斥锁
    // 否则将对线程间并行性有较大影响
	pthread_mutex_t	ticket_mutex;
} ticket_struct;
ticket_struct ticket_list[FLIGHT_NUM];

/* read_ticket_list:初始化ticket_list数组 */
void read_ticket_list(){
    MYSQL mysql;
    MYSQL_RES * result;
    MYSQL_ROW row;
    int i = 0;
    mysql_init(&mysql);
    mysql_real_connect(&mysql, "localhost", "zhj", "666588", "linux", 0, NULL, 0);
    mysql_query(&mysql, "select * from tickets"); // 调用mysql_store_result之前必须检索数据库
    result = mysql_store_result(&mysql);          // 将查询的全部结果读取到客户端
    numRows = mysql_num_rows(result);             // 统计结果集的行数
    if(result){
        for(i=0;i<numRows;i++){
            if((row = mysql_fetch_row(result)) != NULL){
                ticket_list[i].flight_ID = atoi(row[0]);
                ticket_list[i].ticket_num = atoi(row[1]);
                ticket_list[i].ticket_price = atoi(row[2]);
            }
        }
    }
    mysql_free_result(result);      // 释放result空间，避免内存泄漏
    mysql_close(&mysql);

    return;
}

void update_ticket_number(void){
    MYSQL mysql;
    MYSQL_RES * result;
    mysql_init(&mysql);
    mysql_real_connect(&mysql, "localhost", "zhj", "666588", "linux", 0, NULL, 0);
    mysql_query(&mysql, "select * from tickets");
    result = mysql_store_result(&mysql);//将查询的全部结果读取到客户端
    numRows = mysql_num_rows(result);   //统计结果集的行数
    mysql_free_result(result);
    mysql_close(&mysql);
}

#endif
