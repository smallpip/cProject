#include <iostream>
#include <filesystem>
#include <vector>
#include "include/file_utils.h"

int main(int argc, char* argv[]) {
        // if (argc < 5) {
    //     std::cout << "Usage: " << argv[0] << " <dataDir> <DelMaxSecond> <DelMaxSize> <targetExtension>" << std::endl;
    //     return 1;
    // }

    // std::string dataDir = argv[1];
    // long long DelMaxSecond = std::stoll(argv[2]);
    // std::uintmax_t DelMaxSize = std::stoull(argv[3]);
    // std::string targetExtension = argv[4];

    std::string dataDir = "/Users/peng/Cproject/test";
    long long DelMaxSecond = 60 * 60;
    std::uintmax_t DelMaxSize = 2;
    std::string targetExtension = ".txt";

    auto getFiles = fs::getAllFiles(dataDir);
    auto filesToDelete = fs::getDelteFiles(getFiles, DelMaxSize, DelMaxSecond, targetExtension);
    fs::printAndConfirmFiles(filesToDelete);

    return 0;
}