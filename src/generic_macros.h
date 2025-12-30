#ifndef GENERIC_MACROS
#define GENERIC_MACROS

#define IMPORT_BIN2C(x) extern unsigned char x[]; extern unsigned int size_##x;

//#define IRX_LOADBUFF(x, argc, argv, ret) SifExecModuleBuffer(x, size_##x, argc, argv, ret)
#define IRX_LOADBUFF(x, ret) SifExecModuleBuffer(x, size_##x, 0, NULL, ret)

#define IRX_GENERIC_ERRCHK(ID, RET) (ID < 0 || RET != 0)

#define BGR_BLUE   0x800000
#define BGR_RED    0x000080
#define BGR_GREEN  0x008000
#define BGR_WHITE  0xFFFFFF
#define BGR_BLACK  0x000000
#define BGR_PURPLE 0x800080

#endif
