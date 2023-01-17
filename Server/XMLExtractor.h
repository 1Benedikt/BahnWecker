#pragma once
#include "pugi/pugixml.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <time.h>

namespace XMLHelper
{
	std::string GetEndStation(pugi::xml_node);

	std::string GetOriginalStation(pugi::xml_node);

	std::string GetArrivalTime(pugi::xml_node);

	std::string GetDelay(pugi::xml_node);

	std::string GetID(pugi::xml_node);

	std::string FindDelay(pugi::xml_node, std::string, bool Formatted);

	std::string FindNode(pugi::xml_node, std::string);

	bool FindDelayForTrain();

	bool AdvancedFindDelayForTrain(std::string, std::string, std::string); //Nicht wirklich "Advanced sucht nur mehrer Dateien ab"

	bool IsUserInformationFileValid();

	std::string DateToDBDate(struct tm);

};