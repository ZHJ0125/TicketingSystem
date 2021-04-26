#include "login.h"
#include "ui_login.h"
#include<QMessageBox>

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButton_clicked()
{
    if((ui->lineEdit->text()=="qxk"&&ui->lineEdit_2->text()=="qixinkai")||\
        (ui->lineEdit->text()=="ss"&&ui->lineEdit_2->text()=="sunshuo")||\
        (ui->lineEdit->text()=="zhj"&&ui->lineEdit_2->text()=="zhanghoujin")){
        accept();
    }
    else{
        QMessageBox::warning(this,tr("warning"),tr("user name or password error!"),QMessageBox::Yes);
    }
}
