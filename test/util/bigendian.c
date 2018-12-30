#include "util/bigendian.h"
#include "test/testcase.h"

GUIC_TEST(bigendian, encode_1byte)
{
    uint8_t val = 0xAA;
    uint8_t buf[1];

    bigendian_encode(buf, 1, val, 1);
    ASSERT(0xAA, buf[0], ==);
}

GUIC_TEST(bigendian, encode_2byte)
{
    uint16_t val = 0xAABB;
    uint8_t buf[2];

    bigendian_encode(buf, 2, val, 2);
    ASSERT(0xAA, buf[0], ==);
    ASSERT(0xBB, buf[1], ==);
}

GUIC_TEST(bigendian, encode_4byte)
{
    uint32_t val = 0xAABBCCDD;
    uint8_t buf[4];

    bigendian_encode(buf, 4, val, 4);
    ASSERT(0xAA, buf[0], ==);
    ASSERT(0xBB, buf[1], ==);
    ASSERT(0xCC, buf[2], ==);
    ASSERT(0xDD, buf[3], ==);
}

GUIC_TEST(bigendian, encode_8byte)
{
    uint64_t val = 0xAABBCCDD11223344;
    uint8_t buf[8];

    bigendian_encode(buf, 8, val, 8);
    ASSERT(0xAA, buf[0], ==);
    ASSERT(0xBB, buf[1], ==);
    ASSERT(0xCC, buf[2], ==);
    ASSERT(0xDD, buf[3], ==);
    ASSERT(0x11, buf[4], ==);
    ASSERT(0x22, buf[5], ==);
    ASSERT(0x33, buf[6], ==);
    ASSERT(0x44, buf[7], ==);
}

GUIC_TEST(bigendian, decode_1byte)
{
    uint8_t buf[1] = { 0xAA };
    uint8_t val;

    bigendian_decode(buf, 1, &val, 1);
    ASSERT(0xAA, val, ==);
}

GUIC_TEST(bigendian, decode_2byte)
{
    uint8_t buf[2] = { 0xAA, 0xBB };
    uint16_t val;

    bigendian_decode(buf, 2, &val, 2);
    ASSERT(0xAABB, val, ==);
}

GUIC_TEST(bigendian, decode_4byte)
{
    uint8_t buf[4] = { 0xAA, 0xBB, 0xCC, 0xDD };
    uint32_t val;

    bigendian_decode(buf, 4, &val, 4);
    ASSERT(0xAABBCCDD, val, ==);
}

GUIC_TEST(bigendian, decode_8byte)
{
    uint8_t buf[8] = { 0xAA, 0xBB, 0xCC, 0xDD, 0x11, 0x22, 0x33, 0x44 };
    uint64_t val;

    bigendian_decode(buf, 8, &val, 8);
    ASSERT(0xAABBCCDD11223344, val, ==);
}
