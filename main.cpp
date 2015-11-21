#include <iostream>
#include <getopt.h>
#include "SNMPClient.h"

using namespace std;

int main(int argc, char *argv[]) {
	list<Byte> to{};

	SNMPGetPacket ifTablePacket{
			SNMPDataType::Sequence,
			SNMPInteger{kSNMPVersion},
			SNMPOctetString{"public"},
			SNMPPDU{
					SNMPDataType::GetNextRequest,
					SNMPInteger{1879546}, // request_id TODO: generate
					SNMPInteger{0}, // error
					SNMPInteger{0}, // error index
					SNMPVarbindList{
							list<SNMPVarbind>{
									SNMPVarbind{ // add varbind for the object of iftable
											SNMPObjectIdentifier{
													list<Byte>{1, 3, 6, 1, 2, 1, 2, 2} // if table object
											},
											SNMPValue{
													SNMPDataType::Null,
													nullptr
											}
									}
							}
					}
			}
	};

	ifTablePacket.Marshal(to);

	for (auto i = to.begin(); i != to.end(); i++) {
		cout << hex << static_cast<int>(*i) << " ";
	}

	return 0;

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

	return static_cast<int>(client.Run());
}