# import the m5 (gem5) library created when gem5 is built
import m5
# import all of the SimObjects
from m5.objects import *
# set up the root SimObject and start the simulation
root = Root(full_system = False)
# Create an instantiation of the simobject you created
root.hello = TestHelloObject(time_to_wait='2us', number_of_fires=5)
root.hello.testgoodbye_object = TestGoodbyeObject(buffer_size='100B')
# instantiate all of the objects we've created above
m5.instantiate()
print("Beginning simulation!")
exit_event = m5.simulate()
print('Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause()))