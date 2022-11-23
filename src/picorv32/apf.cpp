#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware.h"
#include "uart.h"
#include "apf.h"
#include "printf.h"

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

void dataslot_search_active(uint16_t value)
{
    // int i = dataslot_search_id(value);

}

// This will send the read command

void dataslot_read(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length)
{


}

// This will send the write command

void dataslot_write(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length)
{


}
