from m5.params import *
from m5.SimObject import SimObject

class TestHelloObject(SimObject):
    type = 'TestHelloObject'
    cxx_header = "tutorial2/testhello_object.hh"

    time_to_wait = Param.Latency("Time before firing the event")
    number_of_fires = Param.Int(1, "Number fo times to fire the event before "
                                   "goodbye")

    testgoodbye_object = Param.TestGoodbyeObject("A testgoodbye object")

class TestGoodbyeObject(SimObject):
    type = 'TestGoodbyeObject'
    cxx_header = "tutorial2/testgoodbye_object.hh"

    buffer_size = Param.MemorySize('1kB',
                                   "Size of buffer to fill with goodbye")
    write_bandwidth = Param.MemoryBandwidth('100MB/s', "Bandwidth to fill "
                                            "the buffer")