#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"

typdef struct {
    int floor;
    HardwareOrder orderType;
} FloorOrder;

typedef enum {
    INITIAL_CHECK,
    INITIAL_DRIVE_DOWN,
    DRIVE_DOWN,
    IDLE,
    DRIVE_UP,
    STOP
} State;