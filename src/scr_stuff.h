#ifndef SCRSTUFF_H
#define SCRSTUFF_H
#include <debug.h>


void scr_genericgauge(float progress);
void scr_genericgaugepercent(int percent);
void scr_genericgaugepercentcalc(int curr, int max);

void scr_centerputs(const char* buf, char fillerbyte);
void scr_fillhalf(int size, char filler);

char* scrcol(int COLOR); //little wrapper to scr_setfontcolor to apply a color as a `%c` format specifier on scr_printf

#define BGR_WHITE   0x808080
#define BGR_WHITES  0xFFFFFF
#define BGR_RED     0x000080
#define BGR_REDS    0x0000FF
#define BGR_GREEN   0x008000
#define BGR_GREENS  0x00FF00
#define BGR_YELLOW  0x008080
#define BGR_YELLOWS 0x00FFFF
#define BGR_BLUE    0x800000
#define BGR_BLUES   0xFF0000

#endif
