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

			std::filesystem::file_time_type last_mod;
			std::string content;
			
			std::ifstream file(filepath);
			std::stringstream buffer;
			buffer << file.rdbuf();
			file.close();

			last_mod = std::filesystem::last_write_time(filepath);
			content = buffer.str();
			
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
		while (true) 
		{
			for (const std::string& file : files) 
			{
				FileInfo oldInfo = infos[file];
				FileInfo newInfo = getFileInfo(file);
				
				if (oldInfo.last_mod != newInfo.last_mod) 
				{
					std::ifstream f(file);
					std::stringstream buffer;
					std::string difference;
					Action event;

					buffer << f.rdbuf();
					newInfo.content = buffer.str();
					infos[file] = newInfo;

					auto change = getChanges({ oldInfo.content, newInfo.content });
					
					if (!change.first.empty() && !change.second.empty())
						event = Action::Modify;
					
					else if (!change.first.empty())
						event = Action::Add;
					
					else
						event = Action::Remove;

					callback({ file, oldInfo.content, newInfo.content, change.first, change.second, event });
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}
	}

	Watcher::FileInfo Watcher::getFileInfo(const std::string& filepath) const
	{
		std::filesystem::file_time_type last_mod = std::filesystem::last_write_time(filepath);
		return { last_mod, "" };
	}

	std::pair<std::string, std::string> Watcher::getChanges(const std::pair<std::string, std::string>& datas) const
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
}