#include "test/testcase.h"
#include "wire/pack_store.h"

GUIC_TEST(pack_store, run)
{
    void *ptr = pack_malloc(LONG_HEADER_MAX_SIZE, 1);
    pack_free(ptr);
}
