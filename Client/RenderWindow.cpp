
#include "RenderWindow.h"
#include "DeutscheBahnData.h"

char InputStation[8] = { "8003798" };
DB::Station SelectedStation;
char InputDate[7] = { "221214" };
char InputHour[3] = { "13" };
bool isDataAvailable = false;
const char* wort[3];
char search[30];
Network::RequestStatus RequestStatus = Network::RequestStatus::NOT_SEND;

struct DeviceInfo
{
	sockaddr_in Adresse;
	std::string Vendor;
};

char InputTextIP[16];
char InputTextPort[6];
char InputTextICMP[16];

std::thread* ClientThread = nullptr;
sockaddr_in testSOck;
std::vector<DeviceInfo> AvailableDevices;
std::vector<sockaddr_in> DeviceList;
char name[50];
char TestPut[100];

extern bool isConnected;
int SelectedDevice;
static bool isSearchWindowOpen = false;

static int WindowSizeX = 800;
static int WindowSizeY = 600;

namespace Window
{
	void RenderWindow()
	{

		static std::vector<DB::TrainInformation>* TrainList;
		
		ImGui::SetNextWindowSize(ImVec2(WindowSizeX, WindowSizeY));
		ImGui::Begin("Test");

	//	ImGui::InputText("Station auswaehlen", InputStation, 8);
		
		if (SelectedStation.StationName.empty())
		{

			if (ImGui::IsItemClicked(ImGui::Button(ICON_FA_SEARCH " Station auswaehlen")))
			{
				isSearchWindowOpen = true;
			}		
		}
		else
		{
			if (ImGui::IsItemClicked(ImGui::Button(SelectedStation.StationName.c_str())))
			{
				isSearchWindowOpen = true;
			}
		}

		if (isSearchWindowOpen)
		{
			SearchWindow();
		}

		static time_t t = time(NULL);
		static struct tm tm = *localtime(&t);
		ImGui::IsItemClicked(ImGui::DateChooser("Datum", tm));
		ImGui::InputText("Hour", InputHour, 3);

		if (ImGui::Button(ICON_FA_SEARCH " Bahn suchen"))
		{
			if (TrainList)
			{
				TrainList->clear();
				delete TrainList;
			}
			if (Network::RequestDataFromServer((char*)SelectedStation.EVA.c_str(), (char*)XMLHelper::DateToDBDate(tm).c_str(), InputHour, Network::RequestType::Planned))
			{
				Network::RequestDataFromServer((char*)SelectedStation.EVA.c_str(), (char*)XMLHelper::DateToDBDate(tm).c_str(), InputHour, Network::RequestType::Changes);
				TrainList = DB::GetListOfTrains();
				isDataAvailable = true;
				RequestStatus = Network::RequestStatus::SUCCESS;
			}
			else
			{
				RequestStatus = Network::RequestStatus::FAILED;
			}

		}



		if (RequestStatus == Network::RequestStatus::NOT_SEND)
		{

		}
		else if (RequestStatus == Network::RequestStatus::FAILED)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(240, 10, 10, 255));
			ImGui::Text("Request failed");
			ImGui::PopStyleColor();
		}

		int selectedItem = 0;


		if (isDataAvailable)
		{
			
			
			ImGui::BeginChildFrame(323, ImVec2(WindowSizeX - 50, WindowSizeY - 300));

			if (ImGui::BeginTable("Züge", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
			{
				ImGui::TableNextColumn();
				ImGui::Text("Startstation");
				ImGui::TableNextColumn();
				ImGui::Text("Endstation");
				for (int n = 0; n < TrainList->size(); ++n)
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::IsItemClicked(ImGui::Selectable(TrainList->at(n).Origin.c_str(), true)))
					{
						std::string Settings = XMLHelper::CreateXMLFile(InputStation, TrainList->at(n).TrainNumber.c_str(), InputHour);
						Client::PutCurrentXMLSettings(Settings);
					}
					ImGui::TableNextColumn();
					if (ImGui::IsItemClicked(ImGui::Selectable(TrainList->at(n).Destination.c_str(), true)))std::cout << "HEY!\n";
					ImGui::TableNextColumn();
					if (ImGui::IsItemClicked(ImGui::Selectable(TrainList->at(n).ArrivalTime.c_str(), true)))std::cout << "HEY!\n";
					ImGui::TableNextColumn();
					if (ImGui::IsItemClicked(ImGui::Selectable(TrainList->at(n).ChangedTime.c_str(), true))) std::cout << "HEY!\n";
				}

				ImGui::EndTable();
			}

			ImGui::EndChildFrame();
		}


		ImGui::End();

	
	}

	bool SearchWindow()
	{

		static std::vector<DB::Station>* Stations = File::ReadInStations();
		ImGui::Begin("Suchen", &isSearchWindowOpen);

		ImGui::InputText("Suchen", search, 30);

		if (Stations)
		{
			if (ImGui::BeginListBox("Worter"))
			{
				const bool is_selected = true;


				for (int n = 0; n < Stations->size(); n++)
				{
					if (strncmp(Stations->at(n).StationName.c_str(), search, strlen(search)) == 0)
					{
						if (ImGui::IsItemClicked(ImGui::Selectable(Stations->at(n).StationName.c_str(), is_selected)))
						{
							SelectedStation = Stations->at(n);
							ImGui::EndListBox();
							ImGui::End();
							return false;
							//strcpy_s(InputStation, Stations->at(n).EVA.c_str());
						}
					}
				}
				ImGui::EndListBox();
			}

		}
		ImGui::End();
		return true;
	}



	void RenderConnectionWindow()
	{
		ImGui::Begin("Connection");

		//		ImGui::InputText("IP-Address", InputTextICMP, 16);
		//		ImGui::InputText("Port", InputTextPort, 6);

		if (ImGui::IsItemClicked(ImGui::Button("Connect")))
		{
			
			if (!ClientThread)
			{
				if (!AvailableDevices.empty())
				{
					char tempIP[17];
					inet_ntop(AvailableDevices[SelectedDevice].Adresse.sin_family, (void*)&AvailableDevices[SelectedDevice].Adresse.sin_addr.s_addr, tempIP, 17);
					std::cout << std::string(tempIP);
					ClientThread = new std::thread(Client::Connect, std::string(tempIP));
				}
				else
				{
				
				}

			}
			else {
				std::cout << "[-] Konnte nicht verbinden1\n";
				if (ClientThread->joinable()) //Joinable gibt True zurück wenn der Thread noch Aufgaben hat und False, wenn der Thread joinen kann.
				{
					std::cout << "[-] Konnte nicht verbinden2\n";
					ClientThread->join();
					delete ClientThread;
					char tempIP[17];
					inet_ntop(AvailableDevices[SelectedDevice].Adresse.sin_family, (void*)&AvailableDevices[SelectedDevice].Adresse.sin_addr.s_addr, tempIP, 17);
					ClientThread = new std::thread(Client::Connect, std::string(tempIP));
					std::cout << "[-] Konnte nicht verbinden3\n";
				}
			}
		}

		if (ImGui::IsItemClicked(ImGui::Button("Disconnect")))
		{
			Client::Disconnect();
		}

		static bool HasThreadExitted = true;
		static bool ShowErrorPopup = false;
		static float ProgressBar = 0.0f;
		static bool ShowProgessBar = false;
		if (ImGui::IsItemClicked(ImGui::Button("Geraete suchen")))
		{
			if (HasThreadExitted)
			{
				std::thread fillListThread([&]()
					{
						ShowProgessBar = true;
						HasThreadExitted = false;
						std::cout << "[+] Thread ID: " << std::this_thread::get_id << "\n";
						std::string IP = Client::GetLocalHostIP();
						DeviceList = Client::TestICMP(IP);
						std::string IPA;
						for (int i = 0; i < DeviceList.size(); ++i)
						{
							std::cout << i << "\n";
							char tempIP[17];
							inet_ntop(DeviceList[i].sin_family, (void*)&DeviceList[i].sin_addr.s_addr, tempIP, 17);
							IPA = Client::GetDeviceName(std::string(tempIP));
							if (IPA != "")
							{
								AvailableDevices.push_back(DeviceInfo{ DeviceList[i], IPA });
							}
							ProgressBar += 1.0f / DeviceList.size();

						}
						std::cout << "[+] Thread ID: " << std::this_thread::get_id << "has exitted\n";
						ShowProgessBar = false;
						HasThreadExitted = true;
					});

				fillListThread.detach();
			} 
			else
			{
				ImGui::OpenPopup("ErrorPop");
				
			}
			isSearchWindowOpen = true;
		}

		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(220, 30, 30, 255));
		if (ImGui::BeginPopup("ErrorPop"))
		{
			ImGui::Text("Suche noch nach Devices");
			ImGui::EndPopup();
		}
		ImGui::PopStyleColor();

		if (ShowProgessBar)
		{
			ImGui::ProgressBar(ProgressBar, ImVec2(150, 25));
		}

		

		if (isSearchWindowOpen)
		{

			if (!AvailableDevices.empty())
			{
				ImGui::BeginChildFrame(3242312, ImVec2(400, 200));
				if (ImGui::BeginListBox("Geraete"))
				{
					static int item_current_idx = 0;
					for (int n = 0; n < AvailableDevices.size(); n++)
					{
						const bool is_selected = (item_current_idx == n);
						if (ImGui::IsItemClicked(ImGui::Selectable(AvailableDevices[n].Vendor.c_str(), is_selected)))
						{
							item_current_idx = n;
							ImGui::OpenPopup("Popup");
							if (is_selected)
								//std::cout << AvailableDevices[(int)item_current_idx].c_str() << "\n";
								ImGui::SetItemDefaultFocus();

							std::cout << AvailableDevices[item_current_idx].Vendor << "\n";
						}
					
						}
					if (ImGui::BeginPopup("Popup"))
					{
						ImGui::Text("Soll dieses Gereat ausgewahlt werden?");
						ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(240, 10, 10, 255));
						if (ImGui::IsItemClicked(ImGui::Button("NEIN")))
						{
							ImGui::CloseCurrentPopup();

						}
						ImGui::PopStyleColor();
						ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(50, 200, 50, 255));
						ImGui::SameLine();
						if (ImGui::IsItemClicked(ImGui::Button("JA")))
						{
							std::cout << AvailableDevices[item_current_idx].Vendor.c_str() << "\n";
							SelectedDevice = item_current_idx;
							ImGui::CloseCurrentPopup();

						}
						
						ImGui::PopStyleColor();


						ImGui::EndPopup();

					}
					ImGui::EndListBox();
					
				}
				ImGui::EndChild();

			}
		}


			ImGui::InputText("TestPut", TestPut, 100);
			if (ImGui::IsItemClicked(ImGui::Button("Send Traininfo")))
			{
				Client::PutCurrentXMLSettings(std::string(TestPut));
				std::string File = Client::GetCurrentXMLSettings();
				if (XMLHelper::IsUserInformationFileValid(File))
				{
					std::cout << "[+] Datei ist richtig formatiert\n";

				}
				else
				{
					std::cout << "[-] Datei ist falsch formatiert\n";
				}
				//Client::Send(XMLHelper::CreateXMLFile("300798", "11034", "23").c_str());
			}

			if (isConnected)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(10, 250, 10, 255));
				ImGui::Text("Verbunden");
				ImGui::PopStyleColor();

			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(240, 10, 10, 255));
				ImGui::Text("Nicht Verbunden");
				ImGui::PopStyleColor();
			}
			static bool ShowSettings = false;
			if (ImGui::IsItemClicked(ImGui::Button("Momentane Einstellungen")))
			{
				if (!isConnected)
				{
					ImGui::OpenPopup("ModalPop");
				}
				else
				{
					ShowSettings = true;
				}
				
			}

			if (ImGui::BeginPopupModal("ModalPop"))
			{
				ImGui::Text("Verbinde dich erst zum Raspberry Pi");
				if (ImGui::IsItemClicked(ImGui::Button("OK")))
				{
					ImGui::CloseCurrentPopup();
				}
				
				ImGui::EndPopup();
			}

			

			if (ShowSettings)
			{
				static std::string CurrentTrainSettings = Client::GetCurrentXMLSettings();
				ImGui::Begin("Einstellungen");
				ImGui::Text(CurrentTrainSettings.c_str());

				ImGui::End();

			}

			static char Time[3];
			ImGui::InputText("Zeit", Time, 3);

			if (ImGui::IsItemClicked(ImGui::Button("Aufstehzeit umstellen")))
			{
				std::string XmlSettings = Client::GetCurrentXMLSettings();

				pugi::xml_document TrainSettings;
				pugi::xml_parse_result iResult = TrainSettings.load_buffer(XmlSettings.c_str(), XmlSettings.size());

				TrainSettings.child("TrainList").child("Train").attribute("WakeUpTime").set_value(Time);

				std::cout << "[+] SETTINGS\n"; TrainSettings.print(std::cout);

				std::stringstream NewSettings;
				TrainSettings.print(NewSettings);
				Client::PutCurrentXMLSettings(NewSettings.str());

			}

			ImGui::Button(ICON_FA_CODE " hey");
		
			ImGui::End();
		}

		void Theme()
		{
			ImGuiStyle* style = &ImGui::GetStyle();

			style->Colors[ImGuiCol_TitleBg] = ImColor(236, 0, 22, 255);
			style->Colors[ImGuiCol_TitleBgActive] = ImColor(236, 0, 22, 255);
			style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(236, 0, 22, 255);
			style->Colors[ImGuiCol_WindowBg] = ImColor(0,0,0, 255);

			style->Colors[ImGuiCol_FrameBg] = ImColor(247, 80, 86, 255);
			style->Colors[ImGuiCol_Button] = ImColor(247, 80, 86, 255);
			style->Colors[ImGuiCol_FrameBgActive] = ImColor(247, 80, 86, 255);
			

			style->WindowTitleAlign = ImVec2(0.5, 0.5);
			style->FramePadding = ImVec2(5, 5);
			style->WindowRounding = 10;
			style->FrameRounding = 6;
		}

		void RenderFinalWindow()
		{
			ImGui::Begin("etzed");

			static time_t t = time(NULL);
			static struct tm tm = *localtime(&t);
			ImGui::DateChooser("Datum", tm);
			std::cout << tm.tm_mday << "    " << tm.tm_year << "\n";
			ImGui::End();
		}
	}


