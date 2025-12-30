#include "scr_stuff.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void hexdump(const void* data, uint32_t size, int hdr) {
    char ascii[17];
    uint32_t i, j;
    ascii[16] = '\0';
    if (hdr) {
        for (i = 0; i < 16; i++) {
            if (i == 8)
                printf(" ");
            printf("%02lX ", i);
        }
        printf("\n");
        for (i = 0; i < 23; i++)
            printf("---");
        printf("\n");
    }

    for (i = 0; i < size; ++i) {
        printf("%02X ", ((unsigned char*) data)[i]);
        if (((unsigned char*) data)[i] >= ' ' && ((unsigned char*) data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*) data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i + 1) % 8 == 0 || i + 1 == size) {
            printf(" ");
            if ((i + 1) % 16 == 0) {
                printf("|  %s \n", ascii);
            } else if (i + 1 == size) {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8) {
                    printf(" ");
                }
                for (j = (i + 1) % 16; j < 16; ++j) {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}

void scr_genericgaugepercentcalc(int curr, int max) {
    scr_genericgaugepercent((curr*100/(float)max));
}

void scr_genericgaugepercent(int percent) {
    scr_genericgauge(percent*0.01);
}

void scr_genericgauge(float progress) {
    int barWidth = 70;

    scr_printf("[");
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i)
    {
      if (i < pos)
        scr_printf("=");
      else if (i == pos)
        scr_printf(">");
      else
        scr_printf(" ");
    }

    scr_printf("]\r");
}


void scr_fillhalf(int size, char filler) {
    for (int x=0; x<(80-size)/2;x++) scr_printf("%c", filler);
}

void scr_centerputs(const char* buf, char fillerbyte) {
    scr_fillhalf(strlen(buf), fillerbyte);
    scr_printf("%s", buf);
    scr_fillhalf(strlen(buf), fillerbyte);
    if(strlen(buf) % 2 != 0) scr_printf("\n");
}

char* scrcol(int COLOR) {
    scr_setfontcolor(COLOR);
    return "";
}
