# C++ Stock Data Scraper/Time-Series Analyzer


## Description

Qt-based GUI tool for scraping historical data from Yahoo finance, saving locally or on Mongo clusters and including an API (forked from olmallet81/URT and reconfigured via cmake) for unit root testing of acquired time-series data.

Current Configuration:
* Built on WSL2 Ubuntu 22.04
* Eigen >= 3.3.1
* Intel OneAPI MKL 2024
* libgomp1 12.3.0-1ubuntu1~22.04
* Qt >= 6.0.1
* curl 7.81.0
* mongocxx-v3

## Build Instructions

```
git clone https://github.com/sfgeller18/cpp_yahoo_scraper
git submodule update --init --recursive
cd cpp_yahoo_scraper/scripts && sudo ./recomp.sh
cd ../build && make
./query <mongo_uri>
}
```

## Upcoming Updates

*Unit Testing:* Will add a straightforward 4-5 test GoogleTest/CTest unit-test module to test installs tomorrow.

*Build Flexibility:* Intend to add cross platform (WIN32 for now) functionality, allow for optionality over use of MKL and OpenMP for the time-series module, make cloud functionality optional and provide headless run option once this version's glitches have been resolved.

