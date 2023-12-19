#include <gtest/gtest.h>
#include "testHeader.h"

std::string uri;

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cerr << "Usage: " << argv[0] << "password" << std::endl;
        return 1; // Return an error code}
    } else {
        uri = "mongodb+srv://sfgeller18:" + std::string(argv[1]) + "@cppyahooscraper.fugbtdd.mongodb.net/?retryWrites=true&w=majority";
    }
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}