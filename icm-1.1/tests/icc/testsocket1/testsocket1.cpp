#include "os/OS.h"
#include "icc/Reactor.h"
#include "icc/Acceptor.h"
#include "icc/SelectReactor.h"
#include "icc/SocketAcceptor.h"
#include "icc/InetAddr.h"
#include "icc/SocketStream.h"
#include "icc/SvcHandler.h"
#include "icc/Synch.h"
#include "icc/Log.h"

#include <iostream>
#include <string>
using namespace std;

class MyService;
typedef Acceptor<MyService> ServerAcceptor;

class MyService : public SvcHandler
{
  typedef SvcHandler super;
public:
  int open (void * )
  {
    if (super::open (0) == -1)
      return -1;

    InetAddr peerAddr;
    if (this->getStream ().getRemoteAddr (peerAddr) == 0) {
      std::string strPeer = peerAddr.toString ();
      cout << "connection from " << strPeer << endl;
    }

    return 0;
  }

  int close (void *)
  {
    return 0;
  }

  virtual int handleInput (ACE_HANDLE fd )
  {
    const size_t inputSize = 4096;
    char buffer[inputSize];
    memset (buffer, 0, inputSize);
    ssize_t recvCnt, sendCnt;

    recvCnt = this->getStream().recv (buffer, sizeof(buffer), 0);
    if (recvCnt <= 0) {
      cout << "socket will close" << endl;
      return -1;
    } else {
      buffer[recvCnt] = 0;
      cout << buffer << endl;
    }

    sendCnt = this->getStream ().send (buffer, recvCnt, 0);
    if( sendCnt == -1)
      return -1;

    return 0;
  }

  virtual int handleClose (ACE_HANDLE h,ReactorMask mask)
  {
    return super::handleClose (h, mask);
  }

};



int main (int, char* [])
{
  OS::socket_init (ACE_WSOCK_VERSION);

  Log::instance()->open ("testsocket1.txt");

  InetAddr listenAddr (10002);

  SelectReactor selectReactor;
  Reactor reactor(&selectReactor);
  Reactor::instance(&reactor);

  ServerAcceptor acceptor;
  if (acceptor.open (listenAddr) == -1) {
    LogMsg << "acceptor:" << perr << endl;
    return -1;
  }

  Reactor::instance ()->runReactorEventLoop ();

  return 0;
}

