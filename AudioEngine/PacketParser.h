#ifndef __PACKETPARSER__
#define __PACKETPARSER__
#include "AudioEngine.h"
#include <string>
#include <vector>

using namespace std;

//Should make it so that addSystem in AudioEngine will not make a system if one of the same name already exists
struct dataPacket
{
	vector<char*> usernames;
	char *filename;
	bool play;
	float volume;
};

class packetParser : public audioEngine
{
public:
	void parseData(string packet);
	void applyRequest();
	dataPacket getCurrentRequest() { return request; }

private:
	dataPacket request;
};

/*

{
  group: [
	{ userID: "" },
	{ userID: "" },
	{ userID: "" },
	{ userID: "" },
	{ userID: "" },
  ],
  filename: "",
  play: false,
  parameters: {
	volume: 0.0
  }
}

*/

#endif 
