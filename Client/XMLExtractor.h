#pragma once
#include "pugixml.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <time.h>

namespace XMLHelper
{
	std::string GetEndStation(pugi::xml_node);

	std::string GetOriginalStation(pugi::xml_node);

	std::string GetArrivalTime(pugi::xml_node);

	std::string GetTrainNumber(pugi::xml_node);

	std::string GetDelay(pugi::xml_node);

	std::string GetID(pugi::xml_node);

	std::string FindDelay(pugi::xml_node, std::string);

	std::string CreateXMLFile(const char*, const char*,const char*);

	bool IsUserInformationFileValid(std::string);

	std::string DateToDBDate(struct tm);

};