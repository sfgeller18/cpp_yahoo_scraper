#ifndef TIMEFUNCS_H
#define TIMEFUNCS_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

inline std::time_t dateToUnix(const std::string& dateStr) {
    std::tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d");

    if (ss.fail()) {
        std::cerr << "Failed to parse the date." << std::endl;
        return -1; // Error indicator
    }

    return std::mktime(&tm)-28800;
}

inline std::string unixToString(std::time_t timestamp) {
    std::tm* tm = std::gmtime(&timestamp); // Convert to UTC time
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

inline std::string timestamper(std::string date) {
    return(std::to_string(dateToUnix(date)));
}

std::time_t to_time(const std::string& date) {
    std::tm tmDate = {};
    std::istringstream issDate(date);
    issDate >> std::get_time(&tmDate, "%Y-%m-%d");

    if (issDate.fail()) {
        std::cerr << "Error parsing date input." << std::endl;
        return 0;  // Return an invalid time_t value on error
    }

    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(std::mktime(&tmDate)));
}

std::string unixToString(const std::string& unixTimestamp) {
    // Convert string to integer
    std::time_t timestamp = std::stoi(unixTimestamp);

    // Convert timestamp to tm struct in UTC
    std::tm* timeInfo = std::gmtime(&timestamp);

    // Format the date as yyyy-mm-dd
    std::ostringstream oss;
    oss << std::put_time(timeInfo, "%Y-%m-%d");

    return oss.str();
}

#endif