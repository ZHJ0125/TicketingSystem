#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QtPrintSupport/QPrinter>
#include <QString>
#include <cstring>
#include <string.h>
#include <strings.h>


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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
