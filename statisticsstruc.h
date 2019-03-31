#ifndef STATISTICSSTRUC_H
#define STATISTICSSTRUC_H


struct st_network // networl layer
{
    int ipv4, ipv6, arp, rarp;
    int other;
};
struct st_transport // transport layer
{
    int icmp, tcp, udp;
    int other;
};
struct st_application // application layer
{
    int http, ftp, ssh;
    int other;
};

#endif // STATISTICSSTRUC_H
