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
