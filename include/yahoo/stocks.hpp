#ifndef YAHOO_STOCKS_H
#define YAHOO_STOCKS_H

#define QT_NO_DEBUG_OUTPUT

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
#include <boost/algorithm/string/case_conv.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <filesystem>

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
            {"Timestamp", 0},
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

json stockPacketParser(std::string& packet,std::string errorBuffer) {
    json jsonData;
    json quotes;
    try {
        jsonData = json::parse(packet);

        // Access the "indicators" array under "chart" -> "result"
        if (jsonData.contains("chart") && jsonData["chart"].contains("result")) {
            if (!jsonData["chart"]["result"].empty()) {
                quotes = jsonData["chart"]["result"][0]["indicators"]["quote"][0];

                // Check if "timestamps" array exists
                if (jsonData["chart"]["result"][0].contains("timestamp")) {
                    const json& timestamps = jsonData["chart"]["result"][0]["timestamp"];
                    quotes["timestamp"] = timestamps;
                }
            }
        }
    } catch (const json::parse_error& e) {
        errorBuffer = "Invalid Query: " + std::string(e.what());
    }

    // Return the modified quotes or an empty JSON object
    return quotes;
}


void printObj(const json& jsonData, const std::string& filePath, std::string errorBuffer) {
    std::ofstream csvFile(filePath);
    try {
        if (!csvFile.is_open()) {
            throw std::runtime_error("Error: Unable to open file for writing: " + filePath);
        }

        csvFile << "timestamp, close,low,volume,open,high" << std::endl;

        auto& timestampArray = jsonData["timestamp"];
        auto& closeArray = jsonData["close"];
        auto& lowArray = jsonData["low"];
        auto& volumeArray = jsonData["volume"];
        auto& openArray = jsonData["open"];
        auto& highArray = jsonData["high"];

        size_t arraySize = closeArray.size();

        // Iterate over indices and print values to the CSV file
        for (size_t i = 0; i < arraySize; ++i) {
            csvFile << timestampArray[i] << ", " << closeArray[i] << ","
                    << lowArray[i] << ","
                    << volumeArray[i] << ","
                    << openArray[i] << ","
                    << highArray[i] << std::endl;
        }

        // Check for any writing errors
        if (!csvFile.good()) {
            throw std::runtime_error("Error: Writing to CSV file failed.");
            errorBuffer = "Error: Writing to CSV file failed.";
        }
    } catch (const std::exception& e) {
         errorBuffer = "Exception caught: " + std::string(e.what()); 
    } catch (...) {
        errorBuffer = "Unknown Exception caught in printObj "; 
    }
}




json GetHistoricalPrices(const std::string& symbol, const std::string& startDate, std::string endDate, std::string interval, std::string& errorBuffer) {

        if (startDate == endDate) {return json("");}

        std::string ss1 = timestamper(startDate); 
        std::string ss2 = timestamper(endDate); 
        
        std::string url = "https://query1.finance.yahoo.com/v8/finance/chart/"
                + boost::to_upper_copy(symbol)
                + "?symbol="
                + boost::to_upper_copy(symbol)
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
                errorBuffer = "Error in curl_easy_perform: " + std::string(curl_easy_strerror(res));
                // Handle the error, possibly by returning an empty string or throwing an exception
                curl_easy_cleanup(curl);
                return "";
            }
            // Cleanup
            curl_easy_cleanup(curl);
        }
        return stockPacketParser(responseBuffer, errorBuffer);
    } 

void downloadCSV(const std::string& symbol, const std::string& period1, const std::string& period2, const std::string& interval, std::string& errorBuffer, std::string filePath = ""){
        if (filePath.empty()) {filePath = "../data/" + boost::to_upper_copy(symbol)+ period1 + "_"+ period2 + "_"+ interval+".csv";}
        json data = GetHistoricalPrices(symbol, period1, period2, interval, errorBuffer);
        if (errorBuffer!="") {return;}
        printObj(data, filePath, errorBuffer);
}

void downloadCSVtoCloud(
    std::string symbol,
    std::string period1,
    std::string period2,
    std::string interval,
    mongocxx::collection& collection, std::string& errorBuffer
) {
    try {
        // Get historical prices data
        json data = GetHistoricalPrices(symbol, period1, period2, interval, errorBuffer);

        // Ensure data is not empty
        if (data.empty()) {
            errorBuffer += "Error: Empty data received.";
            return;
        }
        if (errorBuffer != "") {return;}

        auto& timestampArray = data["timestamp"];
        auto& closeArray = data["close"];
        auto& lowArray = data["low"];
        auto& volumeArray = data["volume"];
        auto& openArray = data["open"];
        auto& highArray = data["high"];

        // Create a BSON document to store historical prices data
bsoncxx::builder::stream::document document;

// Create arrays for each label
auto nameArrayBuilder = bsoncxx::builder::basic::array{};
nameArrayBuilder.append(boost::to_upper_copy(symbol));
nameArrayBuilder.append(period1);
nameArrayBuilder.append(period2);
nameArrayBuilder.append(interval);

auto timestampArrayBuilder = bsoncxx::builder::basic::array{};
auto closeArrayBuilder = bsoncxx::builder::basic::array{};
auto lowArrayBuilder = bsoncxx::builder::basic::array{};
auto volumeArrayBuilder = bsoncxx::builder::basic::array{};
auto openArrayBuilder = bsoncxx::builder::basic::array{};
auto highArrayBuilder = bsoncxx::builder::basic::array{};

// Iterate over the arrays and add values to the arrays
for (size_t i = 0; i < closeArray.size(); ++i) {
    bool errorFlag = false;
    try {
        int64_t timestamp = timestampArray[i].get<int64_t>();
        double close = closeArray[i].get<double>();
        double low = lowArray[i].get<double>();
        double volume = volumeArray[i].get<double>();
        double open = openArray[i].get<double>();
        double high = highArray[i].get<double>();

        // Append values to the arrays
        timestampArrayBuilder.append(bsoncxx::types::b_int64{timestamp});
        closeArrayBuilder.append(bsoncxx::types::b_double{close});
        lowArrayBuilder.append(bsoncxx::types::b_double{low});
        volumeArrayBuilder.append(bsoncxx::types::b_double{volume});
        openArrayBuilder.append(bsoncxx::types::b_double{open});
        highArrayBuilder.append(bsoncxx::types::b_double{high});

        // Add other fields accordingly
    } catch (const std::invalid_argument& e) {
        errorBuffer += "Error: Failed to convert string to numeric value. Skipping entry.";
        errorFlag = true;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Numeric value out of range. Skipping entry." << std::endl;
        errorFlag = true;
    }
    if (errorFlag == true) {continue;} //Skip to next numeric value on conversion failure
}

// Add the arrays to the main document
document << "ID:" << nameArrayBuilder
         << "timestamp" << timestampArrayBuilder
         << "close" << closeArrayBuilder
         << "low" << lowArrayBuilder
         << "volume" << volumeArrayBuilder
         << "open" << openArrayBuilder
         << "high" << highArrayBuilder;

// Insert the BSON document into the MongoDB collection
collection.insert_one(document.view());

std::cout << "Data inserted into MongoDB successfully." << std::endl;

    } catch (const std::invalid_argument& e) {
        errorBuffer += "Error: Failed to convert string to numeric value. Skipping entry.";
        return;
    } catch (const std::out_of_range& e) {
        errorBuffer += "Error: Numeric value out of range. Skipping entry.";
        return;
    } 
}

std::vector<long double> getCol(std::string symbol,
        std::string period1,
        std::string period2,
        std::string interval, std::string colName, std::string errorBuffer
        ) {
            std::string responseBuffer = GetHistoricalPrices(symbol, period1, period2, interval, errorBuffer);
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
                    std::cerr << "Invalid argument: " << std::string(e.what()) << std::endl;
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