// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_fcntl.h
 *
 *  $Id: OS_NS_fcntl.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef OS_NS_FCNTL_H
# define OS_NS_FCNTL_H



# include "os/config-all.h"

# if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
# endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/os_fcntl.h"
#include "os/Global_Macros.h"  // for LPSECURITY_ATTRIBUTES :-(
#include "os/Default_Constants.h"
#include /**/ "os/ACE_export.h"

#if defined (ACE_EXPORT_MACRO)
#  undef ACE_EXPORT_MACRO
#endif
#define ACE_EXPORT_MACRO ACE_Export



namespace OS {

  ACE_NAMESPACE_INLINE_FUNCTION
  int fcntl (ACE_HANDLE handle,
             int cmd,
             long arg = 0);

#if !defined (ACE_DEFAULT_OPEN_PERMS)
#  define ACE_DEFAULT_OPEN_PERMS ACE_DEFAULT_FILE_PERMS
#endif  /* ACE_DEFAULT_OPEN_PERMS */

  /// The O_APPEND flag is only partly supported on Win32. If you specify
  /// O_APPEND, then the file pointer will be positioned at the end of
  /// the file initially during open, but it is not re-positioned at
  /// the end prior to each write, as specified by POSIX.  This
  /// is generally good enough for typical situations, but it is ``not
  /// quite right'' in its semantics.
  extern ACE_Export
  ACE_HANDLE open (const char *filename,
                   int mode,
                   mode_t perms = ACE_DEFAULT_OPEN_PERMS,
                   LPSECURITY_ATTRIBUTES sa = 0);
#if defined (ACE_HAS_WCHAR)
  extern ACE_Export
  ACE_HANDLE open (const wchar_t *filename,
                   int mode,
                   mode_t perms = ACE_DEFAULT_OPEN_PERMS,
                   LPSECURITY_ATTRIBUTES sa = 0);
#endif /* ACE_HAS_WCHAR */

} /* namespace OS */



#endif /* OS_NS_FCNTL_H */
