#ifndef MINIMIG_H
#define MINIMIG_H

#include "minimig/minimig_fdd.h"
#include "apf.h"
#include "spi.h"
#include "printf.h"

void minigmig_reset(boolen reset);
void minigmig_startup_dataslot_updates();
void minimig_fdd_update();
void minimig_timer_update();
void minimig_poll_io();


#endif
