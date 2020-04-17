#include <protocol.h>

static TSTree *tst;

int sscreate(char *key)
{
    check(key != NULL || strlen(key) < 1, "key invalid");

    // 1. Check if key is already in the tree.
    Record *rec = (Record *) TSTree_search(tst, key, strlen(key));

    // If it's already there; there's nothing to do.
    if (rec != NULL && rec->deleted == 0) {
        return 1;
    }
    // If it's already there and deleted, undelete it.
    if (rec != NULL && rec->deleted == 1) {
        rec->deleted = 0;

        // Allocate fresh Stats.
        rec->st = Stats_create();
        check(rec->st != NULL, "stats creation failed");

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
   check(key != NULL || strlen(key) < 1, "key invalid");
   check(tst != NULL, "tstree not initialized");

   Record *rec = (Record *) TSTree_search(tst, key, strlen(key));
   if (rec == NULL) {
       // key does not exists.
       return 0;
   }

   // Mark as deleted.
   rec->deleted = 1;

   // Free Stats.
   free(rec->st);

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

    if (rec->deleted == 1) {
        // Record was deleted; nop.
        return 0;
    }

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
    check(rec->deleted != 1, "record was deleted");

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
    check(rec->deleted != 1, "record was deleted");
    check(rec->st != NULL, "record's st invalid");

    // 2. Get mean.
    double m = Stats_mean(rec->st);

    return m;
 error:
    return -1;
}

char *ssdump(char *key)
{
    check(key != NULL && strlen(key) > 0, "key invalid");
    check(tst != NULL, "tstree not initialized");

    // 1. create bstring from 'key'.
    Record *rec = (Record *) TSTree_search(tst, key, strlen(key));

    check(rec != NULL, "record not found");
    check(rec->st != NULL, "stats not found for key");
    check(rec->deleted != 1, "record was deleted");

    // 2. get dump.
    char *dstr = Stats_dump(rec->st);
    check(dstr != NULL, "dump failed for key");

    return dstr;
 error:
    return NULL;
}

// meant to be used by sslist.
void traverse_tree(void *value, void *data)
{
    Record *rec  = (Record *) value;
    bstring bstr = (bstring) data;

    check(rec != NULL, "Record is NULL");
    check(rec->deleted != 1, "Record was deleted");
    check(bstr != NULL, "bstr is NULL");
    check(rec->key != NULL, "Record's key is NULL");
    check(blength(rec->key) > 0, "Record's key is an empty string");

    int rc = bconcat(bstr, rec->key);
    check(rc == BSTR_OK, "bstr key concat failed");

    rc = bconchar(bstr, '\n');
    check(rc == BSTR_OK, "bstr newline concat failed");

 error:
    return;
}


char *sslist()
{
    char *list = NULL, *tmp = NULL;

    if (tst == NULL) {
        list = (char *) calloc(7 + 1, sizeof(char));
        check_mem(list);

        list = strncpy(list, "EMPTY\n\r", 7);

        return list;
    }

    // 1. Create "accumulator" string.
    bstring ks_str = bfromcstr("");
    check(ks_str != NULL, "error creating keys_str");

    // 2. Accumulate keys into "accumulator" string.
    TSTree_traverse(tst, traverse_tree, ks_str);

    // 3. Make result.
    tmp = bstr2cstr(ks_str, ' ');

    list = (char *) calloc(strlen(tmp) + 1, sizeof(char));
    check_mem(list);

    list = strncpy(list, tmp, strlen(tmp));

    // 4. Clean up.
    bcstrfree(tmp);

    // 3. Return result.
    return list;
 error:
    if (tmp) {
        bcstrfree(tmp);
    }
    return NULL;
}

int ssstore(char *key)
{
    check(key != NULL && strlen(key) > 0, "key invalid");
    check(tst != NULL, "tstree not initialized");

    // 1. create bstring from 'key'.
    Record *rec = (Record *) TSTree_search(tst, key, strlen(key));

    check(rec != NULL, "record not found");
    check(rec->st != NULL, "stats not found for key");
    check(rec->deleted != 1, "record was deleted");

    // 2. stringify the stats.
    char *st_str = Stats_stringify(rec->st);
    check(st_str != NULL, "stats stringify failed");

    // 3. store stats in db.
    int rc = db_store(key, st_str);
    check(rc == 0, "db store failed");

    return 0;
 error:
    return -1;
}

int ssload(char *from, char *to)
{
    check(from != NULL && strlen(from) > 0, "from invalid");
    check(to != NULL && strlen(to) > 0, "to invalid");
    check(tst != NULL, "tstree not initialized");

    // 1. Check if 'to' key already exists.
    Record *rec = (Record *) TSTree_search(tst, to, strlen(to));

    // 2. if 'to' key exists return immediately with -2.
    if (rec != NULL && rec->deleted == 0) {
        return -2;
    }

    // 3. read 'from' key from database.
    char *st_str = db_load(from);
    check(st_str != NULL, "db load failed");

    // 4. construct stats from string.
    Stats *st = Stats_unstringify(st_str);
    check(st != NULL, "stats unstringify failed");

    // 5. create Record if needed.
    int rec_created =  0;
    if (rec == NULL)  {
        rec = (Record *) calloc(1, sizeof(Record));
        check_mem(rec);
        rec_created = 1;
    }

    // 6. get things ready for insertiion
    bstring tk = bfromcstr(to);
    check(tk != NULL, "key creation failed");

    rec->key = tk;
    rec->st = st;
    rec->deleted = 0;

    // 7. insert Record into 'to' key in the TSTree if needed.
    if (rec_created == 1) {
        tst = TSTree_insert(tst, to, strlen(to), rec);
        check(tst != NULL, "tstree insert failed");
    }

    return 0;
 error:
    return -1;
}
