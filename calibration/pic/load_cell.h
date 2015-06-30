#ifndef INCLUDED_LOAD_CELL_H
#define INCLUDED_LOAD_CELL_H


void load_cell_init();

int load_cell_read_grams();

int load_cell_raw_value();

void load_cell_continuous_raw(int enable);


#endif // INCLUDED_LOAD_CELL_H

