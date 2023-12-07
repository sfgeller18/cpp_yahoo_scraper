#ifndef YAHOO_STOCKS_H
#define YAHOO_STOCKS_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <timefuncs.hpp>
#include <csv_funcs.hpp>

using json = nlohmann::json;

class Quote {
    std::string ticker;
    std::time_t startDate;
    std::time_t endDate;
    std::string interval;
};

namespace yahoo {

    namespace stockprice {
        const std::unordered_map<std::string, size_t> columnNameToValue = {
            {"Date", 0},
            {"Open", 1},
            {"High", 2},
            {"Low", 3},
            {"Close", 4},
            {"Adj Close", 5},
            {"Volume", 6}
        };

    inline size_t index(const std::string& columnName) {
        auto it = columnNameToValue.find(columnName);
        return (it != columnNameToValue.end()) ? it->second : (std::cerr << "Invalid column name: " << columnName << std::endl, -1);
    }

json stockPacketParser(std::string& packet) {
        json jsonData;
        try {
            jsonData = json::parse(packet);

            // Access the "indicators" array under "chart" -> "result"
            if (jsonData.contains("chart") && jsonData["chart"].contains("result")) {
                const auto& results = jsonData["chart"]["result"];
                for (const auto& result : results) {
                    if (result.contains("indicators")) {
                        const auto& indicators = result["indicators"];
                        std::cout << "Found indicators array: " << std::endl;
                    }
                }
            }
        } catch (const json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
        return jsonData;
    }



void printObj(const json& jsonData, const std::string& filePath) {
    std::ofstream csvFile(filePath);
    if (!csvFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << filePath << std::endl;
        return;
    }
        csvFile << "close,low,volume,open,high" << std::endl;
        auto& closeArray = jsonData["chart"]["result"][0]["indicators"]["quote"][0]["close"];
        auto& lowArray = jsonData["chart"]["result"][0]["indicators"]["quote"][0]["low"];
        auto& volumeArray = jsonData["chart"]["result"][0]["indicators"]["quote"][0]["volume"];
        auto& openArray = jsonData["chart"]["result"][0]["indicators"]["quote"][0]["open"];
        auto& highArray = jsonData["chart"]["result"][0]["indicators"]["quote"][0]["high"];

        // Check if all arrays have the same size
        size_t arraySize = closeArray.size();
        if (lowArray.size() != arraySize || volumeArray.size() != arraySize ||
            openArray.size() != arraySize || highArray.size() != arraySize) {
            std::cerr << "Error: Arrays in JSON data have different sizes." << std::endl;
            return;

        }
        if (!csvFile.is_open()) {
            std::cerr << "Error: Unable to open file for writing: output.csv" << std::endl;
            return;
        }

        // Iterate over indices and print values to the CSV file
        for (size_t i = 0; i < arraySize; ++i) {
            csvFile << closeArray[i] << ","
                    << lowArray[i] << ","
                    << volumeArray[i]<< ","
                    << openArray[i] << ","
                    << highArray[i]<< std::endl;
        }

        // Check for any writing errors
        if (!csvFile.good()) {
            std::cerr << "Error: Writing to CSV file failed." << std::endl;
            return;
        }
    } 

json GetHistoricalPrices(
        std::string symbol,
        std::string startDate,
        std::string endDate,
        std::string interval
    ) {

        std::string ss1 = timestamper(startDate); 
        std::string ss2 = timestamper(endDate); 
        
        std::string url = "https://query1.finance.yahoo.com/v8/finance/chart/"
                + symbol
                + "?symbol="
                + symbol
                + "&period1=" + ss1
                + "&period2=" + ss2
                + "&interval=" + interval;

        std::cout<<url<<std::endl;

        CURL* curl = curl_easy_init();
        std::string responseBuffer;

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            // Write result into the buffer
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

            // Perform the request
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "Error in curl_easy_perform: " << curl_easy_strerror(res) << std::endl;
                // Handle the error, possibly by returning an empty string or throwing an exception
                curl_easy_cleanup(curl);
                return "";
            }
            // Cleanup
            curl_easy_cleanup(curl);
            return stockPacketParser(responseBuffer);
        }

    } 

void downloadCSV(std::string symbol, std::string period1, std::string period2,std::string interval, std::string filePath = ""){
        json data = GetHistoricalPrices(symbol, period1, period2, interval);
        if (filePath.empty()) {filePath = "../data/" + symbol + "_historical.csv";}
        printObj(data, filePath);
        }

std::vector<long double> getCol(std::string symbol,
        std::string period1,
        std::string period2,
        std::string interval, std::string colName
        ) {
            std::string responseBuffer = GetHistoricalPrices(symbol, period1, period2, interval);
            std::istringstream bufferStream(responseBuffer);
            std::vector<long double> closeValues;
            size_t n = index(colName);
            // Parse each line and extract the close value
            std::string line;
            bool skipFirstLine = true;
            while (std::getline(bufferStream, line)) {
                if (skipFirstLine) {
                skipFirstLine = false;
                continue;
            }
                std::istringstream lineStream(line);
                std::string token;

                // Skip columns until the Close column
                for (int i = 0; i < n; ++i) {
                    std::getline(lineStream, token, ',');
                }

                // Extract the Close value and convert it to double
                std::getline(lineStream, token, ',');
                try {closeValues.push_back(std::stold(token));} catch (const std::invalid_argument& e) {
                    std::cerr << "Invalid argument: " << e.what() << std::endl;
                }
            }
            return closeValues;
        }

        inline std::vector<long double> readCol(const std::string& filePath, const std::string& colName) {
            size_t ind = index(colName);
            return csv::readCol(filePath, ind);
        }

    }

}

#endif