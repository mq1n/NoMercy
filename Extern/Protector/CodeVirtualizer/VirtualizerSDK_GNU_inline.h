/******************************************************************************
 * Header: VirtualizerSDK_GNU_inline.h
 * Description: GNU C inline assembly macros definitions
 *
 * Author/s: Oreans Technologies 
 * (c) 2013 Oreans Technologies
 *
 ******************************************************************************/

/***********************************************
 * Definition as inline assembly
 ***********************************************/

#define VIRTUALIZER_START \
__asm__ (".byte 0xEB\n"\
     ".byte 0x10\n"\
     ".byte 0x43\n"\
     ".byte 0x56\n"\
     ".byte 0x20\n"\
     ".byte 0x20\n"\
     ".byte 0x0C\n"\
     ".byte 0x00\n"\
     ".byte 0x00\n"\
     ".byte 0x00\n"\
     ".byte 0x00\n"\
     ".byte 0x00\n"\
     ".byte 0x00\n"\
     ".byte 0x00\n"\
     ".byte 0x43\n"\
     ".byte 0x56\n"\
     ".byte 0x20\n"\
     ".byte 0x20\n");

#define VIRTUALIZER_END \
__asm__ (".byte 0xEB\n"\
     ".byte 0x10\n"\
     ".byte 0x43\n"\
     ".byte 0x56\n"\
     ".byte 0x20\n"\
     ".byte 0x20\n"\
     ".byte 0x0D\n"\
     ".byte 0x00\n"\
     ".byte 0x00\n"\
     ".byte 0x00\n"\
     ".byte 0x00\n"\
     ".byte 0x00\n"\
     ".byte 0x00\n"\
     ".byte 0x00\n"\
     ".byte 0x43\n"\
     ".byte 0x56\n"\
     ".byte 0x20\n"\
     ".byte 0x20\n");

