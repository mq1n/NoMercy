/******************************************************************************
 * Header: SecureEngineCustomVMs.h
 * Description:  Definitions for Custom VMs in SecureEngine
 *
 * Author/s: Oreans Technologies 
 * (c) 2014 Oreans Technologies
 *
 * --- File generated automatically from Oreans VM Generator (19/8/2014) ---
 ******************************************************************************/

// ***********************************************
// Definition of macros as function names
// ***********************************************

 #ifdef __cplusplus
  extern "C" {
 #endif

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000100_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000100_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000103_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000103_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000101_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000101_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000104_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000104_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000102_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000102_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000105_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000105_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000106_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000106_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000107_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000107_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000108_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000108_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000109_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000109_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000110_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000110_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000111_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000111_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000112_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000112_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000113_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000113_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000114_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000114_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000115_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000115_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000116_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000116_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000117_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000117_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000118_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000118_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000119_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000119_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000120_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000120_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000121_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000121_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000122_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000122_End(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000123_Start(void);

DLL_IMPORT void STDCALL_CONVENTION CustomVM00000123_End(void);

DLL_IMPORT void STDCALL_CONVENTION Mutate_Start(void);

DLL_IMPORT void STDCALL_CONVENTION Mutate_End(void);

DLL_IMPORT void STDCALL_CONVENTION Mutate_Start(void);

DLL_IMPORT void STDCALL_CONVENTION Mutate_End(void);

#ifdef __cplusplus
}
#endif


// ***********************************************
// x64 definition as function names
// ***********************************************

#if defined(PLATFORM_X64) && !defined(X64_INSERT_VIA_INLINE)

#define VM_TIGER_WHITE_START CustomVM00000103_Start();
#define VM_TIGER_WHITE_END CustomVM00000103_End();

#define VM_TIGER_RED_START CustomVM00000104_Start();
#define VM_TIGER_RED_END CustomVM00000104_End();

#define VM_TIGER_BLACK_START CustomVM00000105_Start();
#define VM_TIGER_BLACK_END CustomVM00000105_End();

#define VM_FISH_WHITE_START CustomVM00000107_Start();
#define VM_FISH_WHITE_END CustomVM00000107_End();

#define VM_FISH_RED_START CustomVM00000109_Start();
#define VM_FISH_RED_END CustomVM00000109_End();

#define VM_FISH_BLACK_START CustomVM00000111_Start();
#define VM_FISH_BLACK_END CustomVM00000111_End();

#define VM_PUMA_WHITE_START CustomVM00000113_Start();
#define VM_PUMA_WHITE_END CustomVM00000113_End();

#define VM_PUMA_RED_START CustomVM00000115_Start();
#define VM_PUMA_RED_END CustomVM00000115_End();

#define VM_PUMA_BLACK_START CustomVM00000117_Start();
#define VM_PUMA_BLACK_END CustomVM00000117_End();

#define VM_SHARK_WHITE_START CustomVM00000119_Start();
#define VM_SHARK_WHITE_END CustomVM00000119_End();

#define VM_SHARK_RED_START CustomVM00000121_Start();
#define VM_SHARK_RED_END CustomVM00000121_End();

#define VM_SHARK_BLACK_START CustomVM00000123_Start();
#define VM_SHARK_BLACK_END CustomVM00000123_End();

#define VM_MUTATE_ONLY_START Mutate_Start();
#define VM_MUTATE_ONLY_END Mutate_End();

#define CUSTOM_VMS_DEFINED

#endif 


// ***********************************************
// x32 definition as function names
// ***********************************************

#if defined(PLATFORM_X32) && !defined(X32_INSERT_VIA_INLINE)

#define VM_TIGER_WHITE_START CustomVM00000100_Start();
#define VM_TIGER_WHITE_END CustomVM00000100_End();

#define VM_TIGER_RED_START CustomVM00000101_Start();
#define VM_TIGER_RED_END CustomVM00000101_End();

#define VM_TIGER_BLACK_START CustomVM00000102_Start();
#define VM_TIGER_BLACK_END CustomVM00000102_End();

#define VM_FISH_WHITE_START CustomVM00000106_Start();
#define VM_FISH_WHITE_END CustomVM00000106_End();

#define VM_FISH_RED_START CustomVM00000108_Start();
#define VM_FISH_RED_END CustomVM00000108_End();

#define VM_FISH_BLACK_START CustomVM00000110_Start();
#define VM_FISH_BLACK_END CustomVM00000110_End();

#define VM_PUMA_WHITE_START CustomVM00000112_Start();
#define VM_PUMA_WHITE_END CustomVM00000112_End();

#define VM_PUMA_RED_START CustomVM00000114_Start();
#define VM_PUMA_RED_END CustomVM00000114_End();

#define VM_PUMA_BLACK_START CustomVM00000116_Start();
#define VM_PUMA_BLACK_END CustomVM00000116_End();

#define VM_SHARK_WHITE_START CustomVM00000118_Start();
#define VM_SHARK_WHITE_END CustomVM00000118_End();

#define VM_SHARK_RED_START CustomVM00000120_Start();
#define VM_SHARK_RED_END CustomVM00000120_End();

#define VM_SHARK_BLACK_START CustomVM00000122_Start();
#define VM_SHARK_BLACK_END CustomVM00000122_End();

#define VM_MUTATE_ONLY_START Mutate_Start();
#define VM_MUTATE_ONLY_END Mutate_End();

#define CUSTOM_VMS_DEFINED

#endif 


// ***********************************************
// x32/x64 definition as inline assembly
// ***********************************************

#ifndef CUSTOM_VMS_DEFINED

#ifdef __BORLANDC__
  #include "SecureEngineCustomVMs_BorlandC_inline.h"
#endif

#ifdef __GNUC__
  #include "SecureEngineCustomVMs_GNU_inline.h"
#endif

#ifdef __ICL
  #include "SecureEngineCustomVMs_ICL_inline.h"
#endif

#ifdef __LCC__
  #include "SecureEngineCustomVMs_LCC_inline.h"
#endif

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
  #include "SecureEngineCustomVMs_VC_inline.h"
#endif

#endif
