 #include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    c=new ComparisonData;
    connect(this,&MainWindow::comparisonDataShow,c,&ComparisonData::showData);
    ui->plot_Damage->plotLayout()->insertRow(0);
    ui->plot_Damage->plotLayout()->addElement(0,0,new QCPTextElement(ui->plot_Damage,"塑性累积损伤和损伤量",QFont("宋体",12,QFont::Bold)));
    ui->plot_ela->plotLayout()->insertRow(0);
    ui->plot_ela->plotLayout()->addElement(0,0,new QCPTextElement(ui->plot_ela,"累积损伤量-弹性应变能",QFont("宋体",12,QFont::Bold)));
    ui->plot_Pd->plotLayout()->insertRow(0);
    ui->plot_Pd->plotLayout()->addElement(0,0,new QCPTextElement(ui->plot_Pd,"Pd",QFont("宋体",12,QFont::Bold)));
    ui->radioButton_jieti->setChecked(1);   //阶梯加载默认被选中

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::featurePointExtraction()
{
    //首先清除数据
//    QElapsedTimer s0;
//    s0.start();
    if(!tau.empty()){
        torque.clear();
        angle.clear();
        tau.clear();
        strain.clear();
        tau_mid.clear();
        strain_mid.clear();
        maxTau_per.clear();
        maxStrain_per.clear();//特征点A 每个周期的最大应变
        minTau_Per.clear();   //特征点B 每个周期的最小应力
        minStrain_per.clear();//特征点B 每个周期的最小应变
        shearModulus.clear();         //剪切模量 左右平均值
        mid_tau.clear();
        plasticStrain.clear();
        elasticStrain.clear();
        indexA.clear();
        indexB.clear();
        indexC.clear();
        indexD.clear();
        indexE.clear();
        indexF.clear();
    }

    if(!inputData()){
        QMessageBox::warning(this,"警告","请输入实验数据");
    }
    //处理数据文件，先选择数据文件夹，遍历文件夹中txt文件，每个文件按行读取获得扭矩和角度数据
    //QString curPath=QCoreApplication::applicationDirPath();
    QString curPath="E:\\WorkingFile\\PointExtraction\\data";
    QString dglTitle="选择一个目录";
    selectDir=QFileDialog::getExistingDirectory(this,dglTitle,curPath,QFileDialog::ShowDirsOnly);
    while(selectDir.isEmpty()){
//        selectDir=QFileDialog::getExistingDirectory(this,dglTitle,curPath,QFileDialog::ShowDirsOnly);
        return;
    }

    QDir dir(selectDir);
    QStringList filters;
    filters<<"*.Txt";//过滤格式
    dir.setFilter(QDir::Files|QDir::NoSymLinks);//设置类型过滤器，只为文件格式
    dir.setNameFilters(filters);
    int count=dir.count();
    //qDebug()<<count;
    if(count<=0){
        QMessageBox::warning(this,"警告","载入数据为空");
        return ;
    }
    //按照文件名排列dir下文件
    auto entryList = dir.entryList();
    dir.setSorting(QDir::NoSort);
    QCollator collator;
    collator.setNumericMode(true);
    std::sort(entryList.begin(), entryList.end(), collator);

    int lines=0;
    //读取文件的总行数
//    QElapsedTimer s0;
//    s0.start();
    for(int i=0;i<count;i++){
        //qDebug()<<entryList[i];
        QString file_Name=selectDir+'/'+dir[i];  //注意中间加'/'
        QFile File(file_Name);  //QFile构造函数参数为文件绝对路径
        File.open(QIODevice::ReadOnly);
        QTextStream oStream(&File);
        oStream.setAutoDetectUnicode(true);
        int len=File.size();
//        qDebug()<<len;
        QString lastLine;
        if(oStream.seek(len-120)){
            while(!oStream.atEnd()){
                lastLine=oStream.readLine();
            }
        }
        lines+=lastLine.section(" ",0,0).toInt();
        File.close();

    }
//    qDebug()<<s0.elapsed();
//    qDebug()<<lines;

//    QElapsedTimer s1;
//    s1.start();
    QProgressDialog pdialog("正在加载数据...","取消",0,100,this);
    pdialog.setWindowModality(Qt::WindowModal);   //模态对话框
    pdialog.setMinimumDuration(0);                //如果进度条运行的时间小于5，进度条就不会显示，默认是4S
    pdialog.setWindowTitle("加载数据");
    pdialog.show();
    int line=0;
    for(int i=0;i<count;i++){
        QString file_Name=selectDir+'/'+entryList[i];  //注意中间加'/'
        //qDebug()<<entryList[i];
        QFile File(file_Name);  //QFile构造函数参数为文件绝对路径
        if(!File.open(QIODevice::ReadOnly)){
            QMessageBox::warning(this,"警告","打开文件失败");
        }
        QTextStream oStream(&File);
        oStream.setAutoDetectUnicode(true);
        //auto str=oStream.readLine(); //先读取一行，以从第二行开始读
        int flag=0;
        //int flag=1;
        while(!oStream.atEnd()){
            //qDebug()<<line;
            pdialog.setValue(double(line)/double(lines)*100);//读数据所花时间较长
            //qDebug()<<double(line)/double(lines)*100;
            QCoreApplication::processEvents(); //防止界面冻结
            if(pdialog.wasCanceled()) return;
            line++;
            auto str=oStream.readLine();
            str=str.simplified();  //简化字符串 去掉首位空格并使中间连续空格用一个空格代替
            //qDebug()<<str;
            //qDebug()<<str.section(" ",7,7).toInt();

            if(str.section(" ",7,7)==""){
                QMessageBox::warning(this,"警告","读取文件格式出错");
                return ;
            }
            if(flag==(str.section(" ",7,7).toInt())){
                auto str1=str.section(" ",2,2).toDouble()*1;   //以空格问分界点，获取第一个到第一个的数据（从0开始计数）
                double tau_temp=(16000*str1)/(pi*dia*dia*dia);
                torque_per.push_back(str1);
                tau_per.push_back(tau_temp);
                auto str2=str.section(" ",3,3).toDouble();
                str2=str2*pi/180;
                double strain_temp=str2*(dia/2)/length;
                angle_per.push_back(str2);
                strain_per.push_back(strain_temp);
            }
            else{

                if(!tau_per.empty()){
                    tau.push_back(tau_per);
                    tau_per.clear();
                    strain.push_back(strain_per);
                    strain_per.clear();
                    torque.push_back(torque_per);
                    torque_per.clear();
                    angle.push_back(angle_per);
                    angle_per.clear();
                }
                flag=str.section(" ",7,7).toInt();
                double str1=1*str.section(" ",2,2).toDouble();  //扭矩
                double tau_temp=(16000*str1)/(pi*dia*dia*dia);
                tau_per.push_back(tau_temp);
                torque_per.push_back(str1);
                auto str2=str.section(" ",3,3).toDouble();  //角度
                str2=str2*pi/180;
                double strain_temp=str2*(dia/2)/length;
                strain_per.push_back(strain_temp);
                angle_per.push_back(str2);
            }

        }

        File.close();
    }
//    tau.push_back(tau_per);
//    tau_per.clear();
//    strain.push_back(strain_per);
//    strain_per.clear();
//    torque.push_back(torque_per);
//    torque_per.clear();
//    angle.push_back(angle_per);
//    angle_per.clear();
    //qDebug()<<tau.size();
//    qDebug()<<s1.elapsed();
    //至此，将数据文件夹中扭矩角度数据存入容器 torque和angle中,并计算应力应变
    //qDebug()<<torque_per;
    //qDebug()<<angle_per;

    //在每一个加载周期内处理数据
//    QTime tme;
//    tme.restart();

//     qDebug()<<s1.elapsed();
    dataProcess();
    pdialog.setValue(100);
//     QTime s1;
//     s1.start();
    //存储计算数据
    saveData();

    pdialog.close();
    QMessageBox::information(this,"状态","载入数据完成");


}

bool MainWindow::saveText(const QString &fileName, const vector<double> &v)
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






    //测试代码
//    qDebug()<<maxTau_per;  //通过
//    qDebug()<<minTau_Per;  //通过
//    qDebug()<<maxStrain_per;  //通过
//    qDebug()<<minStrain_per;  //通过
//    qDebug()<<negtivePlasticTau_per;  //通过
//    qDebug()<<negtivePlasticStrian_per;  //通过
//    qDebug()<<postivePlasticStrain_per; //通过
//    qDebug()<<postivePlasticTau_per;    ///通过
//    qDebug()<<negtiveInterTau_per;
//    qDebug()<<postiveInterTau_Per;
//    qDebug()<<shearModulus_left;
//    qDebug()<<shearModulus_right;

double MainWindow::abs(double x)
{
    if(x<0) return -x;
    else return x;

}

void MainWindow::maxStrainPer(vector<double> v,vector<double> t)
{
    int n=v.size();
    auto maxNum=v[0];
    int index=0;
    for(int i=1;i<n;i++){
        if(maxNum<v[i]){
            maxNum=v[i];
            index=i;
        }
    }
    maxStrain_per.push_back(maxNum);
    maxTau_per.push_back(abs(t[index]));
    indexA.push_back(index);

}

void MainWindow::minStrainPer(vector<double> v,vector<double> t)
{
    int n=v.size();
    auto minNum=v[0];
    int index=0;
    for(int i=1;i<n;i++){
        if(minNum>v[i]){
            minNum=v[i];
            index=i;
        }
    }
    minStrain_per.push_back(minNum);
    double minT=t[index];
    if(minT>0){
        minT=-minT;
    }
    minTau_Per.push_back(minT);
    indexB.push_back(index);
}

void MainWindow::PlasticStrianPer(vector<double> s,vector<double> t)
{

    s.push_back(s[0]);
    t.push_back(t[0]);
    int n=s.size();
    int index=0;
    for(int i=1;i<n;i++){
        //判断C D两点
        if(t[i-1]<0&&t[i]>=0){
           if(abs(t[i-1])<abs(t[i])){
               indexD.push_back(i-1);

           }
           else{
               if(i==(n-1)) {
                   indexD.push_back(index);
               }else{
                   indexD.push_back(i);
               }

           }
//            s1=s[i-1];
//            indexS1=i-1;
        }
        else if(t[i-1]>0&&t[i]<=0){
            if(abs(t[i-1])<abs(t[i])){
                indexC.push_back(i-1);
            }
            else{
                if(i==(n-1)){
                    indexC.push_back(index);
                }
                else indexC.push_back(i);

            }
        }

        //判断e f两点
        if(s[i-1]<0&&s[i]>=0){
            if(abs(s[i-1])<abs(s[i])){
                indexF.push_back(i-1);
            }
            else{
                if(i==(n-1)){
                    indexF.push_back(index);
                }
                else{
                    indexF.push_back(i);
                }
            }

        }
        else if(s[i-1]>0&&s[i]<=0){
            if(abs(s[i-1])<abs(s[i])){
                indexE.push_back(i-1);
            }
            else{
                if(i==(n-1)){
                    indexE.push_back(index);
                }
                else{
                    indexE.push_back(i);
                }

            }
        }

    }

}

vector<double> MainWindow::Line_Fitting(vector<double> s, vector<double> t)
{
    double a;  //截距
    double b;  //斜率
    int count=s.size();
    double X=0, XX=0, Y=0, YY=0,XY=0;
    double ix=0,iy=0;
    for(int i=0;i<count;i++){
        ix=s[i];
        iy=t[i];

        X+=ix;
        XX+=ix*ix;
        Y+=iy;
        YY+=iy*iy;
        XY+=ix*iy;

    }
    a=(XX*Y-X*XY)/(count*XX-X*X);
    b=(count*XY-X*Y)/(count*XX-X*X);
    return {b,-a/b,a};
}

double MainWindow::maxNum(double a, double b)
{
    return a>b?a:b;
}

double MainWindow::minNum(double a, double b)
{
    return a<b?a:b;
}

double MainWindow::maxNum(const vector<double> &v)
{
    double result=0;
    for(int i=100;i<200;i++){
        result+=v[i];
    }
    return result/100;
}

double MainWindow::minNum(const vector<double> &v)
{
    double result=0;
    for(int i=100;i<200;i++){
        result+=v[i];
    }
    return result/100;

}

void MainWindow::plot()
{
    int loadingCycles=tau_mid.size();
//    for(int i=0;i<loadingCycles;i++){
//        auto tau_temp=tau_mid[i];
//        auto strain_temp=strain_mid[i];
//        vector<double> tau_temp2;
//        vector<double> strain_temp2;
//        tau_temp2.push_back(tau_temp[indexA[i]]);
//        tau_temp2.push_back(tau_temp[indexD[i]]);
//        tau_temp2.push_back(tau_temp[indexF[i]]);
//        tau_temp2.push_back(tau_temp[indexB[i]]);
//        tau_temp2.push_back(tau_temp[indexC[i]]);
//        tau_temp2.push_back(tau_temp[indexE[i]]);
//        tau_temp2.push_back(tau_temp[indexA[i]]);
//        strain_temp2.push_back(strain_temp[indexA[i]]);
//        strain_temp2.push_back(strain_temp[indexD[i]]);
//        strain_temp2.push_back(strain_temp[indexF[i]]);
//        strain_temp2.push_back(strain_temp[indexB[i]]);
//        strain_temp2.push_back(strain_temp[indexC[i]]);
//        strain_temp2.push_back(strain_temp[indexE[i]]);
//        strain_temp2.push_back(strain_temp[indexA[i]]);

//        ui->widget->xAxis->setLabel("应变");
//        ui->widget->yAxis->setLabel("应力 MPa");
//        QCPCurve *cur=new QCPCurve(ui->widget->xAxis,ui->widget->yAxis);
//        QVector<QCPCurveData> data;
//        auto x=strain_temp2.begin();
//        auto y=tau_temp2.begin();
//        int n=tau_temp2.size();
//        for(int j=0;j<n;j++){
//            data.push_back(QCPCurveData(j,*x,*y));
//            x++,y++;
//        }
//        cur->data()->set(data,true);
//        cur->setPen(QPen(Qt::red));
//        //cur->setSmooth(true);


//    }
//    //为曲线图设置标题
//    ui->widget->plotLayout()->insertRow(0);
//    ui->widget->plotLayout()->addElement(0,0,new QCPTextElement(ui->widget,"滞后环随加载周期变化图",QFont("宋体",12,QFont::Bold)));
//    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
//    ui->widget->rescaleAxes();
//    ui->widget->replot();

//    qDebug()<<"开始绘图";
    ui->plot_Damage->xAxis->setLabel("累积损伤量Pd");
    ui->plot_Damage->yAxis->setLabel("损伤量D");
    ui->plot_ela->xAxis->setLabel("累积损伤量Pd");
    ui->plot_ela->yAxis->setLabel("弹性应变能");
    ui->plot_Pd->xAxis->setLabel("加载周期");
    ui->plot_Pd->yAxis->setLabel("Pd");

    ui->plot_Pd->clearGraphs();
    ui->plot_ela->clearGraphs();
    ui->plot_Damage->clearGraphs();
    QCPGraph *da=new QCPGraph(ui->plot_Damage->xAxis,ui->plot_Damage->yAxis);
    QCPGraph *ela=new QCPGraph(ui->plot_ela->xAxis,ui->plot_ela->yAxis);
    QCPGraph *pd=new QCPGraph(ui->plot_Pd->xAxis,ui->plot_Pd->yAxis);


    auto dy=damage.begin();
    auto ey=elasticStrainEnerge.begin();
    auto pdy=Pd.begin();

    for(int i=0;i<loadingCycles;i++){
        da->addData(*pdy,*dy);
        ela->addData(*pdy,*ey);
        pd->addData(i,*pdy);
        dy++,ey++,pdy++;

    }
    ui->plot_Damage->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->plot_Damage->rescaleAxes();
    ui->plot_ela->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->plot_ela->rescaleAxes();
    ui->plot_Pd->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->plot_Pd->rescaleAxes();
    ui->plot_Damage->replot();
    ui->plot_ela->replot();
    ui->plot_Pd->replot();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int closeApp=QMessageBox::question(this,"Note","确认是否退出");
    if(closeApp==QMessageBox::Yes){
        c->close();
        delete c;
        event->accept();
    }
    else{
        event->ignore();
    }
}

void MainWindow::dataProcess()
{
    double loadingCycles=tau.size();
    //qDebug()<<loadingCycles;
//    QElapsedTimer s2;
//    s2.start();
    maxTorquePer.clear();
    minTorquePer.clear();
    maxAnglePer.clear();
    minAnglePer.clear();

    for(int i=0;i<loadingCycles;i++){   //经测试，此循环所耗时间较短。
        auto tau_temp=tau[i];
        auto strain_temp=strain[i];
        //应力应变对中
        int num=tau_temp.size();
        mid_tau.push_back((*max_element(tau_temp.begin(),tau_temp.end())+*min_element(tau_temp.begin(),tau_temp.end()))/2);
        double mid_strain=(*max_element(strain_temp.begin(),strain_temp.end())+*min_element(strain_temp.begin(),strain_temp.end()))/2;
        for(int j=0;j<num;j++){
            tau_temp[j]-=mid_tau[i];
            strain_temp[j]-=mid_strain;
        }
        tau_mid.push_back(tau_temp);
        strain_mid.push_back(strain_temp);
        tau_temp=tau_mid[i];
        strain_temp=strain_mid[i];
        maxStrainPer(strain_temp,tau_temp);

    }
    //去除前几个不稳定的加载周期
    //先计算中间周期的最大应变
//    int flagCycle=loadingCycles/2;
//    auto flagMaxStrain=maxStrain_per[flagCycle];
//    //int flagNum=0;
//    for(int i=0;i<flagCycle;i++){
//        if(abs((flagMaxStrain-maxStrain_per[i])*length/dia*2)>(0.1*pi/180)){
//            flagNum=i;
//        }
//    }

//    if(flagNum>30){
//        flagNum=30;
//    }
//    //qDebug()<<flagNum;

//    tau_mid.erase(tau_mid.begin(),tau_mid.begin()+flagNum);
//    strain_mid.erase(strain_mid.begin(),strain_mid.begin()+flagNum);
//    maxStrain_per.erase(maxStrain_per.begin(),maxStrain_per.begin()+flagNum);
//    maxTau_per.erase(maxTau_per.begin(),maxTau_per.begin()+flagNum);
//    tau.erase(tau.begin(),tau.begin()+flagNum);
//    strain.erase(strain.begin(),strain.begin()+flagNum);
//    torque.erase(torque.begin(),torque.begin()+flagNum);
//    angle.erase(angle.begin(),angle.begin()+flagNum);

    loadingCycles=tau_mid.size();
    negtivePlasticStrian_per.clear();   //特征点C 每个周期的反向塑性应变
    postivePlasticStrain_per.clear();   //特征点D 每个周期的正向塑性应变
    negtiveInterTau_per.clear();        //特征点E 每个周期的负向截距应力
    postiveInterTau_Per.clear();        //特征点F 每个周期的正向截距应力
    shearModulus_left.clear();
    shearModulus_right.clear();

    for(int i=0;i<loadingCycles;i++){
        auto tau_temp=tau_mid[i];
        auto strain_temp=strain_mid[i];
        auto torTemp=torque[i];
        auto angTemp=angle[i];
        maxTorquePer.push_back(maxMinTAper(torTemp,angTemp)[0]);
        minTorquePer.push_back(maxMinTAper(torTemp,angTemp)[1]);
        maxAnglePer.push_back(maxMinTAper(torTemp,angTemp)[2]);
        minAnglePer.push_back(maxMinTAper(torTemp,angTemp)[3]);

        minStrainPer(strain_temp,tau_temp);
        PlasticStrianPer(strain_temp,tau_temp);

    }
    for(int i=0;i<loadingCycles/2;i++){
        qDebug()<<i;
        auto tau_temp=tau_mid[i];
        auto strain_temp=strain_mid[i];
        vector<double> tau_left,strain_left,tau_right,strain_right;
        int num=tau_temp.size();
        if(indexE[i]>indexB[i]){
            for(int j=indexE[i];j<num;j++){
                tau_left.push_back(tau_temp[j]);
                strain_left.push_back(strain_temp[j]);
            }
            for(int j=0;j<indexB[i];j++){
                tau_left.push_back(tau_temp[j]);
                strain_left.push_back(strain_temp[j]);
            }
        }
        else{
            for(int j=indexE[i];j<indexB[i];j++){
                tau_left.push_back(tau_temp[j]);
                strain_left.push_back(strain_temp[j]);
            }
        }
        if(indexF[i]>indexA[i]){
            for(int j=indexF[i];j<num;j++){
                tau_right.push_back(tau_temp[j]);
                strain_right.push_back(strain_temp[j]);
            }
            for(int j=0;j<indexA[i];j++){
                tau_right.push_back(tau_temp[j]);
                strain_right.push_back(strain_temp[j]);
            }
        }
        else{
            for(int j=indexF[i];j<indexA[i];j++){
                tau_right.push_back(tau_temp[j]);
                strain_right.push_back(strain_temp[j]);
            }
        }
        auto a1 = Line_Fitting(strain_left,tau_left);
        auto b1 = Line_Fitting(strain_right,tau_right);
        shearModulus_left.push_back(abs(a1[0]));
        shearModulus_right.push_back(abs(b1[0]));
        double s1=b1[1];
        double s2=a1[1];
        double t1=b1[2];
        double t2=a1[2];
        if(s1<s2){
            postivePlasticStrain_per.push_back(s2);
            negtivePlasticStrian_per.push_back(s1);
        }
        else{
            postivePlasticStrain_per.push_back(s1);
            negtivePlasticStrian_per.push_back(s2);
        }
        if(t1<t2){
            postiveInterTau_Per.push_back(t2);
            negtiveInterTau_per.push_back(t1);
        }
        else{
            postiveInterTau_Per.push_back(t1);
            negtiveInterTau_per.push_back(t2);
        }


    }
    for(int i=loadingCycles/2;i<loadingCycles;i++){
        auto tau_temp=tau_mid[i];
        auto strain_temp=strain_mid[i];
        vector<double> tau_left,strain_left,tau_right,strain_right;
        int num=tau_temp.size();
        if(indexE[i]>indexB[i]){
            for(int j=indexE[i];j<num;j++){
                tau_left.push_back(tau_temp[j]);
                strain_left.push_back(strain_temp[j]);
            }
            for(int j=0;j<indexB[i];j++){
                tau_left.push_back(tau_temp[j]);
                strain_left.push_back(strain_temp[j]);
            }
        }
        else{
            for(int j=indexE[i];j<indexB[i];j++){
                tau_left.push_back(tau_temp[j]);
                strain_left.push_back(strain_temp[j]);
            }
        }
        if(indexF[i]>indexA[i]){
            for(int j=indexF[i];j<num;j++){
                tau_right.push_back(tau_temp[j]);
                strain_right.push_back(strain_temp[j]);
            }
            for(int j=0;j<indexA[i];j++){
                tau_right.push_back(tau_temp[j]);
                strain_right.push_back(strain_temp[j]);
            }
        }
        else{
            for(int j=indexF[i];j<indexA[i];j++){
                tau_right.push_back(tau_temp[j]);
                strain_right.push_back(strain_temp[j]);
            }
        }
        auto a1 = Line_Fitting(strain_left,tau_left);
        auto b1 = Line_Fitting(strain_right,tau_right);
        shearModulus_left.push_back(abs(a1[0]));
        shearModulus_right.push_back(abs(b1[0]));
        double s1=b1[1];
        double s2=a1[1];
        double t1=b1[2];
        double t2=a1[2];
        if(s1<s2){
            postivePlasticStrain_per.push_back(s2);
            negtivePlasticStrian_per.push_back(s1);
        }
        else{
            postivePlasticStrain_per.push_back(s1);
            negtivePlasticStrian_per.push_back(s2);
        }
        if(t1<t2){
            postiveInterTau_Per.push_back(t2);
            negtiveInterTau_per.push_back(t1);
        }
        else{
            postiveInterTau_Per.push_back(t1);
            negtiveInterTau_per.push_back(t2);
        }


    }
    //若是恒幅加载，则取消注释以下代码
    if(!isLadder){
        denoise(maxStrain_per);
        denoise(maxTau_per);
        denoise(minStrain_per);
        denoise(minTau_Per);
        denoise(shearModulus_left);
        denoise(shearModulus_right);
        denoise(negtivePlasticStrian_per);
        denoise(postivePlasticStrain_per);

    }
    //若是阶梯加载，则取消注释以下代码
    if(isLadder){
        denoise2(maxStrain_per);
        denoise2(maxTau_per);
        denoise2(minStrain_per);
        denoise2(minTau_Per);
        denoise2(shearModulus_left);
        denoise2(shearModulus_right);
        denoise2(negtivePlasticStrian_per);
        denoise2(postivePlasticStrain_per);
        denoise2(damage);
        denoise2(elasticStrainEnerge);
        denoise2(plasticStrain);
    }
    plasticStrain.clear();
    elasticStrain.clear();
    shearModulus.clear();
    for(int i=0;i<loadingCycles;i++){
        plasticStrain.push_back((abs(negtivePlasticStrian_per[i])+abs(postivePlasticStrain_per[i]))/2);
        elasticStrain.push_back(maxStrain_per[i]-plasticStrain[i]);
        shearModulus.push_back((shearModulus_left[i]+shearModulus_right[i])/2);
    }
    for(int i=0;i<loadingCycles;i++){
        if(plasticStrain[i]>maxStrain_per[i]){
            plasticStrain[i]=maxStrain_per[i];
        }
    }



//    qDebug()<<s3.elapsed();
//    QElapsedTimer s4;
//    s4.start();
    double shearModulus_0 =shearModulus[0];

    for(int i=0;i<shearModulus.size()*0.3;i++){
        if(shearModulus_0<shearModulus[i]){
            shearModulus_0=shearModulus[i];
        }

    }


//    qDebug()<<s4.elapsed();
    //shearModulus_0=shearModulus_0/100;
    //qDebug()<<shearModulus_0;
    if(shearModulus_0==0) return ;
    if(isnan(shearModulus_0)) return ;

    damage.clear();
    Pd.clear();
    elasticStrainEnerge.clear();
    //计算损伤量和累计损伤
    double tempPd=0;
    for(int i=0;i<loadingCycles;i++){
        damage.push_back(1-shearModulus[i]/shearModulus_0);
        tempPd+=4*plasticStrain[i];
        Pd.push_back(tempPd);
        elasticStrainEnerge.push_back(maxTau_per[i]*elasticStrain[i]);
    }
    qDebug()<<elasticStrainEnerge.size();

   plot();

   QString fileName="E:\\WorkingFile\\PointExtraction\\data\\loop_Strain.txt";
   QString fileName2="E:\\WorkingFile\\PointExtraction\\data\\loop_Tau.txt";
   QFile file(fileName);
   QFile file2(fileName2);
   if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
       QMessageBox::critical(this,"错误","文件打开失败");
   }
   if(!file2.open(QIODevice::WriteOnly|QIODevice::Text)){
       QMessageBox::critical(this,"错误","文件打开失败");
   }
   int num=strain_mid.size()/200;
   //qDebug()<<num;
   int rem=strain_mid.size()-num*200;
   int lastNum;
   if(rem<100) lastNum=rem+200*num-2;
   else lastNum=100+200*num;
   //qDebug()<<lastNum;
   QTextStream atream(&file);
   QTextStream btream(&file2);
   for(int i=0;i<num;i++){
       int index=(2*i+1)*100;
       auto strainTemp=strain_mid[index];
       auto tauTemp=tau_mid[index];
       for(auto s:strainTemp){
           atream<<QString::number(s)<<" ";
       }
       for(auto t:tauTemp){
           btream<<QString::number(t)<<" ";
       }
       atream<<"\n";
       btream<<"\n";

   }
   for(auto s:strain_mid[lastNum]){
       atream<<QString::number(s)<<" ";
   }
   for(auto t:tau_mid[lastNum]){
       btream<<QString::number(t)<<" ";
   }
   file.close();
   file2.close();




}


void MainWindow::denoise(vector<double> &v)
{
    int num=v.size();
    int count=num/30;
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

void MainWindow::denoise2(vector<double> &v)
{
    int num=v.size();
    int count=num/200;
    for(int i=0;i<count;i++){
        double sum=0,average=0,variance=0;
        for(int j=200*i;j<(i+1)*200;j++){
            sum+=v[j];
        }
        average=sum/200;
        for(int j=200*i;j<(i+1)*200;j++){
            variance+=(v[j]-average)*(v[j]-average);
        }
        variance=variance/200;
        double std=sqrt(variance);
        for(int j=200*i;j<(i+1)*200;j++){
            if((v[j]<(average-3*std)||(v[j]>(average+3*std)))){
                v[j]=average;
            }
        }
    }


}

bool MainWindow::inputData()
{
    if(ui->lineEdit->text().toStdString().empty()){
        return false;
    }
    Data::testSpeciment=ui->comboBox->currentText()+ui->lineEdit->text();
    return true;

}

vector<double> MainWindow::maxMinTAper(vector<double> &t, vector<double> &a)
{
    int num=t.size();
    double tmax=t[0];
    double tmin=t[0];
    double amax=a[0];
    double amin=a[0];
    for(int i=1;i<num;i++){
        if(tmax<t[i]){
            tmax=t[i];
        }
        if(tmin>t[i]){
            tmin=t[i];
        }
        if(amax<a[i]){
            amax=a[i];
        }
        if(amin>a[i]){
            amin=a[i];
        }
    }
    return {tmax,tmin,amax,amin};

}

void MainWindow::saveData()
{
    Data::maxTau_per=maxTau_per;
    Data::minTau_Per=minTau_Per;
    Data::maxStrain_per=maxStrain_per;
    Data::minStrain_per=minStrain_per;
    Data::negtivePlasticStrian_per=negtivePlasticStrian_per;
    Data::postivePlasticStrain_per=postivePlasticStrain_per;
    Data::negtiveInterTau_per=negtiveInterTau_per;
    Data::postiveInterTau_Per=postiveInterTau_Per;
    Data::shearModulus_left=shearModulus_left;    //剪切模量 G左侧
    Data::shearModulus_right=shearModulus_right;   //剪切模量 G右侧
    Data::tau_mid=tau_mid;
    Data::strain_mid=strain_mid;
    Data::mid_tau=mid_tau;
    Data::maxAnglePer=maxAnglePer;
    Data::maxTorquePer=maxTorquePer;
    Data::minAnglePer=minAnglePer;
    Data::minTorquePer=minTorquePer;
    Data::plasticStrian_per=plasticStrain;
}

void MainWindow::on_pushButton_2_clicked()
{
    emit comparisonDataShow();
    c->show();
}

void MainWindow::on_pushButton_clicked()
{
    featurePointExtraction();
}

void MainWindow::on_pushButton_3_clicked()
{
    if(!tau.empty()){
        torque.clear();
        angle.clear();
        tau.clear();
        strain.clear();
        tau_mid.clear();
        strain_mid.clear();
        maxTau_per.clear();
        maxStrain_per.clear();//特征点A 每个周期的最大应变
        minTau_Per.clear();   //特征点B 每个周期的最小应力
        minStrain_per.clear();//特征点B 每个周期的最小应变
        shearModulus.clear();         //剪切模量 左右平均值
        mid_tau.clear();
        plasticStrain.clear();
        elasticStrain.clear();
        indexA.clear();
        indexB.clear();
        indexC.clear();
        indexD.clear();
        indexE.clear();
        indexF.clear();
    }
    //输入实验数据
    if(!inputData()){
        QMessageBox::warning(this,"警告","请输入实验数据");
    }
    //处理数据文件，先选择数据文件夹，遍历文件夹中txt文件，每个文件按行读取获得扭矩和角度数据
    //QString curPath=QCoreApplication::applicationDirPath();
    QString curPath="E:\\WorkingFile\\PointExtraction\\data";
    QString dglTitle="选择一个目录";
    selectDir=QFileDialog::getExistingDirectory(this,dglTitle,curPath,QFileDialog::ShowDirsOnly);
    while(selectDir.isEmpty()){
//        selectDir=QFileDialog::getExistingDirectory(this,dglTitle,curPath,QFileDialog::ShowDirsOnly);
        return;
    }

    QDir dir(selectDir);
    QStringList filters;
    filters<<"*.db";//过滤格式
    dir.setFilter(QDir::Files|QDir::NoSymLinks);//设置类型过滤器，只为文件格式
    dir.setNameFilters(filters);
    int count=dir.count();
    //qDebug()<<count;
    if(count<=0){
        QMessageBox::warning(this,"警告","载入数据为空");
        return ;
    }
    int lines=0;
    //读取文件的总行数
//    QElapsedTimer s0;
//    s0.start();

    QSqlDatabase *db=new QSqlDatabase();

    *db = QSqlDatabase::addDatabase("QSQLITE");
    //qDebug()<<db->connectionName();
    QString select_max_sql = "select max(id) from student";
    QSqlQuery query;

    for(int i=0;i<count;i++){
        QString dbFile_Name=selectDir+'/'+dir[i];  //注意中间加'/'
        db->setDatabaseName(dbFile_Name);
        QString name;
        {
            name = QSqlDatabase::database().connectionName();
            //qDebug()<<name;
        }//超出作用域，隐含对象QSqlDatabase::database()被删除。
        if(!db->open()){
            QMessageBox::warning(this,"警告","DB文件打开失败");
            return ;
        }
        QString select_max_sql = "select max(id) from DATA";
        if(!query.exec(select_max_sql)){
            QMessageBox::warning(this,"警告","打开DB文件失败");
            return;
        }else{
            while(query.next()){
                lines+=query.value(0).toInt();
            }
        }
    }
    db->close();
    //QSqlDatabase::removeDatabase(name);

    //qDebug()<<lines;

//    QElapsedTimer s1;
//    s1.start();
    QProgressDialog pdialog("正在加载数据...","取消",0,100,this);
    pdialog.setWindowModality(Qt::WindowModal);   //模态对话框
    pdialog.setMinimumDuration(0);                //如果进度条运行的时间小于5，进度条就不会显示，默认是4S
    pdialog.setWindowTitle("加载数据");
    pdialog.show();
    int line=0;
    for(int i=0;i<count;i++){
        QString dbFile_Name=selectDir+'/'+dir[i];  //注意中间加'/'
        if (QSqlDatabase::contains("qt_sql_default_connection"))
        {
            *db = QSqlDatabase::database("qt_sql_default_connection");
            //qDebug()<<"进入此程序";
            db->setDatabaseName(dbFile_Name);
        }
        else
        {
            *db = QSqlDatabase::addDatabase("QSQLITE");
            //qDebug()<<"新建数据库";
            db->setDatabaseName(dbFile_Name);
        }

        QString name;
        {
            name = QSqlDatabase::database().connectionName();
            //qDebug()<<name;
        }//超出作用域，隐含对象QSqlDatabase::database()被删除。
        if(!db->open()){
            QMessageBox::warning(this,"警告","DB文件打开失败");
            return ;
        }
        QString Qquery = QString("SELECT * FROM DATA");
        QSqlQuery query;
        int flag=0;
        if(!query.exec(Qquery)){
            qDebug()<<"getdata data fail";
            return;
        }else{
            while(query.next()){
                pdialog.setValue(double(line)/double(lines)*100);//读数据所花时间较长
                //qDebug()<<double(line)/double(lines)*100;
                QCoreApplication::processEvents(); //防止界面冻结
                if(pdialog.wasCanceled()) return;
                line++;
                QString str;
                str += query.value(0).toString();
                str += " ";
                str += query.value(1).toString();
                str += " ";
                str += query.value(2).toString();
                str += " ";
                str += query.value(3).toString();
                str += " ";
                str += query.value(4).toString();
                str += " ";
                str += query.value(5).toString();
                str += " ";
                str += query.value(6).toString();

                if(flag==(str.section(" ",7,7).toInt())){
                    auto str1=str.section(" ",2,2).toDouble()*1;   //以空格问分界点，获取第一个到第一个的数据（从0开始计数）
                    double tau_temp=(16000*str1)/(pi*dia*dia*dia);
                    //qDebug()<<str1;
                    torque_per.push_back(str1);
                    tau_per.push_back(tau_temp);
                    auto str2=str.section(" ",3,3).toDouble();
                    str2=str2*pi/180;
                    double strain_temp=str2*(dia/2)/length;
                    //qDebug()<<str2;
                    angle_per.push_back(str2);
                    strain_per.push_back(strain_temp);
                }
                else{
                    if(!tau_per.empty()){
                        tau.push_back(tau_per);
                        tau_per.clear();
                        strain.push_back(strain_per);
                        strain_per.clear();
                        torque.push_back(torque_per);
                        torque_per.clear();
                        angle.push_back(angle_per);
                        angle_per.clear();
                    }
                    flag=str.section(" ",7,7).toInt();
                    double str1=1*str.section(" ",2,2).toDouble();  //扭矩
                    double tau_temp=(16000*str1)/(pi*dia*dia*dia);
                    tau_per.push_back(tau_temp);
                    torque_per.push_back(str1);
                    auto str2=str.section(" ",3,3).toDouble();  //角度
                    str2=str2*pi/180;
                    double strain_temp=str2*(dia/2)/length;
                    strain_per.push_back(strain_temp);
                    angle_per.push_back(str2);
                }
            }

        }
        db->close();
        QSqlDatabase::removeDatabase(name);
    }
    //至此，将数据文件夹中扭矩角度数据存入容器 torque和angle中,并计算应力应变

    //在每一个加载周期内处理数据
    dataProcess();
    pdialog.setValue(100);



     pdialog.close();
     QMessageBox::information(this,"状态","载入数据完成");

}

void MainWindow::on_radioButton_jieti_clicked()
{
    isLadder = true;
}

void MainWindow::on_radioButton_hengfu_clicked()
{
    isLadder = false;
}
