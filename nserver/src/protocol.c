#include <protocol.h>

static Hashmap *hash;
static TSTree *tst;

int sscreate(char *key)
{
    check(key != NULL || strlen(key) < 1, "key invalid");

    // 1. Check if key is already in the tree.
    Record *rec = (Record *) TSTree_search(tst, key, strlen(key));

    // If it's already there; there's nothing to do.
    if (rec != NULL && rec-> deleted == 0) {
        return 1;
    }
    // If it's already there and deleted, undelete it.
    if (rec != NULL && rec->deleted == 1) {
        rec->deleted = 0;

        return 2;
    }

    // 2. Create bstring from 'key'.
    bstring k = bfromcstr(key);
    check(k != NULL, "key creation failed");

    // 3. Allocate fresh Stats.
    Stats *st = Stats_create();
    check(st != NULL, "stats creation failed");

    // 4. Create Record.
    rec = (Record *) calloc(1, sizeof(Record));
    check_mem(rec);

    // 5. Initialize Record.
    rec->key = k;
    rec->st = st;
    rec->deleted = 0;

    // 6. Add Record to tree.
    tst = TSTree_insert(tst, key, strlen(key), rec);
    check(tst != NULL, "tstree insert failed");

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

int sssample_parent(char *key, double s)
{
    check(key != NULL || strlen(key) < 1, "key invalid");
    check(tst != NULL, "tstree not initialized");

    // 1. Try to get Record with key prefix.
    Record *rec = (Record *) TSTree_search_prefix(tst, key, strlen(key));

    if (rec == NULL) {
        // No record with key prefix.
        return 0;
    }
    check(rec->st != NULL, "record's st invalid");

    // 2. Sample!
    Stats_sample(rec->st, s);

    return 1;
 error:
    return -1;
}

double sssample(char *key, double s)
{
    check(key != NULL || strlen(key) < 1, "key invalid");
    check(tst != NULL, "tstree not initialized");

    // 1. Try to get Record for key.
    Record *rec = (Record *) TSTree_search(tst, key, strlen(key));

    check(rec != NULL, "record not found");
    check(rec->st != NULL, "record's st invalid");

    // 2. Sample!
    Stats_sample(rec->st, s);

    // 3. Sample parent!
    int rc = sssample_parent(key, s);
    check(rc >= 0, "sampling parent failed");

    // 4. Get mean.
    double m = Stats_mean(rec->st);

    return m;
 error:
    return -1;
}

double ssmean(char *key)
{
    check(key != NULL || strlen(key) < 1, "key invalid");
    check(tst != NULL, "tstree not initialized");

    // 1. Try to get Record for key.
    Record *rec = (Record *) TSTree_search(tst, key, strlen(key));

    check(rec != NULL, "record not found");
    check(rec->st != NULL, "record's st invalid");

    // 2. Get mean.
    double m = Stats_mean(rec->st);

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

char *sslist()
{
    DArray *ks = NULL;

    check(hash != NULL, "hash not initiliazed");

    // 1. Get keys.
    ks = Hashmap_keys(hash);
    check(ks != NULL, "error getting keys");

    bstring ks_str = bfromcstr("");
    check(ks_str != NULL, "error creating keys_str");

    int i, rc;
    bstring k = NULL;
    for (i = 0; i < DArray_count(ks); ++i) {
        k = (bstring) DArray_get(ks, i);
        check(k != NULL, "k at %d", i);

        rc = bconcat(ks_str, k);
        check(rc == BSTR_OK, "bstr key concat failed");

        rc = bconchar(ks_str, '\n');
        check(rc == BSTR_OK, "bstr newline concat failed");
    }

    // cleanup.
    DArray_destroy(ks);

    return bstr2cstr(ks_str, ' ');
 error:

    // cleanup
    if (ks) {
        DArray_destroy(ks);
    }

    return NULL;
}

