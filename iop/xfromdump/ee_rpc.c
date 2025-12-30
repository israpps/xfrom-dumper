#include <errno.h>
#include <stdbool.h>
#include <tamtypes.h>
#include <string.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>
#include "xfrom_stuff.h"

#define DPRINTF(fmt, x...) printf(fmt, ##x)
#define CHECK_RPC_INIT() if (!rpc_initialized) {DPRINTF("ERROR: Cannot call %s if RPC server is not initialized\n", __FUNCTION__); return -2;}


static SifRpcClientData_t XFROMRPC;
static int rpc_initialized = false;

int xfrom_dumper_init(void)
{
    printf("binding to XFROMDUMP_RPC_IRX (%d) service\n", XFROMDUMP_RPC_IRX);
    int retries = 100;
    if (rpc_initialized)
        {return 0;}

    int E;
	while(retries--)
	{
		if((E = SifBindRpc(&XFROMRPC, XFROMDUMP_RPC_IRX, 0)) < 0)
        {
            DPRINTF("Failed to bind RPC server for XFROM DUMPER (%d)\n", E);
			return SCE_EBINDMISS;
        }

		if(XFROMRPC.server != NULL)
			break;

		nopdelay();
	}

	rpc_initialized = retries;
    printf("%sbound at %d retries\n", (retries) ? "" : "NOT ", 100-retries);
	return (retries) ? 0 : ESRCH;
}

#define RPC_BUFPARAM(x) &x, sizeof(x)

int dump_nandpage(int page, u8* pagebuf) {
    CHECK_RPC_INIT();
    dumpflashpage_t pkt;
    memset(&pkt, 0, sizeof(pkt));
    pkt.page = page;
    if (SifCallRpc(&XFROMRPC, XFM_READPAGE, 0, RPC_BUFPARAM(pkt), RPC_BUFPARAM(pkt), NULL, NULL) < 0)
    {
        DPRINTF("%s: RPC ERROR\n", __FUNCTION__);
        return -SCE_ECALLMISS;
    }
    if (pkt.result == 0 && pagebuf != NULL) memcpy(pagebuf, pkt.data, sizeof pkt.data);
    else printf("%s: not copying result, error on RPC (%d) or output buf is NULL (0x%p)\n", __func__, pkt.result, pagebuf);
    return pkt.result;
}

#define RETURN_SPEC_TO_PTR(x) if (x) *x=pkt.x
int nand_getspecs(s16* pagesize, u16* blocksize, int* cardsize, u8* cardflags) {
    CHECK_RPC_INIT();
    printf("getting specs of XFROM\n");
    cardspecs_packet_t pkt;
    memset(&pkt, 0, sizeof(pkt));
    if (SifCallRpc(&XFROMRPC, XFM_GETSPECS, 0, RPC_BUFPARAM(pkt), RPC_BUFPARAM(pkt), NULL, NULL) < 0)
    {
        DPRINTF("%s: RPC ERROR\n", __FUNCTION__);
        return -SCE_ECALLMISS;
    }
    printf("dump result %d \n", pkt.ret);
    if (pkt.ret == 0) {
        printf("SPECS: %d %d %d %d\n",
            pkt.pagesize,
            pkt.blocksize,
            pkt.cardsize,
            pkt.cardflags);
        RETURN_SPEC_TO_PTR(pagesize);
        RETURN_SPEC_TO_PTR(blocksize);
        RETURN_SPEC_TO_PTR(cardsize);
        RETURN_SPEC_TO_PTR(cardflags);

    }
    else printf("%s: could not get specs of XFROM (%d)\n", __func__, pkt.ret);
    return pkt.ret;
}
