#include <iostream>
#include <future>

#include <QApplication>

#include "UI/mainwindow.h"
#include "yahoo/stocks.hpp"
#include "yahoo/options.hpp"
#include "mongo/connect.hpp"
#include "URT.hpp"

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

int main(int argc, char* argv[]) {
    setenv("QT_QPA_PLATFORM", "xcb", 1);

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <database_url>" << std::endl;
        return 1; // Return an error code
    }

    std::string databaseUrl = "mongodb+srv://sfgeller18:" + argv[1] + "@cppyahooscraper.fugbtdd.mongodb.net/?retryWrites=true&w=majority";;
    std::string errorBuffer;
    QStringList Qexpirations;
    for (const auto& timestamp : yahoo::options::getExpirations("AAPL")) {
        Qexpirations << QString::fromStdString(unixToString(timestamp));
    }

    QApplication a(argc, argv);
    MainWindow w(Qexpirations);

    // Create an instance of StockDataConnection
    mongo::StockDataConnection stockDataConnection(databaseUrl);

    QObject::connect(&w, &MainWindow::buttonPushed, [&w, &stockDataConnection, &errorBuffer]() {
        try {
            auto date1 = (w.getStartDate()).toString("yyyy-MM-dd").toStdString();
            auto date2 = (w.getEndDate()).toString("yyyy-MM-dd").toStdString();
            std::cout << w.getInterval() << std::endl;
            std::cout << w.getExpirationDate().toStdString() << std::endl;
            
            // Use StockDataConnection methods to access MongoDB objects
            auto& stockCollection = stockDataConnection.getStockCollection();
            auto& optionCollection = stockDataConnection.getOptionCollection();

            yahoo::stockprice::downloadCSV(w.getTicker().toStdString(), date1, date2, w.getInterval(), errorBuffer);
            yahoo::options::downloadCSV(w.getTicker().toStdString(), w.getExpirationDate().toStdString());

            w.setError("Download completed successfully");
        } catch (const std::exception& e) {
            w.setError("Error: " + QString::fromStdString(e.what()));
        }
    });

    w.show();

    int appResult = a.exec();
    std::cout << "Cleaning up..." << std::endl;
    return appResult;
}
