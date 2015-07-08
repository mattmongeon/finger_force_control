#ifndef INCLUDED_LOAD_CELL_H
#define INCLUDED_LOAD_CELL_H

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

// Performs various initialiation tasks necessary for properly reading the load cell.
void load_cell_init();


// Returns the most recent reading of the load cell converted to grams.
//
// Return - the most recent value of the load cell in grams.
int load_cell_read_grams();


// Returns the most recent raw load cell value in ADC ticks.
//
// Return - the most recent raw load cell reading in ticks.
int load_cell_raw_value();


#endif // INCLUDED_LOAD_CELL_H

