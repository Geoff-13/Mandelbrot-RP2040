/*
    Mandelbrot for the PicoCalc RP2040

    Functions that iterate the Mandelbrot set

    By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
    Date : Dec 2025
    Ver  : 07
*/


#include  "project.h"


//    __________________
// __/ Global variables \__________________________________________________________________________
extern sMand_data  mandelbrot;                          //  From commands.c
extern sCommon     common;                              //  From commands.c
extern bool        is_running;                          //  From main.c

extern int         iter_asm(fix4_28_t cr, fix4_28_t ci, int iter_limit);   //  From iterate.s


//    _________________
// __/ Local variables \___________________________________________________________________________
volatile bool      work_available = false;              //  Core 1 monitors this to know when to start



// ************************************************************************************************
// **********     LOCAL FUNCTIONS     *************************************************************
// ************************************************************************************************



//       ___________________
// _____/ Using fixed point \______________________________________________________________________
static void  calc_iterate(int core_num) {

    fix4_28_t  cr_min, ci_max;
    fix4_28_t  cr, ci, ci_block;
    int        iter_count, start_row;
    col565_t   colour;

    common.calculation_count[core_num] = 0;

    cr_min = mandelbrot.cr_centre - (mandelbrot.width >> 1);            //  Centre minus half width
    ci_max = mandelbrot.ci_centre + (mandelbrot.width >> 1);

    if (core_num == 0) {
        ci_block  = ci_max;
        start_row = 0;
    } else {
        ci_block  = ci_max - (mandelbrot.pixel_step << 4);
        start_row = 16;                                                 //  (DO NOT CHANGE)
    }

    for (int start = start_row; start < start_row + 16; ++start) {      //  320 / 20 = 16  (DO NOT CHANGE)
        ci = ci_block;
        for (int row = start; row < LCD_HEIGHT; row += 32) {            //  16 from calc above * 2 for dual-core  AND  to match <<5 below  (DO NOT CHANGE)
            cr = cr_min;
            for (int column = 0; column < LCD_WIDTH; ++column) {
                iter_count = iter_asm(cr, ci, common.iteration_limit);  //  Inputs will be in R0, R1 and R2
                if (iter_count <= common.iteration_limit) {             //  Pixels inside cardiod and period 2 will report iter_limit+1
                    common.calculation_count[core_num] += iter_count;   //  Only pixels that were iterated will be added to total
                }
                colour = scr_get_color(iter_count);
                draw_pixel(column, row, colour);
                cr += mandelbrot.pixel_step;
            }
            draw_row_buffer(row);
            ci -= mandelbrot.pixel_step << 5;     //  pixel_step * 16 * 2 (for dual-core) to jump to next block  (DO NOT CHANGE)
        }
        ci_block -= mandelbrot.pixel_step;
    }
}



// ************************************************************************************************
// **********     PUBLIC FUNCTIONS     ************************************************************
// ************************************************************************************************


//       ______________________________
// _____/ Calculate the Mandelbrot set \___________________________________________________________
void  calc_iterate_the_set(void) {

    uint32_t  start;

    start = time_us_32();                               //  Start the timer

        work_available = true;                          //  Tell core 1 there's work to be done
        calc_iterate(0);                                //  Do core 0 iterations
        while (work_available) {                        //  Wait for core 1 to finish
            tight_loop_contents();
        }

    common.calculation_time = time_us_32() - start;     //  Record the time taken to calculate

    common.needs_recalc = false;
    common.needs_redraw = true;
    common.show_cursor  = false;
}


//       _______________
// _____/ Core 1 main() \__________________________________________________________________________
void  calc_core_1_main(void) {

    while (is_running) {
        if (work_available) {                           //  Check if work is available
            calc_iterate(1);                            //  Do core 1 iterations
            work_available = false;                     //  Let core 0 know core 1 is done
        }
        sleep_ms(10);                                   //  Sleep for only 0.01s so core 1 helps quickly
    }
}


