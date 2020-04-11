#include <db.h>

static const char *DB_FILE = "nserver.db";

datum *mk_datum(char *data)
{
    datum *d = NULL;

    check(data != NULL, "data invalid");

    d = calloc(1, sizeof(datum));
    check(d != NULL, "datum mem alloc failed");

    d->dptr = data;
    d->dsize = strlen(data) + 2;

    return d;
 error:
    if (d) {
        free(d);
    }
    return NULL;
}

GDBM_FILE db_open(int flags)
{
   return gdbm_open(DB_FILE, 0,flags, S_IRUSR|S_IWUSR, NULL);
}

int db_init()
{
    // Create DB if it's not already created.
    GDBM_FILE  gf = db_open(GDBM_WRCREAT);
    check(gf != NULL, "unable to init db");

    // Close the DB.
    int rc = gdbm_close(gf);
    check(rc == 0,  "error closing db after init");

    return 0;
 error:
    return -1;
}

int db_store(char *key, char *value)
{
    datum *k_datum = NULL, *v_datum = NULL;
    GDBM_FILE gf = NULL;

    check(key != NULL && strlen(key) > 0, "key invalid");
    check(value != NULL && strlen(value) > 0, "data invalid");

    // make key value datum
    k_datum = mk_datum(key);
    check(k_datum != NULL, "key datum init failed");

    v_datum = mk_datum(value);
    check(v_datum != NULL, "value datum init failed");

    // init db.
    int rc = db_init();
    check(rc == 0, "db init failed");

    // open the gdbm data in write mode
    gf = db_open(GDBM_WRITER|GDBM_SYNC);
    check(gf != NULL, "unable to open db in write  mode");

    // write key -> data to db.
    rc = gdbm_store(gf, *k_datum, *v_datum, GDBM_REPLACE);
    check(rc == 0, "gdbm store failed");

    // close db
    rc = gdbm_close(gf);
    check(rc == 0, "gdbm close failed");

    // cleanup.
    free(k_datum);
    free(v_datum);

    return 0;
 error:
    if (k_datum) {
        free(k_datum);
    }
    if (v_datum)  {
        free(v_datum);
    }
    if (gf) {
        gdbm_close(gf);
    }
    return -1;
}

char *db_load(char *key)
{
    datum *k_datum = NULL;
    GDBM_FILE gf = NULL;

    check(key != NULL && strlen(key) > 0, "key invalid");

    // make key datum
    k_datum = mk_datum(key);
    check(k_datum != NULL, "key datum init failed");

    // init db.
    int rc = db_init();
    check(rc == 0, "db init failed");

    // open the gdbm data in read mode
    gf = db_open(GDBM_READER|GDBM_SYNC);
    check(gf != NULL, "unable to open db in read  mode");

    // try to fetch value for key.
    datum v_datum = gdbm_fetch(gf, *k_datum);
    check(v_datum.dptr != NULL, "key not found");

    // clean up.
    free(k_datum);

    return v_datum.dptr;
 error:
    if (k_datum) {
        free(k_datum);
    }
    if (gf) {
        gdbm_close(gf);
    }
    return NULL;
}
