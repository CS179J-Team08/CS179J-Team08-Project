#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "AudioEngine.h"

TEST_CASE("FMOD System addition and removal", "[FMOD::System]")
{
	auto inst = FMOD_Handler::instance();
	auto a = new audioEngine();
	a->init();

	SECTION("Test addSystem")
	{
		a->addSystem("Test System");
		REQUIRE(inst->_mSystems.find("Test System") != inst->_mSystems.end());
		REQUIRE(inst->_dChannels.find("Test System") != inst->_dChannels.end());
		REQUIRE(inst->_dSounds.find("Test System") != inst->_dSounds.end());

		//Ensure no two same keys can be inserted.
		a->addSystem("Test System");
		REQUIRE(inst->_mSystems.size() == 1);
		REQUIRE(inst->_dChannels.size() == 1);
		REQUIRE(inst->_dSounds.size() == 1);

		a->addSystem("Test System 2");
		REQUIRE(inst->_mSystems.size() == 2);
		REQUIRE(inst->_mSystems.find("Test System 2") != inst->_mSystems.end());
		REQUIRE(inst->_dChannels.size() == 2);
		REQUIRE(inst->_dChannels.find("Test System 2") != inst->_dChannels.end());
		REQUIRE(inst->_dSounds.size() == 2);
		REQUIRE(inst->_dSounds.find("Test System 2") != inst->_dSounds.end());
	}
	
	SECTION("Test removeSystem")
	{
		a->addSystem("Test System 3");
		a->addSystem("Test System 4");
		a->removeSystem("Test System 3");

		//inst should have 3 systems total, 2 of which are from the previous SECTION
		REQUIRE(inst->_mSystems.size() == 3);
		REQUIRE(inst->_mSystems.find("Test System 3") == inst->_mSystems.end());
		REQUIRE(inst->_dChannels.size() == 3);
		REQUIRE(inst->_dChannels.find("Test System 3") == inst->_dChannels.end());
		REQUIRE(inst->_dSounds.size() == 3);
		REQUIRE(inst->_dSounds.find("Test System 3") == inst->_dSounds.end());

		a->removeSystem("Test System");
		a->removeSystem("Test System 2");
		a->removeSystem("Test System 4");
	}	
}

TEST_CASE("Loading and unloading of sounds", "[FMOD::Sound]")
{
	auto inst = FMOD_Handler::instance();
	inst->playlist.push("audio/jaguar.wav");
	auto a = new audioEngine();
	a->init();
	a->addSystem("Test Load");
	a->loadSound("Test Load", "audio/jaguar.wav", false, true, false);
	auto dSoundsIt = inst->_dSounds.find("Test Load");
	REQUIRE(dSoundsIt != inst->_dSounds.end());
	auto mSoundsIt = dSoundsIt->second.find("audio/jaguar.wav");
	REQUIRE(mSoundsIt != dSoundsIt->second.end());

	FMOD_MODE mode;
	FMOD::Sound *sound = inst->_dSounds["Test Load"]["audio/jaguar.wav"];
	audioEngine::errorCheck(sound->getMode(&mode));
	REQUIRE(mode & FMOD_LOOP_NORMAL);
	
	a->unloadSound("Test Load", "audio/jaguar.wav");
	mSoundsIt = dSoundsIt->second.find("audio/jaguar.wav");
	REQUIRE(mSoundsIt == dSoundsIt->second.end());
}

TEST_CASE("Create Channel via aePlaySound and unload channels", "[FMOD::System::playSound]")
{
        auto inst = FMOD_Handler::instance();
	auto a = new audioEngine();
	string n = "test aePlaySound";
	string m = "audio/jaguar.wav";
	a->addSystem(n);
	inst->playlist.push(m);
	a->loadSound(n, m, false, false, false);
	int channelID = a->aePlaySound(n, m);
	REQUIRE(inst->_dChannels[n][channelID] == inst->currentChannel);  
}

TEST_CASE("DSP Test: Add/Remove DSP effects", "[dspEngine::addDSPEffect/removeDSPEffect]")
{
	auto inst = FMOD_Handler::instance();
	auto a = new dspEngine();

	inst->addSystem("Test Add");
	// Add One, Remove One
	a->addDSPEffect("Test Add", FMOD_DSP_TYPE_DISTORTION);
	auto mDSPIt = inst->_mDSP.find("Test Add");
	REQUIRE(mDSPIt != inst->_mDSP.end());
	a->removeDSPEffect("Test Add", FMOD_DSP_TYPE_DISTORTION);
	REQUIRE(inst->_mDSP.empty());
	
	// Add One, "Remove All"
	a->addDSPEffect("Test Add", FMOD_DSP_TYPE_PARAMEQ);
	mDSPIt = inst->_mDSP.find("Test Add");

	REQUIRE(mDSPIt != inst->_mDSP.end());

	a->removeAllDSPEffectsInSystem("Test Add");
	REQUIRE(inst->_mDSP.empty());
	
	// Add Multiple, "Remove All"
	a->addDSPEffect("Test Add", FMOD_DSP_TYPE_MIXER);
	a->addDSPEffect("Test Add", FMOD_DSP_TYPE_ECHO);
	a->addDSPEffect("Test Add", FMOD_DSP_TYPE_CHORUS);
	a->removeAllDSPEffectsInSystem("Test Add");
	REQUIRE(inst->_mDSP.empty());


}

TEST_CASE("DSP Test: Echo effects", "[dspEffects::setEchoParameters]")
{
	auto inst = FMOD_Handler::instance();
	auto a = new dspEngine();
	inst->addSystem("Test Echo");
	// Add Echo
	a->addDSPEffect("Test Echo", FMOD_DSP_TYPE_ECHO);
	a->setEchoParameters("Test Echo", FMOD_DSP_TYPE_ECHO, 10, 20, 3.0, -4.0);
	vector<float> params = a->getEchoParameters("Test Echo", FMOD_DSP_TYPE_ECHO);
	REQUIRE(params.at(0) == 10);
	REQUIRE(params.at(1) == 20);
	REQUIRE(params.at(2) == 3.0);
	REQUIRE(params.at(3) == -4.0);
	a->removeDSPEffect("Test Echo", FMOD_DSP_TYPE_ECHO);
}

TEST_CASE("DSP Test: EQ effects", "[dspEffects::setEqParameters]")
{
	auto inst = FMOD_Handler::instance();
	auto a = new dspEngine();
	inst->addSystem("Test EQ");
	// Add Eq
	a->addDSPEffect("Test EQ", FMOD_DSP_TYPE_THREE_EQ);
	a->setEqParameters("Test EQ", FMOD_DSP_TYPE_THREE_EQ, 5, -20, 4.6);
	vector<float> params = a->getEqParameters("Test EQ", FMOD_DSP_TYPE_THREE_EQ); 
	REQUIRE(params.at(0) == 5);
	REQUIRE(params.at(1) == -20);
	REQUIRE(params.at(2) == 4.6f);
	a->removeDSPEffect("Test EQ", FMOD_DSP_TYPE_THREE_EQ);
}

TEST_CASE("DSP Test: Flange effects", "[dspEffects::setFlangeParameters]")
{
	auto inst = FMOD_Handler::instance();
	auto a = new dspEngine();
	inst->addSystem("Test Flange");
	// Add Flange
	a->addDSPEffect("Test Flange", FMOD_DSP_TYPE_FLANGE);
	a->setFlangeParameters("Test Flange", FMOD_DSP_TYPE_FLANGE, 50, 0.01, 10);
	vector<float> params = a->getFlangeParameters("Test Flange", FMOD_DSP_TYPE_FLANGE); 
	REQUIRE(params.at(0) == 50);
	REQUIRE(params.at(1) == 0.01);
	REQUIRE(params.at(2) == 10);
	a->removeDSPEffect("Test Flange", FMOD_DSP_TYPE_FLANGE);
}

TEST_CASE("DSP Test: PitchShift effects", "[dspEffects::setPitchShiftParameters]")
{
	auto inst = FMOD_Handler::instance();
	auto a = new dspEngine();
	inst->addSystem("Test Pitch");
	// Add PitchShift
	a->addDSPEffect("Test Pitch", FMOD_DSP_TYPE_PITCHSHIFT);
	a->setPitchShiftParameters("Test Pitch", FMOD_DSP_TYPE_PITCHSHIFT, 1, 1024, 0);
	vector<float> params = a->getPitchShiftParameters("Test Pitch", FMOD_DSP_TYPE_PITCHSHIFT); 
	REQUIRE(params.at(0) == 1);
	REQUIRE(params.at(1) == 1024);
	REQUIRE(params.at(2) == 4.0f)
	REQUIRE(params.at(3) == 0);
	a->removeDSPEffect("Test Pitch", FMOD_DSP_TYPE_PITCHSHIFT);
}

TEST_CASE("Volume test", "[ChannelControl::getVolume]")
{
  
        auto inst = FMOD_Handler::instance();
	auto a = new audioEngine();
	a->init();
	string n = "Test Volume";
	string m = "audio/jaguar.wav";
	a->addSystem(n);
	inst->playlist.push(m);
	a->loadSound(n, m, false, false, false);
	int channel = a->aePlaySound(n, m);
	//a->setPauseOnCurrentChannel(n, true);
	a->setCurrentChannelVolume(n, 20.0);
	float volume;
	REQUIRE(inst->currentChannel->getVolume(&volume) == FMOD_OK);
	REQUIRE(volume == 10.0); //converted from db to linear

	a->setCurrentChannelVolume(n, -20.0);
	REQUIRE(inst->currentChannel->getVolume(&volume) == FMOD_OK);
	REQUIRE(volume == 1.0); //Lower bound

	a->setCurrentChannelVolume(n, 1000.0);
	REQUIRE(inst->currentChannel->getVolume(&volume) == FMOD_OK);
	REQUIRE(volume == 56234.13251f); //Upper bound
  
  /*
	auto inst = FMOD_Handler::instance();
	auto a = new audioEngine();
	a->addSystem("Test Volume");
	int channel = a->aePlaySound("Test Volume", "audio/jaguar.wav");
	a->setPauseOnChannel("Test Volume", channel, true);
	
	a->setChannelVolume("Test Volume", channel, 20.0);	
	auto dChannelIt = inst->_dChannels.find("Test Volume");
	auto mChannelIt = dChannelIt->second.find(channel);
	float volume;
	REQUIRE(mChannelIt->second->getVolume(&volume) == FMOD_OK);
	REQUIRE(volume == 10.0);

	a->setChannelVolume("Test Volume", channel, -20.0);
	REQUIRE(mChannelIt->second->getVolume(&volume) == FMOD_OK);
	REQUIRE(volume == 1.0);

	a->setChannelVolume("Test Volume", channel, 1000);
	REQUIRE(mChannelIt->second->getVolume(&volume) == FMOD_OK);
	REQUIRE(volume == 56234.13251f);
  */
}

