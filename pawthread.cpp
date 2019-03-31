#include "pawthread.h"


pawThread::pawThread(QObject *parent)
{
    /* Define the device */
    dev = pcap_lookupdev(errbuf);
    if (dev == nullptr) {
        fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
        return;
    }
}

pawThread::~pawThread()
{
    pcap_close(handle);
    if(lengthArray)
        delete []lengthArray;
    if(packetArray)
        delete []packetArray;
}

void pawThread::init(int packet_count)
{
    lengthArray = nullptr;
    packetArray = nullptr;
    packetsCount = packet_count;
    finishedPacketsCount = 0;
    lastRefreshTime = 0;
    lastPacketsCount = 0;
    if(lengthArray)
        delete []lengthArray;
    if(packetArray)
        delete []packetArray;
//    headerArray = new pcap_pkthdr*[packet_count];
    packetArray = new u_char*[packet_count];
    lengthArray = new int[packet_count];
}

void pawThread::got_packet(u_char *args, const pcap_pkthdr *header, const u_char *packet)
{
    pawThread* pawthread = reinterpret_cast<pawThread*>(args);
//    pawthread->headerArray[pawthread->finishedPacketsCount] = (pcap_pkthdr *)header;
    pawthread->packetArray[pawthread->finishedPacketsCount] = (u_char *)packet;
    pawthread->lengthArray[pawthread->finishedPacketsCount] = header->caplen;

    pawthread->finishedPacketsCount++;
    // if last refresh interval is longer enough, then do a refresh
    if(clock() - pawthread->lastRefreshTime > REFRESH_INTERVAL
            || pawthread->finishedPacketsCount==pawthread->packetsCount)
    {
        pawthread->lastRefreshTime = clock();
        emit pawthread->finishedPacketsCountReturn(pawthread->lastPacketsCount, pawthread->finishedPacketsCount);
        pawthread->lastPacketsCount = pawthread->finishedPacketsCount;
    }
}

void pawThread::run()
{
    /* Find the properties for the device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
        net = 0;
        mask = 0;
    }
    /* Open the session in promiscuous mode */
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return;
    }
    /* Compile and apply the filter */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return;
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return;
    }

    // grab packets by loop
    pcap_loop(handle, packetsCount, got_packet, (u_char*)this);
    // end of loop
}

