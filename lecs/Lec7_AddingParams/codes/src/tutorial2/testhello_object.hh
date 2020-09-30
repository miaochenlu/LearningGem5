#ifndef __LEARNING_GEM5_TEST_HELLO_OBJECT_HH__
#define __LEARNING_GEM5_TEST_HELLO_OBJECT_HH__

#include "tutorial2/testgoodbye_object.hh"
#include "params/TestHelloObject.hh"
#include "sim/sim_object.hh"

class TestHelloObject : public SimObject
{
  private:
    void processEvent();
    EventWrapper<TestHelloObject, &TestHelloObject::processEvent> event;
    TestGoodbyeObject& testgoodbye;
    std::string myName;
    Tick latency;
    int timesLeft;
  public:
    TestHelloObject(TestHelloObjectParams *p);
    void startup();
};

#endif // __LEARNING_GEM5_TEST_HELLO_OBJECT_HH__