#ifndef IO_H
#define IO_H

#include <xc.h>


void io_init();


#define io_activity_default()  { LATA2 = 1; }
#define io_activity_active()  { LATA2 = 0; }


#endif
