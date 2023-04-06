#include "cySampleElim.h"
#include <iostream>
#include <vector>
#include "Eigen/Dense"

using namespace Eigen;
using namespace cy;

int main()
{
    // Define the input points
    const int inputSize = 1000;
    std::vector<Vector3f> inputPoints(inputSize);
    for (int i = 0; i < inputSize; ++i) {
        inputPoints[i] = Vector3f(
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX
        );
    }

    // Define the output points
    const int outputSize = 500;
    std::vector<Vector3f> outputPoints(outputSize);

    // Create an instance of the WeightedSampleElimination class
    WeightedSampleElimination<Vector3f, float, 3> wse;

    // Set the bounds for the points
    Vector3f boundsMin(0, 0, 0);
    Vector3f boundsMax(1, 1, 1);
    wse.SetBoundsMin(boundsMin);
    wse.SetBoundsMax(boundsMax);

    // Eliminate points
    wse.Eliminate(
        inputPoints.data(),
        inputSize,
        outputPoints.data(),
        outputSize
    );

    // Print the output points
    for (int i = 0; i < outputSize; ++i) {
        std::cout << outputPoints[i] << std::endl;
    }

    return 0;
}