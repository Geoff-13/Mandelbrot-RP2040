/*
    Mandelbrot for the PicoCalc RP2040

    Functions that colour and draw the image to the screen

    By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
    Date : Dec 2025
    Ver  : 07
*/


#include  "project.h"


//    __________________
// __/ Global variables \__________________________________________________________________________
extern sMand_data  mandelbrot;                          //  From commands.c
extern sCommon     common;                              //  From commands.c


//    _________________
// __/ Local variables \___________________________________________________________________________




// ************************************************************************************************
// **********     LOCAL FUNCTIONS     *************************************************************
// ************************************************************************************************



//       _________________
// _____/ Draw the cursor \________________________________________________________________________
static void  scr_draw_cursor(void) {

    //  Cursor is drawn directly to the LCD so the frame buffer is untouched

    int  x = common.cursor_x;
    int  y = common.cursor_y;

    draw_scr_rect_filled(x - 3, y - 1, x + 3, y + 1, COLOUR_BLACK);     //  Black background
    draw_scr_rect_filled(x - 1, y - 3, x + 1, y + 3, COLOUR_BLACK);

    draw_scr_rect_filled(x - 2, y,     x + 2, y,     COLOUR_WHITE);     //  White cursor
    draw_scr_rect_filled(x,     y - 2, x,     y + 2, COLOUR_WHITE);
}



// ************************************************************************************************
// **********     PUBLIC FUNCTIONS     ************************************************************
// ************************************************************************************************



//       _____________________
// _____/ Select pixel colour \____________________________________________________________________
col565_t  scr_get_color(int iter_count) {

    float     num, frac;
    float     ratio;
    int       zone;

    uint16_t  r = 0;         //  Only need to calculate non-zero colour components
    uint16_t  g = 0;         //  Will also provide BLACK when iter_count >= iteration_limit
    uint16_t  b = 0;

    if (iter_count < common.iteration_limit) {
        frac  = modff(common.repeats * iter_count * common.mapping_factor, &num);
        ratio = modff(6.0f * frac, &num);               //  Map to the 6 colour zones below
        zone  = (int)num;

        switch (zone) {
            case 0:                                     //  Sawtooth colour scheme from my desktop program
                g = (uint16_t)(ratio * 255.0f);
                b = 255;
                break;
            case 1:
                g = 255;
                b = (uint16_t)((1.0f - ratio) * 255.0f);
                break;
            case 2:
                r = (uint16_t)(ratio * 255.0f);
                g = 255;
                break;
            case 3:
                r = 255;
                g = (uint16_t)((1.0f - ratio) * 255.0f);
                break;
            case 4:
                r = 255;
                b = (uint16_t)(ratio * 255.0f);
                break;
            case 5:
                r = (uint16_t)((1.0f - ratio) * 255.0f);
                b = 255;
                break;
        }
    }
    return RGB_m(r, g, b);
}


//       ___________________
// _____/ Initialize screen \______________________________________________________________________
void  scr_draw_the_screen(void) {

    draw_frame_buffer();                                            //  Only 80 ms, so do a full frame redraw even when not needed

    if (common.show_text) {                                         //  Display the text info
        printf("\033[f");                                               //  Reset cursor to top left corner
        printf("Time  : %.3f s\n", common.calculation_time / 1000000.0f);
        printf("Iters : %d\n", common.calculation_count[0] + common.calculation_count[1]);
        printf("Zoom  : %d\n", common.zoom);
        printf("cr    : %10.7f\n", fix4_28_to_float_m(mandelbrot.cr_centre));
        printf("ci    : %10.7f\n", fix4_28_to_float_m(mandelbrot.ci_centre));
        printf("width : %10.7f\n", fix4_28_to_float_m(mandelbrot.width));
    }

    if (common.show_cursor) {                                       //  Display the zoom cursor
        scr_draw_cursor();
    }

    common.needs_redraw = false;
}


//       ___________________
// _____/ Initialize screen \______________________________________________________________________
void  scr_initialize(void) {

    draw_initialize();
    lcd_enable_cursor(false);
}

//       ________________
// _____/ Destroy screen \_________________________________________________________________________
void  scr_destroy(void) {

    draw_cleanup();
}

