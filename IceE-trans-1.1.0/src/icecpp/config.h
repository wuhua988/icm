// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// This configuration file is based on the one generated by the
// configure script from the gcc-2.8.1 distribution.
//

#if defined(__linux) || defined(__FreeBSD__) \
    || defined(__sun) || defined(__hpux) || defined(__APPLE__) \
    || defined(_AIX) || defined(__osf1__)
#   define TIME_WITH_SYS_TIME 1
#   define HAVE_BCMP 1
#   define HAVE_BCOPY 1
#   define HAVE_BZERO 1
#   define HAVE_GETRLIMIT 1
#   define HAVE_INDEX 1
#   define HAVE_RINDEX 1
#   define HAVE_UNISTD_H 1
#elif defined(_WIN32)
#   include <malloc.h>
#   include <io.h>
#   ifdef _MSC_VER
#       pragma warning( disable : 4018 )
#       pragma warning( disable : 4244 )
//
//     Move deprecated warning on VC8 to level 4
//
#      if _MSC_VER==1400
#         pragma warning( 4 : 4996 )
#         pragma warning( 4 : 4267 )   
#      endif
#   endif
#   define alloca _alloca
#endif

#if defined(__linux) || defined(__FreeBSD__) || defined(__sun) || \
    defined(__hpux) || defined(__APPLE__) || defined(_AIX)
#   define HAVE_INTTYPES_H 1
#endif

#if defined(__hpux) || defined(_AIX)
#   include <alloca.h>
#endif

#if defined(__sun) || defined(__osf1__)
#   include <alloca.h>
#   include <malloc.h>
#   include <strings.h>
#endif

#define HAVE_SETRLIMIT 1
#define HAVE_STRERROR 1
#define HAVE_VPRINTF 1

#define HAVE_FCNTL_H 1
#define HAVE_LIMITS_H 1
#define HAVE_STDDEF_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_STRINGS_H 1

#define CHAR_TYPE_SIZE 8
#define INT_TYPE_SIZE 32

//
// Should be sizeof(long) * 8; but 32 works well.
//
#define LONG_TYPE_SIZE 32


#define TARGET_BELL 007
#define TARGET_BS 010
#define TARGET_TAB 011
#define TARGET_NEWLINE 012
#define TARGET_VT 013
#define TARGET_FF 014
#define TARGET_CR 015

static char *version_string = "2.8.1";

#define INCLUDE_DEFAULTS { { 0, 0, 0 } }
#define GCC_INCLUDE_DIR ""
#define FATAL_EXIT_CODE 33
#define SUCCESS_EXIT_CODE 0
#define PREFIX ""
#define OBJECT_SUFFIX ".cpp" // For -M option.