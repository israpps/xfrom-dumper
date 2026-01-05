
#include "main.h"
#include "../iop/xfromdump/xfrom_stuff.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>
#include <fcntl.h>
#include <loadfile.h>
#include <iopheap.h>
#include <unistd.h>

#define RGB(r, g, b) \
        ((r) | ((g) << 8) | ((b) << 16))
#define BGR(b, g, r) \
        ((b) | ((g) << 8) | ((r) << 16))

char dumpfile[256] = "psx_nand_dump.bin";
int PSXGEN = 0; // PSX Generation, 1 or 2.
uint32_t serialnumber = 0;
int main(int argc, char** argv) {
    SIO_PUTS("reboot IOP");
    reboot_iop("");
    SifInitIopHeap(); // Initialize SIF services for loading modules and files.
    SifLoadFileInit();
    init_scr();
    scr_setCursor(0);

    scr_clear();
    scr_printf("\n\n\n\n\n");

    sbv_patch();
    loadAllModules();
    sleep(2);
    scr_clear();
    scr_printf("\n\n");
    scr_centerputs(" PSX XFROM DUMPER ", '=');
    scr_centerputs("Coded by El_isra", ' ');

    int psxfd = open("rom0:PSXVER", O_RDONLY);
    if (psxfd < 0) {
        //scr_setfontcolor(BGR_REDS);
        //scr_printf("\n\tERROR: This PS2 is not a PSX DESR\n");
        //goto EXIT_IN60S;
    }
    char* romver = getROMVER();
    PSXGEN = (romver[1] - '0'); //PSX gen1 is 0180 and gen2 is 0210. do a substraction and that's it

    getConsoleSerial(&serialnumber);

    scr_printf("\tROMVER  = %s\n", romver);
    scr_printf("\tPSX GEN = %d\n", PSXGEN);
    scr_printf("\tSERIALN = %08lX\n", serialnumber);
    snprintf(dumpfile, sizeof(dumpfile) - 1, "PSX%d_%08X_xfrom.bin", PSXGEN, serialnumber);


    int readytodump = xfrom_dumper_init();
    if (readytodump == 0) {
        int dumpOK = 1;
        s16 pagesize  = 0x0; // Output
        u16 blocksize = 0x0; // Output
        int cardsize  = 0x0; // Output
        u8 cardflags  = 0x0; // Output
        scr_printf("\tGetting XFROM specs...\n");
        int foundnand = nand_getspecs(&pagesize, &blocksize, &cardsize, &cardflags);
        if (foundnand == 0) {
            scr_printf("\t  # pagesize : 0x%X\n", pagesize);
            scr_printf("\t  # blocksize: 0x%X\n", blocksize);
            scr_printf("\t  # cardsize : 0x%X\n", cardsize);
            scr_printf("\t  # cardflags: 0x%X\n", cardflags);

            scr_printf("\t Opening output file...\n");
            int fd = open(dumpfile, O_CREAT|O_WRONLY);
            printf("DUMPFD: %d\n", fd);
            if (fd < 0) {
                scr_printf("\t[FAIL]: Could not open output file '%s' (fd:%d,errno:%d)\n", dumpfile, fd, errno);
            } else {
                    printf("\tBeginning flash dump...\n");
                scr_printf("\tBeginning flash dump...\n");
                uint8_t PAGEBUF[MEMORYCARD_PAGESIZE] = {0};
                for (int x = 0; x < cardsize; x++) {
                    scr_printf(" %04X", x);
                    scr_genericgaugepercentcalc(x, cardsize);
                    int r = dump_nandpage(x, PAGEBUF);
                    if (r != 0) {
                        scr_setfontcolor(BGR_REDS);
                            printf("\n\tFATAL: Failed to dump xfrom page %d: err:0x%X\n\n", x, r);
                        scr_printf("\n\tFATAL: Failed to dump xfrom page %d: err:0x%X\n\n", x, r);
                        scr_setfontcolor(BGR_WHITES);
                        dumpOK = 0;
                        break;
                    }
                    int written = write(fd, PAGEBUF, MEMORYCARD_PAGESIZE);
                    if (written != MEMORYCARD_PAGESIZE) {
                        scr_setfontcolor(BGR_REDS);
                            printf("\n\tI/O error when writing page %d into file\n\n", x);
                        scr_printf("\n\tI/O error when writing page %d into file\n", x);
                        scr_printf("\twrote %X bytes instead of %X\n\n", written, MEMORYCARD_PAGESIZE);
                        scr_setfontcolor(BGR_WHITES);
                        dumpOK = 0;
                        break;
                    }
                }
                if (dumpOK) {
                    scr_setfontcolor(BGR_GREENS);
                    scr_printf(" %04X", cardsize);
                    scr_genericgaugepercentcalc(cardsize, cardsize);
                    scr_setfontcolor(BGR_WHITES);
                    scr_printf("\n\tDump finished! wrote %dMB image\n", ((cardsize*pagesize)/1024)/1024);
                }
                close(fd);
            }
        } else {scr_printf("\t[FAIL]: Cant get XFROM specs\n");}
    } else {scr_printf("Could not contact XFROMDUMP.IRX RPC service");}





    RESTORE_EXCEPTION_HANDLER
    //SleepThread();
EXIT_IN60S:
    scr_printf("\tProcess ended...\r");
    for (int i = 60; i > 0; i--)
    {
        sleep(1);
        scr_printf("\texiting in %02d\r", i);
    }

    return 0;
}


void _ps2sdk_memory_init() {
    INSTALL_EXCEPTION_HANDLER
#ifdef ARCADE
    sio_puts("# Replacing FILEIO and launching CDVDFSV\n");
    while (!SifIopRebootBuffer(ioprp, size_ioprp)) {}; //replace FILEIO to avoid RPC hang
    // stuff that does not require IOP
    while (!SifIopSync()) {};
    SifLoadStartModule("rom0:CDVDFSV", 0, NULL, NULL); // bring back CDVDMAN RPC to avoid hangs on libcglue library
#endif
}
