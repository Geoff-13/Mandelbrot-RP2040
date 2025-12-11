/*
    Mandelbrot for the PicoCalc RP2040

    Main routine

    By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
    Date : Dec 2025
    Ver  : 07
*/


#pragma once


//    __________
// __/ Includes \__________________________________________________________________________________
#include  <stdio.h>
#include  <stdlib.h>
#include  <stdbool.h>               //  Standard booleans (bool, true, false)
#include  <ctype.h>                 //  For toupper() used in process_input
#include  <math.h>                  //  For modff() in scr_get_colour()

#include  "pico/stdlib.h"
#include  "drivers/drivers.h"       //  Drivers for keyboard, screen, etc.


//       ______________
// _____/ Global types \___________________________________________________________________________
typedef  int32_t   fix4_28_t;       //  Used for fixed point calculations (4 bit digit, 28 bit decimal)
typedef  uint16_t  col565_t;        //  Colours using 5-bit R, 6-bit G, 5-bit B   Create using RGB_m macro from drivers.h.


//    _________________
// __/ fix4_28 defines \___________________________________________________________________________
#define  fix4_28_to_float_m(a)   ((float)(a) / (1 << 28))


//       __________________________
// _____/ Preprocessor definitions \_______________________________________________________________
#define  PIXEL_FACTOR  0xCD712      //  fix4_28 value for 1/319 which is 1/(LCD_WIDTH-1) for calculating pixel_step


//       ____________________
// _____/ Struct definitions \_____________________________________________________________________
typedef struct {    //  sMand_data
    fix4_28_t  cr_centre;
    fix4_28_t  ci_centre;
    fix4_28_t  width;               //  Width and height the same, so just store one
    fix4_28_t  pixel_step;          //  Precalculated from width
} sMand_data;

typedef struct {       //  sCommon
    uint32_t  calculation_time;     //  In microseconds (wraps after 1 hour 11 minutes !! )
    float     mapping_factor;       //  Controls how frequently the colour map repeats (fixed at compile, see comm_initialize)
    float     repeats;              //  How many times the colour map is used within the iteration limit (on the fly changes)
    int       zoom;
    int       calculation_count[2];
    int       iteration_limit;      //  Iteration limit for current view
    int       cursor_x, cursor_y;
    bool      show_cursor;
    bool      show_text;
    bool      needs_recalc;         //  Need to recalculate
    bool      needs_redraw;         //  Need to redraw screen
} sCommon;


//       __________________________________
// _____/ commands.c function declarations \_______________________________________________________
void  comm_initialize           (void);
void  comm_reset_to_full_image  (void);
void  comm_move_cursor          (char key);
void  comm_zoom_view            (char key);
void  comm_move_view            (char key);


//       _____________________________________
// _____/ calculation.c function declarations \____________________________________________________
void  calc_iterate_the_set      (void);
void  calc_core_1_main          (void);


//       ________________________________
// _____/ screen.c function declarations \_________________________________________________________
void     scr_initialize         (void);
void     scr_destroy            (void);

col565_t scr_get_color          (int iter_count);
void     scr_draw_the_screen    (void);

