#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip> 
#include <QApplication>
#include <mainwindow.h>
#include <yahoo_stocks.hpp>
#include <yahoo_options.hpp>

int main(int argc, char* argv[]) {

    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QObject::connect(&w, &MainWindow::buttonPushed, [&w]() {
        try {
            std::string date1 = ((w.getStartDate()).toString("yyyy-MM-dd")).toStdString();
            std::string date2 = ((w.getEndDate()).toString("yyyy-MM-dd")).toStdString();
            std::string date3 = ((w.getExpirationDate()).toString("yyyy-MM-dd")).toStdString();
            yahoo::stockprice::downloadCSV(w.getTicker().toStdString(), date1, date2, w.getInterval());
            yahoo::options::downloadCSV(w.getTicker().toStdString(), date3);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cerr << "Incorrect query" << std::endl;
        }
    });

    return a.exec();
}
