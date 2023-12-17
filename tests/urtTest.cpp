#ifndef URT_TEST
#define URT_TEST

#include <gtest/gtest.h>
#include "URT.hpp"

TEST(OLSTest, CheckDataWritingAndFitExecution) {
    int nrows = 1000;
    int ncols = 10;

    // generating random arrays
    urt::Vector<double> y = urt::wiener_process<double>(nrows);
    urt::Matrix<double> x = urt::wiener_process<double>(nrows, ncols);

    // adding intercept to the matrix of independent variables
    urt::add_intercept(x);

    urt::OLS<double> fit(y, x, true);

    // writing data to CSV files

    const std::string yFilePath = "../data/tests/y.csv";
    const std::string xFilePath = "../data/tests/x.csv";
    const std::string olsFilePath = "../data/tests/ols.csv";

    ASSERT_NO_THROW(urt::WriteToCSV(yFilePath, y));
    ASSERT_NO_THROW(urt::WriteToCSV(xFilePath, x));

    // running OLS regression
}

#endif