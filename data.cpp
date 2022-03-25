#include "data.h"

Data::Data()
{

}
QString Data::testSpeciment;
vector<double> Data::maxTau_per;   //特征点A 每个周期的最大应力
vector<double> Data::maxStrain_per;//特征点A 每个周期的最大应变
vector<double> Data::minTau_Per;   //特征点B 每个周期的最小应力
vector<double> Data::minStrain_per;//特征点B 每个周期的最小应变
vector<double> Data::maxTorquePer;   //对中前每个周期最大扭矩
vector<double> Data::minTorquePer;
vector<double> Data::maxAnglePer;
vector<double> Data::minAnglePer;
vector<double> Data::negtivePlasticStrian_per;   //特征点C 每个周期的反向塑性应变
//vector<double> Data::negtivePlasticTau_per;      //特征点C 每个周期的反向塑性应变对应应力
vector<double> Data::postivePlasticStrain_per;   //特征点D 每个周期的正向塑性应变
//vector<double> Data::postivePlasticTau_per;      //特征点D 每个周期的正向塑性应变对应应力
vector<double> Data::negtiveInterTau_per;        //特征点E 每个周期的负向截距应力
//vector<double> Data::negtiveInterStrain_per;     //特征点E 每个周期的负向截距应力对应应变
vector<double> Data::postiveInterTau_Per;        //特征点F 每个周期的正向截距应力
//vector<double> Data::postiveInterStrain_Per;     //特征点F 每个周期的正向截距应力对应应变
vector<double> Data::shearModulus_left;    //剪切模量 G左侧
vector<double> Data::shearModulus_right;   //剪切模量 G右侧
vector<vector<double> > Data::tau_mid;    //对中后的应力
vector<vector<double> > Data::strain_mid; //对中后的应变
vector<double> Data::mid_tau;
int Data::crackPoint;
vector<double> Data::plasticStrian_per;   //每个周期的塑性形变
vector<double> Data::plasticStrian_per_denoise;  //去噪后的塑性形变
vector<double> Data::damage;
vector<double> Data::Pd;
vector<double> Data::elasticStrainEnergy;

vector<int> Data::indexA;
vector<int> Data::indexB;
vector<int> Data::indexE;
vector<int> Data::indexF;


