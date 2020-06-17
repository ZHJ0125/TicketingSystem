#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "globalapi.h"
#include "ticket.h"

int ret = 0;                    // 连接状态返回值
int socket_fd;                  // 套接字描述符
struct sockaddr_in server;		// 服务器地址信息，客户端地址信息
int isconnected = false;		// 是否已连接服务器

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    ui->setupUi(this);
    enable_button(isconnected);
    // 设置工具栏图标样式
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::display_info(QString msg){
    ui->textBrowser->append(msg);
}

void MainWindow::enable_button(bool boolean){
    // 客户端操作
    ui->action_connect->setEnabled(!boolean);
    ui->action_disconnect->setEnabled(boolean);
    ui->action_buyticket->setEnabled(boolean);
    // 机票查询
    ui->action_inquireone->setEnabled(boolean);
    ui->action_inquireall->setEnabled(boolean);
    // 管理员操作
    ui->action_add->setEnabled(boolean);
    ui->action_update->setEnabled(boolean);
    ui->action_delete->setEnabled(boolean);
}

void MainWindow::on_action_connect_triggered(){
    ui->textBrowser->append("Connecting Server....\n");
    char msg[512];		//提示信息

    if(!isconnected){
        /* 创建套接字 */
        socket_fd=socket(AF_INET,SOCK_STREAM,0);
        if(socket_fd<0) {
            sprintf(msg,"创建套接字出错！ \n");
            display_info(msg);
            return;
        }
        /* 设置接收、发送超时值 */
        struct timeval time_out;
        time_out.tv_sec=5;
        time_out.tv_usec=0;
        setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &time_out, sizeof(time_out));

        /* 填写服务器的地址信息 */
        server.sin_family=AF_INET;
        server.sin_addr.s_addr=inet_addr("127.0.0.1");//htonl(INADDR_ANY);
        server.sin_port=htons(SERVER_PORT_NO);

        /* 连接服务器 */
        ret = ::connect(socket_fd,(struct sockaddr*)&server, sizeof(server));
        if(ret<0) {
            sprintf(msg,"连接服务器出错！%d\n",SERVER_PORT_NO);	// 这里改了一下，添加了格式控制符%d
            display_info(msg);
            ::close(socket_fd);
            return;
        }
        /* 成功后输出提示信息 */
        sprintf(msg,"连接服务器成功！\n");
        display_info(msg);
        isconnected=true;
        enable_button(isconnected);
    }
}

void MainWindow::on_action_disconnect_triggered(){
    ui->textBrowser->append("Disonnect Server....\n");
    char msg[512];
    if(isconnected) {
        ::close(socket_fd);
        sprintf(msg,"断开连接成功！\n");
        display_info(msg);
        isconnected=false;
    }
    enable_button(isconnected);
}

void MainWindow::on_action_buyticket_triggered(){
    ui->textBrowser->append("Buy Ticket....\n");
//    char* ticket_num_temp = nullptr;
    QDialog dialog(this);
    QFormLayout form(&dialog);
    dialog.setWindowTitle("机票购买");
    QList<QLineEdit *> fields;
    QLineEdit *ord = new QLineEdit(&dialog);
//    update_ticket_number();
//    sprintf(ticket_num_temp, "请输入要查询的航班号(1-%d):", numRows);
//    form.addRow(new QLabel(ticket_num_temp));
    form.addRow(new QLabel("请输入要查询的航班号:"));
    form.addRow(ord);
    QLineEdit *cnt = new QLineEdit(&dialog);
    form.addRow(new QLabel("请输入要购买票的张数:"));
    form.addRow(cnt);
    fields << ord;
    fields << cnt;
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    /* 点击确认按钮 */
    if (dialog.exec() == QDialog::Accepted) {
        QString flight_ord = ord->text();
        QString flight_cnt = cnt->text();
        /* 获取输入的航班号 */
        char msg[512];
        char send_buf[512],recv_buf[512];
        int flight_ID = flight_ord.toInt();
        int ticket_num= flight_cnt.toInt();
        update_ticket_number();
        /* 判断输入的航班号是否正确，不正确的话，给出提示信息重新输入 */
        if(flight_ID<=0 || flight_ID>numRows) {
            display_info("输入的航班号错误！请重新输入！");
            return;
        }
        /* 判断输入的票数是否正确，不正确的话，给出提示信息，重新输入 */
        if(ticket_num<=0) {
            display_info("输入的票数错误！请重新输入！");
            return;
        }
        /* 购买机票 */
        init_message();
        message.msg_type=BUY_TICKET;
        message.flight_ID=flight_ID;
        message.ticket_num=ticket_num;
        memcpy(send_buf,&message,sizeof(message));
        int ret=send(socket_fd, send_buf,sizeof(message),0);
        /* 发送出错 */
        if(ret == -1) {
            display_info("发送失败！请重新发送！");
            return ;
        }
        ret = recv(socket_fd,recv_buf,sizeof(message),0);
        if(ret==-1) {
            display_info("接收失败！请重新发送！");
            return ;
        }
        memcpy(&message,recv_buf,sizeof(message));
        if(message.msg_type==BUY_SUCCEED){
            sprintf(msg,"购买成功！航班号：%d, 票数：%d, 总票价：%d\n",message.flight_ID,message.ticket_num, message.ticket_total_price);
        }
        else{
            sprintf(msg,"购买失败！航班号：%d, 剩余票数：%d, 请求票数：%d\n",message.flight_ID,message.ticket_num,ticket_num);
        }
        display_info(msg);
    }
}

void MainWindow::on_action_exit_triggered(){
    char msg[512];
    ui->textBrowser->append("Exit\n");
    while(isconnected){
        ::close(socket_fd);
        sprintf(msg,"断开连接成功！\n");
        display_info(msg);
        isconnected=false;
    }
    display_info("即将关闭客户端");
    close();
}

void MainWindow::on_action_inquireone_triggered(){
    ui->textBrowser->append("Inquire One\n");

//    char* ticket_num_temp = nullptr;
    QDialog dialog(this);
    QFormLayout form(&dialog);
    dialog.setWindowTitle("机票查询");
    QList<QLineEdit *> fields;
    QLineEdit *ord = new QLineEdit(&dialog);
//    update_ticket_number();
//    sprintf(ticket_num_temp, "请输入要查询的航班号(1-%d):", numRows);
//    form.addRow(new QLabel(QString(ticket_num_temp)));
    form.addRow(new QLabel("请输入要查询的航班号:"));
    form.addRow(ord);
    fields << ord;
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    /* 点击确认按钮 */
    if (dialog.exec() == QDialog::Accepted) {
        QString flight_ord = ord->text();
        /* 获取输入的航班号 */
        char msg[512];
        char send_buf[512],recv_buf[512];
        int flight_ID = flight_ord.toInt();
        update_ticket_number();
        if(flight_ID<=0 || flight_ID>numRows) {	// 判断输入的航班号是否正确，不正确的话，给出提示信息，重新输入。
            display_info("输入的航班号错误！请重新输入！");
            return;
        }
        init_message();
        message.msg_type=INQUIRE_ONE;
        message.flight_ID=flight_ID;
        memcpy(send_buf,&message,sizeof(message));
        int ret=send(socket_fd, send_buf,sizeof(message),0);
        /* 发送出错 */
        if(ret==-1) {
            display_info("发送失败！请重新发送！");
            return ;
        }
        ret=recv(socket_fd,recv_buf,sizeof(message),0);
        if(ret==-1) {
            display_info("接收失败！请重新连接服务器！\n");
            return ;
        }
        memcpy(&message,recv_buf,sizeof(message));
        if(message.msg_type==INQUIRE_SUCCEED){
            sprintf(msg,"查询成功！航班号：%d, 剩余票数：%d, 票价：%d\n",message.flight_ID,message.ticket_num, message.ticket_total_price);
        }
        else{
            sprintf(msg,"查询失败！航班号：%d, 剩余票数：未知\n",message.flight_ID);
        }
        display_info(msg);
    }
}

void MainWindow::on_action_inquireall_triggered(){
    ui->textBrowser->append("Inquire All\n");
    int i,pos;
    char msg[512];
    char send_buf[1024], recv_buf[1024];
    init_message();
    message.msg_type=INQUIRE_ALL;
    memcpy(send_buf,&message,sizeof(message));
    int ret=send(socket_fd, send_buf,sizeof(message),0);
    /* 发送出错 */
    if(ret==-1) {
        display_info("发送失败！请重新发送！");
        return;
    }
    ret=recv(socket_fd,recv_buf,sizeof(recv_buf),0);
    if(ret==-1) {
        display_info("接收失败！请重新发送！");
        return;
    }
    pos=0;
    sprintf(msg,"查询所有航班结果：\n");
    display_info(msg);
    for (i=0;i<ret;i=i+sizeof(message)) {
        memcpy(&message,recv_buf+pos,sizeof(message));
        if(message.msg_type==INQUIRE_SUCCEED){
            sprintf(msg,"查询成功！航班号：%d, 剩余票数：%d, 票价：%d",message.flight_ID,message.ticket_num, message.ticket_total_price);
        }
        else{
            sprintf(msg,"查询失败！航班号：%d, 剩余票数：未知",message.flight_ID);
        }
        display_info(msg);
        pos+=sizeof(message);
    }
    display_info("\n");
}

void MainWindow::on_action_show_triggered(){
    ui->textBrowser->append("Show User Manual....\n");

    QDialog dialog(this);
    QFormLayout form(&dialog);
    dialog.setWindowTitle("帮助信息");
    form.addRow(new QLabel("<h1><center>功能说明</center></h1>"));
    form.addRow(new QLabel("连接服务器: 与远程服务器建立连接"));
    form.addRow(new QLabel("断开连接：断开与远程服务器的连接"));
    form.addRow(new QLabel("购买机票：购买机票"));
    form.addRow(new QLabel("特定航班查询：查询某一特定航班机票信息"));
    form.addRow(new QLabel("所有航班查询：查询所有航班机票信息"));
    form.addRow(new QLabel("增加航班信息：增加指定的航班机票信息"));
    form.addRow(new QLabel("更新航班信息：更新指定的航班机票信息"));
    form.addRow(new QLabel("删除航班信息：删除指定的航班机票信息"));
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    if (dialog.exec() == QDialog::Accepted) {
        display_info("查询信息成功\n");
    }
}

void MainWindow::on_action_about_triggered(){
    ui->textBrowser->append("About\n");
    QDialog dialog(this);
    QFormLayout form(&dialog);
    dialog.setWindowTitle("关于");
    form.addRow(new QLabel("<h1>网络售票模拟系统管理端</h1>"));
    form.addRow(new QLabel("<center>版本 V0.3</center>"));
    form.addRow(new QLabel("本程序仅用于测试，请勿用于商业目的"));
    form.addRow(new QLabel("作者信息: 孙硕、戚莘凯、张厚今"));
    form.addRow(new QLabel("更新日期: 2020年06月17日"));
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    if (dialog.exec() == QDialog::Accepted) {
        display_info("查询信息成功\n");
    }
}

void MainWindow::on_action_add_triggered()
{
    ui->textBrowser->append("Add Information...");

    QDialog dialog(this);
    QFormLayout form(&dialog);
    dialog.setWindowTitle("增加航班信息(管理员)");
    QList<QLineEdit *> fields;
    QLineEdit *id = new QLineEdit(&dialog);
    form.addRow(new QLabel("请输入该航班的航班号:"));
    form.addRow(id);
    QLineEdit *number = new QLineEdit(&dialog);
    form.addRow(new QLabel("请输入该航班的票数:"));
    form.addRow(number);
    QLineEdit *price = new QLineEdit(&dialog);
    form.addRow(new QLabel("请输入该航班的票价:"));
    form.addRow(price);
    fields << id;
    fields << number;
    fields << price;
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        int flight_ID = id->text().toInt();
        int ticket_num = number->text().toInt();
        int ticket_price = price->text().toInt();
        char send_buf[1024];
        init_message();
        message.msg_type=ADD_TICKET;
        message.flight_ID = flight_ID;
        message.ticket_num = ticket_num;
        message.ticket_total_price = ticket_price;
        memcpy(send_buf,&message,sizeof(message));

        int ret=send(socket_fd, send_buf,sizeof(message),0);
        /* 发送出错 */
        if(ret==-1) {
            display_info("发送失败！请重新发送！");
            return;
        }
        display_info("增加航班信息成功!\n");
    }
}

void MainWindow::on_action_update_triggered()
{
    ui->textBrowser->append("Update Information...");

    QDialog dialog(this);
    QFormLayout form(&dialog);
    dialog.setWindowTitle("更新航班信息(管理员)");
    QList<QLineEdit *> fields;
    QLineEdit *id = new QLineEdit(&dialog);
    form.addRow(new QLabel("请输入该航班的航班号:"));
    form.addRow(id);
    QLineEdit *number = new QLineEdit(&dialog);
    form.addRow(new QLabel("请输入该航班的票数:"));
    form.addRow(number);
    QLineEdit *price = new QLineEdit(&dialog);
    form.addRow(new QLabel("请输入该航班的票价:"));
    form.addRow(price);
    fields << id;
    fields << number;
    fields << price;
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        int flight_ID = id->text().toInt();
        int ticket_num = number->text().toInt();
        int ticket_price = price->text().toInt();

        char send_buf[1024];
        init_message();
        message.msg_type=UPDATE_TICKET;
        message.flight_ID = flight_ID;
        message.ticket_num = ticket_num;
        message.ticket_total_price = ticket_price;
        memcpy(send_buf,&message,sizeof(message));

        int ret=send(socket_fd, send_buf,sizeof(message),0);
        /* 发送出错 */
        if(ret==-1) {
            display_info("发送失败！请重新发送！");
            return;
        }
        display_info("更新航班信息成功!\n");
    }

}

void MainWindow::on_action_delete_triggered()
{
    ui->textBrowser->append("Delete Information...");

    QDialog dialog(this);
    QFormLayout form(&dialog);
    dialog.setWindowTitle("删除航班信息(管理员)");
    QList<QLineEdit *> fields;
    QLineEdit *id = new QLineEdit(&dialog);
    form.addRow(new QLabel("请输入该航班的航班号:"));
    form.addRow(id);
    fields << id;
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        int flight_ID = id->text().toInt();
        char send_buf[1024];
        init_message();
        message.msg_type=DELETE_TICKET;
        message.flight_ID = flight_ID;
        memcpy(send_buf,&message,sizeof(message));

        int ret=send(socket_fd, send_buf,sizeof(message),0);
        /* 发送出错 */
        if(ret==-1) {
            display_info("发送失败！请重新发送！");
            return;
        }
        display_info("删除航班信息成功!\n");
    }
}
