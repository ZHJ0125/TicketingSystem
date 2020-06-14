/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionStart_server;
    QAction *actionClose_server;
    QAction *actionQuire_all_fights;
    QAction *actionQuire_fights;
    QAction *actionExit;
    QAction *action_buy_tickets;
    QWidget *centralWidget;
    QTextEdit *display;
    QMenuBar *menuBar;
    QMenu *menuOperator;
    QMenu *menuInquire;
    QMenu *menuHelp;
    QStatusBar *statusBar;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(693, 379);
        actionStart_server = new QAction(MainWindow);
        actionStart_server->setObjectName(QString::fromUtf8("actionStart_server"));
        actionClose_server = new QAction(MainWindow);
        actionClose_server->setObjectName(QString::fromUtf8("actionClose_server"));
        actionQuire_all_fights = new QAction(MainWindow);
        actionQuire_all_fights->setObjectName(QString::fromUtf8("actionQuire_all_fights"));
        actionQuire_fights = new QAction(MainWindow);
        actionQuire_fights->setObjectName(QString::fromUtf8("actionQuire_fights"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        action_buy_tickets = new QAction(MainWindow);
        action_buy_tickets->setObjectName(QString::fromUtf8("action_buy_tickets"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        display = new QTextEdit(centralWidget);
        display->setObjectName(QString::fromUtf8("display"));
        display->setGeometry(QRect(20, 10, 641, 301));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 693, 31));
        menuOperator = new QMenu(menuBar);
        menuOperator->setObjectName(QString::fromUtf8("menuOperator"));
        menuInquire = new QMenu(menuBar);
        menuInquire->setObjectName(QString::fromUtf8("menuInquire"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);

        menuBar->addAction(menuOperator->menuAction());
        menuBar->addAction(menuInquire->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuOperator->addAction(actionStart_server);
        menuOperator->addAction(actionClose_server);
        menuOperator->addAction(actionExit);
        menuInquire->addAction(actionQuire_all_fights);
        menuInquire->addAction(actionQuire_fights);
        menuInquire->addAction(action_buy_tickets);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\345\256\242\346\210\267\347\253\257", nullptr));
        actionStart_server->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\346\234\215\345\212\241\345\231\250", nullptr));
        actionClose_server->setText(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200\346\234\215\345\212\241", nullptr));
        actionQuire_all_fights->setText(QCoreApplication::translate("MainWindow", "\346\237\245\350\257\242\345\205\250\351\203\250\346\234\272\347\245\250", nullptr));
        actionQuire_fights->setText(QCoreApplication::translate("MainWindow", "\346\214\207\345\256\232\346\237\245\350\257\242\346\234\272\347\245\250", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272", nullptr));
        action_buy_tickets->setText(QCoreApplication::translate("MainWindow", "\346\234\272\347\245\250\350\264\255\344\271\260", nullptr));
        display->setDocumentTitle(QString());
        menuOperator->setTitle(QCoreApplication::translate("MainWindow", "\345\256\242\346\210\267\347\253\257\346\223\215\344\275\234(&S)", nullptr));
        menuInquire->setTitle(QCoreApplication::translate("MainWindow", "\346\211\200\346\234\211\346\234\215\345\212\241(&Q)", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "\345\270\256\345\212\251(&H)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
