#ifndef WATCHER_HPP
#define WATCHER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <utility>
#include <filesystem>
#include <optional>

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
			using DataPair   = std::pair<std::string, std::string>;
		public:
			Watcher();
			~Watcher();
			bool add(const std::string& filepath);
			bool remove(const std::string& filepath);
			void watch(Callback callback);
		private:
			std::optional<FileInfo> getFileInfo(const std::string& filepath) const;
			DataPair getChanges(const DataPair& datas) const;
			std::string getFileContents(const std::string& filepath) const;
			Action getAction(const DataPair& datas) const;
		private:
			std::vector<std::string> files;
			std::unordered_map<std::string, FileInfo> infos;
	};
}

#endif