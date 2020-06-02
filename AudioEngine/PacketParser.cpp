#define _CRT_SECURE_NO_WARNINGS
#include "PacketParser.h"
#include <stdio.h>
#include <cstring>
#include <iostream>

using namespace std;

void packetParser::parseData(string packet)
{
        auto inst = FMOD_Handler::instance();
        request.usernames.clear();

	char *packetChar = new char[packet.size() + 1];
	strncpy(packetChar, packet.c_str(), packet.size() + 1);
	const char *delim = "{}[]\':,\"";
	char *token = std::strtok(packetChar, delim/*, &strtokState*/);

	string https = "https:";
	char *prefix = new char[https.size() + packet.size() + 1];
	//string dir = "audio/";
	//char *prefix = new char[dir.size() + packet.size() + 1];
	//strncpy(prefix, "audio/", dir.size() + packet.size() + 1);

	vector<char *> packetData;
	while (token)
	{
		packetData.push_back(token);
		token = strtok(NULL, delim/*, &strtokState*/);
	}

	for(int i = 0; i < packetData.size(); i++)
	{
	    printf("%s\n", packetData[i]);
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
		else if (strcmp(*it, "name") == 0)
		{
			if (it + 1 != packetData.end() && strcmp(*(it + 1), "userID") != 0)
			{
			        //strncat(prefix, *(it + 2), dir.size() + packet.size() + 1);
			        //request.filename = prefix;
    			        char *url = new char[https.size() + packet.size() + 1];
				strncpy(url, "https:", https.size() + packet.size() + 1);
				strncat(url, *(it + 3), https.size() + packet.size() + 1);
				
     				inst->playlist.push(url);
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
		else if (strcmp(*it, "stop") == 0)
		{
			if (it + 1 != packetData.end())
			{
				if (strcmp(*(it + 2), "true") == 0)
				{
					request.stop = true;
				}
				else if (strcmp(*(it + 2), "false") == 0)
				{
					request.stop = false;
				}
			}
		}
		else if (strcmp(*it, "volume") == 0)
		{
			if (it + 1 != packetData.end())
			{
				// this is throwing an exception
				request.volume = stof(*(it + 2));
				// cout << "error parsing volume, stof throwing exception\n";
			}
		}
		else if (strcmp(*it, "echo") == 0)
		{
			if (it + 1 != packetData.end())
			{
				if (strcmp(*(it + 2), "apply") == 0)
				{
					if (strcmp(*(it + 4), "true") == 0)
					{
						request.echo.apply = true;
					}
					else if (strcmp(*(it + 4), "false") == 0)
					{
						request.echo.apply = false;
					}
				}
				if (strcmp(*(it + 6), "delay") == 0)
				{
					request.echo.delay = stof(*(it + 8));
				}
				if (strcmp(*(it + 10), "feedback") == 0)
				{
				  request.echo.feedback = stof(*(it + 12));
				}
				if (strcmp(*(it + 14), "dry") == 0)
				{
					request.echo.dry = stof(*(it + 16));
				}
				if (strcmp(*(it + 18), "wet") == 0)
				{
					request.echo.wet = stof(*(it + 20));
				}
			}
		}
		else if (strcmp(*it, "equalizer") == 0)
		{
			if (it + 1 != packetData.end())
			{
				if (strcmp(*(it + 2), "apply") == 0)
				{
					if (strcmp(*(it + 4), "true") == 0)
					{
						request.eq.apply = true;
					}
					else if (strcmp(*(it + 4), "false") == 0)
					{
						request.eq.apply = false;
					}
				}
				if (strcmp(*(it + 6), "lowgain") == 0)
				{
					request.eq.lowgain = stof(*(it + 8));
				}
				if (strcmp(*(it + 10), "midgain") == 0)
				{
					request.eq.midgain = stof(*(it + 12));
				}
				if (strcmp(*(it + 14), "highgain") == 0)
				{
					request.eq.highgain = stof(*(it + 16));
				}
			}
		}
	}
	
	for (auto it = request.usernames.begin(); it != request.usernames.end(); it++)
	{
		printf(*it);
		printf("\n");
	}
	//printf("%s", request.filename);
	printf("\n");
	if (request.play)
	{
		printf("play: true\n");
	}
	else
	{
		printf("play: false\n");
	}
	if (request.stop)
	{
		printf("stop: true\n");
	}
	else
	{
		printf("stop: false\n");
	}
	printf("%f\n", request.volume);
	printf("echo: \n");
	if (request.echo.apply)
	{
		printf("apply == true\n");
	}
	else
	{
		printf("apply == false\n");
	}
	printf("%f\n", request.echo.delay);
	printf("%f\n", request.echo.feedback);
	printf("%f\n", request.echo.dry);
	printf("%f\n", request.echo.wet);
	printf("equalizer: \n");
	if (request.eq.apply)
	{
		printf("apply == true\n");
	}
	else
	{
		printf("apply == false\n");
	}
	printf("%f\n", request.eq.lowgain);
	printf("%f\n", request.eq.midgain);
	printf("%f\n", request.eq.highgain);
	
}

void packetParser::applyRequest()
{        
        auto inst = FMOD_Handler::instance();
	auto d = dspEngine();
	int channelID;
	addSystem("mainSystem");
      
	if (request.stop == true)
	{
	  
	        if(inst->currentChannel)
		{
		        inst->currentChannel->stop();
		}
	}
	else
	{
		if (request.play)
		{

		        if(inst->currentChannel)
			{
  			        setPauseOnCurrentChannel("mainSystem", false);
			}
			else
			{		  
			        loadSound("mainSystem", inst->playlist.front(), false, false, true);
				aePlaySound("mainSystem", inst->playlist.front());
				inst->playlist.pop();
			}
		}
		else //request.play == false
		{		  
		        if(inst->currentChannel)
			{
			        setPauseOnCurrentChannel("mainSystem", true);
			}
		}

		if (request.volume != 0.0)
		{
		        if(inst->currentChannel)
			{
			        setCurrentChannelVolume("mainSystem", request.volume);
			}
		}

		if (request.echo.apply == true)
		{
			d.addDSPEffect("mainSystem", FMOD_DSP_TYPE_ECHO);
			d.setEchoParameters("mainSystem", FMOD_DSP_TYPE_ECHO, request.echo.delay, request.echo.feedback, request.echo.dry, request.echo.wet);
		}
		else
		{
			d.removeDSPEffect("mainSystem", FMOD_DSP_TYPE_ECHO);
		}

		if (request.eq.apply == true)
		{
			d.addDSPEffect("mainSystem", FMOD_DSP_TYPE_THREE_EQ);
			d.setEqParameters("mainSystem", FMOD_DSP_TYPE_THREE_EQ, request.eq.lowgain, request.eq.midgain, request.eq.highgain);
		}
		else
		{
			d.removeDSPEffect("mainSystem", FMOD_DSP_TYPE_THREE_EQ);
		}
	}
}
