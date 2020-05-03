#ifndef __AUDIO_ENGINE__
#define __AUDIO_ENGINE__

#include "fmod.hpp"
#include <map>
#include <string>
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

/*
 * Handler for calls to the FMOD API itself.
 * Encapsulated to decouple from the audioEngine and dspEngine.
 * Singleton, so that audioEngine will always use the same and only FMOD_Handler,
 * 
 * Ideally, its instance is never invoked outside of calls from audioEngine
 */

class FMOD_Handler
{
public:	
	static FMOD_Handler *instance();
	void update();
	void addSystem(string systemID);
	void removeSystem(string systemID);
	int getNextChannelID();

	typedef map<string, FMOD::System*> _SystemMap;      //Container for all FMOD systems
	typedef map<string, FMOD::Sound*> _SoundMap;
	typedef map<string, _SoundMap> _SoundDirectory;	    //left: System the SoundMap is set to
													    //right: Map of sounds for a given system
	typedef map<int, FMOD::Channel*> _ChannelMap;
	typedef map<string, _ChannelMap> _ChannelDirectory; //left: System the ChannelMap is set to
													    //right: Map of channels for a given system

	typedef multimap<string, FMOD::DSP*> _dspMap; //Container for DSP effects in a system
												  //Distinguish between the DSP effects by searching with DSP::getType

	_SystemMap _mSystems;
	_ChannelDirectory _dChannels;
	_SoundDirectory _dSounds;
	_dspMap _mDSP;
	
protected:
	FMOD_Handler();
	~FMOD_Handler();

private:
	static FMOD_Handler *_instance;
	int _nextChannelID;
};


/*
 * Handler for loading, unloading, playing, stopping, and changing audio
 */
class audioEngine
{
public:
	static void init();
	static void update();
	static bool errorCheck(FMOD_RESULT result); //returns true on FMOD_OK, and false otherwise
	static void addSystem(string systemID);
	static void removeSystem(string systemID);

	void loadSound(string systemID, const string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
	void unloadSound(string systemID, const string& strSoundName);
	int aePlaySound(string systemID, const string& strSoundName, FMOD_VECTOR vec3 = FMOD_VECTOR{ 0, 0, 0 }, float fVolumedB = 0.0f); //Distinct from FMOD_Channel's playSound
	void unloadChannel(string systemID, int channelID); //Can also be used to preemptively stop sound from a channel
	void unloadAllChannelsInSystem(string systemID); //Unload channels in a GIVEN system, not in ALL systems
	void togglePauseOnChannel(string systemID, int channelID);
	void setChannelVolume(string systemID, int channelID, float fVolumedB);
	bool aeIsPlaying(string systemID, int channelID) const; //distinct from FMOD_Channel's isPlaying

	float dbToVolume(float fVolumedB);
	float volumeTodb(float fVolumedB);
};

class dspEngine
{
public:
	void addDSPEffect(string systemID, FMOD_DSP_TYPE dspType); //Cannot have more than one of the same effect
	void toggleDSPEffect(string systemID, FMOD_DSP_TYPE dspType);
	void stopAllDSPEffectsInSystem(string systemID);
	void removeDSPEffect(string systemID, FMOD_DSP_TYPE dspType);
	void removeAllDSPEffectsInSystem(string systemID);


private:
	inline bool checkIndex(int index, int limit);
		//Checks to see if DSP parameter is in-bounds. 

	bool checkDSPInSystem(string systemID, FMOD_DSP_TYPE dspType, FMOD::DSP** dspOutput = NULL);
		//Optionally reference an FMOD::DSP if you want to output the DSP that corresponds to dspType in system systemID.
};

#endif
