#pragma once

#define NUMBER_OF_STRINGS 1
#define NUMBER_OF_MODULES 3

// Physical limitation of the cell voltage sensor, until this voltage the cell
// voltage sensors are disabled
#define MODULE_VOLTAGE_FOR_CELL_VOLTAGE 30.0

// Overrides the MODULE_VOLTAGE_FOR_CELL_VOLTAGE check and ask for cell voltage
// directly
#define FORCE_REQUEST_CELL_VOLTAGE 1

// Ensures the live count for each module is increasing each time module state
// is received
#define CHECK_LIVE_COUNT 0

// Ensures the norm for each cell voltage response is 1
#define CHECK_CELL_VOLTAGE_NORM 0