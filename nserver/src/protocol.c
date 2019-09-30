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
    check(ssinit() == 0, "ssinit failed");

    return 0;
 error:
    return -1;
}
