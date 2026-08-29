#include "FileProcessor.h"

#include <fstream>
#include <sstream>
#include <stdexcept>


FileStats processFile(
    const std::string& filename
) {

    std::ifstream file(filename);

    if (!file.is_open()) {

        throw std::runtime_error(
            "Unable to open file: " + filename
        );
    }


    FileStats stats;

    stats.filename = filename;


    std::string line;

    while (std::getline(file, line)) {

        // Count lines
        ++stats.lines;

        // Count characters
        stats.characters += line.length();


        // Count words
        std::istringstream stream(line);

        std::string word;

        while (stream >> word) {

            ++stats.words;
        }
    }


    return stats;
}