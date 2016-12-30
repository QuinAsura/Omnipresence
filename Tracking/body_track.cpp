/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0 - Simple Skeleton Sample                               *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include<stdio.h>
#include<winsock2.h>
#include <Ws2tcpip.h>
#include <sstream>
#include <windows.h>
#include <NiTE.h>
#include "NiteSampleUtilities.h"
#define LEFT_RIGHT_TIP 0
#define SOCKET_ENABLE 1
#define MAX_USERS 10
bool g_visibleUsers[MAX_USERS] = { false };
nite::SkeletonState g_skeletonStates[MAX_USERS] = { nite::SKELETON_NONE };
int send_flag = 0;
#define USER_MESSAGE(msg) \
	{printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

//Function Prototypes: Integer to String Conversion.
std::string IntToString(int);

void updateUserState(const nite::UserData& user, unsigned long long ts)
{
	if (user.isNew())
	{
		USER_MESSAGE("New")
			send_flag = 0;
	}
	else if (user.isVisible() && !g_visibleUsers[user.getId()])
	{
		USER_MESSAGE("Visible")
			send_flag = 0;
	}
	else if (!user.isVisible() && g_visibleUsers[user.getId()])
	{
		USER_MESSAGE("Out of Scene")
			send_flag = 0;
	}
	else if (user.isLost())
	{
		USER_MESSAGE("Lost")
			send_flag = 0;
	}
		g_visibleUsers[user.getId()] = user.isVisible();

	
	if (g_skeletonStates[user.getId()] != user.getSkeleton().getState())
	{
		switch (g_skeletonStates[user.getId()] = user.getSkeleton().getState())
		{
		case nite::SKELETON_NONE:
			USER_MESSAGE("Stopped tracking.")
				send_flag = 0;
				break;
		case nite::SKELETON_CALIBRATING:
			USER_MESSAGE("Calibrating...")
				send_flag = 0;
				break;
		case nite::SKELETON_TRACKED:
			USER_MESSAGE("Tracking!")
				send_flag = 1;
				break;
		case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
		case nite::SKELETON_CALIBRATION_ERROR_HANDS:
		case nite::SKELETON_CALIBRATION_ERROR_LEGS:
		case nite::SKELETON_CALIBRATION_ERROR_HEAD:
		case nite::SKELETON_CALIBRATION_ERROR_TORSO:
			USER_MESSAGE("Calibration Failed... :-|")
				break;
		}
	}
}

int main(int argc, char** argv)
{
#if SOCKET_ENABLE
	//Socket Initialisation
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	std::string message_holder ("Mess");
	std::string prev_message ("Prev Holder");
#endif
	int init_origin = 0;
	int origin[3];
	int prev[3];

	//Nite Initialisation:
	nite::UserTracker userTracker;
	nite::Status niteRc;
	nite::NiTE::initialize();
	niteRc = userTracker.create();

#if SOCKET_ENABLE
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
#endif

	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker\n");
		return 3;
	}
	printf("\nStart moving around to get detected...\n(PSI pose may be required for skeleton calibration, depending on the configuration)\n");

	nite::UserTrackerFrameRef userTrackerFrame;
	while (!wasKeyboardHit())
	{
		Sleep(5);
		niteRc = userTracker.readFrame(&userTrackerFrame);
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed\n");
			continue;
		}

		const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
		for (int i = 0; i < users.getSize(); ++i)
		{
			const nite::UserData& user = users[i];
			updateUserState(user, userTrackerFrame.getTimestamp());
			if (user.isNew())
			{
				userTracker.startSkeletonTracking(user.getId());
			}
			else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
			{
				const nite::SkeletonJoint& torso = user.getSkeleton().getJoint(nite::JOINT_TORSO);
				const nite::SkeletonJoint& left_tip = user.getSkeleton().getJoint(nite::JOINT_LEFT_HAND);
				const nite::SkeletonJoint& right_tip = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND);
				if (torso.getPositionConfidence() > .5)
				{
					if (init_origin == 0)
					{
						origin[0] = int(torso.getPosition().x);
						origin[1] = int(torso.getPosition().y);
						origin[2] = int(torso.getPosition().z);
						init_origin = 1;
						prev[0] = int(torso.getPosition().x);
						prev[1] = int(torso.getPosition().y);
						prev[2] = int(torso.getPosition().z);
					}
					else
					{
						int side = prev[0] - int(torso.getPosition().x);
						int forward = prev[2] - int(torso.getPosition().z);
						prev[0] = int(torso.getPosition().x);
						prev[2] = int(torso.getPosition().z);
						float dx = side*0.4 ;
						float dz = forward*0.5;
						if (dz > 8)
						{
							if (dz > 20)
								dz = 20;
							message_holder = "U"+IntToString(int(dz));

							if (prev_message.compare(message_holder) != 0)
							{
								if (send(s, message_holder.c_str(), strlen(message_holder.c_str()), 0) < 0)
								{
									puts("Send failed");
									return 1;
								}
							}
						}
						else if (dz < -8)
						{
							if (dz < -20)
								dz = -20;
							message_holder = "D" +IntToString(int(dz)) ;
							if (prev_message.compare(message_holder) != 0)
							{
								if (send(s, message_holder.c_str(), strlen(message_holder.c_str()), 0) < 0)
								{
									puts("Send failed");
									return 1;
								}
							}
						}
						else if (dx > 8)
						{
							if (dx > 20)
								dx = 20;
							message_holder = "L" + IntToString(int(dx));
							if (prev_message.compare(message_holder) != 0)
							{
								if (send(s, message_holder.c_str(), strlen(message_holder.c_str()), 0) < 0)
								{
									puts("Send failed");
									return 1;
								}
							}
						}
						else if (dx < -8)
						{
							if (dx < -20)
								dx = -20;
							message_holder = "R" + IntToString(int(dx)) ;
							if (prev_message.compare(message_holder) != 0)
							{
								if (send(s, message_holder.c_str(), strlen(message_holder.c_str()), 0) < 0)
								{
									puts("Send failed");
									return 1;
								}
							}
						}
						else
						{
							//send stop and check for speed.
							message_holder = "STOP";
							if (prev_message.compare(message_holder) != 0)
							{
								if (send(s, message_holder.c_str(), strlen(message_holder.c_str()), 0) < 0)
								{
									puts("Send failed");
									return 1;
								}
							}
#if LEFT_RIGHT_TIP
							if (left_tip.getPositionConfidence() > .5)	
							{
								printf("Left TIP :%d. (%5.2f, %5.2f, %5.2f)\n", user.getId(), left_tip.getPosition().x, left_tip.getPosition().y, left_tip.getPosition().z);
							}
							if (right_tip.getPositionConfidence() > .5)
							{
								printf("RIGHT TIP: %d. (%5.2f, %5.2f, %5.2f)\n", user.getId(), right_tip.getPosition().x, right_tip.getPosition().y, right_tip.getPosition().z);
							}
#endif
						}
						prev_message = message_holder;
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
