#include "io.h"


void io_init() {
    io_activity_default();

    TRISA2 = 0;
    ANSELA = ANSELA & 0b11111011;
}
