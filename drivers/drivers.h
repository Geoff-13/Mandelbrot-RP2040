/*
    PicoCalc program template

    Based on Blair Leduc github ( https://github.com/BlairLeduc/picocalc-text-starter/tree/main )

    By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
    Date : Oct 2025
*/


#include  "driver-defines.h"

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <stdatomic.h>

#include  "pico/stdlib.h"
#include  "pico/stdio/driver.h"
#include  "pico/multicore.h"
#include  "pico/platform.h"
#include  "pico/sync.h"             //  Custom include for mutex for Mandelbrot
#include  "hardware/gpio.h"
#include  "hardware/i2c.h"
#include  "hardware/spi.h"
#include  "hardware/clocks.h"


typedef  uint16_t  col565_t;

//    ________
// __/ draw.c \____________________________________________________________________________________
//     ***************************************************************
//     *  Customized for Mandelbrot.  DO NOT use for other projects  *
//     *  Uses the frame_buffer primarily, and other changes.        *
//     ***************************************************************

void  draw_initialize       (void);
void  draw_cleanup          (void);

void  draw_clear_screen     (void);
void  draw_frame_buffer     (void);
void  draw_row_buffer       (uint16_t row);

void  draw_pixel            (int16_t x, int16_t y, col565_t colour);
void  draw_scr_rect_filled  (int16_t x0, int16_t y0, int16_t x1, int16_t y1, col565_t colour);


//    ________
// __/ font.h \____________________________________________________________________________________
typedef struct {
    uint8_t width;
    uint8_t glyphs[];
} font_t;

extern const font_t  font_8x10;
extern const font_t  font_5x10;


//    ____________
// __/ keyboard.h \________________________________________________________________________________
typedef void  (*keyboard_key_available_callback_t)  (void);    // Callback function type for when a key becomes available
void          keyboard_set_key_available_callback   (keyboard_key_available_callback_t callback);

void  keyboard_init                 (void);
void  keyboard_set_background_poll  (bool enable);
void  keyboard_poll                 (void);
bool  keyboard_key_available        (void);
char  keyboard_get_key              (void);


//    _______
// __/ lcd.h \_____________________________________________________________________________________
//  Colour and display state functions
void     lcd_set_foreground     (col565_t colour);
void     lcd_set_background     (col565_t colour);
void     lcd_set_reverse        (bool reverse_on);
void     lcd_set_underscore     (bool underscore_on);
void     lcd_set_bold           (bool bold_on);
void     lcd_set_font           (const font_t *new_font);
uint8_t  lcd_get_columns        (void);
uint8_t  lcd_get_glyph_width    (void);

//  Display control functions
void  lcd_reset                 (void);
void  lcd_display_on            (void);
void  lcd_display_off           (void);

//  Low-level SPI functions
void  lcd_write_cmd             (uint8_t cmd);
void  lcd_write_data            (uint8_t len, ...);
void  lcd_write16_data          (uint8_t len, ...);
void  lcd_write16_buf           (const uint16_t *buffer, size_t len);

//  Display window and drawing functions
void  lcd_blit_rect             (const col565_t *pixels, int16_t x, int16_t y, int16_t width, int16_t height);
void  lcd_write_pixel           (const col565_t colour, int16_t x, int16_t y);

//  Scrolling functions
void  lcd_define_scrolling      (int16_t top_fixed_area, int16_t bottom_fixed_area);
void  lcd_scroll_reset          (void);
void  lcd_scroll_clear          (void);
void  lcd_scroll_up             (void);
void  lcd_scroll_down           (void);

//  Character and cursor functions
void  lcd_putc                  (uint8_t column, uint8_t row, uint8_t c);
void  lcd_putstr                (uint8_t column, uint8_t row, const char *str);
void  lcd_move_cursor           (uint8_t x, uint8_t y);
void  lcd_draw_cursor           (void);
void  lcd_erase_cursor          (void);
void  lcd_enable_cursor         (bool cursor_on);
bool  lcd_cursor_enabled        (void);

//  Initialization
void  lcd_clear_screen          (void);
void  lcd_erase_line            (uint8_t row, uint8_t col_start, uint8_t col_end);
void  lcd_init                  (void);


//    ____________
// __/ picocalc.h \________________________________________________________________________________
//extern stdio_driver_t  picocalc_stdio_driver;
void  picocalc_chars_available_notify   (void);
void  picocalc_init                     (void);


//    _______________
// __/ southbridge.h \_____________________________________________________________________________
void      sb_init                       (void);
bool      sb_available                  (void);

uint16_t  sb_read_keyboard              (void);
uint16_t  sb_read_keyboard_state        (void);
uint8_t   sb_read_battery               (void);
uint8_t   sb_read_lcd_backlight         (void);
uint8_t   sb_write_lcd_backlight        (uint8_t brightness);
uint8_t   sb_read_keyboard_backlight    (void);
uint8_t   sb_write_keyboard_backlight   (uint8_t brightness);
bool      sb_is_power_off_supported     (void);
bool      sb_write_power_off_delay      (uint8_t delay_seconds);
bool      sb_reset                      (uint8_t delay_seconds);


//    ____________
// __/ terminal.h \________________________________________________________________________________
typedef void  (*bell_callback_t)    (void);         // Notify then the bell character is received
typedef void  (*report_callback_t)  (const char *); // Notify when a terminal report is requested

void  terminal_init             (void);
void  term_set_bell_callback    (bell_callback_t callback);
void  term_set_report_callback  (report_callback_t callback);
bool  term_emit_available       (void);
void  term_emit                 (char c);

