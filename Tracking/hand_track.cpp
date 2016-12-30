/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0 - Simple Tracker Sample                                *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include<stdio.h>
#include<winsock2.h>
#include <Ws2tcpip.h>
#include <sstream>
#include <NiTE.h>
#include "NiteSampleUtilities.h"
#include <windows.h>



//Function Prototypes: Integer to String Conversion.
std::string IntToString(int);

int main(int argc, char* argv[])
{
	nite::HandTracker handTracker;
	nite::Status niteRc;
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	std::string message_holder;
	//std::string prev_message;
	char * message;
	int init_origin = 0;
	int origin[3];
	int hand_pos[3];

	
	if (argc != 3)
	{
		printf("\nThe correct useage is %s ip-address port-no", argv[0]);
		return 1;
	}

	printf("\nThe selected IP address and port are ip -- %s port --%s", argv[1], argv[2]);
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	// Connection Information.
	inet_pton(AF_INET, argv[1], &server.sin_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[2]));

	//Connect to remote server
	if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	puts("Connected");
	
	//Send some data
	

	niteRc = nite::NiTE::initialize();
	if (niteRc != nite::STATUS_OK)
	{
		printf("NiTE initialization failed\n");
		return 1;
	}

	niteRc = handTracker.create();
	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker\n");
		return 3;
	}

	handTracker.startGestureDetection(nite::GESTURE_WAVE);
	handTracker.startGestureDetection(nite::GESTURE_CLICK);
	printf("\nWave or click to start tracking your hand...\n");

	nite::HandTrackerFrameRef handTrackerFrame;
	while (!wasKeyboardHit())
	{
		Sleep(1000);
		niteRc = handTracker.readFrame(&handTrackerFrame);
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed\n");
			continue;
		}

		const nite::Array<nite::GestureData>& gestures = handTrackerFrame.getGestures();
		for (int i = 0; i < gestures.getSize(); ++i)
		{
			if (gestures[i].isComplete())
			{
				nite::HandId newId;
				handTracker.startHandTracking(gestures[i].getCurrentPosition(), &newId);
				printf("Successfully Detected\n");
			}
		}

		const nite::Array<nite::HandData>& hands = handTrackerFrame.getHands();		
		for (int i = 0; i < hands.getSize(); ++i)
		{			
			const nite::HandData& hand = hands[i];
						
			if (hand.isTracking())
			{
				
				if (init_origin == 0)
				{
					origin[0] = int(hand.getPosition().x);
					origin[1] = int(hand.getPosition().y);
					origin[2] = int(hand.getPosition().z);
					init_origin = 1;
				}

				else 
				{
					hand_pos[0] = origin[0] - int(hand.getPosition().x);
					hand_pos[1] = origin[1] - int(hand.getPosition().y);
					hand_pos[2] = origin[2] - int(hand.getPosition().z);
					message_holder = "$," +IntToString(hand_pos[0]) + "," + IntToString(hand_pos[1]) + "," + IntToString(hand_pos[2])+",#";
					//printf("%d. (%5.2f, %5.2f, %5.2f)\n", hand.getId(), hand.getPosition().x, hand.getPosition().y, hand.getPosition().z);
					printf("\nThe Current Command is %s\n", message_holder.c_str());
					//printf("\nThe current command is %s\n", prev_message.c_str());
					if (send(s, message_holder.c_str(), strlen(message_holder.c_str()), 0) < 0)
					{
							puts("Send failed");
							return 1;
					}					
				}
			}
		}
	}

	nite::NiTE::shutdown();

}


std::string IntToString(int a)
{
	std::ostringstream temp;
	temp << a;
	return temp.str();
}
