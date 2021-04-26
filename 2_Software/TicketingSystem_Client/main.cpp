#include "mainwindow.h"
#include "welcome.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    welcome d1;
    d1.setWindowTitle("欢迎界面");
    if(d1.exec()==QDialog::Accepted)
    {
        w.show();
    }
    return a.exec();
}
