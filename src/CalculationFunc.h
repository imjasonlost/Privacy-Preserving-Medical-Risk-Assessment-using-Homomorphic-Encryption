//Import OpenFHE library
#include "openfhe.h"

using namespace lbcrypto;
//Chebyshev functions
Ciphertext<DCRTPoly> Ch_9(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input);
Ciphertext<DCRTPoly> Ch_5(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input);
//
Ciphertext<DCRTPoly> f_6(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input);
Ciphertext<DCRTPoly> Sign(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input);
Ciphertext<DCRTPoly> Scaling(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input);
Ciphertext<DCRTPoly> Scaling1(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input);
Ciphertext<DCRTPoly> Sign_direct(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input);
Ciphertext<DCRTPoly> Sign_flag(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input);




Ciphertext<DCRTPoly> Sign_Precision(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> input, double x);
int signum (double x);