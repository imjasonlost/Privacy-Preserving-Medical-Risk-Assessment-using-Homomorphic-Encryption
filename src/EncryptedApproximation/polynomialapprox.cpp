#include "CalculationFunc.h"
#include<iostream>
#include <vector>
#include<cmath>
#include<fstream>
#include <chrono>
#include <thread>
#include <iomanip>
using namespace lbcrypto;
using namespace std;

int main(){
    uint32_t multDepth =60;
    uint32_t scaleModSize = 50;
    uint32_t batchSize = 8192;
   
    uint32_t firstModSize = 60; 
    uint32_t ringDim      =16384;
    SecurityLevel sl      = HEStd_NotSet;
    //uint32_t ringDim      = 8192;
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetBatchSize(batchSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetSecurityLevel(sl);
    parameters.SetRingDim(ringDim);
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);

    // Enable the features that you wish to use
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    cc->Enable(ADVANCEDSHE);
    cout<<endl;
    cout << "CKKS scheme is using ring dimension " << cc->GetRingDimension() << std::endl << std::endl;

    auto keys=cc->KeyGen();
    cc->EvalMultKeyGen(keys.secretKey);
    cc->EvalSumKeyGen(keys.secretKey);

  

   int n =8192; //Number of points
   //I have taken [-0.1, 0.1], [-1,1], [0.1,1], [0.5,1] these ranges to do my experiment
   //Each generate different dataframes and you have to find L2norm, MSE, Max error
   double start1=-0.1;
   double end=0.1;
   
   //Print out Most centered 20 values. 
   vector<double>points(n);
   double step=(end-start1)/(n-1);  //step size
   cout<<"The step value is "<<step<<endl;
   for (int i=0; i<n; i++){
       if (i==0){
           points[i]=start1;
       }
       else {
           points[i]=points[i-1]+step;
       }
   }
vector<double>point;
for (int j=10; j>0; --j){
    double a;
    a=points[n/2-j];
    point.push_back(a);
}
for (int j=0; j<10; ++j){
    double a;
    a=points[n/2+j];
    point.push_back(a);
}
cout<<"The most centered 20 values are "<<endl;
for (double x:point){
    cout<<x<< ", ";
}
cout<<endl;
cout<<endl;

//Pack & Encrypt   
Plaintext plaintext=cc->MakeCKKSPackedPlaintext(points);
auto ciphertext=cc->Encrypt(keys.publicKey,plaintext); 

//Print size of input
cout<<"Number of Elements in the vector:  "<<points.size()<<endl;
    
    

//Apply sign function to approximate between -1 to 1 
auto start = chrono::high_resolution_clock::now(); // start timer
auto signedCiphertext= Sign(cc,ciphertext); 

//Decrypt & Print  
Plaintext result;
cc->Decrypt(keys.secretKey, signedCiphertext,&result);
result->SetLength(points.size());
cout<<"Approximated Results Ch(n) & f6(n) "<<result <<endl;
auto stop = chrono::high_resolution_clock::now();
auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
std::cout << "Time Taken for calculations:" << duration.count() << " ms" << endl;
cout<<"Multiplicative Level Used "<<signedCiphertext->GetLevel()<<endl;

//L2_norm, L-infinity norm, MSE
//Output actual signum function output
vector<double>y_actual;

for (double val:points){
  y_actual.push_back(signum(val));
}
ofstream file("signum_output.csv");
file<<"Value, Signum\n";
for(size_t i=0; i<points.size(); ++i){
    file<<points[i]<<","<<y_actual[i]<<"\n";
}
file.close();
cout<<"Data saved to signum_output.csv\n";

//Save decrypted values to file. 

//to know type of variable , cout<<typeid(result).name()<<endl;
// to know lenght of variable cout<<result->GetLength()<<endl;
//to know encoding type , cout<<result->GetEncodingType()<<endl;

//Generate approximate dataframe
vector<complex<double>>values;
values=result->GetCKKSPackedValue();

vector<double>y_predicted;
for (const auto & val:values) {
    y_predicted.push_back(val.real());
}
ofstream file1("approximated_output.csv");
file1<<"Value, Approximated\n";
for(size_t i=0; i<points.size(); ++i){
    file1<<points[i]<<","<<y_predicted[i]<<"\n";
}
file1.close();
cout<<"Data saved to approximated_output.csv\n";

 
return 0;
}
Ciphertext<DCRTPoly> Sign(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input)
{
   
    auto output=input;

//From here different combination of functions have been used for different ranges
//Only one composite combination can be run at a time. SO it output two dataframe, one with actual signum, other 
//with encrypted approximated dataframe. Then we decrypt to compare with actual. 

 //*************For difference(a-b) values [0.5,1], select input values range above [o.5,1]
   
 /*for (int i=0; i<1; i++){
       output=Ch_5(cc,output);
        }

    for (int i = 0; i <1; i++) {
        output= f_6(cc, output);
   } 
    return output; 
*/
 /*
 //For difference(a-b) values [0.1,1], select input values range above [0.1,1]
  for (int i=0; i<1; i++){
       output=Ch_5(cc,output);
        }

    for (int i = 0; i <2; i++) {
        output= f_6(cc, output);
   } 
    return output;
 */

  /*
 //For difference(a-b) values [-0.1,0.1], 10^-2 precision, select input values range above [-0.1,0.1]
  for (int i=0; i<3; i++){
       output=Ch_5(cc,output);
        }

    for (int i = 0; i <2; i++) {
        output= f_6(cc, output);
   } 
    return output;
 */

   /*
 //For difference(a-b) values [-0.1,0.1], 10^-3 precision, select input values range above [-0.1,0.1]
  for (int i=0; i<2; i++){
       output=Ch_5(cc,output);
        }
  for (int i=0, i<2, i++){
      output=Ch_9(cc,output)
  }
    for (int i = 0; i <2; i++) {
        output= f_6(cc, output);
   } 
    return output;
 */

   /*
 //For difference(a-b) values [-0.1,0.1], 10^-4 precision, select input values range above [-0.1,0.1]
  for (int i=0; i<1; i++){
       output=Ch_5(cc,output);
        }
  for (int i=0, i<4, i++){
      output=Ch_9(cc,output)
  }
    for (int i = 0; i <2; i++) {
        output= f_6(cc, output);
   } 
    return output;
 */
   
 //For difference(a-b) values [-0.1,0.1], 10^-5 precision, select input values range above [-0.1,0.1]
 
  for (int i=0, i<6, i++){
      output=Ch_9(cc,output)
  }
    for (int i = 0; i <2; i++) {
        output= f_6(cc, output);
   } 
    return output;
 
}




Ciphertext<DCRTPoly> Ch_5(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input)
{
   vector<double>coefficients={0.00,3.278671362,0.0,-5.828730143,0.0,3.497226718};
    auto output = cc->EvalPoly(input, coefficients);
    return output;
}

Ciphertext<DCRTPoly> Ch_9(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input)
{
    std::vector<double> coefficients ={0.000000000, 5.464455744, 0.000000000, -29.143706275, 0.000000000, 73.441994378, 0.000000000, -79.936706316, 0.000000000, 31.086435332}
    ;
   
    auto output = cc->EvalPoly(input, coefficients);
    return output;
}



Ciphertext<DCRTPoly> f_6(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input)
{
  
    std::vector<double> coefficients = {0.0,2.9326171875,0.0,-5.865234375,0.0,8.7978515625,0.0,-8.37890625,0.0,4.8876953125,0.0,-1.599609375,0.0,0.2255859375};
    auto output = cc->EvalPoly(input, coefficients);
    return output;
}

int signum (double x){
    if(x>0) return 1;
    else if (x<0) return -1;
    else return 0;
}


