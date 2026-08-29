#ifndef SINGLE_THREAD_PROCESSOR_H
#define SINGLE_THREAD_PROCESSOR_H

#include "FileProcessor.h"

#include <vector>
#include <string>

class SingleThreadProcessor {
public:
    static std::vector<FileStats> processFiles(
        const std::vector<std::string>& filenames
    );
};

#endif