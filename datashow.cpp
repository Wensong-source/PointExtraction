#include "datashow.h"
#include "ui_datashow.h"

DataShow::DataShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataShow)
{
    ui->setupUi(this);
    ui->plot_strain->plotLayout()->insertRow(0);
    ui->plot_strain->plotLayout()->addElement(0,0,new QCPTextElement(ui->plot_strain,"应变随加载周期变化图",QFont("宋体",12,QFont::Bold)));
    ui->plot_Tau->plotLayout()->insertRow(0);
    ui->plot_Tau->plotLayout()->addElement(0,0,new QCPTextElement(ui->plot_Tau,"应力随加载周期变化图",QFont("宋体",12,QFont::Bold)));
}

DataShow::~DataShow()
{
    delete ui;
}

void DataShow::showTable()
{
    //载入数据
    maxStrain_per=Data::maxStrain_per;//特征点A 每个周期的最大应变
    //plasticStrian_per_denoise=Data::plasticStrian_per_denoise;
    plasticStrian_per=Data::plasticStrian_per;
    maxTau_per=Data::maxTau_per;
    mid_tau=Data::mid_tau;
    int loadCycles=maxStrain_per.size();

    //清除graph
    ui->plot_strain->clearGraphs();
    ui->plot_Tau->clearGraphs();

    //绘制最大应变随加载周期变化图
    ui->plot_strain->legend->setVisible(true);
    ui->plot_strain->xAxis->setLabel("加载周期 N");
    ui->plot_strain->yAxis->setLabel("应变");


    ui->plot_strain->addGraph();
    ui->plot_strain->graph(0)->setPen(QPen(Qt::red));
    ui->plot_strain->graph(0)->setName("总应变");

    ui->plot_strain->addGraph();
    ui->plot_strain->graph(1)->setPen(QPen(Qt::blue));
    ui->plot_strain->graph(1)->setName("塑性应变");
    ui->plot_strain->graph(1)->setSmooth(true);

    ui->plot_strain->addGraph();
    ui->plot_strain->graph(2)->setPen(QPen(Qt::black));
    ui->plot_strain->graph(2)->setName("弹性应变");
    ui->plot_strain->graph(2)->setSmooth(true);

//    ui->plot_strain->addGraph();
//    ui->plot_strain->graph(3)->setPen(QPen(Qt::green));
//    ui->plot_strain->graph(3)->setName("塑性应变-平滑前");


    auto y1=maxStrain_per.begin();
    auto y2=plasticStrian_per.begin();
 //   auto y2=plasticStrian_per_denoise.begin();
  //  auto y3=plasticStrian_per.begin();
    for(int i=0;i<loadCycles;i++){
        ui->plot_strain->graph(0)->addData(i,*y1);
        ui->plot_strain->graph(1)->addData(i,*y2);
        ui->plot_strain->graph(2)->addData(i,*y1-*y2);
        //ui->plot_strain->graph(3)->addData(i,*y3);
        y1++,y2++;
    }
    ui->plot_strain->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->plot_strain->rescaleAxes();
    ui->plot_strain->replot();

    //绘制应力随加载周期变化图
    //ui->plot_Tau->plotLayout()->clear();
    ui->plot_Tau->legend->setVisible(true);
    ui->plot_Tau->xAxis->setLabel("加载周期 N");
    ui->plot_Tau->yAxis->setLabel("应力");

    ui->plot_Tau->addGraph();
    ui->plot_Tau->graph(0)->setPen(QPen(Qt::red));
    ui->plot_Tau->graph(0)->setName("最大应力");

    ui->plot_Tau->addGraph();
    ui->plot_Tau->graph(1)->setPen(QPen(Qt::blue));
    ui->plot_Tau->graph(1)->setName("偏移应力");

    auto z1=maxTau_per.begin();
    auto z2=mid_tau.begin();
    for(int i=0;i<loadCycles;i++){
        ui->plot_Tau->graph(0)->addData(i,*z1);
        ui->plot_Tau->graph(1)->addData(i,*z2);
        z1++,z2++;
    }
    ui->plot_Tau->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->plot_Tau->rescaleAxes();
    ui->plot_Tau->replot();




}
