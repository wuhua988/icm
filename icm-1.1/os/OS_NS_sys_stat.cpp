// $Id: OS_NS_sys_stat.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_sys_stat.h"
// -*- C++ -*-
//
// $Id: OS_NS_sys_stat.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_unistd.h"
#include "os/OS_NS_fcntl.h"
#include "os/OS_NS_errno.h"
#include "os/OS_NS_macros.h"



namespace OS
{

  ACE_INLINE ACE_HANDLE
  creat (const ACE_TCHAR *filename, mode_t mode)
  {
    OS_TRACE ("OS::creat");
#if defined (ACE_WIN32)
    return OS::open (filename, O_CREAT|O_TRUNC|O_WRONLY, mode);
#else
    OSCALL_RETURN (::creat (ACE_TEXT_ALWAYS_CHAR (filename), mode),
                       ACE_HANDLE, ACE_INVALID_HANDLE);
#endif /* ACE_WIN32 */
  }

#if !defined (ACE_WIN32)

  ACE_INLINE int
  fstat (ACE_HANDLE handle, ACE_stat *stp)
  {
    OS_TRACE ("OS::fstat");
#if defined (ACE_HAS_X86_STAT_MACROS)
    // Solaris for intel uses an macro for fstat(), this is a wrapper
    // for _fxstat() use of the macro.
    // causes compile and runtime problems.
    OSCALL_RETURN (::_fxstat (_STAT_VER, handle, stp), int, -1);
#elif defined (ACE_WIN32)
    OSCALL_RETURN (::_fstat (handle, stp), int, -1);
#else
# if defined (ACE_OPENVMS)
    ::fsync(handle);
# endif
    OSCALL_RETURN (::fstat (handle, stp), int, -1);
# endif /* !ACE_HAS_X86_STAT_MACROS */
  }

#else /* ACE_WIN32 */

  ACE_INLINE int
  fstat (ACE_HANDLE handle, ACE_stat *stp)
  {
    OS_TRACE ("OS::fstat");
# if 1
    BY_HANDLE_FILE_INFORMATION fdata;

    if (::GetFileInformationByHandle (handle, &fdata) == FALSE)
      {
        OS::set_errno_to_last_error ();
        return -1;
      }
    else if (fdata.nFileSizeHigh != 0)
      {
        errno = EINVAL;
        return -1;
      }
    else
      {
        stp->st_size = fdata.nFileSizeLow;
        stp->st_atime = TimeValue (fdata.ftLastAccessTime).sec ();
        stp->st_mtime = TimeValue (fdata.ftLastWriteTime).sec ();
        stp->st_ctime = TimeValue (fdata.ftCreationTime).sec ();
        stp->st_nlink = static_cast<short> (fdata.nNumberOfLinks);
        stp->st_dev = stp->st_rdev = 0; // No equivalent conversion.
        stp->st_mode = S_IXOTH | S_IROTH |
          (fdata.dwFileAttributes & FILE_ATTRIBUTE_READONLY ? 0 : S_IWOTH) |
          (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? S_IFDIR : S_IFREG);
      }
    return 0;
# else /* 1 */
    // This implementation close the handle.
    int retval = -1;
    int fd = ::_open_osfhandle ((long) handle, 0);
    if (fd != -1)
      retval = ::_fstat (fd, stp);

    ::_close (fd);
    // Remember to close the file handle.
    return retval;
# endif /* 1 */
  }

#endif /* WIN32 */

  // This function returns the number of bytes in the file referenced by
  // FD.

  ACE_INLINE ACE_OFF_T
  filesize (ACE_HANDLE handle)
  {
    OS_TRACE ("OS::filesize");
#if defined (ACE_WIN32)
# if defined (_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64
    LARGE_INTEGER size;
    return
      (::GetFileSizeEx (handle, &size)
       ? size.QuadPart
       : (OS::set_errno_to_last_error (), -1));
# else
    DWORD const size = ::GetFileSize (handle, 0);
    return
      (size != INVALID_FILE_SIZE
       ? static_cast<ACE_OFF_T> (size)
       : (OS::set_errno_to_last_error (), -1));
# endif  /* _FILE_OFFSET_BITS == 64 */
#else /* !ACE_WIN32 */
    ACE_stat sb;
    return OS::fstat (handle, &sb) == -1 ?
                    static_cast<ACE_OFF_T> (-1) : sb.st_size;
#endif
  }

  ACE_INLINE ACE_OFF_T
  filesize (const ACE_TCHAR *filename)
  {
    OS_TRACE ("OS::filesize");

    ACE_HANDLE const h = OS::open (filename, O_RDONLY);
    if (h != ACE_INVALID_HANDLE)
      {
        ACE_OFF_T size = OS::filesize (h);
        OS::close (h);
        return size;
      }
    else
      return -1;
  }

  ACE_INLINE int
  lstat (const char *file, ACE_stat *stp)
  {
    OS_TRACE ("OS::lstat");
# if defined (ACE_LACKS_LSTAT)
    return OS::stat (file, stp);
# elif defined (ACE_HAS_X86_STAT_MACROS)
    // Solaris for intel uses an macro for lstat(), this macro is a
    // wrapper for _lxstat().
    OSCALL_RETURN (::_lxstat (_STAT_VER, file, stp), int, -1);
# else /* !ACE_HAS_X86_STAT_MACROS */
    OSCALL_RETURN (::lstat (file, stp), int, -1);
# endif /* ACE_LACKS_LSTAT */
  }

#if defined (ACE_HAS_WCHAR)
  ACE_INLINE int
  lstat (const wchar_t *file, ACE_stat *stp)
  {
    OS_TRACE ("OS::lstat");
# if defined (ACE_LACKS_LSTAT)
    return OS::stat (file, stp);
# else
    return OS::lstat (ACE_Wide_To_Ascii (file).char_rep (), stp);
# endif /* ACE_LACKS_LSTAT */
  }
#endif /* ACE_HAS_WCHAR */

  ACE_INLINE int
  mkdir (const char *path, mode_t mode)
  {
#if defined (ACE_HAS_WINCE)
    ACE_UNUSED_ARG (mode);
    ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::CreateDirectory (ACE_TEXT_CHAR_TO_TCHAR (path), 0),
                                            ace_result_),
                          int, -1);
#elif defined (ACE_MKDIR_LACKS_MODE)
    ACE_UNUSED_ARG (mode);
    OSCALL_RETURN (::mkdir (path), int, -1);
#else
    OSCALL_RETURN (::mkdir (path, mode), int, -1);
#endif
  }

#if defined (ACE_HAS_WCHAR)

  ACE_INLINE int
  mkdir (const wchar_t *path, mode_t mode)
  {
#if defined (ACE_HAS_WINCE)
    ACE_UNUSED_ARG (mode);
    ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (CreateDirectoryW (path, 0),
                                            ace_result_),
                          int, -1);
#elif defined (ACE_WIN32) && defined (ACE_USES_WCHAR)
    ACE_UNUSED_ARG (mode);
    OSCALL_RETURN (::_wmkdir (path), int, -1);
#else
    return OS::mkdir (ACE_Wide_To_Ascii (path).char_rep (), mode);
#endif /* ACE_HAS_WINCE */
  }

#endif /* ACE_HAS_WCHAR */

  ACE_INLINE int
  mkfifo (const ACE_TCHAR *file, mode_t mode)
  {
    OS_TRACE ("OS::mkfifo");
#if defined (ACE_LACKS_MKFIFO)
    ACE_UNUSED_ARG (file);
    ACE_UNUSED_ARG (mode);
    ACE_NOTSUP_RETURN (-1);
#else
    OSCALL_RETURN (::mkfifo (ACE_TEXT_ALWAYS_CHAR (file), mode), int, -1);
#endif /* ACE_LACKS_MKFIFO */
  }

  ACE_INLINE int
  stat (const char *file, ACE_stat *stp)
  {
    OS_TRACE ("OS::stat");
#if defined (ACE_HAS_NONCONST_STAT)
    OSCALL_RETURN (::stat (const_cast <char *> (file), stp), int, -1);
#elif defined (ACE_HAS_WINCE)
    ACE_TEXT_WIN32_FIND_DATA fdata;

    HANDLE fhandle;

    fhandle = ::FindFirstFile (ACE_TEXT_CHAR_TO_TCHAR (file), &fdata);
    if (fhandle == INVALID_HANDLE_VALUE)
      {
        OS::set_errno_to_last_error ();
        return -1;
      }
    else if (fdata.nFileSizeHigh != 0)
      {
        errno = EINVAL;
        return -1;
      }
    else
      {
        stp->st_mode = static_cast<unsigned short>(fdata.dwFileAttributes);
        stp->st_size = fdata.nFileSizeLow;
        stp->st_atime = TimeValue (fdata.ftLastAccessTime);
        stp->st_mtime = TimeValue (fdata.ftLastWriteTime);
      }
    return 0;
#elif defined (ACE_HAS_X86_STAT_MACROS)
    // Solaris for intel uses an macro for stat(), this macro is a
    // wrapper for _xstat().
    OSCALL_RETURN (::_xstat (_STAT_VER, file, stp), int, -1);
#else
    OSCALL_RETURN (ACE_STAT_FUNC_NAME (file, stp), int, -1);
#endif /* ACE_HAS_NONCONST_STAT */
  }

#if defined (ACE_HAS_WCHAR)
  ACE_INLINE int
  stat (const wchar_t *file, ACE_stat *stp)
  {
    OS_TRACE ("OS::stat");
#if defined (ACE_HAS_WINCE)
    WIN32_FIND_DATAW fdata;

    HANDLE fhandle;

    fhandle = ::FindFirstFileW (file, &fdata);
    if (fhandle == INVALID_HANDLE_VALUE)
      {
        OS::set_errno_to_last_error ();
        return -1;
      }
    else if (fdata.nFileSizeHigh != 0)
      {
        errno = EINVAL;
        return -1;
      }
    else
      {
        stp->st_mode = static_cast<unsigned short>(fdata.dwFileAttributes);
        stp->st_size = fdata.nFileSizeLow;
        stp->st_atime = TimeValue (fdata.ftLastAccessTime);
        stp->st_mtime = TimeValue (fdata.ftLastWriteTime);
      }
    return 0;
#elif defined (__BORLANDC__) \
      || (defined (_MSC_VER) && _MSC_VER >= 1300) \
      || defined (__MINGW32__)
    OSCALL_RETURN (ACE_WSTAT_FUNC_NAME (file, stp), int, -1);
#else /* ACE_HAS_WINCE */
    ACE_Wide_To_Ascii nfile (file);
    return OS::stat (nfile.char_rep (), stp);
#endif /* ACE_HAS_WINCE */
  }
#endif /* ACE_HAS_WCHAR */

  ACE_INLINE mode_t
  umask (mode_t cmask)
  {
    OS_TRACE ("OS::umask");
# if defined (ACE_LACKS_UMASK)
    ACE_UNUSED_ARG (cmask);
    ACE_NOTSUP_RETURN ((mode_t)-1);
# elif defined (ACE_HAS_TR24731_2005_CRT)
    mode_t old_mode;
    ACE_SECURECRTCALL (_umask_s (cmask, &old_mode), mode_t, -1, old_mode);
    return old_mode;
# elif defined (ACE_WIN32) && !defined (__BORLANDC__)
    OSCALL_RETURN (::_umask (cmask), mode_t, -1);
# else
    return ::umask (cmask); // This call shouldn't fail...
# endif /* ACE_LACKS_UMASK */
  }

} // OS namespace

