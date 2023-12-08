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


//Make argc argv

int main(int argc, char* argv[]) {

        mongocxx::instance inst{};

        // Set the version of the Stable API on the client.
        mongocxx::options::client client_options;
        const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
        client_options.server_api_opts(api);

        // Setup the connection.
        mongocxx::uri uri("mongodb+srv://sfgeller18:0TaUq5tN61FzlRjD@cppyahooscraper.fugbtdd.mongodb.net/?retryWrites=true&w=majority");
        mongocxx::client conn{uri, client_options};

        // Call setup_connection with the connection handle.
        mongo::setup_connection(conn);

    mongocxx::v_noabi::database db = conn["StockData"];
    mongocxx::v_noabi::collection stockCollection = db["stockprice"];
    mongocxx::v_noabi::collection optionCollection = db["options"];


    std::string successBuffer;
    
    std::vector<std::string> expirations = yahoo::options::getExpirations("AAPL");
    QStringList Qexpirations;
    for (std::string& timestamp : expirations) {
        timestamp = unixToString(timestamp);
        Qexpirations<<QString::fromStdString(timestamp);
    }

    QApplication a(argc, argv);
    MainWindow w(Qexpirations);
    w.show();

    QObject::connect(&w, &MainWindow::buttonPushed, [&w, &stockCollection, &optionCollection]() {
        try {
            std::string date1 = ((w.getStartDate()).toString("yyyy-MM-dd")).toStdString();
            std::string date2 = ((w.getEndDate()).toString("yyyy-MM-dd")).toStdString();
            yahoo::stockprice::downloadCSVtoCloud(w.getTicker().toStdString(), date1, date2, w.getInterval(), stockCollection);
            yahoo::options::downloadOptionsToCloud(w.getTicker().toStdString(), w.getExpirationDate().toStdString(), optionCollection);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cerr << "Incorrect query" << std::endl;
        }
    });

    return a.exec();
}
