#define _CRT_SECURE_NO_WARNINGS
#include "PacketParser.h"
#include <stdio.h>
#include <cstring>

using namespace std;

void packetParser::parseData(string packet)
{
	request.usernames.clear();

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
		else if (strcmp(*it, "flange") == 0)
		{
			if (it + 1 != packetData.end())
			{
				if (strcmp(*(it + 2), "apply") == 0)
				{
					if (strcmp(*(it + 4), "true") == 0)
					{
						request.flange.apply = true;
					}
					else if (strcmp(*(it + 4), "false") == 0)
					{
						request.flange.apply = false;
					}
				}
				if (strcmp(*(it + 6), "mix") == 0)
				{
					request.flange.mix = stof(*(it + 8));
				}
				if (strcmp(*(it + 10), "depth") == 0)
				{
					request.flange.depth = stof(*(it + 12));
				}
				if (strcmp(*(it + 14), "rate") == 0)
				{
					request.flange.rate = stof(*(it + 16));
				}
			}
		}
	}
	
	for (auto it = request.usernames.begin(); it != request.usernames.end(); it++)
	{
		printf(*it);
		printf("\n");
	}
	printf("%s", request.filename);
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
	printf("flange: \n"));
	if (request.flange.apply)
	{
		printf("apply == true\n");
	}
	{
		printf("apply == false\n");
	}
	printf("%f\n", request.flange.depth);
	printf("%f\n", request.flange.mix);
	printf("%f\n", request.flange.rate);
}

void packetParser::applyRequest()
{
        update(); //TODO: Should be moved out of this function, once process forking is implemented

        auto inst = FMOD_Handler::instance();
	auto d = dspEngine();
	int channelID;
	addSystem("mainSystem");

	auto atcIt = inst->_mAudioToChannel.find(request.filename);
	if (request.stop == true)
	{
		auto channel = inst->_mAudioToChannel.find(request.filename);
		if (channel != inst->_mAudioToChannel.end())
		{
			unloadChannel("mainSystem",  channel->second);
			unloadSound("mainSystem", request.filename);
		}
	}
	else
	{
		if (request.play)
		{
			if (atcIt == inst->_mAudioToChannel.end())
			{
			        unloadAllChannelsInSystem("mainSystem");
				channelID = aePlaySound("mainSystem", request.filename);
				inst->_mAudioToChannel[request.filename] = channelID;
				inst->_mChannelToAudio[channelID] = request.filename;
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

		if (request.flange.apply == true)
		{
			d.addDSPEffect("mainSystem", FMOD_DSP_TYPE_FLANGE);
			d.setFlangeParameters("mainSystem", FMOD_DSP_TYPE_FLANGE, request.flange.mix, request.flange.depth, request.flange.rate);
		}
		else
		{
			d.removeDSPEffect("mainSystem", FMOD_DSP_TYPE_FLANGE);
		}
	}
}
