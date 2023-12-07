cd ..
rm -rf build
mkdir build
cd build
cmake ..
make
./query AAPL Stocks 2023-12-01 2023-12-03 "1m"