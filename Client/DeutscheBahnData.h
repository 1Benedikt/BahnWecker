#pragma once
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include "pugixml.hpp"
#include <vector>
#include "XMLExtractor.h"


#define CLIENTID "99cc64b2f46f9ac154580c291446e583"
#define SECRETKEY "dd2b2e4c468d70225fd0e94bbb1ead8d"

namespace DB
{


	struct Station
	{
		Station(std::string e, std::string s) { EVA = e; StationName = s; }
		Station() = default;

		std::string EVA;
		std::string StationName;
	};
	
	class TrainInformation
	{
	public:
		TrainInformation(std::string D, std::string O, std::string AT, std::string DT, std::string CT, std::string ID, std::string n );
		TrainInformation();
		
		void setChangedTime(std::string);

	//private:
		std::string Destination;
		std::string Origin;
		std::string ArrivalTime;
		std::string DepartureTime;
		std::string ChangedTime;
		std::string TrainID;
		std::string TrainNumber;
	};
 
	
	std::vector<TrainInformation>* GetListOfTrains();
	/// <summary>
	/// Gets a list of trains for a Station and Returns them.
	/// </summary>
	/// <returns> If successfull, a Pointer to a std::vector<TrainInformation> will be returned, if not it it returns a nullptr</returns>
};