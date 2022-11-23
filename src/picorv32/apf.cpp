#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware.h"
#include "uart.h"
#include "apf.h"

// This is to search though the dataslot ram on what the sizes of each
void inline dataslot_search_id(uint16_t value)
{
    int i = 0;
    // while loop from 1 to 5
    while (i <= 5) {
        cout << i << " ";
        ++i;
    }
}

// This will send the read command

void inline dataslot_read(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length)
{


}

// This will send the write command

void inline dataslot_read(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length)
{


}
