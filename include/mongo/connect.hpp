#ifndef CONNECT_H
#define CONNECT_H

#define QT_NO_DEBUG_OUTPUT

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>


#include <iostream>
#include <mongocxx/instance.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>

namespace mongo {

class StockDataConnection {
public:
    StockDataConnection(const std::string& uri) : uri_(uri) {
        setupConnection();
    }

    mongocxx::client& getClient() {
        return conn_;
    }

    mongocxx::database& getDatabase() {
        return db_;
    }

    mongocxx::collection& getStockCollection() {
        return stockCollection_;
    }

    mongocxx::collection& getOptionCollection() {
        return optionCollection_;
    }

private:
    void setupConnection() {
        try {
            mongocxx::instance inst{};
            mongocxx::options::client client_options;
            const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
            client_options.server_api_opts(api);

            // Establish the connection here.
            conn_ = mongocxx::client{mongocxx::uri{uri_}, client_options};

            // Access the "admin" database.
            mongocxx::database db = conn_["admin"];

            // Ping the database.
            const auto ping_cmd = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ping", 1));
            db.run_command(ping_cmd.view());
            std::cout << "Pinged your deployment. You successfully connected to MongoDB!" << std::endl;

            // Assume that "StockData" is the database you want to use.
            db_ = conn_["StockData"];
            stockCollection_ = db_["stockprice"];
            optionCollection_ = db_["options"];
        } catch (const std::exception& e) {
            // Handle errors.
            std::cout << "Exception: " << e.what() << std::endl;
        }
    }

    std::string uri_;
    mongocxx::client conn_;
    mongocxx::database db_;
    mongocxx::collection stockCollection_;
    mongocxx::collection optionCollection_;
};

} // namespace mongo


#endif
