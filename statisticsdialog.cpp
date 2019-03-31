#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"

StatisticsDialog::StatisticsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatisticsDialog)
{
    ui->setupUi(this);
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

void StatisticsDialog::setStatistics(st_network *st_nt, st_transport *st_trsp, st_application *st_app)
{
    double sum = st_nt->arp + st_nt->ipv4 + st_nt->ipv6 + st_nt->rarp + st_nt->other;
    sum /= 100; // percentage
    ui->ipv4->setValue(st_nt->ipv4 / sum);
    ui->ipv6->setValue(st_nt->ipv6 / sum);
    ui->arp->setValue(st_nt->arp / sum);
    ui->rarp->setValue(st_nt->rarp / sum);
    ui->nt_others->setValue(st_nt->other / sum);

    ui->icmp->setValue(st_trsp->icmp / sum);
    ui->tcp->setValue(st_trsp->tcp / sum);
    ui->udp->setValue(st_trsp->udp / sum);
    ui->trsp_others->setValue(st_trsp->other / sum);

    ui->http->setValue(st_app->http / sum);
    ui->ftp->setValue(st_app->ftp / sum);
    ui->ssh->setValue(st_app->ssh / sum);
    ui->app_others->setValue(st_app->other / sum);
}
