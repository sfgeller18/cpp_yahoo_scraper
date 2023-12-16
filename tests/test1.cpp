#include "gtest/gtest.h"
#include <Eigen/Dense>

TEST(EigenMKLTest, MatrixMultiplication) {
    // Create two matrices
    Eigen::MatrixXd A(2, 2);
    A << 1, 2,
         3, 4;

    Eigen::MatrixXd B(2, 2);
    B << 5, 6,
         7, 8;

    // Perform matrix multiplication
    Eigen::MatrixXd result = A * B;

    // Define the expected result
    Eigen::MatrixXd expected(2, 2);
    expected << 19, 22,
                43, 50;

    // Check if the result matches the expected value
    ASSERT_EQ(result, expected);
}


