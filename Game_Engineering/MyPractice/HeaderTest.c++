#include "cySampleElim.h"
#include "cyPolynomial.h"
#include <iostream>
#include <vector>
using namespace std;


// An implementation of the weighted sample elimination algorithm 
// for generating Poisson Disk sample sets in any dimensions.

int main()
{
    constexpr int N = 3;
    double coef[N+1] = {2.0, 3.0, 4.0, 5.0};
    double x = 1.0;
    double derivativeValue = 0.0;
    double p = cy::PolynomialEvalWithDeriv<N>(derivativeValue, coef, x);
    
    cout << "p(x) = " << p << endl;
    cout << "p'(x) = " << derivativeValue << endl;
    
    return 0;
}