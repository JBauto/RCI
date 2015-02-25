#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>


int main(int argc, char **argv)
{
  //bootIP and bootport are the IP adress e UDP port of the starting server

  //Ringport is the TCP server port used for establishing a TCP session in the ring

  char* bootIP;
  int bootport,ringport,option;

  //By default, bootIP="tejo.ist.utl.pt" and bootport=58000
  bootIP="tejo.ist.utl.pt\0";
  bootport=58000;

  if(argc % 2 == 0 && argc != 1)
    {
      printf("The function doesn't have enough arguments or it has more arguments than it can handle.\n");
      exit(1);
    }

  while((option = getopt(argc,argv,"t:i:p:")) != -1) // Checking the various options received on the console application
  {
    switch(option)
    {
      case 't':
        ringport = atoi(optarg); // Get the argument corresponding to this case and converts it from the string format to integer type
      break;

      case 'i':
        bootIP = optarg;
      break;

      case 'p':
        bootport = atoi(optarg);
      break;

      default:
        printf("The expected arguments don't have the proper format. Application Aborted.\n");
        exit(2);
    }

  }

  printf("\n");

  printf("Welcome to your favorite p2p client! You have chosen the following specifications: \n");

  printf("\n");

  printf("\tSelected ringport: %i\n",ringport);
  printf("\tSelected bootIP: %s\n",bootIP);
  printf("\tSelected bootport: %i\n",bootport);

  printf("\n");
  printf("You can type the following commands: \n");
  printf("\n");

}
