#include "tutorial2/TestSimpleMemobj/testsimple_memobj.hh"
#include "debug/TestSimpleMemobj.hh"

TestSimpleMemobj::TestSimpleMemobj(TestSimpleMemobjParams* params):
    SimObject(params),
    instPort(params->name + ".inst_port", this),
    dataPort(params->name + ".data_port", this),
    memPort(params->name + ".mem_port", this),
    blocked(false)
{}


Port &
TestSimpleMemobj::getPort(const std::string &if_name, PortID idx)
{
    panic_if(idx != InvalidPortID, "This object doesn't support vector ports");

    // This is the name from the Python SimObject declaration (SimpleMemobj.py)
    if (if_name == "mem_side") {
        return memPort;
    } else if (if_name == "inst_port") {
        return instPort;
    } else if (if_name == "data_port") {
        return dataPort;
    } else {
        // pass it along to our super class
        return SimObject::getPort(if_name, idx);
    }
}

AddrRangeList
TestSimpleMemobj::CPUSidePort::getAddrRanges() const {
    return owner->getAddrRanges();
}

void 
TestSimpleMemobj::CPUSidePort::recvFunctional(PacketPtr pkt) {
    return owner->handleFunctional(pkt);
}


void
TestSimpleMemobj::handleFunctional(PacketPtr pkt) {
    memPort.sendFunctional(pkt);
}

AddrRangeList
TestSimpleMemobj::getAddrRanges() const {
    DPRINTF(TestSimpleMemobj, "Sending new ranges\n");
    return memPort.getAddrRanges();
}

void
TestSimpleMemobj::MemSidePort::recvRangeChange() {
    owner->sendRangeChange();
}

void
TestSimpleMemobj::sendRangeChange() {
    instPort.sendRangeChange();
    dataPort.sendRangeChange();
}


bool
TestSimpleMemobj::CPUSidePort::recvTimingReq(PacketPtr pkt) {
    if(!owner->handleRequest(pkt)) {
        needRetry = true;
        return false;
    } else {
        return true;
    }
}

bool
TestSimpleMemobj::handleRequest(PacketPtr pkt) {
    if(blocked) {
        return false;
    }

    DPRINTF(TestSimpleMemobj, "Got request for addrr %#x\n", pkt->getAddr());
    blocked = true;
    memPort.sendPacket(pkt);
    return true;
}

void
TestSimpleMemobj::MemSidePort::sendPacket(PacketPtr pkt) {
    panic_if(blockedPacket != nullptr, "Should never try to send if blocked!");

    if(!sendTimingReq(pkt)) {
        blockedPacket = pkt;
    }
}


void
TestSimpleMemobj::MemSidePort::recvReqRetry() {
    assert(blockedPacket != nullptr);

    PacketPtr pkt = blockedPacket;
    blockedPacket = nullptr;

    sendPacket(pkt);
}


bool
TestSimpleMemobj::MemSidePort::recvTimingResp(PacketPtr pkt) {
    return owner->handleResponse(pkt);
}

bool
TestSimpleMemobj::handleResponse(PacketPtr pkt) {
    assert(blocked);
    DPRINTF(TestSimpleMemobj, "Got response for addr %#x\n", pkt->getAddr());
    blocked = false;

    if(pkt->req->isInstFetch()) {
        instPort.sendPacket(pkt);
    } else {
        dataPort.sendPacket(pkt);
    }
    instPort.trySendRetry();
    dataPort.trySendRetry();

    return true;
}

void
TestSimpleMemobj::CPUSidePort::sendPacket(PacketPtr pkt) {
    panic_if(blockedPacket != nullptr, "Should never try to send if blocked!");

    if(!sendTimingResp(pkt)) {
        blockedPacket = pkt;
    }
}

void
TestSimpleMemobj::CPUSidePort::recvRespRetry() {
    assert(blockedPacket != nullptr);

    PacketPtr pkt = blockedPacket;
    blockedPacket = nullptr;
    sendPacket(pkt);
}

void
TestSimpleMemobj::CPUSidePort::trySendRetry() {
    if(needRetry && blockedPacket == nullptr) {
        needRetry = false;
        DPRINTF(TestSimpleMemobj, "Sending retry req for %d\n", id);
        sendRetryReq();
    }
}

TestSimpleMemobj* 
TestSimpleMemobjParams::create() {
    return new TestSimpleMemobj(this);
}