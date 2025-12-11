/*
    PicoCalc program template

    Based on Blair Leduc github ( https://github.com/BlairLeduc/picocalc-text-starter/tree/main )

    By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
    Date : Oct 2025
*/


//    ______________
// __/ Handy macros \______________________________________________________________________________
#define RGB_m(r, g, b)      ((uint16_t)(((r) >> 3) << 11 | ((g) >> 2) << 5 | ((b) >> 3)))
#define MSB_m(x)            ((x) >> 8)      //  Upper byte of a 16-bit value
#define LSB_m(x)            ((x) & 0xFF)    //  Lower byte of a 16-bit value

//  Swap two variables of any type
#define  swap_m(v1, v2, type) {  \
    register type  temp = v2;    \
    v2 = v1;                     \
    v1 = temp;                   \
}


//    _____________________________________
// __/ Standard Colours in col565_t format \_______________________________________________________
#define  COLOUR_WHITE       (0xFFFF)
#define  COLOUR_BLACK       (0x0000)
#define  COLOUR_BLUE        (0x001F)
#define  COLOUR_BLUE_DARK   (0x0010)
#define  COLOUR_BLUE_LIGHT  (0x841F)
#define  COLOUR_CYAN        (0x07FF)
#define  COLOUR_GREEN       (0x07E0)
#define  COLOUR_GREEN_DARK  (0X0400)
#define  COLOUR_GREEN_LIGHT (0X87F0)
#define  COLOUR_GREY        (0x8410)
#define  COLOUR_GREY_DARK   (0X6B4D)
#define  COLOUR_GREY_LIGHT  (0XD69A)
#define  COLOUR_RED         (0xF800)
#define  COLOUR_RED_DARK    (0X8000)
#define  COLOUR_RED_LIGHT   (0XFC10)
#define  COLOUR_PURPLE      (0xF81F)
#define  COLOUR_YELLOW      (0xFFE0)


//    ________
// __/ font.h \____________________________________________________________________________________
#define  GLYPH_HEIGHT       (10)        //  Height of each glyph in pixels


//    ____________
// __/ keyboard.h \________________________________________________________________________________
#define  KEY_MOD_ALT        (0xA1)
#define  KEY_MOD_SHL        (0xA2)
#define  KEY_MOD_SHR        (0xA3)
#define  KEY_MOD_SYM        (0xA4)
#define  KEY_MOD_CTRL       (0xA5)

#define  KEY_STATE_IDLE     (0)
#define  KEY_STATE_PRESSED  (1)
#define  KEY_STATE_HOLD     (2)
#define  KEY_STATE_RELEASED (3)

#define  KEY_BACKSPACE      (0x08)
#define  KEY_TAB            (0x09)
#define  KEY_ENTER          (0x0A)
#define  KEY_RETURN         (0x0D)
#define  KEY_SPACE          (0x20)

#define  KEY_ESC            (0xB1)
#define  KEY_UP             (0xB5)
#define  KEY_DOWN           (0xB6)
#define  KEY_LEFT           (0xB4)
#define  KEY_RIGHT          (0xB7)

#define  KEY_BREAK          (0xD0)
#define  KEY_INSERT         (0xD1)
#define  KEY_HOME           (0xD2)
#define  KEY_DEL            (0xD4)
#define  KEY_END            (0xD5)
#define  KEY_PAGE_UP        (0xD6)
#define  KEY_PAGE_DOWN      (0xD7)

#define  KEY_CAPS_LOCK      (0xC1)

#define  KEY_F1             (0x81)
#define  KEY_F2             (0x82)
#define  KEY_F3             (0x83)
#define  KEY_F4             (0x84)
#define  KEY_F5             (0x85)
#define  KEY_F6             (0x86)
#define  KEY_F7             (0x87)
#define  KEY_F8             (0x88)
#define  KEY_F9             (0x89)
#define  KEY_F10            (0x90)

#define  KEY_POWER          (0x91)

#define  KBD_BUFFER_SIZE    (32)
#define  KEYBOARD_POLL_MS   (100)       //  Poll keyboard every 100 ms


//    _______
// __/ lcd.h \_____________________________________________________________________________________
#define LCD_SPI         (spi1)          // SPI interface for the LCD display

// Raspberry Pi Pico board GPIO pins
#define LCD_SCL         (10)            // serial clock (SCL)
#define LCD_SDI         (11)            // serial data in (SDI)
#define LCD_SDO         (12)            // serial data out (SDO)
#define LCD_CSX         (13)            // chip select (CSX)
#define LCD_DCX         (14)            // data/command (D/CX)
#define LCD_RST         (15)            // reset (RESET)


// LCD interface definitions
// According to the ST7789P datasheet, the maximum SPI clock speed is 62.5 MHz.
// However, the controller can handle 75 MHz in practice.
#define LCD_BAUDRATE    (75000000)      // 75 MHz SPI clock speed
#define LCD_I2C_TIMEOUT_US (1000)       // I2C timeout in microseconds

// LCD command definitions
#define LCD_CMD_NOP     (0x00)          // no operation
#define LCD_CMD_SWRESET (0x01)          // software reset
#define LCD_CMD_SLPIN   (0x10)          // sleep in
#define LCD_CMD_SLPOUT  (0x11)          // sleep out
#define LCD_CMD_INVOFF  (0x20)          // display inversion off
#define LCD_CMD_INVON   (0x21)          // display inversion on
#define LCD_CMD_DISPOFF (0x28)          // display off
#define LCD_CMD_DISPON  (0x29)          // display on
#define LCD_CMD_CASET   (0x2A)          // column address set
#define LCD_CMD_RASET   (0x2B)          // row address set
#define LCD_CMD_RAMWR   (0x2C)          // memory write
#define LCD_CMD_RAMRD   (0x2E)          // memory read
#define LCD_CMD_VSCRDEF (0x33)          // vertical scroll definition
#define LCD_CMD_MADCTL  (0x36)          // memory access control
#define LCD_CMD_VSCSAD  (0x37)          // vertical scroll start address of RAM
#define LCD_CMD_COLMOD  (0x3A)          // pixel format set
#define LCD_CMD_IFMODE  (0xB0)          // interface mode control
#define LCD_CMD_FRMCTR1 (0xB1)          // frame rate control (in normal mode)
#define LCD_CMD_FRMCTR2 (0xB2)          // frame rate control (in idle mode)
#define LCD_CMD_FRMCTR3 (0xB3)          // frame rate control (in partial mode)
#define LCD_CMD_DIC     (0xB4)          // display inversion control
#define LCD_CMD_DFC     (0xB6)          // display function control
#define LCD_CMD_EM      (0xB7)          // entry mode set
#define LCD_CMD_MODESEL (0xB9)          // mode set
#define LCD_CMD_PWR1    (0xC0)          // power control 1
#define LCD_CMD_PWR2    (0xC1)          // power control 2
#define LCD_CMD_PWR3    (0xC2)          // power control 3
#define LCD_CMD_VCMPCTL (0xC5)          // VCOM control
#define LCD_CMD_PGC     (0xE0)          // positive gamma control
#define LCD_CMD_NGC     (0xE1)          // negative gamma control
#define LCD_CMD_DGC1    (0xE2)          // driver gamma control 1
#define LCD_CMD_DGC2    (0xE3)          // driver gamma control
#define LCD_CMD_DOCA    (0xE8)          // driver output control
#define LCD_CMD_E9      (0xE9)          // Manufacturer command
#define LCD_CMD_F0      (0xF0)          // Manufacturer command
#define LCD_CMD_F7      (0xF7)          // Manufacturer command

// LCD display parameters
#define LCD_WIDTH       (320)           // pixels across the LCD
#define LCD_HEIGHT      (320)           // pixels down the LCD
#define FRAME_HEIGHT    (480)           // frame memory height in pixels
#define ROWS            (LCD_HEIGHT/GLYPH_HEIGHT)   // number of lines that fit on the LCD
#define MAX_ROW         (ROWS - 1)      // maximum row index (0-based)



//    ____________
// __/ picocalc.h \________________________________________________________________________________


//    _______________
// __/ southbridge.h \_____________________________________________________________________________
#define  SB_I2C         (i2c1)          //  I2C interface for the south bridge

// Raspberry Pi Pico board GPIO pins
#define  SB_SDA         (6)
#define  SB_SCL         (7)

// Keyboard interface definitions
#define  SB_BAUDRATE       (10000)
#define  SB_ADDR           (0x1F)
#define  SB_I2C_TIMEOUT_US (10000)

// Keyboard register definitions
#define  SB_REG_KEY     (0x04)          //  Key status
#define  SB_REG_BKL     (0x05)          //  Backlight
#define  SB_REG_RST     (0x08)          //  Reset
#define  SB_REG_FIF     (0x09)          //  Fifo
#define  SB_REG_BK2     (0x0A)          //  Keyboard backlight
#define  SB_REG_BAT     (0x0B)          //  Battery
#define  SB_REG_OFF     (0x0E)          //  Power off

#define  SB_WRITE       (0x80)          //  Write to register


//    ____________
// __/ terminal.h \________________________________________________________________________________
//  Processing ANSI escape sequences is a small state machine. These are the states.
#define  STATE_NORMAL    (0)            //  Normal state
#define  STATE_ESCAPE    (1)            //  Escape character received
#define  STATE_CS        (2)            //  Control sequence introducer (CSI) received
#define  STATE_DEC       (3)            //  DEC private mode sequence (?)
#define  STATE_G0_SET    (4)            //  Setting G0 character set ESC(
#define  STATE_G1_SET    (5)            //  Setting G1 character set ESC)
#define  STATE_OSC       (6)            //  Operating System Command (OSC)
#define  STATE_OSC_ESC   (7)            //  Operating System Command (OSC) ESC
#define  STATE_TMC       (8)            //  Terminal Management Control (TMC)

//  Control characters
#define  CHR_BEL         (0x07)         //  Bell
#define  CHR_BS          (0x08)         //  Backspace
#define  CHR_HT          (0x09)         //  Horizontal Tab
#define  CHR_LF          (0x0A)         //  Line Feed
#define  CHR_VT          (0x0B)         //  Vertical Tab
#define  CHR_FF          (0x0C)         //  Form Feed
#define  CHR_CR          (0x0D)         //  Carriage Return
#define  CHR_SO          (0x0E)         //  Shift Out (select G1 character set)
#define  CHR_SI          (0x0F)         //  Shift In (select G0 character set)
#define  CHR_CAN         (0x18)         //  Cancel
#define  CHR_SUB         (0x1A)         //  Substitute
#define  CHR_ESC         (0x1B)         //  Escape

//  Character set definitions
#define  CHARSET_UK      (0)            //  UK character set
#define  CHARSET_ASCII   (1)            //  ASCII character set
#define  CHARSET_DEC     (2)            //  DEC Special Character Set (line drawing)

#define  G0_CHARSET      (0)            //  G0 character set
#define  G1_CHARSET      (1)            //  G0 and G1 character sets

//  Defaults
#define  WHITE_PHOSPHOR  RGB_m(216, 240, 255)   //  White phosphor
#define  GREEN_PHOSPHOR  RGB_m(51, 255, 102)    //  Green phosphor
#define  AMBER_PHOSPHOR  RGB_m(255, 255, 51)    //  Amber phosphor
#define  FOREGROUND      RGB_m(255, 255, 255)   //  Default foreground colour
#define  BACKGROUND      RGB_m(0, 0, 0)         //  Default background colour
#define  BRIGHT          RGB_m(255, 255, 255)   //  White
#define  DIM             RGB_m(128, 128, 128)   //  Dim grey

