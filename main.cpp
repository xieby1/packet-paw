#include "mainwindow.h"
#include <QApplication>
#include <stdio.h>
#include <pcap/pcap.h>

int main(int argc, char *argv[])
{

    // test for pcap learning
    char *dev, errbuf[PCAP_ERRBUF_SIZE];

    dev = pcap_lookupdev(errbuf);
    if (dev == nullptr) {
        fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
        return(2);
    }
    printf("Device: %s\n", dev);
    // end test for pcap learning

//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    return a.exec();
}
