#include "AudioEngine.h"


FMOD_Handler::FMOD_Handler() 
{
	//_system = NULL;
	//audioEngine::errorCheck(FMOD::System_Create(&_system));
	_nextChannelID = 0;
}

FMOD_Handler::~FMOD_Handler()
{
	for (auto it = _mSystems.begin(); it != _mSystems.end(); it++)
	{
		audioEngine::errorCheck(it->second->release());
	}
}

FMOD_Handler *FMOD_Handler::_instance = 0;

FMOD_Handler *FMOD_Handler::instance()
{
	if (_instance == 0)
	{
		_instance = new FMOD_Handler();
	}

	return _instance;
}

//Instantiates a new audio system
//Adds an entry to _mSystems, _dChannels, and _dSounds
//Their keys correspond to the systemID
void FMOD_Handler::addSystem(string systemID)
{
	FMOD::System *newSystem = NULL;
	audioEngine::errorCheck(FMOD::System_Create(&newSystem));
	audioEngine::errorCheck(newSystem->init(512, FMOD_INIT_NORMAL, 0));
	_mSystems[systemID] = newSystem;


	_ChannelMap newChannelMap;
	_dChannels[systemID] = newChannelMap;

	_SoundMap newSoundMap;
	_dSounds[systemID] = newSoundMap;
}

//Removes system and all associated channels and sounds
void FMOD_Handler::removeSystem(string systemID)
{
	auto mChannels = _dChannels[systemID];
	for (auto channelsIt = mChannels.begin(); channelsIt != mChannels.end(); channelsIt++)
	{
		channelsIt->second->stop();
	}
	_dChannels.erase(systemID);

	auto mSounds = _dSounds[systemID];
	for (auto soundsIt = mSounds.begin(); soundsIt != mSounds.end(); soundsIt++)
	{
		soundsIt->second->release();
	}
	_dSounds.erase(systemID);

	_mSystems[systemID]->release();
	_mSystems.erase(systemID);
}

/*
Go through the channel directory and free channels that are not playing audio
Then, update each system
*/
void FMOD_Handler::update()
{
	vector<_ChannelMap::iterator> channelsToFree;

	for (auto dirIt = _dChannels.begin(); dirIt!= _dChannels.end(); dirIt++)
	{
		for (auto mapIt = dirIt->second.begin(); mapIt != dirIt->second.end(); mapIt++)
		{
			bool playing = false;
			mapIt->second->isPlaying(&playing);
			if (!playing)
			{
				channelsToFree.push_back(mapIt);
			}
		}

		for (auto ctfIt = channelsToFree.begin(); ctfIt != channelsToFree.end(); ctfIt++)
		{
			dirIt->second.erase(*ctfIt);
		}

		channelsToFree.clear();
	}

	for (auto sysIt = _mSystems.begin(); sysIt != _mSystems.end(); sysIt++)
	{
		audioEngine::errorCheck(sysIt->second->update());
	}
}

int FMOD_Handler::getNextChannelID()
{
	return _nextChannelID++;
}

void audioEngine::init()
{
	FMOD_Handler::instance();
}

void audioEngine::update()
{
	auto inst = FMOD_Handler::instance();
	inst->update();
}

void audioEngine::addSystem(string systemID)
{
	auto inst = FMOD_Handler::instance();
	inst->addSystem(systemID);
}

void audioEngine::removeSystem(string systemID)
{
	auto inst = FMOD_Handler::instance();
	inst->removeSystem(systemID);
}

int audioEngine::errorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		cout << "FMOD error: " << result << endl;
		return 1;
	}
	return 0;
}

//TODO (Brandon): Handle potential out-of-range exceptions from map.at
void audioEngine::loadSound(string systemID, const string& strSoundName, bool b3d, bool bLooping, bool bStream)
{
	auto inst = FMOD_Handler::instance();
	auto mSounds = inst->_dSounds.at(systemID);

	//Bitmask representing audio properities
	FMOD_MODE modeMask = FMOD_DEFAULT;
	modeMask |= b3d ? FMOD_3D : FMOD_2D; //I think generally we're going to keep all audio 3d. May remove
	modeMask |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	modeMask |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound *sound = NULL;
	audioEngine::errorCheck(inst->_mSystems[systemID]->createSound(strSoundName.c_str(), modeMask, nullptr, &sound));

	inst->_dSounds[systemID][strSoundName] = sound;
}

//TODO (Brandon): Error message on fail
void audioEngine::unloadSound(string systemID, const string &strSoundName)
{
	auto inst = FMOD_Handler::instance();
	auto dSoundsIt = inst->_dSounds.find(systemID);
	if (dSoundsIt != inst->_dSounds.end())
	{
		auto mSoundsIt = dSoundsIt->second.find(strSoundName);
		if (mSoundsIt != dSoundsIt->second.end())
		{
			audioEngine::errorCheck(mSoundsIt->second->release());
			inst->_dSounds[systemID].erase(mSoundsIt);
		}
	}
}

//The volume level should be sent in as decibels.
//The typical decibel level of a normal conversation is 60dB, and a lawnmower is 90dB
int audioEngine::aePlaySound(string systemID, const string& strSoundName, FMOD_VECTOR vec3, float fVolumedB)
{
	auto inst = FMOD_Handler::instance();
	int channelID = inst->getNextChannelID();

	//get the sound map from sound directory via systemID
	//TODO: catch potential out-of-range exception from map.at
	auto mSounds = inst->_dSounds.at(systemID);
	auto soundIt = mSounds.find(strSoundName);

	//if the sound has not been loaded yet, load it
	if (soundIt == mSounds.end())
	{
		//Can't set bitmask this way, so it's generally better to load sounds manually
		loadSound(systemID, strSoundName);
		mSounds = inst->_dSounds.at(systemID);
		soundIt = mSounds.find(strSoundName);
	}

	FMOD::Channel *playChannel = NULL;
	//playChannel initially paused to prevent potential noise while initializing settings
	audioEngine::errorCheck(inst->_mSystems[systemID]->playSound(soundIt->second, nullptr, true, &playChannel));
	if (playChannel)
	{
		FMOD_MODE modeMask = NULL;
		audioEngine::errorCheck(soundIt->second->getMode(&modeMask));

		//Set 3d position if the sound has the 3D attribute
		if (modeMask & FMOD_3D)
		{
			audioEngine::errorCheck(playChannel->set3DAttributes(&vec3, nullptr));
		}

		//setVolume expects a linear volume level, not dB.
		audioEngine::errorCheck(playChannel->setVolume(dbToVolume(fVolumedB)));
		audioEngine::errorCheck(playChannel->setPaused(false));
		
		inst->_dChannels[systemID][channelID] = playChannel;
	}

	return channelID;
}

void audioEngine::unloadChannel(string systemID, int channelID)
{
	auto inst = FMOD_Handler::instance();
	auto dChannelIt = inst->_dChannels.find(systemID);
	if (dChannelIt != inst->_dChannels.end())
	{
		auto mChannelIt = dChannelIt->second.find(channelID);
		if (mChannelIt != dChannelIt->second.end())
		{
			audioEngine::errorCheck(mChannelIt->second->stop());
			inst->_dChannels[systemID].erase(mChannelIt);
		}
	}
}

void audioEngine::unloadAllChannels(string systemID)
{
	auto inst = FMOD_Handler::instance();
	vector<FMOD_Handler::_ChannelMap::iterator> channelsToFree;

	auto dChannelIt = inst->_dChannels.find(systemID);
	if (dChannelIt != inst->_dChannels.end())
	{
		for (auto mapIt = dChannelIt->second.begin(); mapIt != dChannelIt->second.end(); mapIt++)
		{
			errorCheck(mapIt->second->stop());
			channelsToFree.push_back(mapIt);
		}
		
		for (auto ctfIt = channelsToFree.begin(); ctfIt != channelsToFree.end(); ctfIt++)
		{
			dChannelIt->second.erase(*ctfIt);
		}
	}
}

void audioEngine::setChannelVolume(string systemID, int channelID, float fVolumedB)
{
	auto inst = FMOD_Handler::instance();
	auto dChannelIt = inst->_dChannels.find(systemID);
	if (dChannelIt != inst->_dChannels.end())
	{
		auto mChannelIt = dChannelIt->second.find(channelID);
		if (mChannelIt != dChannelIt->second.end())
		{
			float linearVolume = dbToVolume(fVolumedB);
			errorCheck(mChannelIt->second->setVolume(linearVolume));
		}
	}
}

//returning false could either mean the sound isn't playing,
//or that the channel wasn't found.
//TODO (Brandon): This ambiguity shouldn't exist, fix it.
bool audioEngine::aeIsPlaying(string systemID, int channelID) const
{
	auto inst = FMOD_Handler::instance();
	auto dChannelIt = inst->_dChannels.find(systemID);
	if (dChannelIt != inst->_dChannels.end())
	{
		auto mChannelIt = dChannelIt->second.find(channelID);
		if (mChannelIt != dChannelIt->second.end())
		{
			bool playing;
			audioEngine::errorCheck(mChannelIt->second->isPlaying(&playing));
			return playing;
		}
	}
	return false;
}

float audioEngine::dbToVolume(float fVolumedB)
{
	return powf(10.0f, 0.05f * fVolumedB);
}

float audioEngine::volumeTodb(float fVolumeLinear)
{
	return 20.0f * log10f(fVolumeLinear);
}


//Change the entry point to something/somewhere else.
//This main is here just for quick and dirty testing
int main()
{
	string n = "test System";
	string n1 = "test System 2";
	string m = "audio/drumloop.wav";
	string p = "audio/jaguar.wav";
	auto ae = new audioEngine();
	ae->init();
	ae->addSystem(n);
	
	int id = ae->aePlaySound(n, p);
	ae->setChannelVolume(n, id, 0);
	while (1) { ae->update(); }
	return 0;
}