#ifndef __PACKETPARSER__
#define __PACKETPARSER__
#include "AudioEngine.h"
#include <string>
#include <vector>

using namespace std;

struct echoEffect
{
	bool apply;
	float delay;
	float feedback;
	float dry;
	float wet;
};

struct eqEffect
{
	bool apply;
	float lowgain;
	float midgain;
	float highgain;
};

struct flangeEffect
{
	bool apply;
	float mix;
	float depth;
	float rate;
};

struct pitchEffect
{
	bool apply;
	float pitch;
	float fftsize;
	float maxchannels;
};

struct dataPacket
{
	vector<char*> usernames;
	char *filename;
	bool play;
	bool stop;
	float volume;
	echoEffect echo;
	eqEffect eq;
	flangeEffect flange;
	pitchEffect pitchshift;
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

New packet syntax:
let packet = {
  group: [
	{ userID: "" },
	{ userID: "" },
	{ userID: "" },
	{ userID: "" },
	{ userID: "" },
  ],
  filename: "",
  play: false,
  stop: false,
  parameters: {
	volume: 0.0,
	echo: {
	  apply: false,
	  delay: 10.0,
	  feedback: 0.0,
	  dry: 0.0,
	  wet: 0.0
	},
	equalizer: {
	  apply: false,
	  lowgain: 0.0,
	  midgain: 0.0,
	  highgain: 0.0
	}
  }
};

Old packet syntax:
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