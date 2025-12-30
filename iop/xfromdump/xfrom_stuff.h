#ifndef CONQUESTSERV_H
#define CONQUESTSERV_H

#include <tamtypes.h>
#define MEMORYCARD_PAGESIZE 0x200
#define MEMORYCARD_ECCSIZE 0x10
#define MEMORYCARD_SC2CRCSIZE 0x4
#define MEMORYCARD_PAGESIZE_ECC (MEMORYCARD_PAGESIZE + MEMORYCARD_ECCSIZE)
#define CONQUESTCARD_PAGESIZE_CRC (MEMORYCARD_PAGESIZE + MEMORYCARD_SC2CRCSIZE)

#define XFROMDUMP_RPC_IRX 0x7866726F // XFROM: 78 66 72 6F 6D concatenated and narrowed down to ps2 int

typedef struct cardspecs_packet_ {
    s32 port; // Input
    s32 slot; // Input
    s32 ret; // Output, must be 0 to find valid data on the rest of the struct
    s16 pagesize; // Output
    u16 blocksize; // Output
    int cardsize; // Output
    u8 cardflags; // Output
}cardspecs_packet_t;

typedef struct dumpPage {
    s32 page;
    s32 result;
    u8 data[MEMORYCARD_PAGESIZE];
} dumpflashpage_t;

enum RPCCMDS {
    XFM_READPAGE = 0x50,
    XFM_GETSPECS,
    XFM_CHECKNAND,
};

#ifdef _EE // Including this on the EE? then you most likely want access to the EE library for this IRX

int xfrom_dumper_init(void);
int dump_nandpage(int page, u8* pagebuf);
int nand_getspecs(s16* pagesize, u16* blocksize, int* cardsize, u8* cardflags);

#endif

#endif
