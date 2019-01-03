#include "test/testcase.h"
#include "util/variable_integer.h"

GUIC_TEST(variable_integer, encode_1byte)
{
    int val = 37;
    uint8_t buf[1];

    varint_encode(buf, 1, val);
    ASSERT(0x25, buf[0], ==);
}

GUIC_TEST(variable_integer, encode_2byte)
{
    int val = 15293;
    uint8_t buf[2];

    ASSERT(2, varint_encode(buf, 2, val), ==);
    ASSERT(0x7B, buf[0], ==);
    ASSERT(0xBD, buf[1], ==);
}

GUIC_TEST(variable_integer, encode_4byte)
{
    int val = 494878333;
    uint8_t buf[4];

    ASSERT(4, varint_encode(buf, 4, val), ==);
    ASSERT(0x9D, buf[0], ==);
    ASSERT(0x7F, buf[1], ==);
    ASSERT(0x3E, buf[2], ==);
    ASSERT(0x7D, buf[3], ==);
}

GUIC_TEST(variable_integer, encode_8byte)
{
    uint64_t val = 151288809941952652;
    uint8_t buf[8];
    
    ASSERT(8, varint_encode(buf, 8, val), ==);
    ASSERT(0xC2, buf[0], ==);
    ASSERT(0x19, buf[1], ==);
    ASSERT(0x7C, buf[2], ==);
    ASSERT(0x5E, buf[3], ==);
    ASSERT(0xFF, buf[4], ==);
    ASSERT(0x14, buf[5], ==);
    ASSERT(0xE8, buf[6], ==);
    ASSERT(0x8C, buf[7], ==);
}

GUIC_TEST(variable_integer, decode_1byte)
{
    uint8_t buf[] = { 0x25 };
    uint64_t val;
    ASSERT(1, varint_decode(&val, buf, 1), ==);
    ASSERT(37, val, ==);
}

GUIC_TEST(variable_integer, decode_2byte)
{
    uint8_t buf[] = { 0x7B, 0xBD };
    uint64_t val;
    ASSERT(2, varint_decode(&val, buf, 2), ==);
    ASSERT(15293, val, ==);
}

GUIC_TEST(variable_integer, decode_4byte)
{
    uint8_t buf[] = { 0x9D, 0x7F, 0x3E, 0x7D };
    uint64_t val;
    ASSERT(4, varint_decode(&val, buf, 4), ==);
    ASSERT(494878333, val, ==);
}

GUIC_TEST(variable_integer, decode_8byte)
{
    uint8_t buf[] = { 0xC2, 0x19, 0x7C, 0x5E, 0xFF, 0x14, 0xE8, 0x8C };
    uint64_t val;
    ASSERT(8, varint_decode(&val, buf, 8), ==);
    ASSERT(151288809941952652UL, val, ==);
}

GUIC_TEST(variable_integer, number_too_large)
{
    uint64_t val = 0x7FFFFFFFFFFFFFFF;
    uint8_t buf[8];
    ASSERT(VARINT_ENCODE_ERROR_NUM_TOO_LARGE, varint_encode(buf, 8, val), ==);
}

GUIC_TEST(variable_integer, buffer_limit)
{
    uint64_t val = 65536;
    uint8_t buf[1];
    ASSERT(VARINT_ENCODE_ERROR_BUFFER_LIMIT, varint_encode(buf, 1, val), ==);
}
