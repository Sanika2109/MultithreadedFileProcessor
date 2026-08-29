#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H

#include <string>
#include <cstddef>

struct FileStats {

    std::string filename;

    std::size_t lines = 0;
    std::size_t words = 0;
    std::size_t characters = 0;
};


FileStats processFile(
    const std::string& filename
);

#endif