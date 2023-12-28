#include "Watcher.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>

namespace LogianApi
{
	Watcher::Watcher()
	{
	}

	Watcher::~Watcher() 
	{
	}

	bool Watcher::add(const std::string& filepath) 
	{
		if (std::filesystem::exists(filepath)) 
		{
			files.push_back(filepath);

			std::filesystem::file_time_type last_mod = std::filesystem::last_write_time(filepath);;
			std::string content = getFileContents(filepath);
			
			infos[filepath].last_mod = last_mod;
			infos[filepath].content = content;

			return true;
		}

		return false;
	}

	bool Watcher::remove(const std::string& filepath)
	{
		auto it = std::find(files.begin(), files.end(), filepath);
		
		if (it != files.end()) 
		{
			files.erase(it);
			return true;
		}

		return false;
	}

	void Watcher::watch(Callback callback)
	{
		while (files.size()) 
		{
			for (size_t i = 0; i < files.size(); i++) 
			{
				const std::string file = files[i];
				const FileInfo oldInfo = infos[file];
				
				auto newInfoOption = getFileInfo(file);

				if (newInfoOption.has_value())
				{
					FileInfo newInfo = newInfoOption.value();

					if (oldInfo.last_mod != newInfo.last_mod) 
					{
						newInfo.content = getFileContents(file);
						infos[file] = newInfo;

						DataPair change = getChanges({ oldInfo.content, newInfo.content });
						Action action = getAction(change);
				
						callback({ file, oldInfo.content, newInfo.content, change.first, change.second, action });
					}
				}

				else
				{
					files.erase(files.begin() + i);
					infos.erase(file);
					std::cout << "This file was removed because it's no more accessible -> " << file << std::endl;
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}

		std::cout << "Watcher stopped working because there is no file to watch for!" << std::endl;
	}

	std::optional<Watcher::FileInfo> Watcher::getFileInfo(const std::string& filepath) const
	{
		if (!std::filesystem::exists(filepath))
			return {};

		return FileInfo { std::filesystem::last_write_time(filepath), "" };
	}

	Watcher::DataPair Watcher::getChanges(const Watcher::DataPair& datas) const
	{
		const std::string& old = datas.first;
		const std::string& cur = datas.second;
		std::string until;

		for (size_t i = 0; (old[i] == cur[i] && (i < old.length() && i < cur.length())); i++)
			until += old[i];

		std::string removed = old.substr(old.find(until) + until.length());
		std::string added = cur.substr(cur.find(until) + until.length());

		return { added, removed };
	}

	std::string Watcher::getFileContents(const std::string& filepath) const
	{
		std::ifstream file(filepath);
		std::stringstream buffer;

		buffer << file.rdbuf();

		return buffer.str();
	}

	Watcher::Action Watcher::getAction(const DataPair& datas) const
	{
		if (!datas.first.empty() && !datas.second.empty())
			return Action::Modify;

		else if (!datas.first.empty()) 
			return Action::Add;

		else if (!datas.second.empty()) 
			return Action::Remove;
		
		return Action::None;
	}
}