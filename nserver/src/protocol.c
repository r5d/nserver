#include <protocol.h>

static Hashmap *hash;

int ssinit()
{
    if (hash == NULL) {
        hash = Hashmap_create(NULL, NULL);
        check(hash != NULL, "unable to create hashmap");
    }

    return 0;
 error:
    return -1;
}

int sscreate(char *key)
{
    int rc = 0;

    check(ssinit() == 0, "ssinit failed");

    // 1. create bstring from 'key'.
    bstring k = bfromcstr(key);
    check(k != NULL, "key creation failed");

    // 2. allocate fresh Stats.
    Stats *st = Stats_create();
    check(st != NULL, "stats creation failed");

    // 3. add to hashmap.
    rc = Hashmap_set(hash, k, st);
    check(rc == 0, "hashmap set failed");

    return 0;
 error:
    return -1;
}
