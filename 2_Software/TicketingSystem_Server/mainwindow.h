#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QFormLayout>
#include <QList>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QTimer>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void display_info(QString msg);
    void enable_button(bool boolean);

private slots:
    void on_action_start_triggered();
    void on_action_stop_triggered();
    void on_action_exit_triggered();
    void on_action_inquireone_triggered();
    void on_action_inquireall_triggered();
    void on_action_show_triggered();
    void on_action_about_triggered();

private:
    Ui::MainWindow *ui;

protected:
    void paintEvent(QPaintEvent *event); //添加重绘事件
};
#endif // MAINWINDOW_H
