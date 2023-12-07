#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip> 
#include <QApplication>
#include <UI/mainwindow.h>
#include <yahoo/stocks.hpp>
#include <yahoo/options.hpp>
#include <QString>
#include <mongo/connect.hpp>

int main(int argc, char* argv[]) {

    mongocxx::uri uri = mongocxx::uri("mongodb+srv://sfgeller18:7EpoaMnqgB1Cx3RJ@cppyahooscraper.fugbtdd.mongodb.net/?retryWrites=true&w=majority");
    mongo::connect(uri);

    std::string successBuffer;
    
    std::vector<std::string> expirations = yahoo::options::getExpirations("AAPL");
    QStringList Qexpirations;
    for (std::string& timestamp : expirations) {
        timestamp = unixToString(timestamp);
        Qexpirations<<QString::fromStdString(timestamp);
    }
    for (const QString& date : Qexpirations) {
    qDebug() << date;
}


    QApplication a(argc, argv);
    MainWindow w(Qexpirations);
    w.show();

    QObject::connect(&w, &MainWindow::buttonPushed, [&w]() {
        try {
            std::string date1 = ((w.getStartDate()).toString("yyyy-MM-dd")).toStdString();
            std::string date2 = ((w.getEndDate()).toString("yyyy-MM-dd")).toStdString();
            yahoo::stockprice::downloadCSV(w.getTicker().toStdString(), date1, date2, w.getInterval());
            yahoo::options::downloadCSV(w.getTicker().toStdString(), w.getExpirationDate().toStdString());
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cerr << "Incorrect query" << std::endl;
        }
    });

    return a.exec();
}
