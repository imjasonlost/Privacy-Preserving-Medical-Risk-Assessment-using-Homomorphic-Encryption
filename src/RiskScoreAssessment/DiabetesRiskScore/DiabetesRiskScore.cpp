#include "CalculationFunc.h"
#include<iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <memory>
#include <thread>
#include <string>
#include "openfhe.h"

#include "diabetes.h" //getting values from CSV data header file ready

using namespace lbcrypto;
using namespace std;


void readCSV(const string&filename);

int main() {
    readCSV("Dabetics-dataset.csv"); // Data can be downloaded from this link https://data.mendeley.com/datasets/3snnp89967/1/files/4a93694a-ba52-42dc-b5dd-cc79c4b63a4f
    //readCSV("synthetic_hba1c.csv"); 

    vector<double>value512;
    for (size_t i=0; i<512 && i<values.size(); ++i){
    value512.push_back(values[i]);
    }
   // Step 1: Setup CryptoContext for CKKS
    ScalingTechnique scTech = FLEXIBLEAUTO;
    uint32_t multDepth      = 30;
    if (scTech == FLEXIBLEAUTOEXT)
        multDepth += 1;
    uint32_t scaleModSize = 50; //50
    uint32_t firstModSize = 60; //60
    uint32_t ringDim      = 8192;
    SecurityLevel sl      = HEStd_NotSet;
    //BINFHE_PARAMSET slBin = TOY;
   // uint32_t logQ_ccLWE   = 25;
    uint32_t slots        = 512; //6;  // sparsely-packed
    uint32_t batchSize    = slots;

    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetScalingTechnique(scTech);
    parameters.SetSecurityLevel(sl);
    parameters.SetRingDim(ringDim);
    parameters.SetBatchSize(batchSize);
    parameters.SetSecretKeyDist(UNIFORM_TERNARY);
    parameters.SetKeySwitchTechnique(HYBRID);
    parameters.SetNumLargeDigits(3);

    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);

    // Enable the features that you wanna use
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    cc->Enable(ADVANCEDSHE);
    cc->Enable(SCHEMESWITCH);
    cout << "CKKS scheme is using ring dimension " << cc->GetRingDimension() << std::endl << std::endl;

    // Step 2: Key Generation
	
    auto keys = cc->KeyGen();
   
Plaintext plaintextData = cc->MakeCKKSPackedPlaintext(value512);

auto encryptedData= cc->Encrypt(keys.publicKey, plaintextData);

//Generate evaluation sum evaluation key 
cc->EvalSumKeyGen(keys.secretKey);
cc->EvalMultKeyGen(keys.secretKey);

auto signedCipher = Sign(cc, encryptedData);
auto start = chrono::high_resolution_clock::now(); // start timer

Plaintext result;
//add 1 to normalize //add 1 to normalize | see step 8 in algorithm of paper
auto signedCiphertext1=cc->EvalAdd(signedCipher, 1);
auto signedCiphertext=Sign_flag(cc,signedCiphertext1);

//x-b step 9 of algorithm
auto signedCipher1=Sign_direct(cc,encryptedData);

//Add -1 and then 2
auto signedCipher2=cc->EvalAdd(signedCipher1,-1);
//calculate sign of x-b again to colapse
auto cipherX1=Sign_flag(cc, signedCipher2);
auto cipherY1=cc->EvalAdd(cipherX1,2);

//Multiply with flag
auto Multiop=cc->EvalMult(cipherY1,signedCiphertext);
cout<<"Multiplicative Level Used "<<Multiop->GetLevel()<<endl;

cc->Decrypt(keys.secretKey,Multiop,&result);

//Save result to csv file. 
//In CKKKS result are placed in complex number form so we have to extract only real part.
vector<complex<double>>realimag;
realimag=result->GetCKKSPackedValue();
vector<double>realvalue;
for (const auto & val:realimag){
    realvalue.push_back(val.real());
}

//We are generating output CSV file of value and corresponding Risk score
ofstream file("Output.csv");
file<<"Value, Risk_Score\n";
for (size_t i=0; i<value512.size(); ++i){
    file<<value512[i]<<","<<realvalue[i]<<"\n";
}
file.close();
cout<<"Data saved to Output.csv\n"<<endl;

//cout<<"Final output value at UserEnd "<<result<<endl; 

auto stop = chrono::high_resolution_clock::now();
auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

std::cout << "Time Taken for calculations:" << duration.count() << " ms" << endl;

return 0;
}

Ciphertext<DCRTPoly>Sign(CryptoContext<DCRTPoly>cc, Ciphertext<DCRTPoly>input) {
    auto output=Scaling(cc,input);

    //Apply Chebyshev polynomials to converge
    for (int i=0; i<3; i++) {
        //output=Ch_9(cc,output);
          output=Ch_5(cc,output);
    }
    //Apply f6 to converge final result
    for (int i=0; i<2; i++){
        output=f_6(cc,output);
    }
    return output;
}
Ciphertext<DCRTPoly>Sign_direct(CryptoContext<DCRTPoly>cc, Ciphertext<DCRTPoly>input){
    auto output=Scaling1(cc,input );
      //Apply Chebyshev polynomials to converge
    for (int i=0; i<3; i++) {
        //output=Ch_9(cc,output);
          output=Ch_5(cc,output);
    }
    //Apply f6 to converge final result
    for (int i=0; i<2; i++){
        output=f_6(cc,output);
    }
    return output;
}
Ciphertext<DCRTPoly>Sign_flag(CryptoContext<DCRTPoly>cc, Ciphertext<DCRTPoly>input){
    auto output=cc->EvalMult(input,0.5);
      //Apply Chebyshev polynomials to converge
    for (int i=0; i<1; i++) {
        //output=Ch_9(cc,output);
          output=Ch_5(cc,output);
    }
    //Apply f6 to converge final result
    for (int i=0; i<1; i++){
        output=f_6(cc,output);
    }
    return output;
}
Ciphertext<DCRTPoly>Scaling(CryptoContext<DCRTPoly>cc, Ciphertext<DCRTPoly>input){
    const double b=1.0;
    double inpt=5.7;
    //cout<<"Enter lower Threshold 'a' "<<endl; // However it will be always 1 for a in kHorana test, we can take directly
    //cin>>inpt;
    const double max=10;
    auto output=cc->EvalSub(input, inpt);
    auto output=cc->EvalMult(b/max, output);
    return output;
}

Ciphertext<DCRTPoly>Scaling1(CryptoContext<DCRTPoly>cc, Ciphertext<DCRTPoly>input){
    const double b=1.0;
    double inpt=6.4;
    //cout<<"Enter lower Threshold 'b' "<<endl; // However it will be always 3 for a in kHorana test, we can take directly
   // cin>>inpt;
    const double max=10;
    auto output=cc->EvalSub(input, inpt);
    auto output=cc->EvalMult(b/max, output);
    return output;
}

//Chebyshev polynomials. 
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









