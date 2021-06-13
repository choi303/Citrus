#include "SaveSystem.h"

void SaveSystem::OpenFileWrite(std::string file_name)
{
	pFile.open(file_name, std::ios::out);
}

void SaveSystem::OpenFileRead(std::string file_name)
{
	pFile.open(file_name, std::ios::in);
}

void SaveSystem::AddInfo(std::vector<std::string> elements)
{
	if (pFile.is_open()) {
		for (int i = 0; i < elements.size(); i++)
		{
			pFile << elements[i] + "\n";
		}
	}
}

std::string SaveSystem::GetInfo(int line)
{
	std::string data;
	if (pFile.is_open()) {
		data = std::to_string(line);
		std::getline(pFile, data);
		return data;
		
	}

}

void SaveSystem::CloseFile()
{
	pFile.close();
}
