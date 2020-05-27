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
	auto mSystemIt = _mSystems.find(systemID);
	if (mSystemIt == _mSystems.end())
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

	dspEngine *d = new dspEngine();
	d->removeAllDSPEffectsInSystem(systemID);
	delete d;
}

/*
Go through the channel directory and free channels that are not playing audio
Then, update each system
*/
void FMOD_Handler::update()
{
	vector<_ChannelMap::iterator> channelsToFree;

	for (auto dirIt = _dChannels.begin(); dirIt != _dChannels.end(); dirIt++)
	{
		for (auto mapIt = dirIt->second.begin(); mapIt != dirIt->second.end(); mapIt++)
		{
			bool playing = false;
			mapIt->second->isPlaying(&playing);
			if (!playing)
			{
			        string strSoundName = _mChannelToAudio[mapIt->first];
				_mChannelToAudio.erase(mapIt->first);
				_mAudioToChannel.erase(strSoundName);
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

bool audioEngine::errorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		cout << "FMOD error: " << result << endl;
		return 0;
	}
	return 1;
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
int audioEngine::aePlaySound(string systemID, const string& strSoundName, float fVolumedB)
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
		/*
		if (modeMask & FMOD_3D)
		{
			audioEngine::errorCheck(playChannel->set3DAttributes(&vec3, nullptr));
		}
		*/
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
			string strSoundName = inst->_mChannelToAudio[channelID];
			inst->_mChannelToAudio.erase(channelID);
			inst->_mAudioToChannel.erase(strSoundName);
		}
	}
}

void audioEngine::unloadAllChannelsInSystem(string systemID)
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

		inst->_mAudioToChannel.clear();
		inst->_mChannelToAudio.clear();
	}
}

void audioEngine::togglePauseOnChannel(string systemID, int channelID)
{
	auto inst = FMOD_Handler::instance();
	auto dChannelIt = inst->_dChannels.find(systemID);

	if (dChannelIt != inst->_dChannels.end())
	{
		auto mChannelIt = dChannelIt->second.find(channelID);
		if (mChannelIt != dChannelIt->second.end())
		{
			bool pauseState;
			mChannelIt->second->getPaused(&pauseState);
			mChannelIt->second->setPaused(!pauseState);
		}
	}
}

void audioEngine::setPauseOnChannel(string systemID, int channelID, bool pause)
{
	auto inst = FMOD_Handler::instance();
	auto dChannelIt = inst->_dChannels.find(systemID);

	if (dChannelIt != inst->_dChannels.end())
	{
		auto mChannelIt = dChannelIt->second.find(channelID);
		if (mChannelIt != dChannelIt->second.end())
		{
			mChannelIt->second->setPaused(pause);
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
			float currentVolume;
			mChannelIt->second->getVolume(&currentVolume);
			currentVolume = volumeTodb(currentVolume);
			float newVolume = currentVolume + fVolumedB;
			if (newVolume < 0.0)
			{
				newVolume = 0.0;
			}
			else if (newVolume > 95.0)
			{
				newVolume = 95.0;
			}

			newVolume = dbToVolume(newVolume);
			errorCheck(mChannelIt->second->setVolume(newVolume));
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

void dspEngine::addDSPEffect(string systemID, FMOD_DSP_TYPE dspType)
{
	auto test = FMOD_Handler::instance();
	if (!checkDSPInSystem(systemID, dspType))
	{
		auto inst = FMOD_Handler::instance();
		auto systemsIt = inst->_mSystems.find(systemID);
		if (systemsIt != inst->_mSystems.end())
		{
			FMOD::DSP *dsp;
			FMOD::ChannelGroup *master;

			if (audioEngine::errorCheck(systemsIt->second->createDSPByType(dspType, &dsp)) &&
				audioEngine::errorCheck(systemsIt->second->getMasterChannelGroup(&master)))
			{
				if (audioEngine::errorCheck(master->addDSP(0, dsp)))
				{
					inst->_mDSP.insert(pair<string, FMOD::DSP*>(systemID, dsp));
				}
			}
		}
	}
}

void dspEngine::toggleDSPEffect(string systemID, FMOD_DSP_TYPE dspType)
{
	auto inst = FMOD_Handler::instance();
	FMOD::DSP *targetDSP;

	if (checkDSPInSystem(systemID, dspType, &targetDSP))
	{
		bool toggle;
		if (audioEngine::errorCheck(targetDSP->getBypass(&toggle)))
		{
			targetDSP->setBypass(!toggle);
		}
	}
}

void dspEngine::stopAllDSPEffectsInSystem(string systemID)
{
	auto inst = FMOD_Handler::instance();
	auto dspIt = inst->_mDSP.find(systemID);

	if (dspIt != inst->_mDSP.end())
	{
		auto systemDSPs = inst->_mDSP.equal_range(systemID);
		for (auto it = systemDSPs.first; it != systemDSPs.second; it++)
		{
			it->second->setBypass(true);
		}
	}
	else
	{
		cout << "stopAllDSPEffects warning: System \"" << systemID << "\" has no DSP effects" << endl;
	}
}

void dspEngine::removeDSPEffect(string systemID, FMOD_DSP_TYPE dspType)
{
	FMOD::DSP *dspToRemove;
	if (checkDSPInSystem(systemID, dspType, &dspToRemove))
	{
		auto inst = FMOD_Handler::instance();
		auto systemIt = inst->_mSystems.find(systemID);
		if (systemIt != inst->_mSystems.end())
		{
			FMOD::ChannelGroup *master;
			if (audioEngine::errorCheck(systemIt->second->getMasterChannelGroup(&master)))
			{
				FMOD_DSP_TYPE dspCheck;
				auto systemDSPs = inst->_mDSP.equal_range(systemID);
				for (auto it = systemDSPs.first; it != systemDSPs.second; it++)
				{
					it->second->getType(&dspCheck);
					if (dspCheck == dspType)
					{
						inst->_mDSP.erase(it);
						audioEngine::errorCheck(master->removeDSP(dspToRemove));
						audioEngine::errorCheck(dspToRemove->release());
						break;
					}
				}
			}
		}
	}
}

void dspEngine::removeAllDSPEffectsInSystem(string systemID)
{
	auto inst = FMOD_Handler::instance();

	if (inst->_mDSP.count(systemID))
	{
		FMOD::ChannelGroup *master;
		audioEngine::errorCheck(inst->_mSystems[systemID]->getMasterChannelGroup(&master));

		vector<FMOD_Handler::_dspMap::iterator> mapDSPsToRemove;
		auto systemDSPs = inst->_mDSP.equal_range(systemID);
		for (auto it = systemDSPs.first; it != systemDSPs.second; it++)
		{
			audioEngine::errorCheck(master->removeDSP(it->second));
			audioEngine::errorCheck(it->second->release());
			mapDSPsToRemove.push_back(it);
		}

		for (auto it = mapDSPsToRemove.begin(); it != mapDSPsToRemove.end(); it++)
		{
			inst->_mDSP.erase(*it);
		}
	}
}

void dspEngine::setEchoParameters(string systemID, FMOD_DSP_TYPE dspType, float delay, float feedback, float dry, float wet)
{
	FMOD::DSP *echo;
	if (checkDSPInSystem(systemID, dspType, &echo))
	{
		echo->setParameterFloat(0, delay);
		echo->setParameterFloat(1, feedback);
		echo->setParameterFloat(2, dry);
		echo->setParameterFloat(3, wet);
	}
}

vector<float> dspEngine::getEchoParameters(string systemID, FMOD_DSP_TYPE dspType)
{
	vector<float> values;
	float param0, param1, param2, param3;
	FMOD::DSP *echo;
	if (checkDSPInSystem(systemID, dspType, &echo))
	{
		audioEngine::errorCheck(echo->getParameterFloat(0, &param0, 0, 0));
		values.push_back(param0);
		audioEngine::errorCheck(echo->getParameterFloat(1, &param1, 0, 0));
		values.push_back(param1);
		audioEngine::errorCheck(echo->getParameterFloat(2, &param2, 0, 0));
		values.push_back(param2);
		audioEngine::errorCheck(echo->getParameterFloat(3, &param3, 0, 0));
		values.push_back(param3);
	}
	return values;
}

void dspEngine::setEqParameters(string systemID, FMOD_DSP_TYPE dspType, float lowgain, float midgain, float highgain)
{
	FMOD::DSP *eq;
	if (checkDSPInSystem(systemID, dspType, &eq))
	{
		eq->setParameterFloat(0, lowgain);
		eq->setParameterFloat(1, midgain);
		eq->setParameterFloat(2, highgain);
	}
}

vector<float> dspEngine::getEqParameters(string systemID, FMOD_DSP_TYPE dspType)
{
	vector<float> values;
	float param0, param1, param2;
	FMOD::DSP *eq;
	if (checkDSPInSystem(systemID, dspType, &eq))
	{
		audioEngine::errorCheck(eq->getParameterFloat(0, &param0, 0, 0));
		values.push_back(param0);
		audioEngine::errorCheck(eq->getParameterFloat(1, &param1, 0, 0));
		values.push_back(param1);
		audioEngine::errorCheck(eq->getParameterFloat(2, &param2, 0, 0));
		values.push_back(param2);
	}
	return values;
}

void dspEngine::setFlangeParameters(string systemID, FMOD_DSP_TYPE dspType, float mix, float depth, float rate)
{
	FMOD::DSP *flange;
	if (checkDSPInSystem(systemID, dspType, &flange))
	{
		flange->setParameterFloat(0, mix);
		flange->setParameterFloat(1, depth);
		flange->setParameterFloat(2, rate);
	}
}

void dspEngine::setPitchShiftParameters(string systemID, FMOD_DSP_TYPE dspType, float pitch, float fftsize, float maxchannels)
{
	FMOD::DSP *pitchshift;
	if (checkDSPInSystem(systemID, dspType, &pitchshift))
	{
		pitchshift->setParameterFloat(0, pitch);
		pitchshift->setParameterFloat(1, fftsize);
		pitchshift->setParameterFloat(2, maxchannels);
	}
}

bool dspEngine::checkDSPInSystem(string systemID, FMOD_DSP_TYPE dspType, FMOD::DSP** dspOutput)
{
	auto inst = FMOD_Handler::instance();

	if (inst->_mDSP.count(systemID))
	{
		FMOD_DSP_TYPE dspCheck;
		auto systemDSPs = inst->_mDSP.equal_range(systemID);
		for (auto it = systemDSPs.first; it != systemDSPs.second; it++)
		{
			it->second->getType(&dspCheck);
			if (dspCheck == dspType)
			{
				if (dspOutput)
				{
					*dspOutput = it->second;
				}
				return true;
			}
		}

		cout << "checkDSPInSystem warning: dspType \"" << dspType << "\" not found in _mDSP" << endl;
	}
	else
	{
		cout << "checkDSPInSystem warning: System \"" << systemID << "\" has no DSP effects" << endl;
	}

	return false;
	//Returns false if either the system name does not exist or the DSP effect type is not found
}

inline bool dspEngine::checkIndex(int index, int limit)
{
	if ((index >= 0) && (index <= limit))
	{
		return true;
	}
	return false;
}

//Change the entry point to something/somewhere else.
//This main is here just for quick and dirty testing
/*
int main()
{
	string n = "test System";
	string n1 = "test System 2";
	string m = "audio/drumloop.wav";
	string p = "audio/jaguar.wav";
	auto ae = new audioEngine();
	auto de = new dspEngine();
	ae->init();
	ae->addSystem(n);
	ae->loadSound(n, m, true, true, false);
	int id = ae->aePlaySound(n, m);
	ae->setChannelVolume(n, id, 0);
	ae->setPauseOnChannel(n, id, true);
	ae->setPauseOnChannel(n, id, false);
	while (1) { ae->update(); }
	return 0;
}
*/
