#include <gtest/gtest.h>
#include "../src/FileProcessor.h"

#include <fstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;


// --------------------------------------------------
// Test fixture
// --------------------------------------------------

class FileProcessorTest : public ::testing::Test {

protected:

    const std::string testFile =
        "test_file.txt";


    void SetUp() override {

        std::ofstream file(testFile);

        file << "Hello world\n";
        file << "This is a test file\n";
        file << "C++ multithreading is interesting\n";
    }


    void TearDown() override {

        if (fs::exists(testFile)) {

            fs::remove(testFile);
        }
    }
};


// --------------------------------------------------
// Test line counting
// --------------------------------------------------

TEST_F(
    FileProcessorTest,
    CountsLinesCorrectly
) {

    FileStats stats =
        processFile(testFile);


    EXPECT_EQ(
        stats.lines,
        3
    );
}


// --------------------------------------------------
// Test word counting
// --------------------------------------------------

TEST_F(
    FileProcessorTest,
    CountsWordsCorrectly
) {

    FileStats stats =
        processFile(testFile);


    EXPECT_EQ(
        stats.words,
        11
    );
}


// --------------------------------------------------
// Test character counting
// --------------------------------------------------

TEST_F(
    FileProcessorTest,
    CountsCharactersCorrectly
) {

    FileStats stats =
        processFile(testFile);


    EXPECT_EQ(
        stats.characters,
        63
    );
}


// --------------------------------------------------
// Test filename
// --------------------------------------------------

TEST_F(
    FileProcessorTest,
    StoresFilenameCorrectly
) {

    FileStats stats =
        processFile(testFile);


    EXPECT_EQ(
        stats.filename,
        testFile
    );
}


// --------------------------------------------------
// Test missing file
// --------------------------------------------------

TEST_F(
    FileProcessorTest,
    ThrowsWhenFileDoesNotExist
) {

    EXPECT_THROW(
        processFile(
            "file_that_does_not_exist.txt"
        ),
        std::runtime_error
    );
}