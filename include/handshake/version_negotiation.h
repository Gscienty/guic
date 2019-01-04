#ifndef _GUIC_HANDSHAKE_VERSION_NEGOTIATION_H
#define _GUIC_HANDSHAKE_VERSION_NEGOTIATION_H

#include "util/rbtree.h"
#include "wire/type.h"
#include <stdint.h>
#include <stddef.h>

struct __guic_support_version {
    struct rbnode node;

    uint32_t version;
    unsigned int magic;
};

#define GUIC_SUPPORT_VERSION_MAGIC 0x5e44104
#define GUIC_SUPPORT_VERSION_NAME(x) __guic_support_version__##x
#define GUIC_SUPPORT_VERSION_SECTION __attribute__((used, section(".support_version"), aligned(1)))
#define GUIC_SUPPORT_VERSION(x) \
    static struct __guic_support_version GUIC_SUPPORT_VERSION_NAME(x) \
    GUIC_SUPPORT_VERSION_SECTION = { \
        .version = x, \
        .magic = GUIC_SUPPORT_VERSION_MAGIC \
    }

/**
 * check this version is support
 * @param version: version
 * @return: 0 is support
 * 
 */
int version_support(version_t version);

/**
 * get support version count
 *
 * @return: version count
 */
int version_count();

/**
 * construct version negotiation payload
 * @return: version negotiation payload
 * 
 */
size_t version_fill(void * const buf, size_t size);


#endif
