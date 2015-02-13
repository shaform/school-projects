#ifndef IO_H
#define IO_H
#include <regx51.h>
#include "common.h"

uchar get_input_A(void);
uchar get_input_B(void);
// ---------------------------------------------------- //

/* P3.0 RX0, P3.1 TX0 */
/* P3.2 BUZZER */

// Buttons

#define BTNA 1
#define BTNB 2
#define BTNC 3
#define BTND 4

extern bit A_release, B_release;
extern bit A_enable, B_enable;

#endif
