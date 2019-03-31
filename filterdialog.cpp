#include "filterdialog.h"
#include "ui_filterdialog.h"

FilterDialog::FilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterDialog)
{
    ui->setupUi(this);

}

FilterDialog::~FilterDialog()
{
    delete ui;
}

void FilterDialog::accept()
{
    QString filter_exp = "";

    if(!ui->customize->text().isEmpty())
    {
        filter_exp = ui->customize->text();
    }
    else {
        if(ui->ifIP->isChecked())
        {
            filter_exp = "ip ";
        }
        else if(ui->ifTCP->isChecked())
        {
            filter_exp += "tcp ";
        }
        else if(ui->ifHTTP->isChecked())
        {
            filter_exp += "tcp port 80 ";
        }
        if(ui->sPortFrom->value()!=0 || ui->sPortTo->value()!=0)
        {
            if(!filter_exp.isEmpty())
                filter_exp += "and ";
            filter_exp += QString("src portrange %1-%2 ").
                    arg(ui->sPortFrom->value()).
                    arg(ui->sPortTo->value());
        }
        if(ui->dPortFrom->value()!=0 || ui->dPortTo->value()!=0)
        {
            if(!filter_exp.isEmpty())
                filter_exp += "and ";
            filter_exp += QString("dst portrange %1-%2 ").
                    arg(ui->dPortFrom->value()).
                    arg(ui->dPortTo->value());
        }
    }
    // convert QString to char*, then emit it
    QByteArray temp = filter_exp.toLocal8Bit();
    emit filter_expReturn(temp.data());
    this->disconnect();
    this->close();
}
