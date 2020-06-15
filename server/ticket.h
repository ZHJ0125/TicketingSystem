/*ticket.h*/
#ifndef	__TICKET_H
#define	__TICKET_H
#include "globalapi.h"

#define	FLIGHT_NUM 10		//航班总数

/*机票的一个简单描述，flight_ID表示航班号，ticket_num表示机票剩余票数*/
typedef struct ticket_struct_t {
	int		flight_ID;		
	int		ticket_num;		
	int		ticket_price;	//票价
	/*多个线程操作时，必须对机票的剩余数量进行保护。由于这样的操作比较频繁，所以应当对每一个ticket_num使用不同的互斥锁，否则将对线程间并行性有较大影响。*/
	pthread_mutex_t	ticket_mutex;
} ticket_struct;
ticket_struct ticket_list[FLIGHT_NUM];

/*init_ticket_list:初始化ticket_list数组*/
void init_ticket_list()
{
	int i;
	for(i=0; i<FLIGHT_NUM;i++) {
		ticket_list[i].flight_ID=i+1;
		ticket_list[i].ticket_num=100;
		ticket_list[i].ticket_price=300*(i+1);
	}
}


#endif
