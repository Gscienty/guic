#include "congestion/cubic.h"
#include "test/testcase.h"
#include <math.h>

GUIC_TEST(cubic, recv_first_ack)
{
    struct cubic cubic;
    bytecount_t mss = 1000;
    bytecount_t cwnd = 10 * mss;
    bytecount_t next_cwnd;
    bytecount_t expect_cwnd = cwnd
        + 3 * CUBIC_FACTOR_BETA / (2 - CUBIC_FACTOR_BETA) * mss * mss / cwnd;
    cubic_init(&cubic);

    next_cwnd = cubic_received_ack(&cubic, mss, cwnd, mss, 10 * 1000 + 1, 10 * 1000);
    ASSERT(expect_cwnd, next_cwnd, ==);
}

GUIC_TEST(cubic, recv_ack)
{
    struct cubic cubic;
    bytecount_t mss = 1000;
    bytecount_t cwnd = 10 * mss;
    microtime_t rttmin = 100 * 1000;
    microtime_t now = 1000;
    bytecount_t expect = cwnd
        + 3 * CUBIC_FACTOR_BETA / (2 - CUBIC_FACTOR_BETA) * mss * mss / cwnd;
    int i;

    cubic_init(&cubic);

    cwnd = cubic_received_ack(&cubic, mss, cwnd, mss, now, rttmin);
    ASSERT(expect, cwnd, ==);

    for (i = 0; i < 5; i++) {
        expect = cwnd
            + 3 * CUBIC_FACTOR_BETA / (2 - CUBIC_FACTOR_BETA) * mss * mss / cwnd;
        cwnd = cubic_received_ack(&cubic, mss, cwnd, mss, now, rttmin);
        ASSERT(expect, cwnd, ==);
    }

}

GUIC_TEST(cubic, recv_ack2)
{
    bytecount_t mss = 1000;
    bytecount_t cwnd = 1000 * mss;
    bytecount_t init_cwnd = cwnd;
    bytecount_t prev_cwnd;
    microtime_t minrtt = 100 * 1000;
    microtime_t now = 1000;
    microtime_t init_now = now;
    struct cubic cubic;
    int i;
    double off;
    bytecount_t expect_cwnd;

    cubic_init(&cubic);

    cwnd = cubic_received_ack(&cubic, mss, cwnd, mss, now, minrtt);
    now += 600 * 1000;
    cwnd = cubic_received_ack(&cubic, mss, cwnd, mss, now, minrtt);

    for (i = 0; i < 100; i++) {
        now += 10 * 1000;
        off = (now - init_now + minrtt) / DOUBLE_MICROSECONDS;
        expect_cwnd = init_cwnd + 4 * pow(off, 3.0) * mss;
        prev_cwnd = cwnd;
        cwnd = cubic_received_ack(&cubic, mss, cwnd, mss, now, minrtt);

        ASSERT(expect_cwnd, cwnd, ==);
        ASSERT(prev_cwnd, cwnd, <);
    }
}

GUIC_TEST(cubic, recv_ack3)
{
    bytecount_t mss = 1000;
    bytecount_t init_cwnd = 150 * mss;
    bytecount_t cwnd = init_cwnd;
    bytecount_t prev_cwnd;
    microtime_t rttmin = 350 * 1000;
    microtime_t now = 1000;
    struct cubic cubic;
    int i;

    bytecount_t rcwnd = cwnd;
    rcwnd += 4 * 3 * CUBIC_FACTOR_BETA / (2 - CUBIC_FACTOR_BETA) * mss * mss / rcwnd;
    cubic_init(&cubic);
    for (i = 0; i < 3; i++) {
        cwnd = cubic_received_ack(&cubic, mss, cwnd, mss, now, rttmin);
    }
    for (i = 0; i < 5; i++) {
        now += 10 * 1000;
        prev_cwnd = cwnd;
        cwnd = cubic_received_ack(&cubic, mss, cwnd, mss, now, rttmin);
        rcwnd += 3 * CUBIC_FACTOR_BETA / (2 - CUBIC_FACTOR_BETA) * mss * mss / rcwnd;
    }

}

GUIC_TEST(cubic, loss)
{
    bytecount_t mss = 1000;
    microtime_t rttmin = 100 * 1000;
    bytecount_t cwnd = 422 * mss;
    bytecount_t expect = cwnd
        + 3 * CUBIC_FACTOR_BETA / (2 - CUBIC_FACTOR_BETA) * mss * mss / cwnd;
    microtime_t now = 1000;
    struct cubic cubic;
    cubic_init(&cubic);

    cwnd = cubic_received_ack(&cubic, mss, cwnd, mss, now, rttmin);
    expect = cwnd * (1 - CUBIC_FACTOR_BETA);
    cwnd = cubic_packet_loss(&cubic, cwnd);

    ASSERT(expect, cwnd, ==);

    expect = cwnd * (1 - CUBIC_FACTOR_BETA);
    cwnd = cubic_packet_loss(&cubic, cwnd);

    ASSERT(expect, cwnd, ==);
}

