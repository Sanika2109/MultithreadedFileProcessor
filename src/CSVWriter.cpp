#include "CSVWriter.h"

#include <fstream>


bool CSVWriter::writeResults(
    const std::string& filename,
    const std::vector<FileStats>& results
) {

    std::ofstream file(filename);


    if (!file.is_open()) {

        return false;
    }


    file
        << "Filename,Lines,Words,Characters\n";


    for (const auto& result :
         results) {

        file
            << result.filename
            << ","
            << result.lines
            << ","
            << result.words
            << ","
            << result.characters
            << "\n";
    }


    return true;
}


bool CSVWriter::writeErrors(
    const std::string& filename,
    const std::vector<ProcessingError>& errors
) {

    std::ofstream file(filename);


    if (!file.is_open()) {

        return false;
    }


    file
        << "Filename,Error\n";


    for (const auto& error :
         errors) {

        file
            << error.filename
            << ","
            << error.message
            << "\n";
    }


    return true;
}