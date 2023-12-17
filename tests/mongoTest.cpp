#ifndef MONGO_TEST
#define MONGO_TEST

#include <gtest/gtest.h>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include "mongo/connect.hpp"

class MongoConnectionTest : public ::testing::Test {
protected:
    // Add any member variables or helper methods you need

    void TearDown() override {
        // Clean up any resources after each test
        if (test_document_inserted_) {
            // If the test document was inserted, delete it
            try {
                connection_.getStockCollection().delete_one(
                    bsoncxx::builder::stream::document{} << "_id" << this->document_id_ << bsoncxx::builder::stream::finalize);
            } catch (const std::exception& e) {
                // Log or handle the cleanup error if needed
                std::cerr << "Error during cleanup: " << e.what() << std::endl;
            }
        }
    }

    bool test_document_inserted_ = false;
    mongo::StockDataConnection connection_;
    bsoncxx::types::bson_value::view document_id_;
};

// Test case to check MongoDB connection and document insertion/retrieval
TEST(MongoConnectionTest, ConnectAndReadWrite) {
    // Create an instance of the test class
    MongoConnectionTest test;

    // Construct the MongoDB URI for testing
    std::string uri = "mongodb+srv://sfgeller18:" + argv[1] + "@cppyahooscraper.fugbtdd.mongodb.net/?retryWrites=true&w=majority";

    // Establish a connection
    test.connection_ = mongo::StockDataConnection(uri);

    // Insert a test document
    bsoncxx::builder::stream::document document{};
    document << "key" << "value";
    auto result = test.connection_.getStockCollection().insert_one(document.view());
    test.document_id_ = result->inserted_id().view();
    test.test_document_inserted_ = true;

    // Read the document from MongoDB
    auto query_result = test.connection_.getStockCollection().find_one({});
    ASSERT_TRUE(query_result);

    bsoncxx::document::view result_doc = query_result->view();
    EXPECT_TRUE(result_doc["key"]);
    EXPECT_EQ(result_doc["key"].get_utf8().value.to_string(), "value");
}

#endif