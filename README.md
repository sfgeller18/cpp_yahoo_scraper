# C++ Stock Data Scraper/Time-Series Analyzer


## Description

Qt-based GUI tool for scraping historical data from Yahoo finance, saving locally or on Mongo clusters and including an API (forked from olmallet81/URT and reconfigured via cmake) for unit root testing of acquired time-series data.

## Build Instructions

```
git clone https://github.com/sfgeller18/cpp_yahoo_scraper
git submodule update --init --recursive
cd cpp_yahoo_scraper/scripts && sudo ./recomp.sh
cd ../build && make
./query <mongo_uri>
}
```
