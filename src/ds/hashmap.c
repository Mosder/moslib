#define MOS_FORCE_PREFIXES
#include "moslib/ds/hashmap.h"

MosHashmap *mos_hm_new_fn(size_t key_size, size_t val_size, MosHmInitArgs args) {
    (void)key_size;
    (void)val_size;
    (void)args;
    return NULL;
}
