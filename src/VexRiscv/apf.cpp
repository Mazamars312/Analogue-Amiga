/*
 * Copyright 2022 Murray Aickin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware.h"
#include "apf.h"
#include "printf.h"
#include "timer.h"
#include "minimig.h"

#define SSPI_ACK

// This is to search though the dataslot ram looking for the location of the data slot in it
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

// This is to search though the dataslot ram on what the sizes of the dataslow wanted
uint32_t dataslot_size(uint16_t value)
{
  int i;
  i = dataslot_search_id(value);
  printf("number slot: %d \r\n", i);
  return (DATASLOT_RAM_ACCESS((i+1)<<2));
}

// This checks the reg in the system on if there was an apf dataslot update. Once this reg is read it is cleared by hardware
bool dataslot_updated()
{
    return(DATASLOT_UPDATE_REG(0));
}

// This will send the read command to the APF
// The WRITE_* are the regs in the core for the location of the dataslow wanted to be read
// This is used for block data with no pointer changes

uint32_t dataslot_read(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length)
{
  WRITE_TARGET_DATASLOT_ID(0) = dataslot;
  WRITE_TARGET_DATASLOT_BRIDGE_ADD(0) = address;
  WRITE_TARGET_DATASLOT_LENGTH(0) = length;
  WRITE_TARGET_DATASLOT_OFFSET(0) = offset;
  WRITE_TARGET_DATASLOT_CONTROL(0) = TARGET_DATASLOT_READ_REG;
  // printf("WRITE_TARGET_DATASLOT_ID %.4x\r\n", WRITE_TARGET_DATASLOT_ID(0));
  // printf("WRITE_TARGET_DATASLOT_BRIDGE_ADD %.4x\r\n", WRITE_TARGET_DATASLOT_BRIDGE_ADD(0));
  // printf("WRITE_TARGET_DATASLOT_LENGTH %.4x\r\n", WRITE_TARGET_DATASLOT_LENGTH(0));
  // printf("WRITE_TARGET_DATASLOT_OFFSET %.4x\r\n", WRITE_TARGET_DATASLOT_OFFSET(0));

  int apf_codes;
  int i = 0;
  do
  {
    apf_codes = READ_TARGET_DATASLOT_CONTROL(0);
    if ((apf_codes & APF_ERROR) > 0)
    {
      printf("APF FAILD?\r\n", apf_codes & APF_DONE);
      return (apf_codes & APF_ERROR);
    }
    i++;
  } while (!(apf_codes & APF_ACK) | (i <= 1000));

	do
	{
		apf_codes = READ_TARGET_DATASLOT_CONTROL(0);
    if ((apf_codes & APF_ERROR) > 0)
		{
			printf("APF FAILD?\r\n", apf_codes & APF_DONE);
			return (apf_codes & APF_ERROR);
		}
    minimig_input_update();
	} while (!(apf_codes & APF_DONE));
  return(0);
}

// This will send the write command to the APF and send back a error if true
// This is used for block data with no pointer changes

uint32_t dataslot_write(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length)
{
  WRITE_TARGET_DATASLOT_ID(0) = dataslot;
  WRITE_TARGET_DATASLOT_BRIDGE_ADD(0) = address;
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
    minimig_input_update();
	} while (!(apf_codes & APF_DONE));
  return(0);
}

// THis will set the pointers read to go on the APF regs
uint32_t dataslot_read_lba_set_fast(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length)
{
  READ_TARGET_DATASLOT_ID(0) = dataslot;
  READ_TARGET_DATASLOT_BRIDGE_ADD(0) = APF_ADDRESS_OFFSET | address;
  READ_TARGET_DATASLOT_OFFSET(0) = offset << 9;
  READ_TARGET_DATASLOT_LENGTH(0) = length;
  READ_TARGET_DATASLOT_CONTROL(0) = TARGET_DATASLOT_READ_REG;

  return(0);
}

// This will read the data from where the file pointer is (This is set by dataslot_read_lba_set) and does the next batch of data
uint32_t dataslot_read_lba_fast(uint32_t address, uint32_t length)
{
  int i = 0;
  int apf_codes;
  do
	{
    apf_codes = READ_TARGET_DATASLOT_CONTROL(0);
    if ((apf_codes & APF_ERROR) > 0)
		{
			printf("APF FAILD?\r\n", apf_codes & APF_DONE);
			return (apf_codes & APF_ERROR);
		}
    i++;
  } while (!(apf_codes & APF_ACK) | (i <= 1000));


  READ_TARGET_DATASLOT_BRIDGE_ADD(0) = APF_ADDRESS_OFFSET | address;
  READ_TARGET_DATASLOT_LENGTH(0) = length;
  READ_TARGET_DATASLOT_CONTROL(0) = TARGET_DATASLOT_READ_REG;
  // printf("READ_TARGET_DATASLOT_LENGTH Sent %.4x\r\n", WRITE_TARGET_DATASLOT_LENGTH(0));
  do
	{
    apf_codes = READ_TARGET_DATASLOT_CONTROL(0);
  } while (apf_codes & APF_ACK);
  int tmp = WRITE_TARGET_DATASLOT_OFFSET(0);
  WRITE_TARGET_DATASLOT_OFFSET(0) = tmp + length;
  return(0);
}

// THis will set the pointers read to go on the APF regs
uint32_t dataslot_read_lba_set(uint16_t dataslot, uint32_t address, uint32_t offset)
{
  READ_TARGET_DATASLOT_ID(0) = dataslot;
  READ_TARGET_DATASLOT_BRIDGE_ADD(0) = APF_ADDRESS_OFFSET | address;
  READ_TARGET_DATASLOT_OFFSET(0) = offset << 9;

  return(0);
}

// This will read the data from where the file pointer is (This is set by dataslot_read_lba_set)
uint32_t dataslot_read_lba(uint32_t length)
{
  READ_TARGET_DATASLOT_LENGTH(0) = length;
  READ_TARGET_DATASLOT_CONTROL(0) = TARGET_DATASLOT_READ_REG;
  // printf("READ_TARGET_DATASLOT_LENGTH Sent %.4x\r\n", WRITE_TARGET_DATASLOT_LENGTH(0));
  int i = 0;
  int apf_codes;
  do
	{
    apf_codes = READ_TARGET_DATASLOT_CONTROL(0);
    if ((apf_codes & APF_ERROR) > 0)
		{
			printf("APF FAILD?\r\n", apf_codes & APF_DONE);
			return (apf_codes & APF_ERROR);
		}
    i++;
  } while (!(apf_codes & APF_ACK) | (i <= 1000));

	do
	{
		apf_codes = READ_TARGET_DATASLOT_CONTROL(0);
    if ((apf_codes & APF_ERROR) > 0)
		{
			printf("APF FAILD?\r\n", apf_codes & APF_DONE);
			return (apf_codes & APF_ERROR);
		}
    minimig_input_update();
	} while (!(apf_codes & APF_DONE));
  int tmp = WRITE_TARGET_DATASLOT_OFFSET(0);
  WRITE_TARGET_DATASLOT_OFFSET(0) = tmp + length;
  return(0);
}

// THis will set the pointers write to go on the APF regs
uint32_t dataslot_write_lba_set(uint16_t dataslot, uint32_t address, uint32_t offset)
{
  WRITE_TARGET_DATASLOT_ID(0) = dataslot;
  WRITE_TARGET_DATASLOT_BRIDGE_ADD(0) = APF_ADDRESS_OFFSET | address;
  WRITE_TARGET_DATASLOT_OFFSET(0) = offset << 9;
  WRITE_TARGET_DATASLOT_LENGTH(0) = 1;
  WRITE_TARGET_DATASLOT_CONTROL(0) = TARGET_DATASLOT_WRITE_REG;
  // printf("WRITE_TARGET_DATASLOT_ID %.4x\r\n", WRITE_TARGET_DATASLOT_ID(0));
  // printf("WRITE_TARGET_DATASLOT_BRIDGE_ADD %.4x\r\n", WRITE_TARGET_DATASLOT_BRIDGE_ADD(0));
  // printf("WRITE_TARGET_DATASLOT_LENGTH %.4x\r\n", WRITE_TARGET_DATASLOT_LENGTH(0));
  // printf("WRITE_TARGET_DATASLOT_OFFSET %.4x\r\n", WRITE_TARGET_DATASLOT_OFFSET(0));

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

// This will write the data from where the file pointer is (This is set by dataslot_read_lba_set)
uint32_t dataslot_write_lba(uint32_t length)
{
  WRITE_TARGET_DATASLOT_LENGTH(0) = length;
  WRITE_TARGET_DATASLOT_CONTROL(0) = TARGET_DATASLOT_WRITE_REG;
  int apf_codes;
	do
	{
		apf_codes = WRITE_TARGET_DATASLOT_CONTROL(0);
    if ((apf_codes & APF_ERROR) > 0)
		{
			printf("APF FAILD?\r\n", apf_codes & APF_DONE);
			return (apf_codes & APF_ERROR);
		}
    minimig_input_update();
	} while (!(apf_codes & APF_DONE));
  int tmp = WRITE_TARGET_DATASLOT_OFFSET(0);
  WRITE_TARGET_DATASLOT_OFFSET(0) = tmp + length;
  return(0);
}
