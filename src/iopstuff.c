#include <iopcontrol.h>
#include <loadfile.h>
#include <kernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sbv_patches.h>
#include <sio.h>

#include "generic_macros.h"

IMPORT_BIN2C(ioprp)

IMPORT_BIN2C(usbd_irx)
IMPORT_BIN2C(bdm_irx)
IMPORT_BIN2C(bdmfs_fatfs_irx)
IMPORT_BIN2C(usbmass_bd_irx)
IMPORT_BIN2C(fileXio_irx)
IMPORT_BIN2C(iomanX_irx)
IMPORT_BIN2C(mmceman_irx)
IMPORT_BIN2C(sio2man_irx)
IMPORT_BIN2C(padman_irx)
IMPORT_BIN2C(mcserv_irx)
IMPORT_BIN2C(dongleman_irx)
IMPORT_BIN2C(mcman_irx)
IMPORT_BIN2C(xfromdump_irx)
IMPORT_BIN2C(extflash_irx)
IMPORT_BIN2C(xfromman_irx);
IMPORT_BIN2C(ps2dev9_irx);

void reboot_iop(const char* bootstr) {
#ifndef IOPRP
        while (!SifIopReset(bootstr, 0)){};
#else
        while (!SifIopRebootBuffer(ioprp, size_ioprp)) {};
#endif
        while (!SifIopSync()){};
}

void sbv_patch() {
    sbv_patch_enable_lmb(); // patch modload to support SifExecModuleBuffer
    sbv_patch_disable_prefix_check(); // remove security from MODLOAD
#ifndef ARCADE
    // this patch fixes a missing break statement of builtin FILEIO.
    // we skip it on arcade, because we use homebrew FILEIO and that one is fixed
    sbv_patch_fileio();
#endif
}

#include <debug.h>
//prints the result of an IRX execution, if exec was ok, keeps printing on the same line, if something went wrong... keep the line
#define IRX_REPORT(x, id, ret) scr_printf("\t%s: id:%d, ret:%d\t\t%c", #x, id, ret, (id < 0 || ret == 1) ? '\n' : '\r'); if (id < 0 || ret == 1) sleep(1);

void loadAllModules() {
    int id, ret;
    id = IRX_LOADBUFF(iomanX_irx, &ret); IRX_REPORT(IOMANX, id, ret);
    id = IRX_LOADBUFF(fileXio_irx, &ret); IRX_REPORT(FILEXIO, id, ret);

    id = IRX_LOADBUFF(usbd_irx, &ret); IRX_REPORT(USBD, id, ret);
    id = IRX_LOADBUFF(bdm_irx, &ret); IRX_REPORT(BDM, id, ret);
    id = IRX_LOADBUFF(bdmfs_fatfs_irx, &ret); IRX_REPORT(BDM_FATFS, id, ret);
    id = IRX_LOADBUFF(usbmass_bd_irx, &ret); IRX_REPORT(USBDMASSBD, id, ret);

    id = IRX_LOADBUFF(sio2man_irx, &ret); IRX_REPORT(SIO2MAN, id, ret);
    id = IRX_LOADBUFF(padman_irx, &ret); IRX_REPORT(PADMAN, id, ret);
    id = IRX_LOADBUFF(mcman_irx, &ret); IRX_REPORT(MCMAN, id, ret);
    id = IRX_LOADBUFF(mcserv_irx, &ret); IRX_REPORT(MCSERV, id, ret);

    id = IRX_LOADBUFF(ps2dev9_irx, &ret); IRX_REPORT(DEV9, id, ret);
    id = IRX_LOADBUFF(extflash_irx, &ret); IRX_REPORT(EXTFLASH, id, ret);
    id = IRX_LOADBUFF(xfromman_irx, &ret); IRX_REPORT(XFROMMAN, id, ret);
    id = IRX_LOADBUFF(xfromdump_irx, &ret); IRX_REPORT(XFROMDMP, id, ret);
}
