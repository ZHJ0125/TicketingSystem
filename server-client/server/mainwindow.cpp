#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>

#include "servicethread.h"

int listen_fd, conn_fd; 	//监听socket，连接socket
struct sockaddr_in server, cli_addr;			//服务器地址信息，客户端地址信息
int ret, buffer_index, i;
socklen_t cli_len;
unsigned long ip_addr;
int flag=1;
pthread_t listentid,servicetid;						//监听线程ID,服务线程ID；



void MainWindow::canServer(bool b) {
    ui->actionStart_server->setEnabled(!b);
    ui->actionClose_server->setEnabled(b);
    ui->actionQuire_fights->setEnabled(b);
    ui->actionQuire_all_fights->setEnabled(b);
}


void * listen_thread(void *p)
{
    char msg[512];
    while(1) {
        /*接受远端的TCP连接请求*/
        cli_len=sizeof(cli_addr);
        conn_fd=accept(listen_fd,(struct sockaddr *)&cli_addr, &cli_len);
        if(conn_fd<0)
            continue;

        ip_addr=ntohl(cli_addr.sin_addr.s_addr);
        /*检测重复连接*/
        //check_connection(ip_addr);
        /*分配线程缓冲区*/
        buffer_index=get_free_buff();
        if(buffer_index<0) {
            sprintf(msg,"没用空闲的线程缓冲区。\n");
            add_info(msg);
            close(conn_fd);
            continue;
        }
        /*填写服务线程需要的信息*/
        pthread_mutex_lock(&buff_mutex);
        thread_buff[buffer_index].buff_index=buffer_index;
        thread_buff[buffer_index].ip_addr=ip_addr;
        thread_buff[buffer_index].conn_fd=conn_fd;
        thread_buff[buffer_index].buff_status=BUFF_OCCUPIED;
        pthread_mutex_unlock(&buff_mutex);

        /*创建新的服务线程*/
        ret=pthread_create(&servicetid, NULL, service_thread, &thread_buff[buffer_index]);
        if(ret==-1) {
            sprintf(msg,"创建服务线程出错！\n");
            add_info(msg);
            close(conn_fd);
            /*释放线程缓冲区*/
            pthread_mutex_lock(&buff_mutex);
            thread_buff[buffer_index].tid=0;
            thread_buff[buffer_index].buff_status=BUFF_FREED;
            pthread_mutex_unlock(&buff_mutex);
        }
    }
}

int isserveropened=false;		//服务器端是否开启标志位

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    canServer(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionStart_server_triggered()
{
    char msg[512];		//提示信息

    /*初始化数据结构*/
    init_thread_buff();
    init_ticket_list();

    if(!isserveropened)
    {
        /*创建套接字*/
        listen_fd=socket(AF_INET,SOCK_STREAM,0);
        if(listen_fd<0) {
            sprintf(msg,"创建监听套接字出错！ \n");
            display_info(msg);
            return;
        }

        /*填写服务器的地址信息*/
        server.sin_family=AF_INET;
        server.sin_addr.s_addr=htonl(INADDR_ANY);
        server.sin_port=htons(SERVER_PORT_NO);

        setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,(void *)&flag,sizeof(int));

        /*绑定端口*/
        ret=bind(listen_fd,(struct sockaddr*)&server, sizeof(server));
        if(ret<0) {
            sprintf(msg,"绑定TCP端口：%d 出错！ \n",SERVER_PORT_NO);
            display_info(msg);
            ::close(listen_fd);
            return;
        }

        /*转化成倾听套接字*/
        listen(listen_fd,5);
        ret=pthread_create(&listentid, NULL, listen_thread, NULL);
        if(ret==-1) {
            sprintf(msg,"创建监听线程出错！ \n");
            display_info(msg);
            ::close(listen_fd);
            return;
        }

        sprintf(msg,"服务器端开启成功！服务器在端口：%d 准备接受连接！ \n",SERVER_PORT_NO);
        display_info(msg);
        isserveropened=true;
    }
    canServer(true);
}

void MainWindow::display_info(QString str) {
    QMessageBox::warning(this,
                                tr("Waring"),
                                str,
                                QMessageBox::Yes);

}

void MainWindow::on_actionClose_server_triggered()
{
    char msg[512];
    if(isserveropened)
    {

        pthread_mutex_lock(&buff_mutex);
        for(i=0;i<THREAD_BUFF_NUM;i++) {
            if(thread_buff[i].buff_status!=BUFF_FREED) {
                /*退出服务线程*/
                pthread_cancel(thread_buff[i].tid);
                pthread_join(thread_buff[i].tid,NULL);
                /*退出的线程不释放它的缓冲区，释放工作由主线程来处理*/
                thread_buff[i].tid=0;
                thread_buff[i].buff_status=BUFF_FREED;
                ::close(thread_buff[i].conn_fd);
            }
        }

        pthread_mutex_unlock(&buff_mutex);
        pthread_cancel(listentid);
        pthread_join(listentid,NULL);
        ::close(listen_fd);
        sprintf(msg,"服务器端成功关闭！ \n");
        display_info(msg);
        isserveropened=false;
    }
    canServer(false);
}

void MainWindow::on_actionQuire_all_fights_triggered()
{
    int i;
    char msg[512];
    for(i=0;i<FLIGHT_NUM;i++) {
        sprintf(msg,"航班号：%d, 剩余票数：%d, 票价：%d\n",ticket_list[i].flight_ID,ticket_list[i].ticket_num, ticket_list[i].ticket_price);
//        display_info(msg);
        data_show(msg);
    }
}

void MainWindow::on_actionQuire_fights_triggered()
{
    int	flight_ID;
    char msg[512];

    QString ticket = QInputDialog::getText(NULL, "机票查询",
                                           "请输入要查询的航班号(1-9):",
                                           QLineEdit::Normal,
                                           "",
                                           0);
   flight_ID = ticket.toInt();

   if (flight_ID <= 0 || flight_ID > 10) {	//判断输入的航班号是否正确，不正确的话，给出提示信息，重新输入。
           display_info("输入的航班号错误！请重新输入！");
           return;
   }

   for(i=0;i<FLIGHT_NUM;i++) {
               pthread_mutex_lock(&ticket_list[i].ticket_mutex);
               if(ticket_list[i].flight_ID==flight_ID) {
                   sprintf(msg,"你查询的航班号是：%d, 剩余票数：%d,票价：%d\n",ticket_list[i].flight_ID,ticket_list[i].ticket_num,ticket_list[i].ticket_price);
//                   display_info(msg);
                   data_show(msg);

                   pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
                   break;
               }
               pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
    }

}

void MainWindow::data_show(QString str) {
    ui->display->append(str);
}



void MainWindow::on_actionExit_triggered()
{
    close();
    qDebug() << "exit" << endl;
}
