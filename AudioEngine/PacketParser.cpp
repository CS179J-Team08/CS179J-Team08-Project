#include "PacketParser.h"
#include <stdio.h>
#include <cstring>

using namespace std;

void packetParser::parseData(string packet)
{
	request.usernames.clear();

	char *strtokState;
	char *packetChar = new char[packet.size() + 1];
	strncpy(packetChar, packet.c_str(), packet.size() + 1);
	const char *delim = "{}[]\':,\"";
	char *token = std::strtok(packetChar, delim/*, &strtokState*/);

	string dir = "audio/";
	char *prefix = new char[dir.size() + packet.size() + 1];
	strncpy(prefix, "audio/", dir.size() + packet.size() + 1);
        
	vector<char *> packetData;
	while (token)
	{
		packetData.push_back(token);
		token = strtok(NULL, delim/*, &strtokState*/);
	}

	for (auto it = packetData.begin(); it != packetData.end(); it++)
	{
		if (strcmp(*it, "userID") == 0)
		{
			if (it + 1 != packetData.end() && strcmp(*(it + 1), "userID") != 0 && strcmp(*(it + 1), "filename") != 0)
			{
				request.usernames.push_back(*(it + 1));
			}
		}
		else if (strcmp(*it, "filename") == 0)
		{
			if (it + 1 != packetData.end() && strcmp(*(it + 1), "userID") != 0)
			{
				strncat(prefix, *(it + 2), dir.size() + packet.size() + 1);
        			request.filename = prefix; 
			}
		}
		else if (strcmp(*it, "play") == 0)
		{
			if (it + 1 != packetData.end())
			{
				if (strcmp(*(it + 2), "true") == 0)
				{
					request.play = true;
				}
				else if (strcmp(*(it + 2), "false") == 0)
				{
					request.play = false;
				}
			}
		}
		else if (strcmp(*it, "volume") == 0)
		{
			if (it + 1 != packetData.end())
			{
                                // this is throwing an exception
				request.volume = stof(*(it + 1));
//                                cout << "error parsing volume, stof throwing exception\n";
			}
		}
	}
/*	
	for (auto it = request.usernames.begin(); it != request.usernames.end(); it++)
	{
		//printf(*it);
		//printf("\n");
	}
	//printf("%s", request.filename);
	//printf("\n");
	if (request.play)
	{
		printf("true\n");
	}
	else
	{
		printf("false\n");
	}
	printf("%f", request.volume);
*/
	
}

void packetParser::applyRequest()
{
	auto inst = FMOD_Handler::instance();
	int channelID;
	addSystem("mainSystem");

	//Potential bug:
	//If the same audio file is passed in again, its channel it is playing on will still be here
	auto atcIt = inst->_mAudioToChannel.find(request.filename);
	if (request.play)
	{
		if (atcIt == inst->_mAudioToChannel.end())
		{
			channelID = aePlaySound("mainSystem", request.filename); //error here
			inst->_mAudioToChannel[request.filename] = channelID;
			atcIt = inst->_mAudioToChannel.find(request.filename);
		}
		else
		{
			setPauseOnChannel("mainSystem", atcIt->second, false);
		}
	}
	else //request.play == false
	{
		if (atcIt == inst->_mAudioToChannel.end())
		{
			loadSound("mainSystem", request.filename);
		}
		else
		{
			setPauseOnChannel("mainSystem", atcIt->second, true);
		}
	}

	if (request.volume != 0.0)
	{
		if (atcIt != inst->_mAudioToChannel.end())
		{
			setChannelVolume("mainSystem", atcIt->second, request.volume);
		}
	}
}



