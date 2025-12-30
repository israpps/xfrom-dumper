/**
 *     ___________  ___  ___  ___  ______ _   ____  _________ ___________
 *    /  ___| ___ \/ _ \ |  \/  |  |  _  \ | | |  \/  || ___ \  ___| ___ \
 *    \ `--.| |_/ / /_\ \| .  . |  | | | | | | | .  . || |_/ / |__ | |_/ /
 *     `--. \    /|  _  || |\/| |  | | | | | | | |\/| ||  __/|  __||    /
 *    /\__/ / |\ \| | | || |  | |  | |/ /| |_| | |  | || |   | |___| |\ \
 *    \____/\_| \_\_| |_/\_|  |_/  |___/  \___/\_|  |_/\_|   \____/\_| \_|
 *
 *  Namco System 2x6 SRAM dumper
 *  Copyright (c) 2025 Matias Israelson - MIT license
 *
 */
#include "irx_imports.h"
#include <errno.h>
#include "xfrom_stuff.h"
#include <stdint.h>

#define FUNLOG() DPRINTF("%s\n", __func__)
#define DPRINTF(format, args...) printf(MODNAME ": " format, ##args) // module printf

#define MODNAME "xfrom_dumper"
IRX_ID(MODNAME, 1, 1);

static SifRpcDataQueue_t rpcqueue;
static SifRpcServerData_t serverdata;
static int rpcthid;
static unsigned char SifServerBuffer[0x1000];

static void *cmdHandler(int cmd, void *buffer, int nbytes)
{
    //printf("%s: CMD %d w buff of %d bytes\n", __FUNCTION__, cmd, nbytes);

    switch (cmd)
    {
    case XFM_READPAGE:
        dumpflashpage_t* Params = buffer;
        //printf("XFM_READPAGE: read page %d\n", Params->page);
        Params->result = McReadPage(0, 0, Params->page, Params->data);
        break;
    case XFM_GETSPECS:
        cardspecs_packet_t* Specs = buffer;
        //printf("XFM_GETSPECS\n");
        Specs->ret = McGetCardSpec(0, 0, &Specs->pagesize, &Specs->blocksize, &Specs->cardsize, &Specs->cardflags);
        break;
    default:
        DPRINTF("INVALID CMD 0x%X\n", cmd);
        break;
    }
    return buffer;
}


static void ServiceThread(void *parameters)
{
    (void)parameters;
    printf("XFROMDUMP RPC THREAD RUNNING\n");

    /*if (!sceSifCheckInit()) {
        DPRINTF("sceSifCheckInit 0. initializing SIF\n");
        sceSifInit();
    }*/

    sceSifInitRpc(0);
    sceSifSetRpcQueue(&rpcqueue, GetThreadId());
    sceSifRegisterRpc(&serverdata, XFROMDUMP_RPC_IRX, &cmdHandler, SifServerBuffer, NULL, NULL, &rpcqueue);
    sceSifRpcLoop(&rpcqueue);
}

int _start(int argc, char** argv) {
    printf("%s start\n", MODNAME);
    iop_thread_t thread;
    thread.attr      = TH_C;
    thread.priority  = 0x28;
    thread.stacksize = 0x1000;

    thread.thread = &ServiceThread;
    if ((rpcthid = CreateThread(&thread)) == 0) {
        return MODULE_NO_RESIDENT_END;
    }
    StartThread(rpcthid, NULL);


    return MODULE_RESIDENT_END;
}
