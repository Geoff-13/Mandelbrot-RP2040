/*
    Mandelbrot for the PicoCalc RP2040

    Functions used by main

    By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
    Date : Dec 2025
    Ver  : 07
*/


#include  "project.h"


//    __________________
// __/ Global variables \__________________________________________________________________________
sMand_data  mandelbrot;
sCommon     common;

extern fix4_28_t  mul_asm_safe(fix4_28_t a, fix4_28_t b);   //  From multiply_safe.s


//    _________________
// __/ Local variables \___________________________________________________________________________
static const fix4_28_t  two     = 1 << 29;                  //  Constants in fix4_28_t format
static const fix4_28_t  one     = 1 << 28;
static const fix4_28_t  half    = 1 << 27;
static const fix4_28_t  quarter = 1 << 26;



// ************************************************************************************************
// **********     LOCAL FUNCTIONS     *************************************************************
// ************************************************************************************************



//       ______________
// _____/ Reset cursor \___________________________________________________________________________
static void  comm_reset_cursor(void) {

    common.show_cursor = false;
    common.cursor_x    = LCD_WIDTH / 2;
    common.cursor_y    = LCD_HEIGHT / 2;
}



// ************************************************************************************************
// **********     PUBLIC FUNCTIONS     ************************************************************
// ************************************************************************************************


//       _____________________
// _____/ Zoom view at cursor \____________________________________________________________________
void  comm_zoom_view(char key) {

    int  x_offset, y_offset;

    x_offset = common.cursor_x - LCD_WIDTH / 2;
    y_offset = common.cursor_y - LCD_HEIGHT / 2;

    if (x_offset > 0) {
        for (int i = 0; i < x_offset; ++i) {
            mandelbrot.cr_centre += mandelbrot.pixel_step;
        }
    }
    if (x_offset < 0) {
        for (int i = 0; i > x_offset; --i) {
            mandelbrot.cr_centre -= mandelbrot.pixel_step;
        }
    }

    if (y_offset > 0) {
        for (int i = 0; i < y_offset; ++i) {
            mandelbrot.ci_centre -= mandelbrot.pixel_step;
        }
    }
    if (y_offset < 0) {
        for (int i = 0; i > y_offset; --i) {
            mandelbrot.ci_centre += mandelbrot.pixel_step;
        }
    }

    switch (key) {
        case KEY_RETURN:                            //  Zoom in
            if (common.zoom < 2000000) {
                mandelbrot.width       >>= 1;           //  Divide width by 2
                mandelbrot.pixel_step    = mul_asm_safe(mandelbrot.width, PIXEL_FACTOR);
                common.zoom            <<= 1;           //  Multiply zoom by 2
                common.iteration_limit   = (int)(common.iteration_limit * 1.25f);
                common.needs_recalc      = true;        //  Recalculate set, which will then force a redraw
            }
            break;
        case KEY_BACKSPACE:                         //  Zoom out
            if (common.zoom > 2) {
                mandelbrot.width       <<= 1;           //  Multiply width by 2
                mandelbrot.pixel_step    = mul_asm_safe(mandelbrot.width, PIXEL_FACTOR);
                common.zoom            >>= 1;           //  Divide zoom by 2
                common.iteration_limit   = (int)(common.iteration_limit / 1.25f);
                common.needs_recalc      = true;        //  Recalculate set, which will then force a redraw
            } else {
                comm_reset_to_full_image();
            }
            break;
    }

    comm_reset_cursor();
}


//       _____________
// _____/ Move cursor \____________________________________________________________________________
void  comm_move_cursor(char key) {

    switch (key) {
        case KEY_UP:                                //  Up 1 pixel
            if (common.cursor_y > 3) {
                --common.cursor_y;
            }
            break;
        case KEY_DOWN:                              //  Down 1 pixel
            if (common.cursor_y < LCD_HEIGHT - 3) {
                ++common.cursor_y;
            }
            break;
        case KEY_LEFT:                              //  Left 1 pixel
            if (common.cursor_x > 3) {
                --common.cursor_x;
            }
            break;
        case KEY_RIGHT:                             //  Right 1 pixel
            if (common.cursor_x < LCD_WIDTH - 3) {
                ++common.cursor_x;
            }
            break;
        case 'W':                                   //  Up 10 pixels
            if (common.cursor_y > 13) {
                common.cursor_y -= 10;
            }
            break;
        case 'S':                                   //  Down 10 pixels
            if (common.cursor_y < LCD_HEIGHT - 13) {
                common.cursor_y += 10;
            }
            break;
        case 'A':                                   //  Left 10 pixels
            if (common.cursor_x > 13) {
                common.cursor_x -= 10;
            }
            break;
        case 'D':                                   //  Right 10 pixels
            if (common.cursor_x < LCD_WIDTH -  13) {
                common.cursor_x += 10;
            }
            break;
    }

    common.show_cursor  = true;
    common.needs_redraw = true;
}


//       ___________
// _____/ Move view \______________________________________________________________________________
void  comm_move_view(char key) {

    switch (key) {
        case 'I':                                   //  Move quarter screen up
            mandelbrot.ci_centre += mandelbrot.width >> 2;
            if (mandelbrot.ci_centre + (mandelbrot.width >> 1) > (one + quarter)) {
                mandelbrot.ci_centre = (one + quarter) - (mandelbrot.width >> 1);
            }
            break;
        case 'K':                                   //  Move quarter screen down
            mandelbrot.ci_centre -= mandelbrot.width >> 2;
            if (mandelbrot.ci_centre - (mandelbrot.width >> 1) < (0 - one - quarter)) {
                mandelbrot.ci_centre = (0 - one - quarter) + (mandelbrot.width >> 1);
            }
            break;
        case 'L':                                   //  Move quarter screen right
            mandelbrot.cr_centre += mandelbrot.width >> 2;
            if (mandelbrot.cr_centre + (mandelbrot.width >> 1) > half) {
                mandelbrot.cr_centre = half - (mandelbrot.width >> 1);
            }
            break;
        case 'J':                                   //  Move quarter screen left
            mandelbrot.cr_centre -= mandelbrot.width >> 2;
            if (mandelbrot.cr_centre - (mandelbrot.width >> 1) < (0 - two)) {
                mandelbrot.cr_centre = (0 - two) + (mandelbrot.width >> 1);
            }
            break;
    }
    common.needs_recalc = true;
}


//       __________________________
// _____/ Initialize common values \_______________________________________________________________
void  comm_initialize(void) {

    common.show_text      = false;
    common.needs_recalc   = false;
    common.needs_redraw   = false;
    common.mapping_factor = 1.0f / 256.0f;          //  The colour map will repeat every xx interation counts
                                                    //  common.repeats can further increase this on-the-fly

    multicore_launch_core1(calc_core_1_main);
}


//       ________________________________
// _____/ Reset parameters to full image \_________________________________________________________
void  comm_reset_to_full_image(void) {

    mandelbrot.cr_centre   = 0 - half - quarter;     //  Limits in fix4_28
    mandelbrot.ci_centre   = 0;
    mandelbrot.width       = two + half;
    mandelbrot.pixel_step  = mul_asm_safe(mandelbrot.width, PIXEL_FACTOR);     //  This multiply does : width / (LCD_WIDTH - 1)

    common.iteration_limit = 200;
    common.repeats         =   3;
    common.zoom            =   1;

    comm_reset_cursor();

    common.needs_recalc = true;                     //  Recalculate set, which will then force a redraw
}

