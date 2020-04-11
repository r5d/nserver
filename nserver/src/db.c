#include <db.h>


static const char *DB_FILE = "nserver.db";

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
