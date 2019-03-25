#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QWheelEvent>
#include "packetheaders.h"
#include <QByteArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pawthread = new pawThread();
    ui->availableDevices->addItem(pawthread->dev);
    connect(pawthread, &pawThread::finishedPacketsCountReturn, this, &MainWindow::showCapturedPackets);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::grabPackets()
{
    ui->grabPackets->setEnabled(false);
    pawthread->init(ui->packetsCount->value());
    pawthread->start();
}

void MainWindow::showCapturedPackets(int from, int to)
{
    for(int i=from; i<to; i++)
    {
        u_char* packet = pawthread->packetArray[i];
        QByteArray rawData((char *)packet, pawthread->lengthArray[i]);
        ui->packetRaw->append(rawData.toHex());

        /* ethernet headers are always exactly 14 bytes */
        #define SIZE_ETHERNET 14

        const struct sniff_ethernet *ethernet; /* The ethernet header */
        const struct sniff_ip *ip; /* The IP header */
        const struct sniff_tcp *tcp; /* The TCP header */
        const char *payload; /* Packet payload */

        u_int size_ip;
        u_int size_tcp;

        ethernet = (struct sniff_ethernet*)(packet);
        ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
        size_ip = IP_HL(ip)*4;
        if (size_ip < 20) {
            printf("   * Invalid IP header length: %u bytes\n", size_ip);
            return;
        }
        tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
        size_tcp = TH_OFF(tcp)*4;
        if (size_tcp < 20) {
            printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
            return;
        }
        payload = (char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);

        QTreeWidgetItem* etherItem = new QTreeWidgetItem(QStringList( QObject::tr("ether net") ));
        QString ipAddr = QObject::tr("s=%1.%2.%3.%4 d=%5.%6.%7.%8").
                arg(ip->ip_src.addr1).arg(ip->ip_src.addr2).arg(ip->ip_src.addr3).arg(ip->ip_src.addr4).
                arg(ip->ip_dst.addr1).arg(ip->ip_dst.addr2).arg(ip->ip_dst.addr3).arg(ip->ip_dst.addr4);
        QTreeWidgetItem* ipItem = new QTreeWidgetItem(etherItem, QStringList({"ip", ipAddr}));
        QTreeWidgetItem* tcpItem = new QTreeWidgetItem(ipItem, QStringList({"tcp", payload}));
        ui->packetDetails->addTopLevelItem(etherItem);
    }
    if(to==pawthread->packetsCount)
    {
        ui->grabPackets->setEnabled(true);
    }
}
