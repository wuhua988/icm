// Pipe.cpp,v 4.32 2001/03/05 19:31:27 coryan Exp

#include "icc/Pipe.h"
#include "icc/SocketAcceptor.h"
#include "icc/SocketConnector.h"
#include "icc/Log.h"


ACE_Pipe::~ACE_Pipe (void)
{
  ACE_TRACE ("ACE_Pipe::~ACE_Pipe");
  // Notice that the destructor doesn't close the handles for you.
}

ACE_HANDLE
ACE_Pipe::read_handle (void) const
{
  ACE_TRACE ("ACE_Pipe::read_handle");
  return this->handles_[0];
}

ACE_HANDLE
ACE_Pipe::write_handle (void) const
{
  ACE_TRACE ("ACE_Pipe::write_handle");
  return this->handles_[1];
}

void
ACE_Pipe::dump (void) const
{
  ACE_TRACE ("ACE_Pipe::dump");
  ACE_DEBUG ((LM_DEBUG, ACE_BEGIN_DUMP, this));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("handles_[0] = %d"), this->handles_[0]));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nhandles_[1] = %d"), this->handles_[1]));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\n")));
  ACE_DEBUG ((LM_DEBUG, ACE_END_DUMP));
}

int
ACE_Pipe::open (int buffer_size)
{
  ACE_TRACE ("ACE_Pipe::open");

#if defined (ACE_WIN32) || defined (ACE_LACKS_SOCKETPAIR) || defined (__Lynx__)
  InetAddr my_addr;
  SocketAcceptor acceptor;
  SocketConnector connector;
  SocketStream reader;
  SocketStream writer;
  int result = 0;

  // Bind listener to any port and then find out what the port was.
  if (acceptor.open (Addr::sapAny) == -1
      || acceptor.getLocalAddr (my_addr) == -1)
    result = -1;
  else
    {
      InetAddr sv_addr (my_addr.getPortNumber (),
                             ACE_LOCALHOST);

      // Establish a connection within the same process.
      if (connector.connect (writer, sv_addr) == -1)
        result = -1;
      else if (acceptor.accept (reader) == -1)
        {
          writer.close ();
          result = -1;
        }
    }

  // Close down the acceptor endpoint since we don't need it anymore.
  acceptor.close ();
  if (result == -1)
    return -1;

  this->handles_[0] = reader.getHandle ();
  this->handles_[1] = writer.getHandle ();

# if !defined (ACE_LACKS_TCP_NODELAY)
  int one = 1;

  // Make sure that the TCP stack doesn't try to buffer small writes.
  // Since this communication is purely local to the host it doesn't
  // affect network performance.

  if (writer.setOption (ACE_IPPROTO_TCP,
                         TCP_NODELAY,
                         &one,
                         sizeof one) == -1)
    {
      this->close ();
      return -1;
    }
# endif /* ! ACE_LACKS_TCP_NODELAY */

# if defined (ACE_LACKS_SOCKET_BUFSIZ)
    ACE_UNUSED_ARG (buffer_size);
# else  /* ! ACE_LACKS_SOCKET_BUFSIZ */
  if (reader.setOption (SOL_SOCKET,
                         SO_SNDBUF,
                         ACE_reinterpret_cast (void *, &buffer_size),
                         sizeof (buffer_size)) == -1
      && errno != ENOTSUP)
    {
      this->close ();
      return -1;
    }
  else if (writer.setOption (SOL_SOCKET,
                              SO_RCVBUF,
                              ACE_reinterpret_cast (void *, &buffer_size),
                              sizeof (buffer_size)) == -1
           && errno != ENOTSUP)
    {
      this->close ();
      return -1;
    }
# endif /* ! ACE_LACKS_SOCKET_BUFSIZ */

#elif defined (ACE_HAS_STREAM_PIPES) || defined(__QNX__)
  ACE_UNUSED_ARG (buffer_size);
  if (OS::pipe (this->handles_) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("%p\n"),
                       ACE_LIB_TEXT ("pipe")),
                      -1);

#if !defined(__QNX__)
  int arg = RMSGN;

  // Enable "msg no discard" mode, which ensures that record
  // boundaries are maintained when messages are sent and received.
  if (OS::ioctl (this->handles_[0],
                     I_SRDOPT,
                     (void *) arg) == -1
      || OS::ioctl (this->handles_[1],
                        I_SRDOPT,
                        (void *) arg) == -1)
    {
      this->close ();
      ACE_ERROR_RETURN ((LM_ERROR,
                         ACE_LIB_TEXT ("%p\n"),
                         ACE_LIB_TEXT ("ioctl")), -1);
    }
#endif /* __QNX__ */

#else  /* ! ACE_WIN32 && ! ACE_LACKS_SOCKETPAIR && ! ACE_HAS_STREAM_PIPES */
  if (OS::socketpair (AF_UNIX,
                          SOCK_STREAM,
                          0,
                          this->handles_) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("%p\n"),
                       ACE_LIB_TEXT ("socketpair")),
                      -1);
# if defined (ACE_LACKS_SOCKET_BUFSIZ)
  ACE_UNUSED_ARG (buffer_size);
# else  /* ! ACE_LACKS_SOCKET_BUFSIZ */
  if (OS::setsockopt (this->handles_[0],
                          SOL_SOCKET,
                          SO_RCVBUF,
                          ACE_reinterpret_cast (const char *,
                                                &buffer_size),
                          sizeof (buffer_size)) == -1
      && errno != ENOTSUP)
    {
      this->close ();
      return -1;
    }
  if (OS::setsockopt (this->handles_[1],
                          SOL_SOCKET,
                          SO_SNDBUF,
                          ACE_reinterpret_cast (const char *, &buffer_size),
                          sizeof (buffer_size)) == -1
      && errno != ENOTSUP)
    {
      this->close ();
      return -1;
    }
# endif /* ! ACE_LACKS_SOCKET_BUFSIZ */
#endif  /* ! ACE_WIN32 && ! ACE_LACKS_SOCKETPAIR && ! ACE_HAS_STREAM_PIPES */
  // Point both the read and write HANDLES to the appropriate socket
  // HANDLEs.

  return 0;
}

int
ACE_Pipe::open (ACE_HANDLE handles[2])
{
  ACE_TRACE ("ACE_Pipe::open");

  if (this->open () == -1)
    return -1;
  else
    {
      handles[0] = this->handles_[0];
      handles[1] = this->handles_[1];
      return 0;
    }
}

// Do nothing...

ACE_Pipe::ACE_Pipe (void)
{
  ACE_TRACE ("ACE_Pipe::ACE_Pipe");

  this->handles_[0] = ACE_INVALID_HANDLE;
  this->handles_[1] = ACE_INVALID_HANDLE;
}

ACE_Pipe::ACE_Pipe (ACE_HANDLE handles[2])
{
  ACE_TRACE ("ACE_Pipe::ACE_Pipe");

  if (this->open (handles) == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("ACE_Pipe::ACE_Pipe")));
}

ACE_Pipe::ACE_Pipe (ACE_HANDLE read,
                    ACE_HANDLE write)
{
  ACE_TRACE ("ACE_Pipe::ACE_Pipe");
  this->handles_[0] = read;
  this->handles_[1] = write;
}

int
ACE_Pipe::close (void)
{
  ACE_TRACE ("ACE_Pipe::close");

  int result = 0;

  // Note that the following will work even if we aren't closing down
  // sockets because <OS::closesocket> will just call <::close> in
  // that case!

  if (this->handles_[0] != ACE_INVALID_HANDLE)
    result = OS::closesocket (this->handles_[0]);
  this->handles_[0] = ACE_INVALID_HANDLE;

  if (this->handles_[1] != ACE_INVALID_HANDLE)
    result |= OS::closesocket (this->handles_[1]);
  this->handles_[1] = ACE_INVALID_HANDLE;

  return result;
}
