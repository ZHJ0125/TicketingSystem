#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>

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
    void on_action_connect_triggered();

    void on_action_disconnect_triggered();

    void on_action_buyticket_triggered();

    void on_action_exit_triggered();

    void on_action_inquireone_triggered();

    void on_action_inquireall_triggered();

    void on_action_show_triggered();

    void on_action_about_triggered();

    void on_action_add_triggered();

    void on_action_update_triggered();

    void on_action_delete_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
