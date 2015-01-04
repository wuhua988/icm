// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_poll.h
 *
 *  definitions for the poll() function
 *
 *  $Id: os_poll.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_OS_POLL_H
#define OS_INCLUDE_OS_POLL_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if !defined (ACE_LACKS_POLL_H)
# include /**/ <poll.h>
#endif /* !ACE_LACKS_POLL_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_OS_POLL_H */
