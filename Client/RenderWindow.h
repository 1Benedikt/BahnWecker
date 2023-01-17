#pragma once
#include "Client.h"
#include "imgui.h"
#include "DeutscheBahnData.h"
#include "FileHelper.h"
#include "NetworkRequests.h"
#include <chrono>
#include "Client.h"
#include <thread>
#include "SharedVariables.h"
#include "XMLExtractor.h"
#include "icons.h"
#include "addons/imguidatechooser/imguidatechooser.h"


namespace Window
{
	

	void RenderWindow();

	void RenderConnectionWindow();

	void RenderCurrentTrainConnection();

	void Theme();

	bool SearchWindow();

	void RenderFinalWindow();
};