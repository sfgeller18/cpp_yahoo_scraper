#ifndef YAHOO_OPTIONS_H
#define YAHOO_OPTIONS_H

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

namespace yahoo{
    namespace options {

        const std::unordered_map<std::string, int> optionCols = {
            {"contractSymbol", 0},
            {"strike", 1},
            {"lastPrice", 2},
            {"currency", 3},
            {"change", 4},
            {"percentChange", 5},
            {"volume", 6},
            {"openInterest", 7},
            {"bid", 8},
            {"ask", 9},
            {"lastTradeDate", 12},
            {"impliedVolatility", 13},
            {"inTheMoney", 14}
        };

        inline int index(const std::string& columnName) {
            auto it = optionCols.find(columnName);
            return (it != optionCols.end()) ? it->second : (std::cerr << "Invalid column name: " << columnName << std::endl, -1);
        }

        inline std::vector<long double> readCol(const std::string& filePath, const std::string& colName) {
            int ind = index(colName);
            return csv::readCol(filePath, ind);
        }

        std::string GetJSON(const std::string& symbol, const std::string& origin_date) {
            std::string date = timestamper(origin_date); 
            CURL* curl = curl_easy_init();

            std::string url = "https://query1.finance.yahoo.com/v7/finance/options/" + symbol;
            url += "?date=" + date;

            std::string responseBuffer;

            try {
                if (curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

                    // Set the write callback function
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

                    // Perform the request
                    CURLcode res = curl_easy_perform(curl);

                    if (res != CURLE_OK) {
                        throw std::runtime_error("Curl request failed: " + std::string(curl_easy_strerror(res)));
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error in GetOptionChain: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Unknown error occurred in GetOptionChain." << std::endl;
            }

            // Cleanup
            curl_easy_cleanup(curl);

            return responseBuffer;
        }

        void parseOptions(const std::string& jsonResponse) {
            try {
                json data = json::parse(jsonResponse);

                // Check if "optionChain" exists in the JSON response
                if (data.find("optionChain") != data.end() && data["optionChain"].find("result") != data["optionChain"]["result"].end()) {
                    // Iterate over the "result" array and extract the "options" category
                    for (const auto& result : data["optionChain"]["result"]) {
                        if (result.find("options") != result.end()) {
                            // Print or process the "options" category as needed
                            std::cout << result["options"] << std::endl;
                        }
                    }
                } else {
                    std::cerr << "Error: Invalid JSON structure. Missing 'optionChain' or 'result'." << std::endl;
                }
            } catch (const json::parse_error& e) {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            }
        }

        void parseBuffer(std::string& responseBuffer) {
            size_t optionsPos = responseBuffer.find("contractSymbol") -3;
            if (optionsPos != std::string::npos) {
                responseBuffer.erase(0, optionsPos);
                size_t endPos = responseBuffer.find("]}]}]")+1;
                if (endPos != std::string::npos) {
                    responseBuffer.erase(endPos);
                } else {
                    std::cerr << "Error: Unable to find \"options\" in the JSON string." << std::endl;
                }
            } else {
                    std::cerr << "Error: Unable to find \"options\" in the JSON string." << std::endl;
            }

        }

        std::pair<std::string, std::string> splitBuffer(std::string& responseBuffer) {
            // Find the position of the substring `,"puts":`
            size_t putsPos = responseBuffer.find(",\"puts\":");

            if (putsPos != std::string::npos) {
                // Extract the string up to the substring
                std::string beforePuts = std::move(responseBuffer.substr(0, putsPos));

                // Extract the string after the substring
                std::string afterPuts = std::move(responseBuffer.substr(putsPos + 8));

                // Return the two parts using move semantics
                return {std::move(beforePuts), std::move(afterPuts)};
            } else {
                std::cerr << "Error: Unable to find \",\"puts\": in the JSON string." << std::endl;
                return {};  // Return an empty pair in case of an error
            }
        }

        void printObj(json& optionsJSON, std::ofstream& csvFile) {
            for (const auto& obj : optionsJSON) {
                csvFile << obj.value("contractSymbol", "") << ","
                        << obj.value("strike", 0.0) << ","
                        << obj.value("currency", "") << ","
                        << obj.value("lastPrice", 0.0) << ","
                        << obj.value("change", 0.0) << ","
                        << obj.value("percentChange", 0.0) << ","
                        << obj.value("volume", 0) << ","
                        << obj.value("openInterest", 0) << ","
                        << obj.value("bid", 0.0) << ","
                        << obj.value("ask", 0.0) << ","
                        << obj.value("lastTradeDate", 0) << ","
                        << obj.value("impliedVolatility", 0.0) << ","
                        << obj.value("inTheMoney", false) << std::endl;
            }
        }

        void JSONtoCSV(json& data, const std::string& filePath) {
                std::ofstream csvFile(filePath);
                if (!csvFile.is_open()) {
                    throw std::runtime_error("Error: Unable to open file for writing.");
                }
                csvFile << "contractSymbol,strike,currency,lastPrice,change,percentChange,volume,openInterest,bid,ask,contractSize,expiration,lastTradeDate,impliedVolatility,inTheMoney" << std::endl;
                printObj(data, csvFile);
            csvFile.close();
            std::cout << "Data successfully saved to CSV" << std::endl;
        }

        void downloadCSV(const std::string& ticker, const std::string& date, std::string filePath = "") {
            try {
                std::string responseBuffer = GetJSON(ticker, date);
                parseBuffer(responseBuffer);
                std::pair<std::string, std::string> CPpair = splitBuffer(responseBuffer);

                std::pair<json, json> data = {json::parse(CPpair.first), json::parse(CPpair.second)};
                
                std::string callPath = filePath + "../data/" + ticker + "_call_chain.csv";
                std::string putPath = filePath + "../data/" + ticker + "_put_chain.csv";

                JSONtoCSV(data.first, callPath);
                JSONtoCSV(data.second, putPath);

            } catch (const std::exception& e) {
                std::cerr << "Error in downloadOptions: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Unknown error occurred in downloadOptions." << std::endl;
            }
        }
    }
}

#endif
