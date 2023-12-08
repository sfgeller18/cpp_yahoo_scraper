#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>


namespace mongo {
void setup_connection(const mongocxx::client& conn) {
    try {
        // Access the "admin" database.
        mongocxx::database db = conn["admin"];

        // Ping the database.
        const auto ping_cmd = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ping", 1));
        db.run_command(ping_cmd.view());
        std::cout << "Pinged your deployment. You successfully connected to MongoDB!" << std::endl;
    } catch (const std::exception& e) {
        // Handle errors.
        std::cout << "Exception: " << e.what() << std::endl;
    }
}
}
