/****************************************************************************** 
  Header: SecureEngineSDK.h
  Description: SDK header definition for the C/C++ language

  Author/s: Oreans Technologies  
  (c) 2013 Oreans Technologies
*****************************************************************************/ 

#pragma once


// ***********************************************
// Cross Compiler definitions
// ***********************************************

#ifdef __GNUC__
 #define DLL_IMPORT             extern
 #define STDCALL_CONVENTION
#else
 #define DLL_IMPORT             __declspec(dllimport)
 #define STDCALL_CONVENTION     __stdcall
#endif


// ***********************************************
// Specify platform
// ***********************************************

#ifdef __GNUC__

  #ifdef __x86_64__
  #define PLATFORM_X64
  #else
  #define PLATFORM_X32
  #endif

#else

  #ifdef _WIN64
  #define PLATFORM_X64
  #else
  #define PLATFORM_X32
  #endif

#endif


// ***********************************************
// Defines
// ***********************************************

#ifdef __GNUC__

#define X32_INSERT_VIA_INLINE
#define X64_INSERT_VIA_INLINE

#else

#define X32_INSERT_VIA_INLINE
//#define X64_INSERT_VIA_INLINE

#endif


// ***********************************************
// Include files
// ***********************************************

#include "SecureEngineCustomVMs.h"


// ***********************************************
// Definition of macros as function names
// ***********************************************

 #ifdef __cplusplus
  extern "C" {
 #endif

 DLL_IMPORT void STDCALL_CONVENTION VMStart(void);

 DLL_IMPORT void STDCALL_CONVENTION VMEnd(void);

 DLL_IMPORT void STDCALL_CONVENTION CodeReplaceStart(void);

 DLL_IMPORT void STDCALL_CONVENTION CodeReplaceEnd(void);

 DLL_IMPORT void STDCALL_CONVENTION RegisteredStart(void);

 DLL_IMPORT void STDCALL_CONVENTION RegisteredEnd(void);

 DLL_IMPORT void STDCALL_CONVENTION EncodeStart(void);

 DLL_IMPORT void STDCALL_CONVENTION EncodeEnd(void);

 DLL_IMPORT void STDCALL_CONVENTION ClearStart(void);

 DLL_IMPORT void STDCALL_CONVENTION ClearEnd(void);

 DLL_IMPORT void STDCALL_CONVENTION MutateStart(void);

 DLL_IMPORT void STDCALL_CONVENTION MutateEnd(void);

 DLL_IMPORT void STDCALL_CONVENTION StrEncryptStart(void);

 DLL_IMPORT void STDCALL_CONVENTION StrEncryptEnd(void);

 DLL_IMPORT void STDCALL_CONVENTION StrEncryptWStart(void);

 DLL_IMPORT void STDCALL_CONVENTION StrEncryptWEnd(void);

 DLL_IMPORT void STDCALL_CONVENTION UnregisteredStart(void);

 DLL_IMPORT void STDCALL_CONVENTION UnregisteredEnd(void);

 DLL_IMPORT void STDCALL_CONVENTION RegisteredVMStart(void);

 DLL_IMPORT void STDCALL_CONVENTION RegisteredVMEnd(void);

 DLL_IMPORT void STDCALL_CONVENTION UnprotectedStart(void);

 DLL_IMPORT void STDCALL_CONVENTION UnprotectedEnd(void);

 DLL_IMPORT void STDCALL_CONVENTION SECheckProtection(int *user_var, int user_value);

 DLL_IMPORT void STDCALL_CONVENTION SECheckCodeIntegrity(int *user_var, int user_value);

 DLL_IMPORT void STDCALL_CONVENTION SECheckRegistration(int *user_var, int user_value);

 DLL_IMPORT void STDCALL_CONVENTION SECheckVirtualPC(int *user_var, int user_value);

 #ifdef __cplusplus
 }
 #endif


#ifdef PLATFORM_X64

// ***********************************************
// SecureEngine x64 macros definitions to keep
// compatibility with old x32 inline macros
// ***********************************************

 #define VM_START VMStart();
 #define VM_END VMEnd();
 #define VM_START_WITHLEVEL(x) VMStart();

 #define CODEREPLACE_START CodeReplaceStart();
 #define CODEREPLACE_END CodeReplaceEnd();

 #define REGISTERED_START RegisteredStart();
 #define REGISTERED_END RegisteredEnd();

 #define ENCODE_START EncodeStart();
 #define ENCODE_END EncodeEnd();

 #define CLEAR_START ClearStart();
 #define CLEAR_END ClearEnd();

 #define MUTATE_START MutateStart();
 #define MUTATE_END MutateEnd();

 #define STR_ENCRYPT_START StrEncryptStart();
 #define STR_ENCRYPT_END StrEncryptEnd();

 #define STR_ENCRYPTW_START StrEncryptWStart();
 #define STR_ENCRYPTW_END StrEncryptWEnd();

 #define UNREGISTERED_START UnregisteredStart();
 #define UNREGISTERED_END UnregisteredEnd();

 #define CLEAR_START ClearStart();
 #define CLEAR_END ClearEnd();

 #define REGISTEREDVM_START RegisteredVMStart();
 #define REGISTEREDVM_END RegisteredVMEnd();

 #define UNPROTECTED_START UnprotectedStart();
 #define UNPROTECTED_END UnprotectedEnd();

 #define CHECK_PROTECTION(var, val) SECheckProtection(&var, val);
 #define CHECK_CODE_INTEGRITY(var, val) SECheckCodeIntegrity(&var, val);
 #define CHECK_REGISTRATION(var, val) SECheckRegistration(&var, val);
 #define CHECK_VIRTUAL_PC(var, val) SECheckVirtualPC(&var, val);
 
#else

// ***********************************************
// SecureEngine x32 inline macros definitions
// ***********************************************

 // Borland macros definitions

 #ifdef __BORLANDC__
 
 #define REMOVE_BLOCK_START     __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, \
                                           0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 #define REMOVE_BLOCK_END       __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x01, 0x00, 0x00, 0x00, \
                                           0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);

 #define CODEREPLACE_START      __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 #define CODEREPLACE_END        __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x01, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 
 #define REGISTERED_START       __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x02, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 #define REGISTERED_END         __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x03, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 
 #define ENCODE_START           __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x04, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 #define ENCODE_END             __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x05, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);

 #define CLEAR_START            __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x06, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 #define CLEAR_END              __emit__ (0xEB, 0x15, 0x57, 0x4C, 0x20, 0x20, 0x07, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20, 0x00, 0x00, \
                                          0x00, 0x00, 0x00);

 #define UNREGISTERED_START     __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x08, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 #define UNREGISTERED_END       __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x09, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);

 #define VM_START               __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x0C, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 #define VM_END                 __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x0D, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);

 #define MUTATE_START           __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x10, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 #define MUTATE_END             __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x11, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);

 #define STR_ENCRYPT_START      __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x12, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 #define STR_ENCRYPT_END        __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x13, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);

 #define STR_ENCRYPTW_START     __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x22, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 #define STR_ENCRYPTW_END       __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x23, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);

 #define REGISTEREDVM_START     __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x0E, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 #define REGISTEREDVM_END       __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x0F, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);

 #define VM_START_WITHLEVEL(x)  __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x0C, 0x00, 0x00, 0x00, \
                                          0x00, x, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);

 #define UNPROTECTED_START      __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);
 #define UNPROTECTED_END        __emit__ (0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x21, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20);

 #define CHECK_PROTECTION(var, val) \
 asm {  \
   dw    0x10EB; \
   dd    0x091ab3167; \
   dd    0x08a8b717a; \
   dd    0x0bc117abd; \
   dd    0x0; \
   push  val; \
   pop   var; \
   dw    0x0CEB; \
   dd    0x0bc117abd; \
   dd    0x08a8b717a; \
   dd    0x091ab3167; \
}

 #define CHECK_CODE_INTEGRITY(var, val) \
 asm {  \
   dw    0x10EB; \
   dd    0x091ab3167; \
   dd    0x08a8b717a; \
   dd    0x0bc117abd; \
   dd    0x1; \
   push  val; \
   pop   var; \
   dw    0x0CEB; \
   dd    0x0bc117abd; \
   dd    0x08a8b717a; \
   dd    0x091ab3167; \
}

 #define CHECK_REGISTRATION(var, val) \
 asm {  \
   dw    0x10EB; \
   dd    0x091ab3167; \
   dd    0x08a8b717a; \
   dd    0x0bc117abd; \
   dd    0x2; \
   push  val; \
   pop   var; \
   dw    0x0CEB; \
   dd    0x0bc117abd; \
   dd    0x08a8b717a; \
   dd    0x091ab3167; \
}

 #define CHECK_VIRTUAL_PC(var, val) \
 asm {  \
   dw    0x10EB; \
   dd    0x091ab3167; \
   dd    0x08a8b717a; \
   dd    0x0bc117abd; \
   dd    0x3; \
   push  val; \
   pop   var; \
   dw    0x0CEB; \
   dd    0x0bc117abd; \
   dd    0x08a8b717a; \
   dd    0x091ab3167; \
}
 
#else
#ifdef __GNUC__

 // GNUC (MinGW) Compatible compiler macros definitions

 #define NO_OPTIMIZATION __attribute__((optimize("O0")))

 #define REMOVE_BLOCK_START \
 asm ( ".byte 0xEB\n"\
       ".byte 0x10\n"\
       ".byte 0x57\n"\
       ".byte 0x4C\n"\
       ".byte 0x20\n"\
       ".byte 0x20\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x57\n"\
       ".byte 0x4C\n"\
       ".byte 0x20\n"\
       ".byte 0x20\n"\
     );

 #define REMOVE_BLOCK_END \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x01\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );
 
 #define CODEREPLACE_START \
 asm ( ".byte 0xEB\n"\
       ".byte 0x10\n"\
       ".byte 0x57\n"\
       ".byte 0x4C\n"\
       ".byte 0x20\n"\
       ".byte 0x20\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x57\n"\
       ".byte 0x4C\n"\
       ".byte 0x20\n"\
       ".byte 0x20\n"\
     );

#define CODEREPLACE_END \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x01\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

 #define REGISTERED_START \
 asm ( ".byte 0xEB\n"\
       ".byte 0x10\n"\
       ".byte 0x57\n"\
       ".byte 0x4C\n"\
       ".byte 0x20\n"\
       ".byte 0x20\n"\
       ".byte 0x02\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x00\n"\
       ".byte 0x57\n"\
       ".byte 0x4C\n"\
       ".byte 0x20\n"\
       ".byte 0x20\n"\
     );

#define REGISTERED_END \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x03\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define ENCODE_START \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x04\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define ENCODE_END \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x05\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define CLEAR_START \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x06\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define CLEAR_END \
asm ( ".byte 0xEB\n"\
      ".byte 0x15\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x07\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
    );

#define UNREGISTERED_START \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x08\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define UNREGISTERED_END \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x09\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define VM_START \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
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
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define VM_END \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
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
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define MUTATE_START \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x10\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define MUTATE_END \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x11\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define STR_ENCRYPT_START \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x12\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define STR_ENCRYPT_END \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x13\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define STR_ENCRYPTW_START \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x22\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define STR_ENCRYPTW_END \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x23\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );


#define REGISTEREDVM_START \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x0E\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define REGISTEREDVM_END \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x0F\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define VM_START_WITHLEVEL(x) \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x0C\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte $" #x "\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define UNPROTECTED_START \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );


#define UNPROTECTED_END \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
      ".byte 0x21\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x57\n"\
      ".byte 0x4C\n"\
      ".byte 0x20\n"\
      ".byte 0x20\n"\
    );

#define CHECK_PROTECTION(var, val) \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x67\n"\
      ".byte 0x31\n"\
      ".byte 0xAB\n"\
      ".byte 0x91\n"\
      ".byte 0x7A\n"\
      ".byte 0x71\n"\
      ".byte 0x8B\n"\
      ".byte 0x8A\n"\
      ".byte 0xBD\n"\
      ".byte 0x7A\n"\
      ".byte 0x11\n"\
      ".byte 0xBC\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      "push  $" #val "\n"); \
      __asm__  ("pop %0" : "=m" (var):); \
      asm (".byte 0xEB\n"\
      ".byte 0x0C\n"\
      ".byte 0xBD\n"\
      ".byte 0x7A\n"\
      ".byte 0x11\n"\
      ".byte 0xBC\n"\
      ".byte 0x7A\n"\
      ".byte 0x71\n"\
      ".byte 0x8B\n"\
      ".byte 0x8A\n"\
      ".byte 0x67\n"\
      ".byte 0x31\n"\
      ".byte 0xAB\n"\
      ".byte 0x91\n"\
      );

#define CHECK_CODE_INTEGRITY(var, val) \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x67\n"\
      ".byte 0x31\n"\
      ".byte 0xAB\n"\
      ".byte 0x91\n"\
      ".byte 0x7A\n"\
      ".byte 0x71\n"\
      ".byte 0x8B\n"\
      ".byte 0x8A\n"\
      ".byte 0xBD\n"\
      ".byte 0x7A\n"\
      ".byte 0x11\n"\
      ".byte 0xBC\n"\
      ".byte 0x01\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      "push  $" #val "\n"); \
      __asm__  ("pop %0" : "=m" (var):); \
      asm (".byte 0xEB\n"\
      ".byte 0x0C\n"\
      ".byte 0xBD\n"\
      ".byte 0x7A\n"\
      ".byte 0x11\n"\
      ".byte 0xBC\n"\
      ".byte 0x7A\n"\
      ".byte 0x71\n"\
      ".byte 0x8B\n"\
      ".byte 0x8A\n"\
      ".byte 0x67\n"\
      ".byte 0x31\n"\
      ".byte 0xAB\n"\
      ".byte 0x91\n"\
      );

#define CHECK_REGISTRATION(var, val) \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x67\n"\
      ".byte 0x31\n"\
      ".byte 0xAB\n"\
      ".byte 0x91\n"\
      ".byte 0x7A\n"\
      ".byte 0x71\n"\
      ".byte 0x8B\n"\
      ".byte 0x8A\n"\
      ".byte 0xBD\n"\
      ".byte 0x7A\n"\
      ".byte 0x11\n"\
      ".byte 0xBC\n"\
      ".byte 0x02\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      "push  $" #val "\n"); \
      __asm__  ("pop %0" : "=m" (var):); \
      asm (".byte 0xEB\n"\
      ".byte 0x0C\n"\
      ".byte 0xBD\n"\
      ".byte 0x7A\n"\
      ".byte 0x11\n"\
      ".byte 0xBC\n"\
      ".byte 0x7A\n"\
      ".byte 0x71\n"\
      ".byte 0x8B\n"\
      ".byte 0x8A\n"\
      ".byte 0x67\n"\
      ".byte 0x31\n"\
      ".byte 0xAB\n"\
      ".byte 0x91\n"\
      );

#define CHECK_VIRTUAL_PC(var, val) \
asm ( ".byte 0xEB\n"\
      ".byte 0x10\n"\
      ".byte 0x67\n"\
      ".byte 0x31\n"\
      ".byte 0xAB\n"\
      ".byte 0x91\n"\
      ".byte 0x7A\n"\
      ".byte 0x71\n"\
      ".byte 0x8B\n"\
      ".byte 0x8A\n"\
      ".byte 0xBD\n"\
      ".byte 0x7A\n"\
      ".byte 0x11\n"\
      ".byte 0xBC\n"\
      ".byte 0x03\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      ".byte 0x00\n"\
      "push  $" #val "\n"); \
      __asm__  ("pop %0" : "=m" (var):); \
      asm (".byte 0xEB\n"\
      ".byte 0x0C\n"\
      ".byte 0xBD\n"\
      ".byte 0x7A\n"\
      ".byte 0x11\n"\
      ".byte 0xBC\n"\
      ".byte 0x7A\n"\
      ".byte 0x71\n"\
      ".byte 0x8B\n"\
      ".byte 0x8A\n"\
      ".byte 0x67\n"\
      ".byte 0x31\n"\
      ".byte 0xAB\n"\
      ".byte 0x91\n"\
      );

#else
#ifdef __ICL

// ICL macros definitions

 #define REMOVE_BLOCK_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
 
 #define REMOVE_BLOCK_END \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x01 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \

  #define CODEREPLACE_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
 
 #define CODEREPLACE_END \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x01 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \

 #define REGISTERED_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x02 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
 
 #define REGISTERED_END \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x03 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
 
 
 #define ENCODE_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x04 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
 
 
 #define ENCODE_END \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x05 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
 
 
 #define CLEAR_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x06 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \

 
 #define CLEAR_END \
  __asm __emit 0xEB \
  __asm __emit 0x15 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x07 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 
 
  #define UNREGISTERED_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x08 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
 
 #define UNREGISTERED_END \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x09 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \

  #define VM_START_WITHLEVEL(x) \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x0C \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit x \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \

  #define VM_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
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
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  
 #define VM_END \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
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
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \

  #define MUTATE_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x10 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  
 #define MUTATE_END \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x11 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \

  #define STR_ENCRYPT_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x12 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  
 #define STR_ENCRYPT_END \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x13 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \

  #define STR_ENCRYPTW_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x22 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  
 #define STR_ENCRYPTW_END \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x23 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \


  #define REGISTEREDVM_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x0E \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  
 #define REGISTEREDVM_END \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x0F \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \

  #define UNPROTECTED_START \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  
 #define UNPROTECTED_END \
  __asm __emit 0xEB \
  __asm __emit 0x10 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \
  __asm __emit 0x21 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x00 \
  __asm __emit 0x57 \
  __asm __emit 0x4C \
  __asm __emit 0x20 \
  __asm __emit 0x20 \

#else
#ifdef __LCC__

// LCC macros definitions
 
 #define REMOVE_BLOCK_START     __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");
 #define REMOVE_BLOCK_END       __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x01, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define CODEREPLACE_START      __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");
 #define CODEREPLACE_END        __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x01, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");
 
 #define REGISTERED_START       __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x02, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");
 #define REGISTERED_END         __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x03, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");
 
 #define ENCODE_START           __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x04, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");
 #define ENCODE_END             __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x05, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");
 
 #define CLEAR_START            __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x06, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");
 
 #define CLEAR_END              __asm__ (" .byte\t0xEB, 0x15, 0x57, 0x4C, 0x20, 0x20, 0x07, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20, 0x00, 0x00, \
                                         0x00, 0x00, 0x00");

 #define UNREGISTERED_START     __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x08, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");
 #define UNREGISTERED_END       __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x09, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define VM_START_WITHLEVEL(x)  __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x0C, 0x00, 0x00, 0x00, \
                                         0x00, "x", 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define VM_START               __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x0C, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define VM_END                 __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x0D, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define MUTATE_START           __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x10, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define MUTATE_END             __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x11, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define STR_ENCRYPT_START      __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x12, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define STR_ENCRYPT_END        __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x13, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define STR_ENCRYPT_START      __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x22, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define STR_ENCRYPT_END        __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x23, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define REGISTEREDVM_START     __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x0E, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define REGISTEREDVM_END       __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x0F, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define UNPROTECTED_START      __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

 #define UNPROTECTED_END        __asm__ (" .byte\t0xEB, 0x10, 0x57, 0x4C, 0x20, 0x20, 0x21, 0x00, 0x00, 0x00, \
                                         0x00, 0x00, 0x00, 0x00, 0x57, 0x4C, 0x20, 0x20");

#else

 // Visual Studio macros definitions

 #define REMOVE_BLOCK_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
 
 #define REMOVE_BLOCK_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x01 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

 #define CODEREPLACE_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
 
 #define CODEREPLACE_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x01 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

 #define REGISTERED_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x02 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
 
#define REGISTERED_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x03 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
 
 #define ENCODE_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x04 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
 
 #define ENCODE_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x05 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
 
 #define CLEAR_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x06 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
 
 #define CLEAR_END \
  __asm _emit 0xEB \
  __asm _emit 0x15 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x07 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 

 #define UNREGISTERED_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x08 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
 
#define UNREGISTERED_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x09 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

 #define VM_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x0C \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

 #define VM_START_WITHLEVEL(x) \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x0C \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit x \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
 
 #define VM_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x0D \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

 #define MUTATE_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x10 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

 #define MUTATE_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x11 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

 #define STR_ENCRYPT_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x12 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

 #define STR_ENCRYPT_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x13 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

 #define STR_ENCRYPTW_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x22 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

 #define STR_ENCRYPTW_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x23 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \


 #define REGISTEREDVM_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x0E \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

 #define REGISTEREDVM_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x0F \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \


 #define UNPROTECTED_START \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

 #define UNPROTECTED_END \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \
  __asm _emit 0x21 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x57 \
  __asm _emit 0x4C \
  __asm _emit 0x20 \
  __asm _emit 0x20 \

  #define CHECK_PROTECTION(var, val) \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x67 \
  __asm _emit 0x31 \
  __asm _emit 0xAB \
  __asm _emit 0x91 \
  __asm _emit 0x7A \
  __asm _emit 0x71 \
  __asm _emit 0x8B \
  __asm _emit 0x8A \
  __asm _emit 0xBD \
  __asm _emit 0x7A \
  __asm _emit 0x11 \
  __asm _emit 0xBC \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm push  val \
  __asm pop   var \
  __asm _emit 0xEB \
  __asm _emit 0x0C \
  __asm _emit 0xBD \
  __asm _emit 0x7A \
  __asm _emit 0x11 \
  __asm _emit 0xBC \
  __asm _emit 0x7A \
  __asm _emit 0x71 \
  __asm _emit 0x8B \
  __asm _emit 0x8A \
  __asm _emit 0x67 \
  __asm _emit 0x31 \
  __asm _emit 0xAB \
  __asm _emit 0x91 \

  #define CHECK_CODE_INTEGRITY(var, val) \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x67 \
  __asm _emit 0x31 \
  __asm _emit 0xAB \
  __asm _emit 0x91 \
  __asm _emit 0x7A \
  __asm _emit 0x71 \
  __asm _emit 0x8B \
  __asm _emit 0x8A \
  __asm _emit 0xBD \
  __asm _emit 0x7A \
  __asm _emit 0x11 \
  __asm _emit 0xBC \
  __asm _emit 0x01 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm push  val \
  __asm pop   var \
  __asm _emit 0xEB \
  __asm _emit 0x0C \
  __asm _emit 0xBD \
  __asm _emit 0x7A \
  __asm _emit 0x11 \
  __asm _emit 0xBC \
  __asm _emit 0x7A \
  __asm _emit 0x71 \
  __asm _emit 0x8B \
  __asm _emit 0x8A \
  __asm _emit 0x67 \
  __asm _emit 0x31 \
  __asm _emit 0xAB \
  __asm _emit 0x91 \

  #define CHECK_REGISTRATION(var, val) \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x67 \
  __asm _emit 0x31 \
  __asm _emit 0xAB \
  __asm _emit 0x91 \
  __asm _emit 0x7A \
  __asm _emit 0x71 \
  __asm _emit 0x8B \
  __asm _emit 0x8A \
  __asm _emit 0xBD \
  __asm _emit 0x7A \
  __asm _emit 0x11 \
  __asm _emit 0xBC \
  __asm _emit 0x02 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm push  val \
  __asm pop   var \
  __asm _emit 0xEB \
  __asm _emit 0x0C \
  __asm _emit 0xBD \
  __asm _emit 0x7A \
  __asm _emit 0x11 \
  __asm _emit 0xBC \
  __asm _emit 0x7A \
  __asm _emit 0x71 \
  __asm _emit 0x8B \
  __asm _emit 0x8A \
  __asm _emit 0x67 \
  __asm _emit 0x31 \
  __asm _emit 0xAB \
  __asm _emit 0x91 \

  #define CHECK_VIRTUAL_PC(var, val) \
  __asm _emit 0xEB \
  __asm _emit 0x10 \
  __asm _emit 0x67 \
  __asm _emit 0x31 \
  __asm _emit 0xAB \
  __asm _emit 0x91 \
  __asm _emit 0x7A \
  __asm _emit 0x71 \
  __asm _emit 0x8B \
  __asm _emit 0x8A \
  __asm _emit 0xBD \
  __asm _emit 0x7A \
  __asm _emit 0x11 \
  __asm _emit 0xBC \
  __asm _emit 0x03 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm _emit 0x00 \
  __asm push  val \
  __asm pop   var \
  __asm _emit 0xEB \
  __asm _emit 0x0C \
  __asm _emit 0xBD \
  __asm _emit 0x7A \
  __asm _emit 0x11 \
  __asm _emit 0xBC \
  __asm _emit 0x7A \
  __asm _emit 0x71 \
  __asm _emit 0x8B \
  __asm _emit 0x8A \
  __asm _emit 0x67 \
  __asm _emit 0x31 \
  __asm _emit 0xAB \
  __asm _emit 0x91 \

  #endif  
 #endif   
#endif
#endif
#endif
