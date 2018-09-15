/******************************************************************************
 * Header: VirtualizerSDK_ICL_inline.h
 * Description: ICL inline assembly macros definitions
 *
 * Author/s: Oreans Technologies 
 * (c) 2013 Oreans Technologies
 ******************************************************************************/

/***********************************************
 * Definition as inline assembly
 ***********************************************/

#define VIRTUALIZER_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x43\
  __asm __emit 0x56\
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x0C \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x43\
  __asm __emit 0x56\
  __asm __emit 0x20 \
  __asm __emit 0x20 \

#define VIRTUALIZER_END \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x43\
  __asm __emit 0x56\
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x0D \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x43\
  __asm __emit 0x56\
  __asm __emit 0x20 \
  __asm __emit 0x20 \

