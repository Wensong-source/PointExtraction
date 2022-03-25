#ifndef SHEARMODULUSSHOW_H
#define SHEARMODULUSSHOW_H

#include <QWidget>

#include "data.h"
#include "qcustomplot.h"

namespace Ui {
class ShearModulusShow;
}

class ShearModulusShow : public QWidget
{
    Q_OBJECT

public:
    explicit ShearModulusShow(QWidget *parent = nullptr);
    ~ShearModulusShow();
public slots:
    void plot();

private:
    Ui::ShearModulusShow *ui;
    vector<double> shearModulus_left;    //剪切模量 G左侧
    vector<double> shearModulus_right;   //剪切模量 G右侧

};

#endif // SHEARMODULUSSHOW_H
