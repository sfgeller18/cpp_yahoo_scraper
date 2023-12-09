
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip> 
#include <future>


#include <QApplication>
#include <UI/mainwindow.h>

#include <yahoo/stocks.hpp>
#include <yahoo/options.hpp>
#include <mongo/connect.hpp>


//Make argc argv

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <database_url>" << std::endl;
        return 1; // Return an error code
    }
    std::string databaseUrl = argv[1];
    std::vector<std::string> expirations = yahoo::options::getExpirations("AAPL");
    QStringList Qexpirations;
    for (std::string& timestamp : expirations) {
        timestamp = unixToString(timestamp);
        Qexpirations << QString::fromStdString(timestamp);
    }

    std::string errorBuffer;

    mongocxx::client conn{};
    mongocxx::v_noabi::database db{};
    mongocxx::v_noabi::collection stockCollection{};
    mongocxx::v_noabi::collection optionCollection{};

     // Establish MongoDB connection asynchronously
    std::future<mongocxx::client> mongoFuture = std::async(std::launch::async, [&databaseUrl]() {
        mongocxx::instance inst{};
        mongocxx::options::client client_options;
        const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
        client_options.server_api_opts(api);

        mongocxx::uri uri(databaseUrl);
        mongocxx::client conn{uri, client_options};
        mongo::setup_connection(conn);
        return conn;
    });

        std::future<int> appFuture = std::async(std::launch::async, [&argc, &argv, &Qexpirations, &stockCollection, &optionCollection, &mongoFuture, &errorBuffer]() {
        QApplication a(argc, argv);
        MainWindow w(Qexpirations);
        mongoFuture.wait();
        QObject::connect(&w, &MainWindow::buttonPushed, [&w, &stockCollection, &optionCollection, &errorBuffer]() {
            QString errorQstring{};
            try {
                std::string date1 = ((w.getStartDate()).toString("yyyy-MM-dd")).toStdString();
                std::string date2 = ((w.getEndDate()).toString("yyyy-MM-dd")).toStdString();
                std::cout<< w.getInterval()<<std::endl;
                std::cout << w.getExpirationDate().toStdString()<<std::endl;
                yahoo::stockprice::downloadCSV(w.getTicker().toStdString(), date1, date2, w.getInterval(), errorBuffer);
                yahoo::options::downloadCSV(w.getTicker().toStdString(), w.getExpirationDate().toStdString());
                errorQstring = QString::fromStdString(errorBuffer);
                w.setError(errorQstring);
            } catch (const std::exception& e) {
                errorQstring = QString::fromStdString("Error: " + std::string(e.what()));
                w.setError(errorQstring);
            }
        });
        w.show();
        return a.exec();
    });

    conn = mongoFuture.get();
    db = conn["StockData"];
    stockCollection = db["stockprice"];
    optionCollection = db["options"];

        // Launch the UI asynchronously

    // Now that the MongoDB connection is established, connect the UI signal to slo

    // Wait for the UI to finish
    int appResult = appFuture.get();

    return appResult;

}
