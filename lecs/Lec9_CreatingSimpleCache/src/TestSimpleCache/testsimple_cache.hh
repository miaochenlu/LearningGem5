#ifndef __TESTSIMPLECACHE_HH__
#define __TESTSIMPLECACHE_HH__

#include <unordered_map>

#include "base/statistics.hh"
#include "mem/port.hh"
#include "params/TestSimpleCache.hh"
#include "sim/clocked_object.hh"

/**
 * A very simple cache object. Has a fully-associative data store with random
 * replacement.
 * This cache is fully blocking (not non-blocking). Only a single request can
 * be outstanding at a time.
 * This cache is a writeback cache.
 */
class TestSimpleCache : public ClockedObject
{
  private:
    class CPUSidePort : public SlavePort
    {
      private:
        /// Since this is a vector port, need to know what number this one is
        int id;

        /// The object that owns this object (TestSimpleCache)
        TestSimpleCache *owner;

        /// True if the port needs to send a retry req.
        bool needRetry;

        /// If we tried to send a packet and it was blocked, store it here
        PacketPtr blockedPacket;

      public:
        CPUSidePort(const std::string& name, int id, TestSimpleCache *owner) :
            SlavePort(name, owner), id(id), owner(owner), needRetry(false),
            blockedPacket(nullptr)
        { }

        void sendPacket(PacketPtr pkt);
        AddrRangeList getAddrRanges() const override;
        void trySendRetry();

      protected:
        Tick recvAtomic(PacketPtr pkt) override
        { panic("recvAtomic unimpl."); }
        void recvFunctional(PacketPtr pkt) override;
        bool recvTimingReq(PacketPtr pkt) override;
        void recvRespRetry() override;
    };

    class MemSidePort : public MasterPort
    {
      private:
        /// The object that owns this object (TestSimpleCache)
        TestSimpleCache *owner;

        /// If we tried to send a packet and it was blocked, store it here
        PacketPtr blockedPacket;

      public:
        MemSidePort(const std::string& name, TestSimpleCache *owner) :
            MasterPort(name, owner), owner(owner), blockedPacket(nullptr)
        { }

        void sendPacket(PacketPtr pkt);

      protected:
        bool recvTimingResp(PacketPtr pkt) override;
        void recvReqRetry() override;
        void recvRangeChange() override;
    };

    /**
     * Handle the request from the CPU side. Called from the CPU port
     * on a timing request.
     *
     * @param requesting packet
     * @param id of the port to send the response
     * @return true if we can handle the request this cycle, false if the
     *         requestor needs to retry later
     */
    bool handleRequest(PacketPtr pkt, int port_id);

    /**
     * Handle the respone from the memory side. Called from the memory port
     * on a timing response.
     *
     * @param responding packet
     * @return true if we can handle the response this cycle, false if the
     *         responder needs to retry later
     */
    bool handleResponse(PacketPtr pkt);

    /**
     * Send the packet to the CPU side.
     * This function assumes the pkt is already a response packet and forwards
     * it to the correct port. This function also unblocks this object and
     * cleans up the whole request.
     *
     * @param the packet to send to the cpu side
     */
    void sendResponse(PacketPtr pkt);
    void handleFunctional(PacketPtr pkt);

    /**
     * Access the cache for a timing access. This is called after the cache
     * access latency has already elapsed.
     */
    void accessTiming(PacketPtr pkt);

    /**
     * This is where we actually update / read from the cache. This function
     * is executed on both timing and functional accesses.
     *
     * @return true if a hit, false otherwise
     */
    bool accessFunctional(PacketPtr pkt);

    /**
     * Insert a block into the cache. If there is no room left in the cache,
     * then this function evicts a random entry t make room for the new block.
     *
     * @param packet with the data (and address) to insert into the cache
     */
    void insert(PacketPtr pkt);

    AddrRangeList getAddrRanges() const;
    void sendRangeChange() const;

    /// Latency to check the cache. Number of cycles for both hit and miss
    const Cycles latency;

    /// The block size for the cache
    const unsigned blockSize;

    /// Number of blocks in the cache (size of cache / block size)
    const unsigned capacity;

    /// Instantiation of the CPU-side port
    std::vector<CPUSidePort> cpuPorts;

    /// Instantiation of the memory-side port
    MemSidePort memPort;

    /// True if this cache is currently blocked waiting for a response.
    bool blocked;

    /// Packet that we are currently handling. Used for upgrading to larger
    /// cache line sizes
    PacketPtr originalPacket;

    /// The port to send the response when we recieve it back
    int waitingPortId;

    /// For tracking the miss latency
    Tick missTime;

    /// An incredibly simple cache storage. Maps block addresses to data
    std::unordered_map<Addr, uint8_t*> cacheStore;

    /// Cache statistics
    Stats::Scalar hits;
    Stats::Scalar misses;
    Stats::Histogram missLatency;
    Stats::Formula hitRatio;

  public:
    TestSimpleCache(TestSimpleCacheParams *params);
    Port &getPort(const std::string &if_name,
                  PortID idx=InvalidPortID) override;

    /**
     * Register the stats
     */
    void regStats() override;
};


#endif