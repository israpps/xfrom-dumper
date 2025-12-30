
EE_BIN ?= XFROMDMP.ELF
EE_OBJS_DIR = embed/
EE_OBJS = main.o modelname.o pad.o iopstuff.o scr_stuff.o \
	$(IOPOBJS) iop/xfromdump/ee_rpc.o

IOPOBJS += $(addprefix $(EE_OBJS_DIR), usbd.o bdm.o bdmfs_fatfs.o usbmass_bd.o fileXio.o iomanX.o mmceman.o sio2man.o mcserv.o padman.o xfromdump.o extflash.o xfromman.o)

DEBUG ?= 0
EE_CFLAGS += -fdata-sections -ffunction-sections -DNEWLIB_PORT_AWARE
EE_LDFLAGS += -Wl,--gc-sections
EE_LIBS += -ldebug -lpatches -lfileXio -lcdvd -lpadx

all: $(DEPENDENCIES) $(EE_BIN)

clean:
	rm -rf $(EE_OBJS) $(EE_BIN)

vpath %.c src/

# Include makefiles
include Makefile.extra
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
