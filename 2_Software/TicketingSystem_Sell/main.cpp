#include "mainwindow.h"
#include "login.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Login d1;
    d1.setWindowTitle("登录界面");
    if(d1.exec()==QDialog::Accepted)
    {
        w.show();
    }
    return a.exec();
}
