#include <iopcontrol.h>
#include <loadfile.h>
#include <kernel.h>
#include <stdlib.h>
#include <stdio.h>
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

void loadAllModules() {
    IRX_LOADBUFF(iomanX_irx, NULL);
    IRX_LOADBUFF(fileXio_irx, NULL);

    IRX_LOADBUFF(usbd_irx, NULL);
    IRX_LOADBUFF(bdm_irx, NULL);
    IRX_LOADBUFF(bdmfs_fatfs_irx, NULL);
    IRX_LOADBUFF(usbmass_bd_irx, NULL);

    IRX_LOADBUFF(sio2man_irx, NULL);
    IRX_LOADBUFF(padman_irx, NULL);
    IRX_LOADBUFF(mcserv_irx, NULL);
    IRX_LOADBUFF(mcman_irx, NULL);

    IRX_LOADBUFF(extflash_irx, NULL);
    IRX_LOADBUFF(xfromman_irx, NULL);
    IRX_LOADBUFF(xfromdump_irx, NULL);
}
