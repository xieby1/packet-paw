#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include "pawthread.h"
#include "filterdialog.h"
#include "statisticsdialog.h"
#include "statisticsstruc.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showCapturedPackets(int, int);
    void on_actionFilter_triggered();
    void setFilter(char *);

    void on_actionClear_triggered();

    void on_actionStart_triggered();

    void on_actionStatistics_triggered();

private:
    Ui::MainWindow *ui;
    pawThread* pawthread;
    FilterDialog* filterDialog;
    // function list //
    // data link layer
    void ethernetHandler(u_char* payload);
    // network layer
    void ipv4Handler(u_char* payload, QTreeWidgetItem* parent);
    // transport layer
    void tcpHandler(u_char* payload, QTreeWidgetItem* parent, unsigned int fullLength);
    // application layer
    void httpHandler(u_char* payload, QTreeWidgetItem* parent, unsigned int fullLength);
    // statistics of recieved packets
    st_network st_nt;
    st_transport st_trsp;
    st_application st_app;
    void initStatistic();
};

#endif // MAINWINDOW_H
