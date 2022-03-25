#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QFileDialog>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QVector>
#include <QTime>
#include "math.h"
#include "qcustomplot.h"
#include "data.h"
#include "comparisondata.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"

using namespace  std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void featurePointExtraction();
    bool saveText(const QString &fileName,const vector<double> &v);
    double abs(double x);
    void maxStrainPer(vector<double> v,vector<double> t);  //生成A点数据
    void minStrainPer(vector<double> v,vector<double> t);  //生成B点数据
    void PlasticStrianPer(vector<double> s,vector<double> t); //生成C,D,E,F点数据
    vector<double> Line_Fitting(vector<double> s,vector<double> t); //最小二乘法拟合弹性卸载范围直线并求剪切模量G
    double maxNum(double a,double b);
    double minNum(double a,double b);
    double maxNum(const vector<double> &v);
    double minNum(const vector<double> &v);
    void plot();
    void closeEvent(QCloseEvent *event);
    void dataProcess();
    void denoise(vector<double> &v);
    void denoise2(vector<double> &v);
    bool inputData();
    vector<double> maxMinTAper(vector<double> &t,vector<double> &a);
    void saveData();

signals:
    void comparisonDataShow();
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_radioButton_jieti_clicked();

    void on_radioButton_hengfu_clicked();

private:
    int load = 70000;    //软件最大处理寿命数
    Ui::MainWindow *ui;
    ComparisonData *c;
    double pi=3.1415926;
    double dia=6;                //试棒直径  mm
    double length=16;            //试棒长度  mm
    bool isLadder = true;       //判断为阶梯加载还是横幅加载
    QString selectDir;
    vector<double> angle_per;    //角度单位为弧度
    vector<double> torque_per;   //扭矩 单位为N.m
    vector<double> tau_per;      //应力  单位MPa
    vector<double> strain_per;   //应变
    vector<vector<double> > angle;
    vector<vector<double> > torque;
    vector<vector<double> > tau;
    vector<vector<double> > strain;
    vector<vector<double> > &tau_mid = Data::tau_mid;    //对中后的应力
    vector<vector<double> > &strain_mid= Data::strain_mid; //对中后的应变
    vector<double> &maxTorquePer = Data::maxTorquePer ;   //对中前每个周期最大扭矩
    vector<double> &minTorquePer = Data::minTorquePer;
    vector<double> &maxAnglePer = Data::maxAnglePer;
    vector<double> &minAnglePer = Data::minAnglePer;
    vector<double> &mid_tau=Data::mid_tau;
    vector<double> &maxTau_per=Data::maxTau_per;   //特征点A 每个周期的最大应力
    vector<double> &maxStrain_per=Data::maxStrain_per;//特征点A 每个周期的最大应变
    vector<double> &minTau_Per=Data::minTau_Per;   //特征点B 每个周期的最小应力
    vector<double> &minStrain_per=Data::minStrain_per;//特征点B 每个周期的最小应变
    vector<double> &negtivePlasticStrian_per=Data::negtivePlasticStrian_per;   //特征点C 每个周期的反向塑性应变
    vector<double> &postivePlasticStrain_per=Data::postivePlasticStrain_per;   //特征点D 每个周期的正向塑性应变
    vector<double> &negtiveInterTau_per=Data::negtiveInterTau_per;        //特征点E 每个周期的负向截距应力
    vector<double> &postiveInterTau_Per=Data::postiveInterTau_Per;        //特征点F 每个周期的正向截距应力
    vector<int> &indexA=Data::indexA;
    vector<int> &indexB=Data::indexB;
    vector<int> indexC;
    vector<int> indexD;
    vector<int> &indexE=Data::indexE;
    vector<int> &indexF=Data::indexF;
    vector<double> &shearModulus_left = Data::shearModulus_left;    //剪切模量 G左侧
    vector<double> &shearModulus_right = Data::shearModulus_right;   //剪切模量 G右侧
    vector<double> shearModulus;         //剪切模量 左右平均值
    vector<double> &plasticStrain=Data::plasticStrian_per;
    vector<double> elasticStrain;
    vector<double> &damage=Data::damage;
    vector<double> &Pd=Data::Pd;
    vector<double> &elasticStrainEnerge=Data::elasticStrainEnergy;
    int flagNum=0;

};
#endif // MAINWINDOW_H
