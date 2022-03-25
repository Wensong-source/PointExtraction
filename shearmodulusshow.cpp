#include "shearmodulusshow.h"
#include "ui_shearmodulusshow.h"

ShearModulusShow::ShearModulusShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShearModulusShow)
{
    ui->setupUi(this);
    ui->widget->plotLayout()->insertRow(0);
    ui->widget->plotLayout()->addElement(0,0,new QCPTextElement(ui->widget,"剪切模量随加载周期变化图",QFont("宋体",12,QFont::Bold)));
}

ShearModulusShow::~ShearModulusShow()
{
    delete ui;
}

void ShearModulusShow::plot()
{
    shearModulus_left=Data::shearModulus_left;
    shearModulus_right=Data::shearModulus_right;
    int num=shearModulus_left.size();
    ui->widget->clearGraphs();

    ui->widget->xAxis->setLabel("加载周期 N");
    ui->widget->yAxis->setLabel("剪切模量 GPa");
    ui->widget->legend->setVisible(true);

    QCPGraph *g1=new QCPGraph(ui->widget->xAxis,ui->widget->yAxis);
    g1->setPen(QPen(Qt::red));
    g1->setName("剪切模量（左侧）");

    QCPGraph *g2=new QCPGraph(ui->widget->xAxis,ui->widget->yAxis);
    g2->setName("剪切模量（右侧）");
    g2->setPen(QPen(Qt::blue));

    auto y1=shearModulus_left.begin();
    auto y2=shearModulus_right.begin();
    for(int i=0;i<num;i++){
        g1->addData(i+1,(*y1)*1e-3);
        g2->addData(i+1,(*y2)*1e-3);
        y1++,y2++;
    }
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->widget->rescaleAxes();
    ui->widget->replot();
    g1->setSmooth(true);
    g2->setSmooth(true);


}
