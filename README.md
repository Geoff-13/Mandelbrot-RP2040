# Mandelbrot for the PicoCalc RP2040

Welcome to my Mandelbrot program for the PicoCalc RP2040.  It’s fairly simple (only 1 colour map, no smoothing, limited zoom, etc) but still let’s me do some basic exploring of the Mandelbrot on my PicoCalc.  For serious exploring I would use the desktop program I wrote for my main PC.

## Instructions

There are only a few keys that the program uses :

| Key     | Action                                           |
| ------- | ------------------------------------------------ |
| Enter	  | Zoom in at the screen centre or the zoom cursor  |
| Bkspc	  | Zoom out at the screen centre or the zoom cursor |
| Arrows  | Move the zoom cursor 1 pixel                     |
| W A S D | Move the zoom cursor 10 pixels                   |
| I J K L | Shift the screen by 1/4 (80 pixels)              |
| 1 to 9  | Repeat colour map x times (default 3)            |
| T       | Toggle the text information on and off           |
| R	      | Reset to full Mandelbrot image                   |

The program does not check for a keypress while rendering.  However, Blair's keyboard driver stores key presses in a buffer, so if you press a key while rendering it will execute immeadiately upon completing the current image.

## Comments

I wrote this for the RP2040, since that’s what I have.  The key starting point was Blair Leduc's picocalc-text-starter code.  Without this foundation this program wouldn't exist.

Initially I used the pico_float module, but it was slow since this MCU has only 32-bit integer hardware.  Thus float must be implemented in software.

To speed up the iterations I wrote custom fixed point ( 4.28 format using int32_t ) assembly to use the integer hardware.  This runs in less than 1/3 the time of pico_float.  Zoom is limited to 2^21 or 2,097,152 since pixel_step = 0 beyond that.  My assembly skills are minimal so I won't be surprised if this code can be improved by a knowledgable programmer.

The program uses multicore to further speed up the iterations.  Each core does half the blocks.  The clock is set to 200 MHz as this is now certified by Raspberry.

The final program creates the full Mandelbrot image in 0.72 seconds.  Very slow compared to my desktop, but not too bad for a simple microcontroller.

The RP2350 has hardware floating point, so it may be faster but then I wouldn’t have had the fun of programming in assembly.  For obvious reasons I haven’t tried this.

## Warnings

I will not be maintaining or updating this repo.  Hopefully since it compiles and runs for me it will work for all Picocalc RP2040's.  I've included a UF2 for those who just want to see what it does.

I learned programming as a retirement hobby.  I didn’t like some of the “standard” C conventions, and so I did my own thing.  As long as my code was only for my own eyes this was a non-issue.

However I only succeeded with this program by learning and borrowing from the PicoCalc community.  As a thankyou I’m posting this code back to the community, but you’ll have to live with my non-standard coding style.  ;-)
