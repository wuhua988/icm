#include "os/OS.h"
#include "icc/SynchTraits.h"
#include "icc/Task.h"
#include "icc/ThreadManager.h"

#include <iostream>
#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "msg/IcmMsg.h"
#include "EventI.h"

using namespace std;
using namespace IcmMsg;

class MyTask1: public Task<MT_SYNCH> {
public:
  virtual int svc(void) {
    ObjectAdapter* adapter = Communicator::instance()->createObjectAdapterWithEndpoint("Subscriber", "127.0.0.1 9999");
    IcmProxy::Object* networkProxy = adapter->add(new NetworkI(), "NetworkTopic");

    ::IcmProxy::IcmMsg::Topic* topic = topicManager->retrieve("NetworkTopic");
    if (topic == 0)
      topic = topicManager->create("NetworkTopic");
    if (topic == 0)
      return -1;
    topic->subscribe(networkProxy);

    Communicator::instance()->run();

    return 0;
  }

  IcmProxy::IcmMsg::TopicManager* topicManager;
};

class MyTask2: public Task<MT_SYNCH> {
public:

  virtual int svc(void) {
    ::IcmProxy::IcmMsg::Topic* topic = topicManager->retrieve("AlarmTopic");
    if (topic == 0)
      topic = topicManager->create("AlarmTopic");
    if (topic == 0) {
      cout << "err create topic " << endl;
      return -1;
    }

    ::IcmProxy::Object* pubObj = topic->getPublisher();
    if (pubObj == 0) {
      cout << "err get publisher " << endl;
      return -1;
    }
    IcmProxy::demo::Alarm alarm;
    alarm.setReference(pubObj->getReference());

    cout << "publishing alarm events:" << endl;
    demo::AlarmEvent event;

    event.id = "alarm908333";
    event.image = "image00001";

    for (int i = 0; i < 10; i++) {
      ostringstream oss;
      oss << "evt:" << i;
      event.mds = i;
      alarm.reportAlarm(event);
    }

  }

  IcmProxy::IcmMsg::TopicManager* topicManager;
};

int main(int argc, char* argv[]) {
  Communicator* comm = Communicator::instance();
  if (comm->init(true) == -1)
    return -1;

  Reference ref(comm, Identity("TopicManager"), Endpoint("TCP", "127.0.0.1", 5555));
  IcmProxy::IcmMsg::TopicManager topicManager;
  topicManager.setReference(&ref);

  MyTask1 task1;
  task1.topicManager = &topicManager;
  task1.activate();

  MyTask2 task2;
  task2.topicManager = &topicManager;
  task2.activate();

  ThreadManager::instance ()->wait ();

  return 0;
}
