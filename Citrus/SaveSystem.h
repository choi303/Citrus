#pragma once
#include <fstream>
#include <string>
#include <vector>

class SaveSystem
{
public:
	SaveSystem() {}
	void OpenFileWrite(std::string file_name);
	void OpenFileRead(std::string file_name);
	void AddInfo(std::vector<std::string> elements);
	std::string GetInfo(int line);
	void CloseFile();
	std::fstream& GetFile() noexcept;
	~SaveSystem() {}
private:
	std::fstream pFile;
};

