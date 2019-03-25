#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pawthread.h"

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
    void grabPackets();
    void showCapturedPackets(int, int);

private:
    Ui::MainWindow *ui;
    pawThread* pawthread;
};

#endif // MAINWINDOW_H
