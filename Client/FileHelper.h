#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "DeutscheBahnData.h"

namespace File
{

	bool StationfileDeleteRows();

	std::vector<DB::Station>* ReadInStations();

};