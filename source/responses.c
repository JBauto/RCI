#include "responses.h"
#include "triggers.h"
#include "network.h"

int FDsocket;

int Message_ID(ringStruct* node,char * request, int senderSocket)
{
	char msg[128], cmd[128];
	int ID;

	if(sscanf(request,"%s %d",cmd,&ID) != 2)
	{
		printf("Bad Message (ID)\n");
		return 1;
	}

	if(strcmp(cmd,"ID") == 0)
	{
		node->NEWfd = senderSocket;
		memset(msg,0,128);
		if((node->succiFD == -1 && node->prediFD == -1) || responsability(node->prediID,node->myID,ID))
		{
			sprintf(msg,"SUCC %d %s %d\n",node->myID, node->myIP, node->myPort);
			sendTCP(msg, senderSocket);
		}
		else
		{
			sprintf(msg,"QRY %d %d\n",node->myID,ID);			
			sendTCP(msg, node->succiFD);
		}
		message_handler(DEBUG_MODE,5,msg,node,0);
		return 0;
	}
	return 1;
}

int Message_NEW(ringStruct* node, char* request, int senderSocket)
{
	char  cmd[128], msg[128];
	int  Port, ID;
	char IP[128];
	extern int startTimer;
	socketStruct tmp;
	if(sscanf(request,"%s %d %s %d",cmd,&ID,IP,&Port) != 4)
	{
		printf("Bad Message (NEW)\n");
		return 1;
	}

	if(strcmp(cmd,"NEW") == 0)
	{
		if(node->succiID == -1 && node->prediID == -1)
		{
			node->prediID = ID;
			strcpy(node->prediIP,IP);
			node->prediPort = Port;
			node->prediFD = senderSocket;
			tmp = setupSocket(IP,Port,'T');
			node->succiID = ID;
			strcpy(node->succiIP,IP);
			node->succiPort = Port;
			node->succiFD = tmp.socketFD;
			memset(msg,0,128);
			sprintf(msg,"NEW %d %s %d\n", node->myID, node->myIP, node->myPort);
			sendTCP(msg,node->succiFD);
			message_handler(DEBUG_MODE,6,NULL,node,0);
		    return 0;
		}
		else
		{
			if(node->prediID == -1)
			{
				node->prediID = ID;
				strcpy(node->prediIP,IP);
				node->prediPort = Port;
				node->prediFD = senderSocket;
				startTimer = 0;
			}
			else
			{
				memset(msg,0,128);
				sprintf(msg,"CON %d %s %d\n",ID, IP, Port);
				sendTCP(msg,node->prediFD);
				close(node->prediFD);
				node->prediID = ID;
				strcpy(node->prediIP,IP);
				node->prediPort = Port;
				node->prediFD = FDsocket;
			}
			message_handler(DEBUG_MODE,7,NULL,node,0);
			return 0;
		}
	}
	return 1;
}

int Message_RSP(ringStruct* node, char* request)
{
	char cmd[128], msg[128];
	int  Port, ID, Master, Destination, n;
	char IP[128];
	char trailing_message[20];

	n = sscanf(request,"%s %d %d %d %s %d %s",cmd,&Master,&ID,&Destination,IP,&Port,trailing_message);
	//printf("RSP. n=%d\n", n);
	if(n != 6)
	{
		//printf("%s", trailing_message);
		if(n == 7 && strcmp(trailing_message, "FAILED") == 0)
		{
			if(node->myID == Master)
			{
				socketStruct tmp = setupSocket(IP, Port, 'T');
				sprintf(msg,"CON %d %s %d\n",node->myID, node->myIP, node->myPort);
				sendTCP(msg, tmp.socketFD);
				close(tmp.socketFD);
				return 0;
			}
			else
			{
				sendTCP(request,node->prediFD);
				return 0;
			}
		}
		else
		{
		printf("Bad Message (RSP)\n");
		return 1;
		}
	}

	if(node->search_status == 0)
	{
		if(Master == node->myID)
		{
			sprintf(msg,"SUCC %d %s %d\n",Destination, IP, Port);
			sendTCP(msg, node->NEWfd);
			message_handler(DEBUG_MODE,8,msg,node,0);
		}
		else
		{
			sendTCP(request,node->prediFD);
		}
	}
	else
	{
		if(node->myID == Master)
		{
				printf("Search Result: %i %s %i\n", Destination, IP, Port);
				node->search_status = 0;
		}
		else
			sendTCP(request,node->prediFD);
	}
	return 0; //REVER
}

int Message_QRY(ringStruct*node, char* request)
{
	char cmd[128], msg[128];
	int   ID, Master;


	if(sscanf(request,"%s %d %d",cmd, &Master, &ID) != 3)
	{
		printf("Bad Message (QRY)\n");
		return 1;
	}

	if(node->succiFD == -1 && node->prediFD == -1)
	{
		printf("Something went wrong with socket creation\n");
		return 1;
	}
	else
	{
		if(node->succiID == -1) // broken ring
		{
			memset(msg,0,128);
			sprintf(msg,"RSP %d %d %d %s %d FAILED\n",Master, ID,node->myID,node->myIP,node->myPort);
			sendTCP(msg, node->prediFD);
			printf("Ring is broken, dealing with it.\n");
		}

		else if(responsability(node->prediID,node->myID,ID))
		{
			memset(msg,0,128);
			sprintf(msg,"RSP %d %d %d %s %d \n",Master, ID,node->myID,node->myIP,node->myPort);
			sendTCP(msg, node->prediFD);
			message_handler(DEBUG_MODE,9,msg,node,0);
		}
		else
		{
			sendTCP(request, node->succiFD);
			message_handler(DEBUG_MODE,10,request,node,0);
		}
		message_handler(DEBUG_MODE,8,msg,node,0);
		return 0;
	 }
	 return 1;
}

int Message_SUCC(ringStruct*node, char* request)
{
	char cmd[128];
	int dest_ID, dest_Port;
	char dest_IP[128];

	if(sscanf(request,"%s %d %s %d",cmd, &dest_ID, dest_IP, &dest_Port) != 4)
	{
		printf("Bad Message (SUCC)\n");
		return 1;
	}
	while(dest_ID == node->myID)
	{
		printf("Can't use identifier %d, please choose a different one and try again\n",node->myID);
        node->myID = -1;
        return 1;
	}
	joinRing_KnownSucci(node, dest_ID, dest_IP, dest_Port);
	message_handler(DEBUG_MODE,8,request,node,0);
	return 0;
}

int Message_CON(ringStruct* node, char* request)
{
	char cmd[128], msg[128];
	int dest_ID, dest_Port;
	char dest_IP[128];

	if(sscanf(request,"%s %d %s %d",cmd, &dest_ID, dest_IP, &dest_Port) != 4)
	{
		printf("Bad Message (CON)\n");
		return 1;
	}

	close(node->succiFD);
	joinRing_KnownSucci(node, dest_ID, dest_IP, dest_Port);

	message_handler(DEBUG_MODE,8,msg,node,0);
	return 0;
}

int Message_BOOT(ringStruct* node, char* request)
{
    node->starter=1;
    return 0;
}

int JR_Message(char* request,ringStruct* node, int nodeFD)
{
	char cmd[128];
	int tmp;
	message_handler(DEBUG_MODE,12,request,node,0);
	FDsocket = nodeFD;
	sscanf(request,"%s",cmd);
	if(strcmp(cmd,"NEW") == 0)
	{
		tmp = Message_NEW(node,request,nodeFD);
		return (tmp == 1) ? tmp : 0;
	}
	if(strcmp(cmd,"ID") == 0)
	{
		tmp = Message_ID(node,request,nodeFD);
		return (tmp == 1) ? tmp : 0;
	}
	if(strcmp(cmd,"RSP") == 0)
	{
		tmp = Message_RSP(node,request);
		return (tmp == 1) ? tmp : 0;
	}
	if(strcmp(cmd,"QRY") == 0)
	{
		tmp = Message_QRY(node,request);
		return (tmp == 1) ? tmp : 0;
	}
	if(strcmp(cmd,"SUCC") == 0)
	{
		tmp = Message_SUCC(node,request);
		return (tmp == 0) ? tmp : 1;
	}
	if(strcmp(cmd,"CON") == 0)
	{
		tmp = Message_CON(node,request);
		return (tmp == 1) ? tmp : 0;
	}
	if(strcmp(cmd,"BOOT") == 0)
	{
		tmp = Message_BOOT(node,request);
		return (tmp == 0) ? tmp : 1;
	}
	return 1;
}
