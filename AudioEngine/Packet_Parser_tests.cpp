#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "PacketParser.h"

TEST_CASE("Parse of Incoming Data Packets from Python Server", "[PacketParser::parseData]") 
{
	packetParser parser;
	dataPacket audioSettings;
	string data = "{\"group\": [{\"userID\": \"TutorialTestQueue\"},{\"userID\": \"\"},{\"userID\": \"\"},{\"userID\": \"\"},{\"userID\": \"\"}],\"filename\": \"Igorr- Downgrade Desert.flac\",\"play\": \"true\",\"parameters\":   {\"volume\":\"0\"}}";
	char* expected = "audio/Igorr- Downgrade Desert.flac\0"; 
	SECTION("Parse Data") {
		parser.parseData(data);
		audioSettings = parser.getCurrentRequest();
		REQUIRE(strcmp(audioSettings.filename, expected) == 0);
		REQUIRE(audioSettings.play == true);
		REQUIRE(audioSettings.volume == 0);

	}


}
