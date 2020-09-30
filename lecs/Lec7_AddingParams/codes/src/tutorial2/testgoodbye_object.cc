#include "tutorial2/testgoodbye_object.hh"
#include "debug/TestHello.hh"
#include "sim/sim_exit.hh"

TestGoodbyeObject::TestGoodbyeObject(TestGoodbyeObjectParams* params):
    SimObject(params), event(*this), bandwidth(params->write_bandwidth), bufferSize(params->buffer_size),
    buffer(nullptr), bufferUsed(0)
{
    buffer = new char[bufferSize];
    DPRINTF(TestHello, "Create the testgoodbye object\n");
}

TestGoodbyeObject::~TestGoodbyeObject() 
{
    delete[] buffer;
}

void TestGoodbyeObject::processEvent()
{
    DPRINTF(TestHello, "Processing the event!\n");
    fillBuffer();
}

void TestGoodbyeObject::sayGoodbye(std::string other_name)
{
    DPRINTF(TestHello, "Saying goodbye to %s\n", other_name);

    message = "Goodbye " + other_name + "!! ";
    fillBuffer();
}

void TestGoodbyeObject::fillBuffer()
{
    // There better be a message
    assert(message.length() > 0);

    // Copy from the message to the buffer per byte
    int bytes_copied = 0;

    for(auto it = message.begin();
        it < message.end() && bufferUsed < bufferSize - 1;
        it++, bufferUsed++, bytes_copied++) {
            // copy the character into the buffer
            buffer[bufferUsed] = *it;
    }

    if(bufferUsed < bufferSize - 1) {
        // wait for the next copy for as long as it would have taken
        DPRINTF(TestHello, "Scheduling another fillBuffer in %d ticks\n", bandwidth * bytes_copied);
        schedule(event, curTick() + bandwidth * bytes_copied);
    } else {
        DPRINTF(TestHello, "Goodbye done copying!\n");
        exitSimLoop(buffer, 0, curTick() + bandwidth * bytes_copied);
    }

}

TestGoodbyeObject* TestGoodbyeObjectParams::create() {
    return new TestGoodbyeObject(this);
}

