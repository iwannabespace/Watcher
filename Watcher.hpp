#ifndef WATCHER_HPP
#define WATCHER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <utility>
#include <filesystem>

namespace LogianApi
{
	class Watcher
	{
		public:
			enum class Action 
			{
				Add,
				Remove,
				Modify,
				None,
			};
			struct FileInfo 
			{
				std::filesystem::file_time_type last_mod;
				std::string content;
			};
			struct FileChangeResult 
			{
				std::string filename;
				std::string previousValue;
				std::string currentValue;
				std::string added;
				std::string removed;
				Action action;
			};
			using Callback = std::function<void(const FileChangeResult& result)>;
		public:
			Watcher();
			~Watcher();
			bool add(const std::string& filepath);
			bool remove(const std::string& filepath);
			void watch(Callback callback);
		private:
			FileInfo getFileInfo(const std::string& filepath) const;
			std::pair<std::string, std::string> getChanges(const std::pair<std::string, std::string>& datas) const;
			std::string getFileContents(const std::string& filepath) const;
		private:
			std::vector<std::string> files;
			std::unordered_map<std::string, FileInfo> infos;
	};
}

#endif