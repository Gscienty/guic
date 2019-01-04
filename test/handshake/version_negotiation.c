#include "test/testcase.h"
#include "handshake/version_negotiation.h"
#include <stdint.h>

#ifdef DEBUG

GUIC_SUPPORT_VERSION(0xABCD1234);

GUIC_SUPPORT_VERSION(0x1234ABCD);

#endif

GUIC_TEST(version_negotiation, support)
{
    ASSERT(0, version_support(0xABCD1234), ==);
    ASSERT(0, version_support(0x1234ABCD), ==);
    ASSERT(0, version_support(0x11223344), !=);
    ASSERT(0, version_support(0x00000000), !=);
}

GUIC_TEST(version_negotiation, count)
{
    ASSERT(2, version_count(), ==);
}

GUIC_TEST(version_negotiation, fill)
{
    uint8_t buf[8];
    int exist = 0;
    int i;

    ASSERT(8, version_fill(buf, 8), ==);

    for (i = 0; i < 2; i++) {
        if (((uint32_t *) buf)[i] == 0x3412CDAB) {
            exist = 1;
            break;
        }
    }

    ASSERT(1, exist, ==);
}
