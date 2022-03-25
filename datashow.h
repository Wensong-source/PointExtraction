#ifndef DATASHOW_H
#define DATASHOW_H

#include <QWidget>
#include "data.h"

namespace Ui {
class DataShow;
}

class DataShow : public QWidget
{
    Q_OBJECT

public:
    explicit DataShow(QWidget *parent = nullptr);
    ~DataShow();
public slots:
    void showTable();
private:
    Ui::DataShow *ui;
    vector<double> maxTau_per;   //特征点A 每个周期的最大应力
    vector<double> maxStrain_per;//特征点A 每个周期的最大应变
    vector<double> mid_tau;
    vector<double> plasticStrian_per;   //每个周期的塑性应变
    vector<double> plasticStrian_per_denoise;

};

#endif // DATASHOW_H
