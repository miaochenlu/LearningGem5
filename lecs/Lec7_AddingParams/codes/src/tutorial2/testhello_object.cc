#include "tutorial2/testhello_object.hh"
#include "tutorial2/testgoodbye_object.hh"
#include "debug/TestHello.hh"

TestHelloObject::TestHelloObject(TestHelloObjectParams *params) :
    SimObject(params), event(*this),
    testgoodbye(*(params->testgoodbye_object)),
    myName(params->name),
    latency(params->time_to_wait),
    timesLeft(params->number_of_fires)
{
    DPRINTF(TestHello, "Created the test hello object with the name %s\n", myName);
}


TestHelloObject*
TestHelloObjectParams::create()
{
    return new TestHelloObject(this);
}


void TestHelloObject::processEvent()
{
    timesLeft--;
    DPRINTF(TestHello, "Test Hello World! Processing the event! %d left\n", timesLeft);

    if(timesLeft <= 0) {
        DPRINTF(TestHello, "Done firing!\n");
        testgoodbye.sayGoodbye(myName);
    } else {
        schedule(event, curTick() + latency);
    }
}

void TestHelloObject::startup() 
{
    schedule(event, latency);
}