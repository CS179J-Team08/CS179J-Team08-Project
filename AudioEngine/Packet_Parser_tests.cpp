#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "PacketParser.h"

TEST_CASE("Parse of Incoming Data Packets from Python Server", "[PacketParser::parseData]") 
{
	packetParser parser;
	dataPacket audioSettings;
	string data = "{\"group\": [{\"userID\": \"TutorialTestQueue\"},{\"userID\": \"\"},{\"userID\": \"\"},{\"userID\": \"\"},{\"userID\": \"\"}],\"filename\": \"Igorr- Downgrade Desert.flac\",\"play\": \"true\", \"stop\":\"false\", \"parameters\":{\"volume\":\"0.0\", \"echo\":{\"apply\": \"false\", \"delay\":\"0.0\",\"feedback\":\"0.0\",\"dry\":\"0.0\",\"wet\":\"0.0\"}, \"equalizer\":{\"apply\":\"false\", \"lowgain\":\"0.0\", \"midgain\":\"0.0\", \"highgain\":\"0.0\"}}}";
	char* expected = "audio/Igorr- Downgrade Desert.flac\0"; 
	SECTION("Parse Data") {
		parser.parseData(data);
		audioSettings = parser.getCurrentRequest();
		REQUIRE(strcmp(audioSettings.filename, expected) == 0);
		REQUIRE(audioSettings.play == true);
		REQUIRE(audioSettings.volume == 0.0);

	}


}
