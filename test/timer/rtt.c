#include "test/testcase.h"
#include "timer/rtt.h"

GUIC_TEST(rtt, init)
{
    struct rtt_status rtt;
    rtt_status_init(&rtt);

    ASSERT(0, rtt.srtt, ==);
    ASSERT(24, rtt.rttvar, ==);
    ASSERT(12, rtt.rto, ==);
}

GUIC_TEST(rtt, syn_ack)
{
    struct rtt_status rtt;
    rtt_status_init(&rtt);
    
    rtt_status_update(&rtt, 2);

    ASSERT(16, rtt.srtt, ==);
    ASSERT(4, rtt.rttvar, ==);
    ASSERT(6, rtt.rto, ==);
}

GUIC_TEST(rtt, ack1)
{
    struct rtt_status rtt;
    rtt_status_init(&rtt);
    
    rtt_status_update(&rtt, 2);
    rtt_status_update(&rtt, 2);

    ASSERT(15, rtt.srtt, ==);
    ASSERT(4, rtt.rttvar, ==);
    ASSERT(5, rtt.rto, ==);
}

GUIC_TEST(rtt, ack2)
{

    struct rtt_status rtt;
    rtt_status_init(&rtt);
    
    rtt_status_update(&rtt, 2);
    rtt_status_update(&rtt, 2);
    rtt_status_update(&rtt, 3);

    ASSERT(16, rtt.srtt, ==);
    ASSERT(4, rtt.rttvar, ==);
    ASSERT(6, rtt.rto, ==);
}

GUIC_TEST(rtt, ack3)
{

    struct rtt_status rtt;
    rtt_status_init(&rtt);
    
    rtt_status_update(&rtt, 2);
    rtt_status_update(&rtt, 2);
    rtt_status_update(&rtt, 3);
    rtt_status_update(&rtt, 3);

    ASSERT(16, rtt.srtt, ==);
    ASSERT(3, rtt.rttvar, ==);
    ASSERT(5, rtt.rto, ==);
}
