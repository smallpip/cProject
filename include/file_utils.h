#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <set>
#include <filesystem>

namespace fs
{
    namespace sfs = std::filesystem;

    std::string getOperatingSystem();
    struct FileInfo;
    auto getTimeFromAccessToNow(const sfs::file_time_type &fileTime);
    auto getAllFiles(const std::string &filePath, sfs::file_status s = sfs::file_status{});
    auto searchFileInfo(const std::string &filePath);
    auto operateFile(const FileInfo &fileInfo, const int &DelMaxSize, const long long &DelMaxSecond, const std::string &targetExtension, std::set<std::string> &filesToDelete, const sfs::file_type &DelfileType = sfs::file_type::regular);
    auto printAndConfirmFiles(const std::set<std::string> filesToDelete);
    auto getDelteFiles(const std::set<std::string> getFiles, const int &DelMaxSize, const long long &DelMaxSecond, const std::string &targetExtension);
}

#endif
