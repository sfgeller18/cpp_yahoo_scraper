#ifndef EIGEN_TEST_H
#define EIGEN_TEST_H

#include "gtest/gtest.h"
#include <eigen3/Eigen/Dense>


TEST(EigenMKLTest, MatrixMultiplication) {
    // Set the seed for reproducibility
    srand(123);

    Eigen::MatrixXd A = Eigen::MatrixXd::Random(20, 20);
    Eigen::MatrixXd B = Eigen::MatrixXd::Random(20, 20);

    // Perform matrix multiplication
    Eigen::MatrixXd result = A * B;

    // Check if the result is correct
    Eigen::MatrixXd expected = A * B;
    ASSERT_TRUE(result.isApprox(expected));  // Check if the matrices are approximately equal
}

#endif

