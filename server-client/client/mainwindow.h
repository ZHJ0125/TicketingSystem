#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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


#include <QMainWindow>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QtPrintSupport/QPrinter>
#include <QString>
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QLabel>
#include <QDialogButtonBox>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void display_info(QString str);
    void data_show(QString str);
    void canServer(bool b);

private slots:
    void on_actionStart_server_triggered();

    void on_actionClose_server_triggered();

    void on_actionQuire_all_fights_triggered();

    void on_actionQuire_fights_triggered();

    void on_actionExit_triggered();

    void on_action_buy_tickets_triggered();

//    void on_action_dialog_tickets_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
