/******************************************************************************
 * Header: VirtualizerSDK_BorlandC_inline.h
 * Description: Borland C++ inline assembly macros definitions
 *
 * Author/s: Oreans Technologies 
 * (c) 2013 Oreans Technologies
 *
 ******************************************************************************/

/***********************************************
 * Definition as inline assembly
 ***********************************************/

#define VIRTUALIZER_START  __emit__ (0xEB, 0x10, 0x43, 0x56, 0x20, 0x20, 0x0C, 0x00, 0x00, 0x00, \
                                     0x00, 0x00, 0x00, 0x00, 0x43, 0x56, 0x20, 0x20);

#define VIRTUALIZER_END    __emit__ (0xEB, 0x10, 0x43, 0x56, 0x20, 0x20, 0x0D, 0x00, 0x00, 0x00, \
                                     0x00, 0x00, 0x00, 0x00, 0x43, 0x56, 0x20, 0x20);

