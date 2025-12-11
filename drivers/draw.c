/*
    PicoCalc program template

    My code for drawing directly to the LCD or to a frame_buffer

    ***************************************************************
    *  Customized for Mandelbrot.  DO NOT use for other projects  *
    *  Uses the frame_buffer only.                                *
    ***************************************************************

    By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
    Date : Nov 2025
    Ver  : 05

*/

#include  "drivers.h"


//    _________________
// __/ Local variables \___________________________________________________________________________
static col565_t  *frame_buffer = NULL;
mutex_t          blit_mutex;



// ************************************************************************************************
// **********     LOCAL FUNCTIONS     *************************************************************
// ************************************************************************************************



// ************************************************************************************************
// **********     GLOBAL FUNCTIONS     ************************************************************
// ************************************************************************************************



//    _______
// __/ Pixel \_____________________________________________________________________________________
void  draw_pixel(int16_t x, int16_t y, col565_t colour) {

    frame_buffer[x + LCD_WIDTH * y] = colour;   //  Save in the frame_buffer
}


//    _____________
// __/ Filled rect \_______________________________________________________________________________
void  draw_scr_rect_filled(int16_t x0, int16_t y0, int16_t x1, int16_t y1, col565_t colour) {

    col565_t  pixels[LCD_WIDTH];
    int16_t   width;

    if (x1 < x0)  swap_m(x0, x1, int16_t);
    if (y1 < y0)  swap_m(y0, y1, int16_t);

    if (x0 < 0)            x0 = 0;
    if (x1 >= LCD_WIDTH)   x1 = LCD_WIDTH - 1;
    if (y0 < 0)            y0 = 0;
    if (y1 >= LCD_HEIGHT)  y1 = LCD_HEIGHT - 1;

    width = x1 - x0 + 1;
    for (int16_t i = 0; i < width; ++i) {
        pixels[i] = colour;
    }

    for (int16_t row = y0; row <= y1; ++row) {
        lcd_blit_rect(pixels, x0, row, width, 1);   //  Draw on screen only (used for cursor)
    }
}


//    ______________
// __/ Clear screen \______________________________________________________________________________
void  draw_clear_screen(void) {

    memset(frame_buffer, 0, sizeof(col565_t) * LCD_WIDTH * LCD_HEIGHT);
    draw_frame_buffer();
}


//    ______________________
// __/ Display frame buffer \______________________________________________________________________
void  draw_frame_buffer(void) {

    lcd_blit_rect(frame_buffer, 0, 0, LCD_WIDTH, LCD_HEIGHT);
}


//    ____________________
// __/ Display row buffer \________________________________________________________________________
void  draw_row_buffer(uint16_t row) {

    while (!mutex_try_enter(&blit_mutex, NULL)) {       //  Only one core at a time should blit to the screen
        tight_loop_contents();
    }

        lcd_blit_rect(&frame_buffer[row * LCD_WIDTH], 0, row, LCD_WIDTH, 1);

    mutex_exit(&blit_mutex);
}


//    ____________
// __/ Initialize \________________________________________________________________________________
void  draw_initialize(void) {

    frame_buffer = malloc(sizeof(col565_t) * LCD_WIDTH * LCD_HEIGHT);
    draw_clear_screen();

    mutex_init(&blit_mutex);
}


//    _________
// __/ Cleanup \___________________________________________________________________________________
void  draw_cleanup(void) {

    free(frame_buffer);
    frame_buffer = NULL;
}

