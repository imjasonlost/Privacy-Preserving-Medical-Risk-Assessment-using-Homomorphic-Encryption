#include "CalculationFunc.h"
#include<iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <memory>
#include <thread>
#include <string>
#include "openfhe.h"
using namespace lbcrypto;
using namespace std;

int main () {
    //Declare a vector to hold 64 flot values
    vector<double>values={0,1,0,2,3,4,4,5,5,0,1,2,2,4,3,2,3,2,1,3,5,6,5,3,4,5,4,4,2,0,0,1,0,1,0,2,3,4,4,5,5,0,1,2,2,4,3,2,3,2,1,3,5,6,5,3,4,5,4,4,2,0,0,1};
    cout<<"\n The values are: ";
    for (const double &value:values){
        cout<<value<<" ";
    }
    cuot<<endl;

    //Step1: Setup CryptoContext for CKKS
    ScalingTechnique scTech=FLEXIBLEAUTO;
    uint32_t multDepth=50;
    if(scTech==FLEXIBLEAUTOEXT)
         multDepth +=1;
    uint32_t scaleModSize=50;
    uint32_t firstModSize=60;
    uint32_t ringDim=8192;
    uint32_t slots=64; //Sparsely Packed
    uint32_t batchSize=slots;

    CCParams<CryptoContextCKKSRNS>parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetBatchSize(batchSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetRingDim(ringDim);
//  parameters.SetKeySwitchTechnique(HYBRID)
    CryptoContext<DCRTPoly>cc=GenCryptoContext(parameters);
//Enable the features that you gonna use
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    cc->Enable(ADVANCEDSHE);
    cout<<endl;
    cout << "CKKS scheme is using ring dimension " << cc->GetRingDimension() << std::endl << std::endl;
 //Step2:Key Generation
    auto keys=cc->KeyGen();
    Plaintext plaintextData=cc->MakeCKKSPackedPlaintext(values);
    auto encryptedData=cc->Encrypt(keys.publicKey,plaintextData);
// Generate evaluation sum,mult evaluation key
    cc->EvalMultKeyGen(keys.secretKey);
    cc->EvalSumKeyGen(keys.secretKey);
    auto signedCipher=Sign(cc,encryptedData);
    auto start=chrono::high_resolution_clock::now(); //start timer to record time for process
    Plaintext result;

    //add 1 to normalize | see step 8 in algorithm of paper
    auto signedCiphetext1=cc->EvalAdd(signedCipher,1);
    auto signedCiphertext=Sign_flag(cc,signedCiphetext1);

    //just to display values and check
    cc->Decrypt(keys.secretKey,signedCipher, &result);
    cout<<"Signed output of  X-a "<<result<<endl;

    cc->Decrypt(keys.secretKey, signedCiphertext1, &result);
    cout<<"Signed output of (X-a)+1 "<<result<<endl;

    cc->Decrypt(keys.secretKey, signedCiphertext,&result);
    cout<<"Flag value when multiplied by 1/2 & signed value "<<result<<endl;

    //x-b step 9 of algorithm
    auto signedCipher1=Sign_direct(cc,encryptedData);
    cc->Decrypt(keys.secretKey,signedCipher1,&result);
    cout<<"Signed output of X-b "<<result<<endl;

    //Add -1 and then 2
    auto signedCipher2=cc->EvalAdd(signedCipher1,-1);
    cc-Decrypt(keys.secretKey,signedCipher2,&result);
    cout<<"Signed output of x-b minus 1 "<<result<<endl;

    //Calculate sign of x-b again to colapse value
    auto cipherX1=Sign_flag(cc,signedCipher2);
    auto cipherY1=cc->EvalAdd(cipherX1,2);
    cc->Decrypt(keys.secretKey,cipherY1,&result);
    cout<<"Signed output of average x-b minus 1 plus 2 "<<result<<endl;

    //Multiply Both flag to get final Risk score
    auto Multiop=cc->EvalMult(cipherY1,signedCiphertext);
    cout<<"Multiplicative Level Used "<<Multiop->GetLevel()<<endl;
    cc->Decrypt(keys.secretKey,Multiop,&result);
    cout<<"Final Risk score at ClientEnd "<<result<<endl;

    auto stop=chrono::high_resolution_clock::now();
    auto duration=chrono::duration_cast<chrono::milliseconds>(stop-start);
    cout<<"Time taken while calculating Risk score: "<<duration.count()<< " milliseconds"<<endl;
    return 0;
}

Ciphertext<DCRTPoly>Sign(CryptoContext<DCRTPoly>cc, Ciphertext<DCRTPoly>input) {
    auto output=Scaling(cc,input);

    //Apply Chebyshev polynomials to converge
    for (int i=0; i<2; i++) {
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
    for (int i=0; i<2; i++) {
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
    double inpt;
    cout<<"Enter lower Threshold 'a' "<<endl; // However it will be always 1 for a in kHorana test, we can take directly
    cin>>inpt;
    const double max=(5*inpt);
    auto output=cc->EvalSub(input, inpt);
    auto output=cc->EvalMult(b/max, output);
    return output;
}

Ciphertext<DCRTPoly>Scaling1(CryptoContext<DCRTPoly>cc, Ciphertext<DCRTPoly>input){
    const double b=1.0;
    double inpt;
    cout<<"Enter lower Threshold 'b' "<<endl; // However it will be always 3 for a in kHorana test, we can take directly
    cin>>inpt;
    const double max=(2*inpt);
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


