#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ifaddrs.h>

extern int DEBUG_MODE;

typedef struct socketStruct
{
  int socketFD;
  struct sockaddr_in * addr;
  socklen_t addrlen;
} socketStruct;

typedef struct ringStruct
{
  socketStruct ListenSocket;
  int ringID;
  int myID;
  char myIP[128];
  int myPort;

  int succiID;
  char succiIP[128];
  int succiPort;
  int succiFD;

  int prediID;
  char prediIP[128];
  int prediPort;
  int prediFD;

  int starter;
  int search_status;
  int NEWfd;
  char externalIP[128];

}ringStruct;

#endif