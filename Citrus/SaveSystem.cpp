#include "SaveSystem.h"

void SaveSystem::OpenFileWrite(std::string file_name)
{
	//open file in write mode
	pFile.open(file_name, std::ios::out);
}

void SaveSystem::OpenFileRead(std::string file_name)
{
	//open file in read mode
	pFile.open(file_name, std::ios::in);
}

void SaveSystem::AddInfo(std::vector<std::string> elements)
{
	//Get all data in vector and add that data to file
	if (pFile.is_open()) {
		for (int i = 0; i < elements.size(); i++)
		{
			pFile << elements[i] + "\n";
		}
	}
}

std::string SaveSystem::GetInfo(int line)
{
	//create data string
	std::string data;
	data = std::to_string(line); //set data value to line number
	std::getline(pFile, data); 	//get data from refernce line
	return data; //return the data
}

void SaveSystem::CloseFile()
{
	//close the file
	pFile.close();
}
