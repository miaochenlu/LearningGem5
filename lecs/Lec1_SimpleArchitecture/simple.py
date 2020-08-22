import m5
from m5.objects import *
# instantiate a system
# 'System' is a Python class wrapper for the System C++ SimObject

system = System()

# Initialize a clock and voltage domain
# 'clk_domain' is a *parameter* of the SimObject
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
# gem5 is start enough automatically convert units
system.clk_domain.voltage_domain = VoltageDomain()
# Let's set up the memory system
system.mem_mode = 'timing'
# You want to use *timing* for simulations, the options are things 'atomic' 'functional'(debug)

# All systems need memory!
system.mem_ranges = [AddrRange('512MB')]

# Let's create a CPU
system.cpu = TimingSimpleCPU()

# Now, we need a memory bus
system.membus = SystemXBar()

# Hook up CPU
system.cpu.icache_port = system.membus.slave
system.cpu.dcache_port = system.membus.slave

# Now, some BS to get things right
system.cpu.createInterruptController()
system.cpu.interrupts[0].pio = system.membus.master
system.cpu.interrupts[0].int_master = system.membus.slave
system.cpu.interrupts[0].int_slave = system.membus.master

system.system_port = system.membus.slave

# Finally, let's make the memory controller
system.mem_ctrl = DDR3_1600_8x8()

# Set up physical memory ranges
system.mem_ctrl.range = system.mem_ranges[0]

# Connect memory controller to bus
system.mem_ctrl.port = system.membus.master

# Now tell the system what we want it to do
# Build scons build/X86/gem5.opt -j5
process = Process()
process.cmd = ['tests/test-progs/hello/bin/x86/linux/hello']
system.cpu.workload = process
system.cpu.createThreads()

# Now, we're almost done!
# Create a root object
root = Root(full_system = False, system = system)

# Instantiate all of the C++
m5.instantiate()

# We're ready to run!
print("Beginning simulation!")
exit_event = m5.simulate()

print('Exiting @ tick {} because {}'
      .format(m5.curTick(), exit_event.getCause()))

