#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
// #include "../include/file_utils.h"
#include <chrono>
#include <ctime>
#include <set>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

std::string getOperatingSystem()
{
#ifdef _WIN32
	return "Windows";
#else
	return "unix";
#endif
}

namespace fs
{
	namespace sfs = std::filesystem;
	
	// <--- 转换时间格式（暂未使用） --->
	template <typename TP>
	std::time_t to_time_t(TP tp)
	{
		using namespace std::chrono;
		auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
		return system_clock::to_time_t(sctp);
	}

	struct FileInfo
	{
		sfs::file_time_type lastWriterTime;
		sfs::file_time_type lastAccessTime;
		std::uintmax_t fileSize;
		std::string filePath;
		std::string fileExtension;
		sfs::file_type fileType;
	};

	inline auto getTimeFromAccessToNow(const sfs::file_time_type &fileTime)
	{

		sfs::file_time_type currentTime = sfs::file_time_type::clock::now();

		sfs::file_time_type::duration diff = currentTime - fileTime;

		auto diffInSeconds = std::chrono::duration_cast<std::chrono::seconds>(diff);
		auto diffInSecondsValue = diffInSeconds.count();
		return diffInSecondsValue;
	}

	inline auto getAllFiles(const std::string &filePath, sfs::file_status s = sfs::file_status{})
	{
		std::set<std::string> files;
		if (sfs::status_known(s) ? sfs::exists(s) : sfs::exists(filePath))
		{
			std::cout << " exists\n";
			
			// <--- 遍历当前文件下所有文件 --->
			for (auto const &dir_entry : sfs::recursive_directory_iterator{filePath})
			{
				std::string filename = dir_entry.path().filename().u8string();
				std::string filepath = filePath + "/" + filename;
				files.insert(filepath);
			}
		}
		else{
			std::cout << " does not exist\n";
			}
		return files;
	}

	inline auto searchFileInfo(const std::string &filePath)
	{
		FileInfo fileInfo;
		sfs::file_status fileStatus = sfs::status(filePath);

		// <--- 获取最后一次修改时间 --->
		fileInfo.lastWriterTime = sfs::last_write_time(filePath);

		// <--- 获取最后访问时间 --->
		sfs::file_time_type lastAccessTime;
		if (getOperatingSystem() == "windows")
		{
			std::cout << "win待续" << std::endl;
		}
		else
		{
			struct stat st;
			if (stat(filePath.c_str(), &st) == 0)
				// fileInfo.lastAccessTime = static_cast<std::time_t>(st.st_atime);
				fileInfo.lastAccessTime = sfs::file_time_type(std::chrono::seconds(st.st_atime));
			else{
				fileInfo.lastAccessTime = sfs::file_time_type::clock::now();
			}
		}

		fileInfo.fileSize = sfs::file_size(filePath);
		fileInfo.fileExtension = sfs::path(filePath).extension().string();
		fileInfo.fileType = fileStatus.type();
		fileInfo.filePath = filePath;
		return fileInfo;
	}

	inline auto operateFile(const FileInfo &fileInfo, const int &DelMaxSize, const long long &DelMaxSecond, const std::string &targetExtension, std::set<std::string> &filesToDelete, const sfs::file_type &DelfileType = sfs::file_type::regular)
	{
		if (fileInfo.fileType == DelfileType && fileInfo.fileExtension == targetExtension)
		{
			auto writerTimeDiff = getTimeFromAccessToNow(fileInfo.lastWriterTime);
			auto accessTimeDiff = getTimeFromAccessToNow(fileInfo.lastAccessTime);

			// <--- 筛选大文件 --->
			if (fileInfo.fileSize > DelMaxSize) {
				std::cout << "Over MaxSize file: " << fileInfo.filePath << std::endl;
				std::cout << "file size: " << fileInfo.fileSize << std::endl;
				filesToDelete.insert(fileInfo.filePath);
				return; // 使用卫语句，满足条件时立即返回
			}

			// <--- 筛选很久没使用的文件 --->
			if (accessTimeDiff > DelMaxSecond) {
				std::cout << "Over Maxtime file: " << fileInfo.filePath << std::endl;
				std::cout << "diff time: " << accessTimeDiff << std::endl;
				filesToDelete.insert(fileInfo.filePath);
				return; // 使用卫语句，满足条件时立即返回
			}
		}
		else{
			return; 
		}
	}

	inline auto printAndConfirmFiles(const std::set<std::string> filesToDelete)
	{
		std::cout << "The following files will be deleted:" << std::endl;

		// <--- 打印文件列表 --->
		for (const auto &filePath : filesToDelete)
		{
			std::cout << filePath << std::endl;
		}

		std::cout << "Deletion? (y/n): ";
		std::string userInput;
		std::cin >> userInput;

		// <--- 确认是否删除 --->
		if (userInput == "y" || userInput == "Y")
		{
			for (const auto &filePath : filesToDelete)
			{
				// fs::remove(filePath);
				std::cout << "Deleted file: " << filePath << std::endl;
			}
		}
		else
		{
			std::cout << "Deletion canceled" << std::endl;
		}
	}

	inline auto getDelteFiles(const std::set<std::string> getFiles,const int &DelMaxSize, const long long &DelMaxSecond, const std::string &targetExtension){
		std::set<std::string> filesToDelete;
		for (auto const &dir_file : getFiles)
		{
			FileInfo fileinfo = fs::searchFileInfo(dir_file);

			operateFile(fileinfo, DelMaxSize, DelMaxSecond, targetExtension,filesToDelete);
		}
		return filesToDelete;
	}
// 
}


// #include <io.h> //win
// void win_getAllFiles(std::string path, std::vector<std::string>& files) {
//     long hFile = 0;
//     struct _finddata_t fileinfo;
//     std::string p;
//     if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
//     {
//         do
//         {
//             if ((fileinfo.attrib & _A_SUBDIR))
//             {
//                 if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
//                 {
//                     files.push_back(p.assign(path).append("/").append(fileinfo.name));
//                     getAllFiles(p.assign(path).append("/").append(fileinfo.name), files);
//                 }
//             }
//             else {
//                 files.push_back(p.assign(path).append("/").append(fileinfo.name));
//             }
//         } while (_findnext(hFile, &fileinfo) == 0);
//         _findclose(hFile);
//     }
// }

// #include <dirent.h>
// void getAllFiles(const std::string &path, std::vector<std::string> &files)
// {
// 	DIR *dir;
// 	struct dirent *entry;

// 	if ((dir = opendir(path.c_str())) != nullptr)
// 	{
// 		while ((entry = readdir(dir)) != nullptr)
// 		{
// 			std::string filename = entry->d_name;
// 			if (filename != "." && filename != "..")
// 			{
// 				std::string filepath = path + "/" + filename;
// 				files.push_back(filepath);

// 				if (entry->d_type == DT_DIR)
// 				{
// 					getAllFiles(filepath, files); // 递归遍历子文件夹
// 				}
// 			}
// 		}
// 		closedir(dir);
// 	}
// }