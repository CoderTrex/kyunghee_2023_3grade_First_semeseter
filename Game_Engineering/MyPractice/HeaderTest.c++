#include "cySampleElim.h"
#include "cyPolynomial.h"
#include <iostream>
#include <vector>
using namespace std;


//! 주어진 차수 `N`의 다항식을 `x`에서 평가합니다.
//! coef` 배열의 계수는 차수가 증가하는 순서여야 합니다.
void PolynomianlEval()
{
    // 다항식 2x^2 + 3x + 4의 계수를 나타내는 배열
    double coef[3] = {2.0, 3.0, 4.0};

    double x = 1.0;
    double result = cy::PolynomialEval<2>(coef, x);
    cout << "방정식 2x^2 + 3x + 4의 x = 1.0일 때 결과값: " << result << endl;
    
    x = 2.0;
    result = cy::PolynomialEval<2>(coef, x);
    cout << "방정식 2x^2 + 3x + 4의 x = 2.0일 때 결과값: " << result << endl;
}


void PolynomialEvalWithDeriv()
{
/*--------------------------------------------------------------------------------------------*/
    // //! 주어진 다항식과 그 도함수를 `x`에서 평가합니다.
    //! 이 함수는 다항식의 도함수를 계산할 필요가 없습니다,
    //! 하지만 다항식과 미리 계산된 도함수를 따로 계산하는 것보다 느립니다.
    //! 따라서 다항식과 그 도함수를 반복적으로 계산할 때는 사용하지 않는 것이 좋습니다.
    //! coef` 배열의 계수는 차수가 증가하는 순서로 정렬되어야 합니다.

    // 3차 방정식 2x^3 + 3x^2 + 4x + 5
    double coef[4] = {2.0, 3.0, 4.0, 5.0};
    double x = 1.0; // 방정식의 값의 도출을 위해 넣을 해
    double derivativeValue = 0.0; // 도함수 결과값
    double p = cy::PolynomialEvalWithDeriv<3>(derivativeValue, coef, x);
    cout << "2x^3 + 3x^2 + 4x + 5 | x = 1.0 | p(x) = " << p << endl;
    cout << "2x^3 + 3x^2 + 4x + 5 | x = 1.0 | p'(x) (이것은 미분 값을 계속해서 합한 값) = " << derivativeValue << endl;
}

// PolynomialDerivative 함수는 입력된 다항식의 도함수를 계산하여 
// 출력 배열 deriv에 저장하는 함수입니다.
void PolynomialDerivative()
{
    double deriv[3];
    double coef[4] = {4.0, 3.0, 2.0, 1.0};
    cy::PolynomialDerivative<3>(deriv, coef);
    for (int i = 0; i < 3; i++) {
        cout << deriv[i] << " ";
    }
    cout << endl;
}



void Polynomial_class()
{
    // 다항식 2x^3 + 3x^2 + 4x + 1 생성
    cy::Polynomial<double, 4> p({1, 4, 3, 2});

    // x = 2일 때 다항식 계산
    cout << "p(2) = " << p(2) << endl;

    // 다항식 덧셈
    cy::Polynomial<double, 4> q({3, 1, 5, 1});
    cy::Polynomial<double, 4> r = p + q;

    // 다항식 뺄셈
    cy::Polynomial<double, 4> s = p - q;

    // 다항식 곱셈
    cy::Polynomial<double, 8> t = p * q;

    // 다항식 출력
    // cout << "p(x) = " << p << endl;
    // cout << "q(x) = " << q << endl;
    // cout << "r(x) = " << r << endl;
    // cout << "s(x) = " << s << endl;
    // cout << "t(x) = " << t << endl;
}




int main()
{
    PolynomianlEval();
    PolynomialEvalWithDeriv();
    PolynomialDerivative();
    Polynomial_class();
}
