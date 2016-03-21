

#include "project.h"
#include "drv_fmc.h"


int fmc_write( int data1 , int data2)
{
	
	FLASH_Unlock();
  FLASH_OB_Unlock();
	
	int flashstatus; 
	int flasherror = 0;
	
  flashstatus = FLASH_OB_Erase();
	
	if ( flashstatus == FLASH_ERROR_PROGRAM || flashstatus == FLASH_ERROR_WRP || flashstatus ==  FLASH_TIMEOUT)
	{
	//	handle error
	flasherror = 1;	
	}
	
	flashstatus = FLASH_OB_ProgramData( 0x1FFFF804, data1 );
	
	if ( flashstatus == FLASH_ERROR_PROGRAM || flashstatus == FLASH_ERROR_WRP || flashstatus ==  FLASH_TIMEOUT)
	{
	//	handle error
		flasherror = 1;
	}
	
	flashstatus = FLASH_OB_ProgramData( 0x1FFFF806, data2 );
	
	if ( flashstatus == FLASH_ERROR_PROGRAM || flashstatus == FLASH_ERROR_WRP || flashstatus ==  FLASH_TIMEOUT)
	{
	//	handle error
		flasherror = 1;
	}

	
	FLASH_Lock();
	FLASH_OB_Lock();
	return flasherror;
}


// x = readdata( OB->DATA0 );
// x = readdata( OB->DATA1 );
	
int readdata( unsigned int data )
{
	
	unsigned int userdata = data ;
	int complement = ((userdata &0x0000FF00)>>8 );
	complement |=0xFFFFFF00;

	userdata&=0x000000FF;
	
	if ( userdata!=~complement) 
		return 127;
	
	else return userdata;
}

