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
	}
	
}
