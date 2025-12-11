/*
    PicoCalc program template

    Based on Blair Leduc github ( https://github.com/BlairLeduc/picocalc-text-starter/tree/main )

    By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
    Date : Oct 2025
*/



#include  "drivers.h"


// Callback for when characters become available
static void (*chars_available_callback)(void *) = NULL;
static void *chars_available_param = NULL;

static void picocalc_out_chars(const char *buf, int length) {
    for (int i = 0; i < length; ++i) {
        term_emit(buf[i]);
    }
}

static void picocalc_out_flush(void) {
    // No flush needed for this driver
}

static int picocalc_in_chars(char *buf, int length) {
    int n = 0;
    while (n < length) {
        int c = keyboard_get_key();
        if (c == -1)
            break; // No key pressed
        buf[n++] = (char)c;
    }
    return n;
}

static void picocalc_set_chars_available_callback(void (*fn)(void *), void *param) {
    chars_available_callback = fn;
    chars_available_param = param;
}

// Function to be called when characters become available
void picocalc_chars_available_notify(void) {
    if (chars_available_callback) {
        chars_available_callback(chars_available_param);
    }
}

stdio_driver_t picocalc_stdio_driver = {
    .out_chars = picocalc_out_chars,
    .out_flush = picocalc_out_flush,
    .in_chars = picocalc_in_chars,
    .set_chars_available_callback = picocalc_set_chars_available_callback,
    .next = NULL,
};

void picocalc_init() {

    set_sys_clock_khz(200000, false);               //  Increase clock to 200 MHz
    stdio_init_all();                               //  Re-init after clock change

    sb_init();
    terminal_init();
    keyboard_init();
    keyboard_set_key_available_callback(picocalc_chars_available_notify);
    keyboard_set_background_poll(true);

    stdio_set_driver_enabled(&picocalc_stdio_driver, true);
    stdio_set_translate_crlf(&picocalc_stdio_driver, true);
}
