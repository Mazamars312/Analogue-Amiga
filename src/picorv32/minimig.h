#ifndef MINIMIG_H
#define MINIMIG_H

void minigmig_reset(int reset);
void minigmig_startup_dataslot_updates();
void minimig_fdd_update();
void minimig_timer_update();
void minimig_poll_io();

#endif
