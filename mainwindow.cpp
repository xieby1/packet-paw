#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QWheelEvent>
#include "packetheaders.h"
#include <QByteArray>
#include <arpa/inet.h>
#include <string.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pawthread = new pawThread();
    ui->availableDevices->addItem(pawthread->dev);
    connect(pawthread, &pawThread::finishedPacketsCountReturn, this, &MainWindow::showCapturedPackets);
    initStatistic();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showCapturedPackets(int from, int to)
{
    for(int i=from; i<to; i++)
    {
        u_char* packet = pawthread->packetArray[i];
        QByteArray rawData((char *)packet, pawthread->lengthArray[i]);
        ui->packetRaw->append(rawData.toHex());

        ethernetHandler(packet);
    }
    ui->statusBar->showMessage(QObject::tr("Packet paw is sniffing ... finished %1/%2").arg(to).arg(pawthread->packetsCount), 1000);
    ui->progressBar->setValue(to*100/pawthread->packetsCount);
    if(to==pawthread->packetsCount)
    {
        ui->actionStart->setEnabled(true);
    }
}

void MainWindow::ethernetHandler(u_char *payload)
{
    /* ethernet headers are always exactly 14 bytes */
    #define SIZE_ETHERNET 14

    const struct sniff_ethernet *ethernet; /* The ethernet header */
    ethernet = (struct sniff_ethernet*)(payload);

    u_char* nextLevelPayload = payload + SIZE_ETHERNET;

    // create tree view entry and add it to main window
    QTreeWidgetItem* etherItem = new QTreeWidgetItem(QStringList( QObject::tr("Ether net") ));
    ui->packetDetails->addTopLevelItem(etherItem);

    // add subtree
    // source MAC address and destination MAC address
    QString sMACAddr = QString("");
    for(int i=0; i<ETHER_ADDR_LEN; i++)
    {
        // arg(value, fieldwidth, base)
        sMACAddr += QString("%1").arg(ethernet->ether_shost[i], 0, 16);
        if(i!=ETHER_ADDR_LEN-1)
            sMACAddr += ":";
    }
    QStringList sMAC = QStringList({QObject::tr("Source MAC"), sMACAddr});
    new QTreeWidgetItem(etherItem, sMAC);

    QString dMACAddr = "";
    for(int i=0; i<ETHER_ADDR_LEN; i++)
    {
        dMACAddr += QString("%1").arg(ethernet->ether_dhost[i], 0, 16);
        if(i!=ETHER_ADDR_LEN-1)
            dMACAddr += ":";
    }
    QStringList dMAC = QStringList({QObject::tr("Destination MAC"), dMACAddr});
    new QTreeWidgetItem(etherItem, dMAC);
    // subtree adding end here

    switch (ntohs(ethernet->ether_type)) {
    case ETHER_TYPE_ARP:
        st_nt.arp++;
        new QTreeWidgetItem(etherItem ,QStringList(QObject::tr("ARP")));
        break;
    case ETHER_TYPE_RARP:
        st_nt.rarp++;
        new QTreeWidgetItem(etherItem ,QStringList(QObject::tr("RARP")));
        break;
    case ETHER_TYPE_IPV6:
        st_nt.ipv6++;
        new QTreeWidgetItem(etherItem ,QStringList(QObject::tr("IPv6")));
        break;
    case ETHER_TYPE_IPV4:
        st_nt.ipv4++;
        ipv4Handler(nextLevelPayload, etherItem);
        break;
    default:
        st_nt.other++;
        break;
    }
}

void MainWindow::ipv4Handler(u_char *payload, QTreeWidgetItem* parent)
{
    const struct sniff_ip *ip = (struct sniff_ip*)payload; /* The IP header */
    u_int size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }
    u_char* nextLevelPayload = payload + size_ip;

    // create sub-tree view entry and add it to its parent
    QTreeWidgetItem* ipItem = new QTreeWidgetItem(parent, QStringList(QObject::tr("IPv4")));
    // subtree adding
    // source IP address
    new QTreeWidgetItem(ipItem, QStringList({QObject::tr("Source IP"), inet_ntoa(ip->ip_src)}));
    // destination IP address
    new QTreeWidgetItem(ipItem, QStringList({QObject::tr("Destination IP"), inet_ntoa(ip->ip_dst)}));
    // end of subtree adding

    switch (ip->ip_p) {
    case IP_TYPE_ICMP:
        st_trsp.icmp++;
        new QTreeWidgetItem(ipItem ,QStringList(QObject::tr("ICMP")));
        break;
    case IP_TYPE_TCP:
        st_trsp.tcp++;
        tcpHandler(nextLevelPayload, ipItem, ip->ip_len-size_ip);
        break;
    case IP_TYPE_UDP:
        st_trsp.udp++;
        new QTreeWidgetItem(ipItem ,QStringList(QObject::tr("UDP")));
        break;
    default:
        st_trsp.other++;
        break;
    }
}

void MainWindow::tcpHandler(u_char *payload, QTreeWidgetItem* parent, unsigned int fullLength)
{
    const struct sniff_tcp *tcp = (struct sniff_tcp*)payload; /* The TCP header */
    u_int size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
        printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
        return;
    }
    u_char* nextLevelPayload = payload + size_tcp;

    QTreeWidgetItem* tcpItem = new QTreeWidgetItem(parent, QStringList(QObject::tr("TCP")));
    // subtree adding

    new QTreeWidgetItem(tcpItem, QStringList({QObject::tr("Source port"), QString("%1").arg((unsigned int)ntohs(tcp->th_sport))}));
    new QTreeWidgetItem(tcpItem, QStringList({QObject::tr("Destination port"), QString("%1").arg((unsigned int)ntohs(tcp->th_dport))}));
    // ending of subtree adding


    switch ((unsigned int)ntohs(tcp->th_dport)) {
    case 80:
        st_app.http++;
        httpHandler(nextLevelPayload, tcpItem, fullLength-size_tcp);
        break;
    case 21:
        st_app.ftp++;
        new QTreeWidgetItem(tcpItem ,QStringList(QObject::tr("FTP")));
        break;
    case 22:
        st_app.ssh++;
        new QTreeWidgetItem(tcpItem ,QStringList(QObject::tr("SSH/SCP")));
        break;
    default:
        st_app.other++;
        break;
    }
}

void MainWindow::httpHandler(u_char *payload, QTreeWidgetItem *parent, unsigned int fullLength)
{
    QTreeWidgetItem* tcpItem = new QTreeWidgetItem(parent, QStringList({"HTTP", QString((QChar*)payload, fullLength)}));
}

void MainWindow::initStatistic()
{
    st_nt = {0,0,0,0,0};
    st_trsp = {0,0,0,0};
    st_app = {0,0,0,0};
}

void MainWindow::on_actionFilter_triggered()
{
    filterDialog = new FilterDialog(this);
    filterDialog->setModal(true);
    filterDialog->show();
    connect(filterDialog, &FilterDialog::filter_expReturn, this, &MainWindow::setFilter);
}

void MainWindow::setFilter(char * fe)
{
    strcpy(pawthread->filter_exp,fe);
}

void MainWindow::on_actionClear_triggered()
{
    ui->packetDetails->clear();
    ui->packetRaw->clear();
    initStatistic();
}

void MainWindow::on_actionStart_triggered()
{
    ui->actionStart->setEnabled(false);
    pawthread->init(ui->packetsCount->value());
    pawthread->start();
}

void MainWindow::on_actionStatistics_triggered()
{
    StatisticsDialog* statisticsDialog = new StatisticsDialog(this);
    statisticsDialog->setStatistics(&st_nt, &st_trsp, &st_app);
    statisticsDialog->show();
}
