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

int ssdelete(char *key)
{
    check(hash != NULL, "hash not initialized");

    // 1. create bstring from 'key'.
    bstring k = bfromcstr(key);
    check(k != NULL, "key creation failed");

    // 2. check if key exists.
    Stats *st = (Stats *) Hashmap_get(hash, k);
    if (st == NULL) {
        // key does not exists.
        return 0;
    }

    // 3. delete key.
    st = (Stats *) Hashmap_delete(hash, k);
    check(st != NULL, "hash key delete failed");

    // 4. clean up the stats for this key.
    free(st);

    return 0;
 error:
    return -1;
}

int sssample(char *key, double s)
{
    check(hash != NULL, "hash not initialized");

    // 1. create bstring from 'key'.
    bstring k = bfromcstr(key);
    check(k != NULL, "key creation failed");

    // 2. try to get Stats for key.
    Stats *st = (Stats *) Hashmap_get(hash, k);
    check(st != NULL, "stats not found for key");

    // 3. sample!
    Stats_sample(st, s);

    return 0;
 error:
    return -1;
}

double ssmean(char *key)
{
    check(hash != NULL, "hash not initialized");

    // 1. create bstring from 'key'.
    bstring k = bfromcstr(key);

    // 2. try to get Stats for key.
    Stats *st = (Stats *) Hashmap_get(hash, k);
    check(st != NULL, "stats not found for key");

    // 3. get mean.
    double m = Stats_mean(st);

    return m;
 error:
    return -1;
}

char *ssdump(char *key)
{
    check(hash != NULL, "hash not initialized");

    // 1. create bstring from 'key'.
    bstring k = bfromcstr(key);

    // 2. try to get Stats for key.
    Stats *st = (Stats *) Hashmap_get(hash, k);
    check(st != NULL, "stats not found for key");

    // 3. get dump.
    char *dstr = Stats_dump(st);
    check(dstr != NULL, "dump failed for key");

    return dstr;
 error:
    return NULL;
}
