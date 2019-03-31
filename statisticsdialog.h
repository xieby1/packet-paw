#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include "statisticsstruc.h"

namespace Ui {
class StatisticsDialog;
}

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(QWidget *parent = nullptr);
    ~StatisticsDialog();
    void setStatistics(st_network* st_nt, st_transport* st_trsp, st_application* st_app);

private:
    Ui::StatisticsDialog *ui;
};

#endif // STATISTICSDIALOG_H
