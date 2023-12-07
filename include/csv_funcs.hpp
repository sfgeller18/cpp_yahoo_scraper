#pragma once

#include <vector>
#include <iostream>
#include <sstream>

size_t writeCallback(char *content, size_t size, size_t nmemb, void *userdata) {
    // Append the content to user data
    ((std::string*)userdata)->append(content, size * nmemb);

    // Return the real content size
    return size * nmemb;
}

namespace csv {
    std::vector<long double> readCol(std::string filePath, const int& index) {
            std::ifstream file(filePath);
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << filePath << std::endl;
                return {};
            }

            std::vector<long double> colValues;
            int columnIndex = index;
            std::vector<long double> result;
            bool skipFirstLine = true;
            std::string line;
            while (std::getline(file, line)) {
                    if (skipFirstLine) {
                    skipFirstLine = false;
                    continue;
                }
                    std::istringstream lineStream(line);
                    std::string token;

                    // Skip columns until the Close column
                    for (int i = 0; i < columnIndex; ++i) {
                        std::getline(lineStream, token, ',');
                    }

                    // Extract the Close value and convert it to double
                    std::getline(lineStream, token, ',');
                    try {result.push_back(std::stold(token));} catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
            // Handle the error, possibly by skipping the current line or using a default value
        }
                }

                return result;
            }
}