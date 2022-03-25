#ifndef COMPARISONDATA_H
#define COMPARISONDATA_H

#include <QWidget>
#include "data.h"
#include "shearmodulusshow.h"
#include "datashow.h"

namespace Ui {
class ComparisonData;
}

class ComparisonData : public QWidget
{
    Q_OBJECT

public:
    explicit ComparisonData(QWidget *parent = nullptr);
    ~ComparisonData();
    double maxNum(double a,double b);
    double minNum(double a,double b);
    double abs(double x);
    int crackJudge(vector<double> v);   //判断断裂点
    void denoise(vector<double> &v);
    bool saveText(const QString &fileName,const vector<double> &v);
    vector<double> smooth(vector<double> v);
    void closeEvent(QCloseEvent *event);
signals:
    void shearPlot();
    void dataShow();
public slots:
    void showData();

private slots:
    void on_refresh_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    bool on_pushButton_3_clicked();

private:
    Ui::ComparisonData *ui;
    ShearModulusShow *s;
    DataShow *d;
    double pi=3.1415926;
    double dia=6;                //试棒直径  mm
    double length=16;            //试棒长度  mm
    const vector<double> &maxTorquePer = Data::maxTau_per ;   //对中前每个周期最大扭矩
    const vector<double> &minTorquePer = Data::minTorquePer;
    const vector<double> &maxAnglePer = Data::maxAnglePer;
    const vector<double> &minAnglePer = Data::minAnglePer;
    const vector<double> &mid_tau=Data::mid_tau;
    const vector<double> &maxTau_per=Data::maxTau_per;   //特征点A 每个周期的最大应力
    const vector<double> &maxStrain_per=Data::maxStrain_per;//特征点A 每个周期的最大应变
    const vector<double> &minTau_Per=Data::minTau_Per;   //特征点B 每个周期的最小应力
    const vector<double> &minStrain_per=Data::minStrain_per;//特征点B 每个周期的最小应变
    const vector<double> &negtivePlasticStrian_per=Data::negtivePlasticStrian_per;   //特征点C 每个周期的反向塑性应变
    const vector<double> &postivePlasticStrain_per=Data::postivePlasticStrain_per;   //特征点D 每个周期的正向塑性应变
    const vector<double> &negtiveInterTau_per=Data::negtiveInterTau_per;        //特征点E 每个周期的负向截距应力
    const vector<double> &postiveInterTau_Per=Data::postiveInterTau_Per;        //特征点F 每个周期的正向截距应力
    const vector<double> &shearModulus_left = Data::shearModulus_left;    //剪切模量 G左侧
    const vector<double> &shearModulus_right = Data::shearModulus_right;   //剪切模量 G右侧
    const vector<vector<double> > &tau_mid = Data::tau_mid;    //对中后的应力
    const vector<vector<double> > &strain_mid= Data::strain_mid; //对中后的应变
    const vector<double> &plasticStrian_per=Data::plasticStrian_per;   //每个周期的塑性应变
    vector<double> &plasticStrain=Data::plasticStrian_per;
    vector<double> plasticStrian_per_denoise;

    const vector<int> &indexA=Data::indexA;
    const vector<int> &indexB=Data::indexB;
    const vector<int> &indexE=Data::indexE;
    const vector<int> &indexF=Data::indexF;

};

#endif // COMPARISONDATA_H
