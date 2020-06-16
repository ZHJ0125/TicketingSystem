#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "servicethread.h"

int listen_fd, conn_fd;                 // 监听socket，连接socket
struct sockaddr_in server, cli_addr;    // 服务器地址信息，客户端地址信息
int ret, buffer_index, i;
socklen_t cli_len;
unsigned long ip_addr;
int flag = 1;
pthread_t listentid,servicetid;         // 监听线程ID,服务线程ID；
int isserveropened = false;             // 服务器端是否开启标志位

void * listen_thread(void*)
{
    char msg[512];
    while(1) {
        /* 接受远端的TCP连接请求 */
        cli_len = sizeof(cli_addr);
        conn_fd = accept(listen_fd,(struct sockaddr *)&cli_addr, &cli_len);
        if(conn_fd < 0){
            continue;
        }
        ip_addr = ntohl(cli_addr.sin_addr.s_addr);
        /* 检测重复连接 */
        //check_connection(ip_addr);
        /* 分配线程缓冲区 */
        buffer_index = get_free_buff();
        if(buffer_index < 0) {
            sprintf(msg, "没用空闲的线程缓冲区。\n");
            add_info(msg);
            close(conn_fd);
            continue;
        }
        /* 填写服务线程需要的信息 */
        pthread_mutex_lock(&buff_mutex);
        thread_buff[buffer_index].buff_index=buffer_index;
        thread_buff[buffer_index].ip_addr=ip_addr;
        thread_buff[buffer_index].conn_fd=conn_fd;
        thread_buff[buffer_index].buff_status=BUFF_OCCUPIED;
        pthread_mutex_unlock(&buff_mutex);
        /* 创建新的服务线程 */
        ret=pthread_create(&servicetid, NULL, service_thread, &thread_buff[buffer_index]);
        if(ret==-1) {
            sprintf(msg,"创建服务线程出错！\n");
            add_info(msg);
            close(conn_fd);
            /* 释放线程缓冲区 */
            pthread_mutex_lock(&buff_mutex);
            thread_buff[buffer_index].tid=0;
            thread_buff[buffer_index].buff_status=BUFF_FREED;
            pthread_mutex_unlock(&buff_mutex);
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    enable_button(isserveropened);      // 按键使能
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);   // 设置工具栏图标样式
    QTimer *timer = new QTimer();       // 设置1秒定时器
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* 更新界面缓冲区内容,每秒执行一次 */
void MainWindow::paintEvent(QPaintEvent*){
    int i;
    // qDebug() << "Here!!!";
    pthread_mutex_lock(&info_mutex);
    for(i=0;i<INFO_NUM;i++){
        if(info[i].status==INFO_OCCUPIED) {
            display_info(info[i].msg);
            info[i].status=INFO_FREED;
        }
    }
    pthread_mutex_unlock(&info_mutex);
    return;
}

/* 向界面输出msg消息 */
void MainWindow::display_info(QString msg){
    ui->textBrowser->append(msg);
}

/* 按钮使能函数 */
void MainWindow::enable_button(bool boolean){
    // 客户端操作
    ui->action_start->setEnabled(!boolean);
    ui->action_stop->setEnabled(boolean);
    // 机票查询
    ui->action_inquireone->setEnabled(boolean);
    ui->action_inquireall->setEnabled(boolean);
}

void MainWindow::on_action_start_triggered(){
    ui->textBrowser->append("Server Start...");
    char msg[512];		//提示信息
    /* 初始化数据结构 */
    init_thread_buff();
    read_ticket_list();
    if(!isserveropened){
        /* 创建套接字 */
        listen_fd=socket(AF_INET,SOCK_STREAM,0);
        if(listen_fd<0) {
            sprintf(msg,"创建监听套接字出错！ \n");
            display_info(msg);
            return;
        }
        /* 填写服务器的地址信息 */
        server.sin_family=AF_INET;
        server.sin_addr.s_addr=htonl(INADDR_ANY);
        server.sin_port=htons(SERVER_PORT_NO);
        setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,(void *)&flag,sizeof(int));
        /* 绑定端口 */
        ret=bind(listen_fd,(struct sockaddr*)&server, sizeof(server));
        if(ret<0) {
            sprintf(msg,"绑定TCP端口：%d 出错！ \n",SERVER_PORT_NO);
            display_info(msg);
            ::close(listen_fd);
            return;
        }
        /* 转化成倾听套接字 */
        listen(listen_fd,5);
        ret=pthread_create(&listentid, NULL, listen_thread, NULL);
        if(ret==-1) {
            sprintf(msg,"创建监听线程出错！ \n");
            display_info(msg);
            ::close(listen_fd);
            return;
        }
        /* 成功后输出提示信息 */
        sprintf(msg,"服务器端开启成功！服务器在端口：%d 准备接受连接！ \n",SERVER_PORT_NO);
        display_info(msg);
        isserveropened=true;
        enable_button(isserveropened);
    }
}

void MainWindow::on_action_stop_triggered(){
    ui->textBrowser->append("Server Stop...");
    char msg[512];
    if(isserveropened){
        pthread_mutex_lock(&buff_mutex);
        for(i=0;i<THREAD_BUFF_NUM;i++) {
            if(thread_buff[i].buff_status!=BUFF_FREED) {
                /* 退出服务线程 */
                pthread_cancel(thread_buff[i].tid);
                pthread_join(thread_buff[i].tid,NULL);
                /* 退出的线程不释放它的缓冲区，释放工作由主线程来处理 */
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
        enable_button(isserveropened);
    }
}

void MainWindow::on_action_exit_triggered(){
    ui->textBrowser->append("server Exit...");
    char msg[512];
    if(isserveropened) {
        ::close(listen_fd);
        sprintf(msg,"断开连接成功！\n");
        display_info(msg);
        isserveropened=false;
    }
    enable_button(isserveropened);
    close();
}

void MainWindow::on_action_inquireone_triggered(){
    ui->textBrowser->append("Inquire One...");

    QDialog dialog(this);
    QFormLayout form(&dialog);
    dialog.setWindowTitle("机票查询");
    QList<QLineEdit *> fields;
    QLineEdit *ord = new QLineEdit(&dialog);
    form.addRow(new QLabel("请输入要查询的航班号:"));
    form.addRow(ord);
    fields << ord;
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    /* 点击确认按钮 */
    if (dialog.exec() == QDialog::Accepted){
        char msg[512];
        read_ticket_list();     // 读取数据库机票信息
        update_ticket_number();
        QString flight_ord = ord->text();
        unsigned int flight_ID = flight_ord.toInt();
        if(flight_ID<=0 || flight_ID>(unsigned int)numRows) {	//判断输入的航班号是否正确，不正确的话，给出提示信息，重新输入。
            display_info("输入的航班号错误！请重新输入！");
            return;
        }
        for(i=0;i<numRows;i++) {
            pthread_mutex_lock(&ticket_list[i].ticket_mutex);
            if(ticket_list[i].flight_ID==flight_ID) {
                sprintf(msg,"你查询的航班号是：%d, 剩余票数：%d,票价：%d\n",ticket_list[i].flight_ID,ticket_list[i].ticket_num,ticket_list[i].ticket_price);
                display_info(msg);
                pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
                break;
            }
            pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
        }
    }
}

void MainWindow::on_action_inquireall_triggered(){
    ui->textBrowser->append("Inquire All...");
    int i;
    char msg[512];
    read_ticket_list();     // 读取数据库机票信息
    update_ticket_number();

    for(i=0;i<numRows;i++) {
        sprintf(msg,"航班号：%d, 剩余票数：%d, 票价：%d",ticket_list[i].flight_ID,ticket_list[i].ticket_num, ticket_list[i].ticket_price);
        display_info(msg);
    }
    display_info("\n");
}

void MainWindow::on_action_show_triggered(){
    ui->textBrowser->append("Show Message...");
    QDialog dialog(this);
    QFormLayout form(&dialog);
    dialog.setWindowTitle("帮助信息");
    form.addRow(new QLabel("<h1><center>功能说明</center></h1>"));
    form.addRow(new QLabel("开启服务器: 启动服务器程序"));
    form.addRow(new QLabel("关闭服务器: 关闭服务器程序"));
    form.addRow(new QLabel("购买机票: 购买机票"));
    form.addRow(new QLabel("特定航班查询: 查询某一特定航班机票信息"));
    form.addRow(new QLabel("所有航班查询: 查询所有航班机票信息"));
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    if (dialog.exec() == QDialog::Accepted) {
        display_info("查询信息成功\n");
    }
}

void MainWindow::on_action_about_triggered(){
    ui->textBrowser->append("Show About...");
    QDialog dialog(this);
    QFormLayout form(&dialog);
    dialog.setWindowTitle("关于");
    form.addRow(new QLabel("<h1>网络售票模拟系统服务端</h1>"));
    form.addRow(new QLabel("<center>版本 V0.2</center>"));
    form.addRow(new QLabel("本程序仅用于测试，请勿用于商业目的"));
    form.addRow(new QLabel("作者信息: 孙硕、戚莘凯、张厚今"));
    form.addRow(new QLabel("更新日期: 2020年06月16日"));
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    if (dialog.exec() == QDialog::Accepted) {
        display_info("查询信息成功\n");
    }
}
