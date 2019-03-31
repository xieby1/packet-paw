#ifndef PAWTHREAD_H
#define PAWTHREAD_H

#include <QThread>
#include <pcap.h>
#include <time.h>

class pawThread : public QThread
{
    Q_OBJECT
public:
    explicit pawThread(QObject *parent = nullptr);
    ~pawThread();

    pcap_t *handle;			/* Session handle */
    char *dev;			/* The device to sniff on */
    char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
    struct bpf_program fp;		/* The compiled filter */
#define FILTER_EXP_LEN 100
    char filter_exp[FILTER_EXP_LEN] = "";	/* The filter expression */
    bpf_u_int32 mask;		/* Our netmask */
    bpf_u_int32 net;		/* Our IP */
    struct pcap_pkthdr header;	/* The header that pcap gives us */
    const u_char *packet;		/* The actual packet */

    int packetsCount;
    int finishedPacketsCount;
    int lastPacketsCount;
    clock_t lastRefreshTime; // time unit ms
#define REFRESH_INTERVAL 500

//    pcap_pkthdr** headerArray;  // array size = ...
    u_char** packetArray;       // ... packetsCount
    int* lengthArray;

    void init(int packet_count);

signals:
    void finishedPacketsCountReturn(int, int);

public slots:

protected:
    void run();
    static void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
};

#endif // PAWTHREAD_H
