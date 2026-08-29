#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include "FileProcessor.h"
#include "ThreadPool.h"

#include <vector>
#include <string>


class CSVWriter {

public:

    static bool writeResults(
        const std::string& filename,
        const std::vector<FileStats>& results
    );


    static bool writeErrors(
        const std::string& filename,
        const std::vector<ProcessingError>& errors
    );
};


#endif