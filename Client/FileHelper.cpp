#include "FileHelper.h"

namespace File
{
	bool StationfileDeleteRows()
	{
		std::fstream StationFile("StationList.csv", std::fstream::in | std::fstream::out);
		std::ofstream tempFile("EvaAndStation.txt", std::ios::trunc);
		std::string line;
		int position[4];

		if (!tempFile.is_open()) return false;
		if (!StationFile.is_open()) return false;

		while (!StationFile.eof())
		{
			std::getline(StationFile, line);
		
			position[0] = line.find_first_of(";");
			position[1] = line.find_first_of(";", position[0] + 1);
			position[2] = line.find_first_of(";", position[1]+ 1);
			position[3] = line.find_first_of(";", position[2]+ 1);	line = line.substr(0, position[0] + 1) + line.substr(position[2] + 1, position[3] - position[2]);

			tempFile.write(line.c_str(), line.size());
			tempFile.write("\n", 1);
		
		}
		
		StationFile.close();
		tempFile.close();

		return true;
	}

	std::vector<DB::Station>* ReadInStations()
	{
		std::ifstream StationFile("EvaAndStation.txt");
		if (StationFile.is_open()) std::cout << "Station File Openend\n";

		std::vector<DB::Station>* Stations = new std::vector<DB::Station>;
	
		std::string temp;
		while (!StationFile.eof())
		{
			std::getline(StationFile, temp);
			
			Stations->push_back(DB::Station(temp.substr(0, 7), temp.substr(8, temp.size() - 8 - 1)));
		}
	

		StationFile.close();
		//std::vector<std::string>* woerter = new std::vector<std::string> { "Hey","Was", "Geht", "Mir", "Geht ", "Es", "Gut", "Hey", "Hem", "Hello", "Hexe" };
		std::cout << "\n DATEI GELESEN\n";
		return  Stations;
	}
}