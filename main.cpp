#include <iostream>
#include <getopt.h>
#include "SNMPClient.h"

#include <algorithm>

using namespace std;

int main(int argc, char *argv[]) {

	int flag; // input flags are: -i interval, -c community_string
	opterr = 0; // clear error flag

	int interval = 100; // default interval
	string community_string, agent;

	// check arguments
	while ((flag = getopt(argc, argv, "i:c:")) != -1) {
		switch (flag) {
			case 'i':
				interval = stoi(optarg);
				break;

			case 'c':
				community_string = string{optarg};
				break;

			default:
				abort();
		}
	}

	// look for specified agent
	if (optind < argc) {
		agent = string{argv[optind]};
	} else {
		return 1;
	}

	// create client and run it
	SNMPClient client{agent, community_string, interval};

	/*
	SNMPGetPacket packet{
			SNMPDataType::Sequence,
			SNMPInteger{kSNMPVersion},
			SNMPOctetString{"public"},
			SNMPPDU{
					SNMPDataType::GetNextRequest,
					SNMPInteger{1}, // request_id
					SNMPInteger{0}, // error
					SNMPInteger{0}, // error index
					SNMPVarbindList{
							list<SNMPVarbind>{
									SNMPVarbind{ // add varbind for the object of iftable
											std::list<Byte>{1,3,6,1,2,1,2,2,1,1,2},
											new SNMPValue{
													SNMPDataType::OctetString,
													new SNMPOctetString{"hello world"}
											}
									}
							}
					}
			}
	};

	client.interface_container().ProcessPacket(&packet);
	return 0;*/

	return static_cast<int>(client.Run());
}
