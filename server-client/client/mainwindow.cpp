#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "globalapi.h"  // h file



int socket_fd; 	//连接socket
struct sockaddr_in server;			//服务器地址信息，客户端地址信息
int ret, i;
int flag = 1;


int isconnected = false;		//是否已连接服务器


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

void MainWindow::canServer(bool b) {
    ui->actionStart_server->setEnabled(!b);
    ui->actionClose_server->setEnabled(b);
    ui->actionQuire_fights->setEnabled(b);
    ui->actionQuire_all_fights->setEnabled(b);
    ui->action_buy_tickets->setEnabled(b);
}

/* server connevt */
void MainWindow::on_actionStart_server_triggered()
{

    char msg[512];		//提示信息

    if (!isconnected) {
        /*创建套接字*/
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {

            display_info("创建套接字出错！");
            return;
        }

        /*设置接收、发送超时值*/
        struct timeval time_out;
        time_out.tv_sec = 5;
        time_out.tv_usec = 0;
        setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &time_out, sizeof(time_out));

        /*填写服务器的地址信息*/
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr("127.0.0.1"); //htonl(INADDR_ANY);
        server.sin_port = htons(SERVER_PORT_NO);

        /*连接服务器*/
        ret = ::connect(socket_fd, (struct sockaddr*)&server, sizeof(server));

        if(ret < 0) {
            sprintf(msg,"连接服务器出错！\n", SERVER_PORT_NO);
            display_info(msg);
            ::close(socket_fd);
            return;
        }

        //成功后输出提示信息
        sprintf(msg,"连接服务器成功！\n");

        isconnected = true;
    }
    canServer(true);
    display_info(msg);



    qDebug() << "yes" << endl;
}

/* server close */
void MainWindow::on_actionClose_server_triggered()
{
    if (isconnected) {
        ::close(socket_fd);

        display_info("断开连接成功！");
        isconnected = false;

    } else
        display_info("未连接!");
    canServer(false);
    qDebug() << "no" << endl;
}

/* query all */
void MainWindow::on_actionQuire_all_fights_triggered()
{


//    display_info("error test!");


    int i, pos;
    char msg[512];
    char send_buf[512], recv_buf[512];

    init_message();
    message.msg_type=INQUIRE_ALL;
    memcpy(send_buf, &message, sizeof(message));
    int ret = send(socket_fd, send_buf, sizeof(message), 0);	// 接收查询的k返回信息到 message 中， ret 代表返回值用于判断

    /*发送出错*/
    if (ret == -1) {
        display_info("发送失败！请重新发送！");
        return ;
    }
    ret = recv(socket_fd, recv_buf, sizeof(recv_buf), 0);
    if(ret == -1) {

        display_info("接收失败！请重新发送！");
        return ;
    }
    pos = 0;
    sprintf(msg, "查询所有航班结果：\n");
    data_show(msg);
    qDebug() << ret << ' ' <<  sizeof(message) << endl;
    for (i = 0; i < ret; i = i + sizeof(message)) {
        memcpy(&message, recv_buf + pos, sizeof(message));
        if (message.msg_type == INQUIRE_SUCCEED)
            sprintf(msg,"查询成功！航班号：%d, 剩余票数：%d, 票价：%d\n", message.flight_ID, message.ticket_num, message.ticket_total_price);
        else
            sprintf(msg,"查询失败！航班号：%d, 剩余票数：未知\n", message.flight_ID);
        data_show(msg);
        pos += sizeof(message);
    }



    qDebug() << "query all" << endl;
}


void MainWindow::on_actionQuire_fights_triggered()
{
    QString ticket = QInputDialog::getText(NULL, "机票查询",
                                           "请输入要查询的航班号(1-9):",
                                           QLineEdit::Normal,
                                           "",
                                           0);

    char msg[512];
    char send_buf[512],recv_buf[512];
            int flight_ID = ticket.toInt();
            if(flight_ID<=0 || flight_ID>10) {	//判断输入的航班号是否正确，不正确的话，给出提示信息，重新输入。
                display_info("输入的航班号错误！请重新输入！");
                return ;
            }
            init_message();
            message.msg_type = INQUIRE_ONE;
            message.flight_ID = flight_ID;
            memcpy(send_buf,&message,sizeof(message));
            int ret=send(socket_fd, send_buf,sizeof(message),0);
            /*发送出错*/
            if(ret==-1) {
                display_info("发送失败！请重新发送！");
                return ;
            }
            ret=recv(socket_fd,recv_buf,sizeof(message),0);
            if(ret==-1) {
                display_info("接收失败！请重新连接服务器！");
                return ;
            }
            memcpy(&message,recv_buf,sizeof(message));
            sprintf(msg, "查询 %d 号航班的结果为：\n\n", flight_ID);
            if(message.msg_type==INQUIRE_SUCCEED)
                sprintf(msg,"%s查询成功！航班号：%d, 剩余票数：%d, 票价：%d\n", msg, message.flight_ID,message.ticket_num, message.ticket_total_price);
            else
                sprintf(msg,"查询失败！航班号：%d, 剩余票数：未知\n",message.flight_ID);
            data_show(msg);

}




void MainWindow::on_actionExit_triggered()
{

    qDebug() << "exit" << endl;
    close();
}

void MainWindow::on_action_buy_tickets_triggered()
{
    qDebug() << "buy tickets" << endl;

    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);
    dialog.setWindowTitle("机票购买");

    QList<QLineEdit *> fields;
    QLineEdit *ord = new QLineEdit(&dialog);
    form.addRow(new QLabel("请输入要购买的航班号(1-10):"));
    form.addRow(ord);

    QLineEdit *cnt = new QLineEdit(&dialog);
    form.addRow(new QLabel("请输入要购买票的张数:"));
    form.addRow(cnt);

    fields << ord;
    fields << cnt;

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {

        QString flight_ord = ord->text();
        QString flight_cnt = fields[1]->text();
        /*获取输入的航班号*/

        char msg[512];
        char send_buf[512],recv_buf[512];

        int flight_ID = flight_ord.toInt();
        int ticket_num= flight_cnt.toInt();

        if(flight_ID<=0 || flight_ID>10) {	//判断输入的航班号是否正确，不正确的话，给出提示信息，重新输入。
            display_info("输入的航班号错误！请重新输入！");
            return ;
        }

        if(ticket_num<=0) {	//判断输入的票数是否正确，不正确的话，给出提示信息，重新输入。

            display_info("输入的票数错误！请重新输入！");
            return;
        }
        /*购买机票*/



        init_message();
        message.msg_type=BUY_TICKET;
        message.flight_ID=flight_ID;
        message.ticket_num=ticket_num;
        memcpy(send_buf,&message,sizeof(message));
        int ret=send(socket_fd, send_buf,sizeof(message),0);
        /*发送出错*/
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
        if(message.msg_type==BUY_SUCCEED)
            sprintf(msg,"购买成功！航班号：%d, 票数：%d, 总票价：%d\n",message.flight_ID,message.ticket_num, message.ticket_total_price);
        else
            sprintf(msg,"购买失败！航班号：%d, 剩余票数：%d, 请求票数：%d\n",message.flight_ID,message.ticket_num,ticket_num);
        data_show(msg);


    }

}

void MainWindow::display_info(QString str) {
    QMessageBox::warning(this,
                                tr("Waring"),
                                str,
                                QMessageBox::Yes);

}

void MainWindow::data_show(QString str) {
    ui->display->append(str);
}



