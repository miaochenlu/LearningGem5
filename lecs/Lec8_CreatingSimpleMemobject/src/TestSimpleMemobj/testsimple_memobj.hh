#ifndef __TESTSIMPLEMEMOBJ__
#define __TESTSIMPLEMEMOBJ__
#include "mem/port.hh"
#include "params/TestSimpleMemobj.hh"
#include "sim/sim_object.hh"

class TestSimpleMemobj: public SimObject 
{
private:
    class CPUSidePort: public SlavePort
    {
    private:
        TestSimpleMemobj* owner;

        bool needRetry;
        PacketPtr blockedPacket;

    public:
        CPUSidePort(const std::string& name, TestSimpleMemobj* owner):
            SlavePort(name, owner), owner(owner),
            needRetry(false), blockedPacket(nullptr)
        {}

        void sendPacket(PacketPtr pkt);

        AddrRangeList getAddrRanges() const override;

        void trySendRetry();
    protected:
        Tick recvAtomic(PacketPtr pkt) override { panic("recvAtomic unimpl."); }
        void recvFunctional(PacketPtr pkt) override;
        bool recvTimingReq(PacketPtr pkt) override;
        void recvRespRetry() override;
    };

    class MemSidePort: public MasterPort
    {
    private:
        TestSimpleMemobj* owner;

        PacketPtr blockedPacket;
    public:
        MemSidePort(const std::string& name, TestSimpleMemobj* owner):
            MasterPort(name, owner), owner(owner)
        {}

        void sendPacket(PacketPtr pkt);

    protected:
        bool recvTimingResp(PacketPtr pkt) override;
        void recvReqRetry() override;
        void recvRangeChange() override;
    };
    /**
     * Handle the request from the CPU side
     *
     * @param requesting packet
     * @return true if we can handle the request this cycle, false if the
     *         requestor needs to retry later
     */
    bool handleRequest(PacketPtr pkt);

    /**
     * Handle the response from the memory side
     *
     * @param responding packet
     * @return true if we can handle the response this cycle, false if the
     *         responder needs to retry later
     */
    bool handleResponse(PacketPtr pkt);

    /**
     * Handle a packet functionally. Update the data on a write and get the
     * data on a read.
     *
     * @param packet to functionally handle
     */
    void handleFunctional(PacketPtr pkt);

    /**
     * Return the address ranges this memobj is responsible for. Just use the
     * same as the next upper level of the hierarchy.
     *
     * @return the address ranges this memobj is responsible for
     */
    AddrRangeList getAddrRanges() const;

    /**
     * Tell the CPU side to ask for our memory ranges.
     */
    void sendRangeChange();

    /// Instantiation of the CPU-side ports
    CPUSidePort instPort;
    CPUSidePort dataPort;

    /// Instantiation of the memory-side port
    MemSidePort memPort;

    /// True if this is currently blocked waiting for a response.
    bool blocked;


public:
    /**
     * constructor
     */
    TestSimpleMemobj(TestSimpleMemobjParams* params); 
    
    /**
     * Get a port with a given name and index. This is used at
     * binding time and returns a reference to a protocol-agnostic
     * port.
     *
     * @param if_name Port name
     * @param idx Index in the case of a VectorPort
     *
     * @return A reference to the given port
     */
    Port &getPort(const std::string &if_name,
                  PortID idx=InvalidPortID) override;
};


#endif