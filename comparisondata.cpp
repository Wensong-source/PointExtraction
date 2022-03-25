#include "comparisondata.h"
#include "ui_comparisondata.h"

ComparisonData::ComparisonData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComparisonData)
{
    ui->setupUi(this);
    ui->plot_orignal->plotLayout()->insertRow(0);
    ui->plot_orignal->plotLayout()->addElement(0,0,new QCPTextElement(ui->plot_orignal,"滞后环曲线",QFont("宋体",12,QFont::Bold)));
    ui->plot_processing->plotLayout()->insertRow(0);
    ui->plot_processing->plotLayout()->addElement(0,0,new QCPTextElement(ui->plot_processing,"拟合数据绘制的滞后环",QFont("宋体",12,QFont::Bold)));
    s = new ShearModulusShow;
    d=new DataShow;
    connect(this,&ComparisonData::shearPlot,s,&ShearModulusShow::plot);
    connect(this,&ComparisonData::dataShow,d,&DataShow::showTable);
}

ComparisonData::~ComparisonData()
{
    delete ui;
}

double ComparisonData::maxNum(double a, double b)
{
    return a>b?a:b;
}

double ComparisonData::minNum(double a, double b)
{
    return a<b?a:b;
}

double ComparisonData::abs(double x)
{
    if(x<0) return -x;
    else return  x;
}

int ComparisonData::crackJudge(vector<double> v)
{
    int n=v.size();
    for(int i=n-1;i>0;i--){
        if((v[i-1]-v[i]<1&&v[i-2]-v[i-1]<1)) return i;
    }
    return -1;
}

void ComparisonData::denoise( vector<double> &v)
{
    int num=v.size();
    int count=num/15;
    double sum=0,average,variance=0;
    for(int j=0;j<count;j++){
        sum+=v[j];
    }
    average=sum/count;
    for(int j=0;j<count;j++){
        variance+=(v[j]-average)*(v[j]-average);
    }
    variance=variance/count;
    double std=sqrt(variance);
    for(int i=0;i<num*0.9;i++){
        if(i<count){
            if((v[i]<(average-3*std)||(v[i]>(average+3*std)))){
                v[i]=average;
            }
        }
        else{
            variance=0;
            sum=sum-v[i-count]+v[i];
            average=sum/count;
            for(int j=(i-count+1);j<=i;j++){
                variance+=(v[j]-average)*(v[j]-average);
            }
            variance=variance/count;
            std=sqrt(variance);
            if((v[i]<(average-3*std)||(v[i]>(average+3*std)))){
                v[i]=average;
            }
        }
    }
}

bool ComparisonData::saveText(const QString &fileName,const vector<double> &v)
{
    QFile afile(fileName);
    if(!afile.open(QIODevice::WriteOnly|QIODevice::Text)){
        return false;
    }
    QTextStream aStream(&afile);
    aStream.setAutoDetectUnicode(true);
    int num=v.size();
    for(int i=0;i<num;i++){
        QString str=QString::number(v[i]);
        aStream<<str<<'\n';
    }
    afile.close();
    return true;
}

vector<double> ComparisonData::smooth(vector<double> v)
{
    int num=v.size();
    vector<double> r(num);
    r[0]=v[0];
    r[1]=(v[0]+v[1]+v[2])/3;
    for(int i=2;i<num-2;i++){
        r[i]=(v[i-2]+v[i-1]+v[i]+v[i+1]+v[i+2])/5;
    }
    r[num-2]=(v[num-3]+v[num-2]+v[num-1])/3;
    r[num-1]=v[num-1];
    return r;

}

void ComparisonData::closeEvent(QCloseEvent *event)
{
    s->close();
    //delete s;
    d->close();
    //delete d;

}

void ComparisonData::showData()
{
    //判断断裂点
//    crackPoint=crackJudge(maxTau_per);
//    if(crackPoint==-1){
//        ui->label_crack->setText("判断错误");
//    }
//    else{
//        ui->label_crack->setText("出现裂纹加载周期为"+QString::number(crackPoint));
//    }
    int loadCycles=minTau_Per.size();
    qDebug()<<loadCycles;
    double startLoadCycle=0;
    double endLoadCycle=loadCycles;
    ui->label->setText("最大周期数为"+QString::number(loadCycles));
    ui->endCycle->setText(QString::number(loadCycles));

    //求解每个加载周期的塑性变形
//    plasticStrian_per.clear();
//    for(int i=0;i<loadCycles;i++){
//        plasticStrian_per.push_back((abs(postivePlasticStrain_per[i])+abs(negtivePlasticStrian_per[i]))/2);
//    }

//    //保存塑性应变数据
//    QString curPath=QCoreApplication::applicationDirPath();
//    //qDebug()<<curPath;
//    QString fileName=curPath+"/"+"plasticStrian_per.txt";
//    if(!saveText(fileName,plasticStrian_per)){
//        QMessageBox::information(this,"保存数据","塑性应变数据保存未完成",QMessageBox::Ok);
//    }
    //qDebug()<<plasticStrian_per;

    //保存剪切模量数据
//    QString fileName2=curPath+"/"+"shearModulus.txt";
//    if(!saveText(fileName2,shearModulus_left)){
//        QMessageBox::information(this,"保存数据","剪切模量数据保存未完成",QMessageBox::Ok);
//    }


    //塑性形变去噪
    //plasticStrian_per_denoise=denoise(plasticStrian_per);
//    plasticStrian_per_denoise=smooth(plasticStrian_per);   //低通滤波 滑动平移滤波
//    Data::plasticStrian_per_denoise=plasticStrian_per_denoise;
    //Data::plasticStrian_per=plasticStrian_per;

    //输入到表格
    ui->tableWidget->setRowCount(loadCycles);
    for(int i=startLoadCycle;i<endLoadCycle;i++){
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(i+1)));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(QString::number(maxStrain_per[i]*1e6)+","+QString::number(maxTau_per[i])));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(QString::number(minStrain_per[i]*1e6)+","+QString::number(minTau_Per[i])));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(QString::number(negtivePlasticStrian_per[i]*1e6)+","+QString::number(0)));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(QString::number(postivePlasticStrain_per[i]*1e6)+","+QString::number(0)));
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(QString::number(0)+","+QString::number(postiveInterTau_Per[i])));
        ui->tableWidget->setItem(i,6,new QTableWidgetItem(QString::number(0)+","+QString::number(negtiveInterTau_per[i])));
        ui->tableWidget->setItem(i,7,new QTableWidgetItem(QString::number(shearModulus_left[i]/1e3)));
        ui->tableWidget->setItem(i,8,new QTableWidgetItem(QString::number(shearModulus_right[i]/1e3)));
//        ui->tableWidget->setItem(i,9,new QTableWidgetItem(QString::number(indexA[i])));
//        ui->tableWidget->setItem(i,10,new QTableWidgetItem(QString::number(indexF[i])));
//        ui->tableWidget->setItem(i,11,new QTableWidgetItem(QString::number(indexB[i])));
//        ui->tableWidget->setItem(i,12,new QTableWidgetItem(QString::number(indexE[i])));

        ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i,3)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i,4)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i,5)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i,6)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i,7)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i,8)->setTextAlignment(Qt::AlignCenter);
//        ui->tableWidget->item(i,9)->setTextAlignment(Qt::AlignCenter);
//        ui->tableWidget->item(i,10)->setTextAlignment(Qt::AlignCenter);
//        ui->tableWidget->item(i,11)->setTextAlignment(Qt::AlignCenter);
//        ui->tableWidget->item(i,12)->setTextAlignment(Qt::AlignCenter);
    }
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for(int i=ui->plot_processing->plottableCount()-1;i>=0;i--){
        ui->plot_orignal->removePlottable(i);
        ui->plot_processing->removePlottable(i);   //此方法与上面方法所用时间差不多
//        qDebug()<<i;
    }
   // qDebug()<<"ni";
//    //绘制能量曲线
//    ui->plot_orignal->xAxis->setLabel("加载周期");
//    ui->plot_orignal->yAxis->setLabel("应变能");
//    ui->plot_orignal->clearGraphs();
//    QCPGraph *g1=new QCPGraph(ui->plot_orignal->xAxis,ui->plot_orignal->yAxis);
//    auto x=maxTau_per.begin();
//    auto y=maxStrain_per.begin();
//    for(int i=0;i<maxTau_per.size();i++){
//        g1->addData(i+1,(*x)*(*y));
//        x++,y++;
//    }
//    ui->plot_orignal->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
//    ui->plot_orignal->rescaleAxes();
//    ui->plot_orignal->replot();

//    绘制原始和拟合后的滞后环
//    绘制原始滞后环
    QCPCurve *cur_Orignal=new QCPCurve(ui->plot_orignal->xAxis,ui->plot_orignal->yAxis);
    QVector<QCPCurveData> data2;
    for(int i=0;i<maxTau_per.size();i++){
        vector<double> tau_temp;
        vector<double> strain_temp;
        tau_temp=tau_mid[i];
        strain_temp=strain_mid[i];
        auto x=strain_temp.begin();
        auto y=tau_temp.begin();
        int n=strain_temp.size();
        for(int j=0;j<n;j++){
            data2.push_back(QCPCurveData(j+i*n,*x,*y));
            x++,y++;
        }

        //cur->setSmooth(false);

    }
    cur_Orignal->data()->set(data2,true);
    cur_Orignal->setPen(QPen(Qt::red));
    ui->plot_orignal->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->plot_orignal->rescaleAxes();
    ui->plot_orignal->replot();

   //绘制拟合后的滞后环
    QCPCurve *cur_processing=new QCPCurve(ui->plot_processing->xAxis,ui->plot_processing->yAxis);
    QVector<QCPCurveData> data;
    ui->plot_processing->xAxis->setLabel("应变");
    ui->plot_processing->yAxis->setLabel("应力 MPa");

    for(int i=0;i<10;i++){
        vector<double> tau_temp2;
        vector<double> strain_temp2;
        tau_temp2.push_back(maxTau_per[i]);  //A点
        tau_temp2.push_back(0);//D点
        tau_temp2.push_back(negtiveInterTau_per[i]); //F点
        tau_temp2.push_back(minTau_Per[i]);           //B点
        tau_temp2.push_back(0); //C点
        tau_temp2.push_back(postiveInterTau_Per[i]);  //E点
        tau_temp2.push_back(maxTau_per[i]);        //A点
        strain_temp2.push_back(maxStrain_per[i]);
        strain_temp2.push_back(postivePlasticStrain_per[i]);
        strain_temp2.push_back(0);
        strain_temp2.push_back(minStrain_per[i]);
        strain_temp2.push_back(negtivePlasticStrian_per[i]);
        strain_temp2.push_back(0);
        strain_temp2.push_back(maxStrain_per[i]);


        auto x=strain_temp2.begin();
        auto y=tau_temp2.begin();
        int n=tau_temp2.size();
        for(int j=0;j<n;j++){
            data.push_back(QCPCurveData(j+i*n,*x,*y));
            x++,y++;
        }

    }
    cur_processing->data()->set(data,true);
    cur_processing->setPen(QPen(Qt::red));
    cur_processing->setSmooth(true);
    ui->plot_processing->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->plot_processing->rescaleAxes();
    ui->plot_processing->replot();

}

void ComparisonData::on_refresh_clicked()
{
//    QElapsedTimer time1;
//    time1.start();

    int loadCycles=maxTau_per.size();
    double startLoadCycle=ui->startCycle->text().toInt();
    double endLoadCycle=ui->endCycle->text().toInt();
    QString dlgTitle="Waring";
    QString strInfo="数据不合法，请输入1-"+QString::number(loadCycles)+"之间整数";
    if(startLoadCycle<=0||startLoadCycle>loadCycles||endLoadCycle<=0||endLoadCycle>loadCycles){
        QMessageBox::critical(this,dlgTitle,strInfo);
        return ;
    }
    if(startLoadCycle>endLoadCycle){
        QMessageBox::critical(this,"Waring","起始加载周期大于结束加载周期");
        return ;
    }


    //输入到表格
//    QElapsedTimer time2;
//    time2.start();
    ui->tableWidget->setRowCount(endLoadCycle-startLoadCycle+1);
    for(int i=startLoadCycle-1;i<endLoadCycle;i++){
        ui->tableWidget->setItem(i-startLoadCycle+1,0,new QTableWidgetItem(QString::number(i+1)));
        ui->tableWidget->setItem(i-startLoadCycle+1,1,new QTableWidgetItem(QString::number(maxStrain_per[i]*1e6)+","+QString::number(maxTau_per[i])));
        ui->tableWidget->setItem(i-startLoadCycle+1,2,new QTableWidgetItem(QString::number(minStrain_per[i]*1e6)+","+QString::number(minTau_Per[i])));
        ui->tableWidget->setItem(i-startLoadCycle+1,3,new QTableWidgetItem(QString::number(negtivePlasticStrian_per[i]*1e6)+","+QString::number(0)));
        ui->tableWidget->setItem(i-startLoadCycle+1,4,new QTableWidgetItem(QString::number(postivePlasticStrain_per[i]*1e6)+","+QString::number(0)));
        ui->tableWidget->setItem(i-startLoadCycle+1,5,new QTableWidgetItem(QString::number(0)+","+QString::number(postiveInterTau_Per[i])));
        ui->tableWidget->setItem(i-startLoadCycle+1,6,new QTableWidgetItem(QString::number(0)+","+QString::number(negtiveInterTau_per[i])));
        ui->tableWidget->setItem(i-startLoadCycle+1,7,new QTableWidgetItem(QString::number(shearModulus_left[i]*1e-3)));
        ui->tableWidget->setItem(i-startLoadCycle+1,8,new QTableWidgetItem(QString::number(shearModulus_right[i]*1e-3)));
        ui->tableWidget->setItem(i-startLoadCycle+1,9,new QTableWidgetItem(QString::number(indexA[i])));
        ui->tableWidget->setItem(i-startLoadCycle+1,10,new QTableWidgetItem(QString::number(indexF[i])));
        ui->tableWidget->setItem(i-startLoadCycle+1,11,new QTableWidgetItem(QString::number(indexB[i])));
        ui->tableWidget->setItem(i-startLoadCycle+1,12,new QTableWidgetItem(QString::number(indexE[i])));

        ui->tableWidget->item(i-startLoadCycle+1,0)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i-startLoadCycle+1,1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i-startLoadCycle+1,2)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i-startLoadCycle+1,3)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i-startLoadCycle+1,4)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i-startLoadCycle+1,5)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i-startLoadCycle+1,6)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i-startLoadCycle+1,7)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i-startLoadCycle+1,8)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i-startLoadCycle+1,9)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i-startLoadCycle+1,10)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i-startLoadCycle+1,11)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i-startLoadCycle+1,12)->setTextAlignment(Qt::AlignCenter);
    }
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //自动调整列宽
//    qDebug()<<"绘制表格时间为"<<time2.elapsed();
    //绘制原始和拟合后的滞后环

    //ui->plot_orignal->clearGraphs();
//    QElapsedTimer time1;
//    time1.start();
//    ui->plot_orignal->clearPlottables();  //清除已绘制的数据
//    ui->plot_processing->clearPlottables();  //清除已绘制的拟合数据
//    qDebug()<<time1.elapsed(); //清除绘制数据所用时间较长，8s多
//    QElapsedTimer time3;
//    time3.start();
    for(int i=ui->plot_processing->plottableCount()-1;i>=0;i--){
        ui->plot_orignal->removePlottable(i);
        ui->plot_processing->removePlottable(i);   //此方法与上面方法所用时间差不多
//        qDebug()<<i;
    }

//    qDebug()<<"删除图像时间为"<<time3.elapsed();


    //绘制原始滞后环
    //绘图区基本设置
//    QElapsedTimer time4;
//    time4.start();
    QCPCurve *cur0=new QCPCurve(ui->plot_orignal->xAxis,ui->plot_orignal->yAxis);
    QVector<QCPCurveData> data0;
    //ui->plot_orignal->plotLayout()->clear();
    ui->plot_orignal->xAxis->setLabel("应变");
    ui->plot_orignal->yAxis->setLabel("应力 MPa");
    for(int i=startLoadCycle-1;i<endLoadCycle;i++){
        vector<double> tau_temp=tau_mid[i];
        vector<double> strain_temp=strain_mid[i];

        auto x=strain_temp.begin();
        auto y=tau_temp.begin();
        int n=strain_temp.size();
        for(int j=0;j<n;j++){
            data0.push_back(QCPCurveData(j+i*n,*x,*y));
            x++,y++;
        }


    }
    cur0->data()->set(data0,true);
    cur0->setPen(QPen(Qt::red));
    ui->plot_orignal->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->plot_orignal->rescaleAxes();
    ui->plot_orignal->replot();
//    qDebug()<<"绘制原始数据时间为"<<time4.elapsed();

    //绘制拟合后的滞后环
//    QElapsedTimer time5;
//    time5.start();
    ui->plot_processing->xAxis->setLabel("应变");
    ui->plot_processing->yAxis->setLabel("应力 MPa");

    QCPCurve *cur=new QCPCurve(ui->plot_processing->xAxis,ui->plot_processing->yAxis);
    QVector<QCPCurveData> data;
    for(int i=startLoadCycle-1;i<endLoadCycle ;i++){
        vector<double> tau_temp2;
        vector<double> strain_temp2;
        tau_temp2.push_back(maxTau_per[i]);  //A点
        tau_temp2.push_back(0);//D点
        tau_temp2.push_back(negtiveInterTau_per[i]); //F点
        tau_temp2.push_back(minTau_Per[i]);           //B点
        tau_temp2.push_back(0); //C点
        tau_temp2.push_back(postiveInterTau_Per[i]);  //E点
        tau_temp2.push_back(maxTau_per[i]);        //A点
        strain_temp2.push_back(maxStrain_per[i]);
        strain_temp2.push_back(postivePlasticStrain_per[i]);
        strain_temp2.push_back(0);
        strain_temp2.push_back(minStrain_per[i]);
        strain_temp2.push_back(negtivePlasticStrian_per[i]);
        strain_temp2.push_back(0);
        strain_temp2.push_back(maxStrain_per[i]);

        auto x=strain_temp2.begin();
        auto y=tau_temp2.begin();
        int n=tau_temp2.size();
        for(int j=0;j<n;j++){
            data.push_back(QCPCurveData(j+i*n,*x,*y));
            x++,y++;
        }
    }
    cur->data()->set(data,true);
    cur->setPen(QPen(Qt::red));
    cur->setSmooth(false);

    ui->plot_processing->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->plot_processing->rescaleAxes();
    ui->plot_processing->replot();
//    qDebug()<<"绘制拟合数据时间为"<<time5.elapsed();
//    qDebug()<<"总时间为"<<time1.elapsed();

}


void ComparisonData::on_pushButton_clicked()
{
    emit shearPlot();
    s->show();
}

void ComparisonData::on_pushButton_2_clicked()
{
    emit dataShow();
    d->show();
}
//保存数据
bool ComparisonData::on_pushButton_3_clicked()
{
    QString curPath=QCoreApplication::applicationDirPath();
    QString dglTitle="选择一个目录";
    QString selectDir=QFileDialog::getExistingDirectory(this,dglTitle,curPath,QFileDialog::ShowDirsOnly);
    if(selectDir.isEmpty()){
//        selectDir=QFileDialog::getExistingDirectory(this,dglTitle,curPath,QFileDialog::ShowDirsOnly);
        QMessageBox::warning(this,"警告","保存数据失败，为选择保存文件夹");
        return false;
    }
    QString fileName1=selectDir+"/"+Data::testSpeciment+"original.txt";
    QString fileName2=selectDir+"/"+Data::testSpeciment+"processed.txt";
    QFile afile(fileName1);
    QFile bfile(fileName2);
    if(afile.exists()||bfile.exists()){
        int closeApp=QMessageBox::question(this,"警告","该文件已经存在，是否要覆盖");
        if(closeApp==QMessageBox::No){
            return false;
        }

    }
   if(!afile.open(QIODevice::WriteOnly|QIODevice::Text)){
       QMessageBox::critical(this,"错误","文件打开失败");
       return false;
   }
   if(!bfile.open(QIODevice::WriteOnly|QIODevice::Text)){
       QMessageBox::critical(this,"错误","文件打开失败");
       return false;
   }
   QTextStream aStream(&afile);
   QTextStream bStream(&bfile);
   aStream.setAutoDetectUnicode(true);
   bStream.setAutoDetectUnicode(true);
   QString str="循环次数 最大转角 最小转角 最大扭矩 最小扭矩";
   QString str2="循环次数 扭转角             扭矩         总应变         弹性应变        塑性应变          应力       残余应力   剪切模量";
   aStream<<str<<"\t"<<"\n";
   bStream<<str2<<"\t"<<"\n";
   int num=maxTau_per.size();
   for(int i=0;i<num;i++){
       QString str;
       QString str2;
       double Angle=maxStrain_per[i]*length/(dia/2)*180/pi;
       double torque=maxTau_per[i]*pi*dia*dia*dia/16000;
       double elasticStrain=maxStrain_per[i]-plasticStrian_per[i];
       double shearmulus=(shearModulus_left[i]+shearModulus_right[i])/2;
       str="    "+QString::number(i)+"        "+QString::number(Data::maxAnglePer[i]*180/pi)+"  "+QString::number(Data::minAnglePer[i]*180/pi)+"   "
               +QString::number(Data::maxTorquePer[i])+"  "+QString::number(Data::minTorquePer[i]);
       str2="     "+QString::number(i)+"       "+QString::number(Angle)+"     "+QString::number(torque)+"    "+QString::number(maxStrain_per[i])
               +"   "+QString::number(elasticStrain)+"    "+QString::number(plasticStrian_per[i])+"    "+QString::number(maxTau_per[i])+"    "
               +QString::number(postiveInterTau_Per[i])+"    "+QString::number(shearmulus);
       aStream<<str<<'\t'<<"\n";
       bStream<<str2<<"\t"<<"\n";
   }
   afile.close();
   bfile.close();
   QString fileName3=selectDir+"/"+Data::testSpeciment+"plasticStrain.txt";
   QFile file3(fileName3);
   if(!file3.open(QIODevice::WriteOnly|QIODevice::Text)){
       QMessageBox::critical(this,"错误","文件打开失败");
       return false;
   }
   QTextStream cStream(&file3);
   QString str3="塑性应变   塑性应变能   总应变能    损伤量    塑性累积量   弹性应变能";
   cStream<<str3<<"\n";

   for(int i=0;i<num;i++){
       QString str;
       str=QString::number(plasticStrian_per[i])+"    "+QString::number(plasticStrian_per[i]*maxTau_per[i])+"    "
               +QString::number(maxStrain_per[i]*maxTau_per[i])+"   "+QString::number(Data::damage[i])+"    "+QString::number(Data::Pd[i])+
               "    "+QString::number(Data::elasticStrainEnergy[i]);
       cStream<<str<<"\n";
   }
   file3.close();
   QMessageBox::information(this,"保存数据","保存数据已完成");
//   QString fileDamage=selectDir+"/dataProcess"+"/Damage.txt";
//   QString filePd=selectDir+"/dataProcess"+"/Pd.txt";
//   QString fileEla=selectDir+"/dataProcess"+"/elasticStrainEnerge.txt";

//   qDebug()<<fileDamage;
//   if(!saveText(fileDamage,damage)){
//       QMessageBox::warning(this,"警告","保存数据失败");
//   }
//   if(!saveText(filePd,Pd)){
//       QMessageBox::warning(this,"警告","保存数据失败");
//   }
//   if(!saveText(fileEla,elasticStrainEnerge)){
//       QMessageBox::warning(this,"警告","保存数据失败");
//   }

}
