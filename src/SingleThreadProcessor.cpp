#include "SingleThreadProcessor.h"

std::vector<FileStats> SingleThreadProcessor::processFiles(
    const std::vector<std::string>& filenames
) {

    std::vector<FileStats> results;

    for (const auto& filename : filenames) {

        results.push_back(
            processFile(filename)
        );
    }

    return results;
}