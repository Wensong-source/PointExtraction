#ifndef DATA_H
#define DATA_H

#include <vector>
#include <QString>

using namespace std;
class Data
{
public:
    Data();
    static QString testSpeciment;
    static vector<double> maxTau_per;   //特征点A 每个周期的最大应力
    static vector<double> maxStrain_per;//特征点A 每个周期的最大应变
    static vector<double> maxTorquePer;   //对中前每个周期最大扭矩
    static vector<double> minTorquePer;
    static vector<double> maxAnglePer;
    static vector<double> minAnglePer;
    static vector<double> minTau_Per;   //特征点B 每个周期的最小应力
    static vector<double> minStrain_per;//特征点B 每个周期的最小应变
    static vector<double> negtivePlasticStrian_per;   //特征点C 每个周期的反向塑性应变
    //static vector<double> negtivePlasticTau_per;      //特征点C 每个周期的反向塑性应变对应应力
    static vector<double> postivePlasticStrain_per;   //特征点D 每个周期的正向塑性应变
    //static vector<double> postivePlasticTau_per;      //特征点D 每个周期的正向塑性应变对应应力
    static vector<double> negtiveInterTau_per;        //特征点E 每个周期的负向截距应力
    //static vector<double> negtiveInterStrain_per;     //特征点E 每个周期的负向截距应力对应应变
    static vector<double> postiveInterTau_Per;        //特征点F 每个周期的正向截距应力
    //static vector<double> postiveInterStrain_Per;     //特征点F 每个周期的正向截距应力对应应变
    static vector<double> shearModulus_left;    //剪切模量 G左侧
    static vector<double> shearModulus_right;   //剪切模量 G右侧
    static vector<vector<double> > tau_mid;    //对中后的应力
    static vector<vector<double> > strain_mid; //对中后的应变
    static vector<double> mid_tau;
    static int crackPoint;   //可能出现裂纹点
    static vector<double> plasticStrian_per;   //每个周期的塑性形变
    static vector<double> plasticStrian_per_denoise;  //去噪后的塑性形变
    static vector<double> damage;
    static vector<double> Pd;
    static vector<double> elasticStrainEnergy;
    static vector<int> indexA;
    static vector<int> indexB;
    static vector<int> indexE;
    static vector<int> indexF;

};

#endif // DATA_H
