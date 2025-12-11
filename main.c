/*
    Mandelbrot for the PicoCalc RP2040

    Main routine

    By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
    Date : Dec 2025
    Ver  : 07
*/

/*
    The iteration calc uses 32-bit fixed point math.  The format is 4.28.  Custom add, subtract, multiply and
    square algorithms were coded in assembly.  This creates the full image in 0.7s.  The numerical accuracy
    will be the full 32-bits.  Extra checking is done to watch for overflow outside the -8.0 to +7.99 range
    available with the 4.28 format.  Zoom is limited to 2^21 or 2,097,152 since pixel_step = 0 beyond that.

    Pixels inside the cardiod and period 2 bulb are not iterated.  Small overhead when zoomed in, but major
    savings on the full image.

    The iteration calcs run on both cores to cut the time in half.  Core 1 sleeps the rest of the time.
*/

#include  "project.h"


//    __________________
// __/ Global variables \__________________________________________________________________________
bool            is_running;

extern sCommon  common;                                 //  From commands.c


//    _________________
// __/ Local variables \___________________________________________________________________________


//    ________________________
// __/ Process keyboard input \____________________________________________________________________
void  process_input(void) {

    char  key;

    if (keyboard_key_available()) {
        key = keyboard_get_key();
        key = toupper(key);
        switch (key) {
            case 'I':                                   //  Scroll by 1/4 screen
            case 'J':
            case 'K':
            case 'L':
                comm_move_view(key);
                break;
            case KEY_UP:                                //  Move zoom cursor 1 pixel
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
            case 'W':                                   //  Move zoom cursor 10 pixels
            case 'A':
            case 'S':
            case 'D':
                comm_move_cursor(key);
                break;
            case KEY_RETURN:                            //  Zoom in at cursor (or screen centre)
            case KEY_BACKSPACE:                         //  Zoom out at cursor (or screen centre)
                comm_zoom_view(key);
                break;
            case 'R':                                   //  Reset to full Mandelbrot image
                comm_reset_to_full_image();
                break;
            case 'T':                                   //  Toggle text display
                common.show_text    = !common.show_text;
                common.needs_redraw = true;
                break;
            case '1':                                   //  Set number of times the colour map repeats
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                common.repeats      = key - 48;
                common.needs_recalc = true;
                break;
            default:
                break;
        }
    }
}


//    ______
// __/ Main \______________________________________________________________________________________
int  main(void) {

    stdio_init_all();
    picocalc_init();                                    //  Initialize the needed PicoCalc drivers (lcd, keyboard, etc)

    is_running = true;                                  //  Must be set before launching core 1

    comm_initialize();                                  //  Init data and launch core 1
    comm_reset_to_full_image();                         //  Reset image parameters to full Mandelbrot

    scr_initialize();                                   //  Init the screen (the frame_buffer)

    while (is_running) {
        process_input();
        if (common.needs_recalc) {
            draw_clear_screen();                        //  Clear the screen so it's easy to watch it appear
            calc_iterate_the_set();                     //  This also draws the image row-by-row
        }
        if (common.needs_redraw) {
            scr_draw_the_screen();                      //  Refreshes the full screen, including text and cursor if enabled
        }
        sleep_ms(100);                                  //  Checking for keypress every 0.1s is often enough
    }

    scr_destroy();                                      //  Destroy the frame_buffer (currently never gets here)

}
