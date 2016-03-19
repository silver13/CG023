

#include "project.h"


#define offset 0x08007C00 // last K of 32k

int fmc_erasepage( void)
{
 return 0;
}

int fmc_write( unsigned int address , int data)
{

	return 0;
}

int fmc_read( unsigned int address )
{
	return 0;
//address = address*4+offset;
//unsigned int* addressptr = (unsigned int*) address;
//return (*addressptr );
}

