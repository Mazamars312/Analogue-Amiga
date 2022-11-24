#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware.h"
#include "apf.h"
#include "printf.h"

#define SSPI_ACK

// This is to search though the dataslot ram on what the sizes of each
uint32_t dataslot_search_id(uint16_t value)
{
    int i = 0;
    // while loop from 1 to 5
    do  {
      if (DATASLOT_RAM_ACCESS(i<<2) == value){
        return (i);
      };
      i=i+2;
    } while (i <= 0x100);
    return (0);
}

uint32_t dataslot_size(uint16_t value)
{
  int i;
  i = dataslot_search_id(value);
  printf("number slot: %d \r\n", i);
  return (DATASLOT_RAM_ACCESS((i+1)<<2));
}

bool dataslot_updated()
{
    return(DATASLOT_UPDATE_REG(0));
}

// This will send the read command to the APF
// The WRITE_* are the regs in the core for the location of the

uint32_t dataslot_read(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length)
{
  WRITE_TARGET_DATASLOT_ID(0) = dataslot;
  WRITE_TARGET_DATASLOT_BRIDGE_ADD(0) = APF_ADDRESS_OFFSET | address;
  WRITE_TARGET_DATASLOT_LENGTH(0) = length;
  WRITE_TARGET_DATASLOT_OFFSET(0) = offset;
  WRITE_TARGET_DATASLOT_CONTROL(0) = TARGET_DATASLOT_READ_REG;
  int apf_codes;
	do
	{
		apf_codes = READ_TARGET_DATASLOT_CONTROL(0);
    if ((apf_codes & APF_ERROR) > 0)
		{
			printf("APF FAILD?\r\n", apf_codes & APF_DONE);
			return (apf_codes & APF_ERROR);
		}
	} while (!(apf_codes & APF_DONE));
  return(0);
}


// This will send the write command to the APF and send back a error if true

uint32_t dataslot_write(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length)
{
  WRITE_TARGET_DATASLOT_ID(0) = dataslot;
  WRITE_TARGET_DATASLOT_BRIDGE_ADD(0) = APF_ADDRESS_OFFSET | address;
  WRITE_TARGET_DATASLOT_LENGTH(0) = length;
  WRITE_TARGET_DATASLOT_OFFSET(0) = offset;
  WRITE_TARGET_DATASLOT_CONTROL(0) = TARGET_DATASLOT_WRITE_REG;
  int apf_codes;
	do
	{
		apf_codes = READ_TARGET_DATASLOT_CONTROL(0);
    if ((apf_codes & APF_ERROR) > 0)
		{
			printf("APF FAILD?\r\n", apf_codes & APF_DONE);
			return (apf_codes & APF_ERROR);
		}
	} while (!(apf_codes & APF_DONE));
  return(0);
}
