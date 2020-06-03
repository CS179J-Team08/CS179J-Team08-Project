#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "PacketParser.h"

TEST_CASE("Parse of Incoming Data Packets from Python Server", "[PacketParser::parseData](default values)") 
{
	packetParser parser;
	dataPacket audioSettings;
	string data = "{\"group\": [{\"userID\": \"TutorialTestQueue\"},{\"userID\": \"\"},{\"userID\": \"\"},{\"userID\": \"\"},{\"userID\": \"\"}],\"filename\": \"drumloop.wav\",\"play\": \"true\", \"stop\":\"false\", \"parameters\":{\"volume\":\"0.0\", \"echo\":{\"apply\": \"false\", \"delay\":\"0.0\",\"feedback\":\"0.0\",\"dry\":\"0.0\",\"wet\":\"0.0\"}, \"equalizer\":{\"apply\":\"false\", \"lowgain\":\"0.0\", \"midgain\":\"0.0\", \"highgain\":\"0.0\"}}}";
	char* expected = "audio/drumloop.wav\0"; 
	SECTION("Parse Data") {
		parser.parseData(data);
		audioSettings = parser.getCurrentRequest();
		REQUIRE(strcmp(audioSettings.filename, expected) == 0);
		REQUIRE(audioSettings.play == true);
		REQUIRE(audioSettings.volume == 0.0);
		REQUIRE(audioSettings.echo.apply == false);
		REQUIRE(audioSettings.echo.delay == 0.0);
		REQUIRE(audioSettings.echo.feedback == 0.0);
		REQUIRE(audioSettings.echo.dry == 0.0);
		REQUIRE(audioSettings.echo.wet == 0.0);
		REQUIRE(audioSettings.eq.apply == false);
		REQUIRE(audioSettings.eq.highgain == 0.0);
		REQUIRE(audioSettings.eq.midgain == 0.0);
		REQUIRE(audioSettings.eq.lowgain == 0.0);
	}
}

TEST_CASE("Parse of Incoming Data Packets from Python Server", "[PacketParser::parseData](Reasonable Values)") 
{
	packetParser parser;
	dataPacket audioSettings;
	string data = "{\"group\": [{\"userID\": \"TutorialTestQueue\"},{\"userID\": \"\"},{\"userID\": \"\"},{\"userID\": \"\"},{\"userID\": \"\"}],\"filename\": \"drumloop.wav\",\"play\": \"true\", \"stop\":\"true\", \"parameters\":{\"volume\":\"12.5\", \"echo\":{\"apply\": \"true\", \"delay\":\"898\",\"feedback\":\"5000\",\"dry\":\"25\",\"wet\":\"-60\"}, \"equalizer\":{\"apply\":\"true\", \"lowgain\":\"561.45\", \"midgain\":\"9999\", \"highgain\":\"-9999\"}}}";
	char* expected = "audio/drumloop.wav\0"; 
	SECTION("Parse Data") {
		parser.parseData(data);
		audioSettings = parser.getCurrentRequest();
		REQUIRE(strcmp(audioSettings.filename, expected) == 0);
		REQUIRE(audioSettings.play == true);
		REQUIRE(audioSettings.stop == false);
		REQUIRE(audioSettings.volume == 3);
		REQUIRE(audioSettings.echo.apply == true);
		REQUIRE(audioSettings.echo.delay == 500);
		REQUIRE(audioSettings.echo.feedback == 50);
		REQUIRE(audioSettings.echo.dry == -2);
		REQUIRE(audioSettings.echo.wet == 6);
		REQUIRE(audioSettings.eq.apply == true);
		REQUIRE(audioSettings.eq.highgain == 0);
		REQUIRE(audioSettings.eq.midgain == 2);
		REQUIRE(audioSettings.eq.lowgain == -3);
	}
}

TEST_CASE("Parse of Incoming Data Packets from Python Server", "[PacketParser::parseData](Unreasonable Values)") 
{
	packetParser parser;
	dataPacket audioSettings;
	string data = "{\"group\": [{\"userID\": \"TutorialTestQueue\"},{\"userID\": \"\"},{\"userID\": \"\"},{\"userID\": \"\"},{\"userID\": \"\"}],\"filename\": \"drumloop.wav\",\"play\": \"true\", \"stop\":\"true\", \"parameters\":{\"volume\":\"12.5\", \"echo\":{\"apply\": \"true\", \"delay\":\"898\",\"feedback\":\"5000\",\"dry\":\"25\",\"wet\":\"-60\"}, \"equalizer\":{\"apply\":\"true\", \"lowgain\":\"561.45\", \"midgain\":\"9999\", \"highgain\":\"-9999\"}}}";
	char* expected = "audio/drumloop.wav\0"; 
	SECTION("Parse Data") {
		parser.parseData(data);
		audioSettings = parser.getCurrentRequest();
		REQUIRE(strcmp(audioSettings.filename, expected) == 0);
		REQUIRE(audioSettings.play == true);
		REQUIRE(audioSettings.stop == true);
		REQUIRE(audioSettings.volume == 12.5);
		REQUIRE(audioSettings.echo.apply == true);
		REQUIRE(audioSettings.echo.delay == -898);
		REQUIRE(audioSettings.echo.feedback == 5000);
		REQUIRE(audioSettings.echo.dry == 25);
		REQUIRE(audioSettings.echo.wet == -60);
		REQUIRE(audioSettings.eq.apply == false);
		REQUIRE(audioSettings.eq.highgain == 561.45);
		REQUIRE(audioSettings.eq.midgain == 9999);
		REQUIRE(audioSettings.eq.lowgain == -9999);
	}
}