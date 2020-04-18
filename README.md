# nserver

A nihilistic stats server written is part of reading Learn C the
Harday.


## compiling

You'll need a C compiler (clang or gcc), the [gdbm][1] library and
[GNU make][2].

[1]: https://www.gnu.org.ua/software/gdbm
[2]: https://www.gnu.org/software/make

To compile the server, do:

```
make
```

To compile a debuggable version of the server, do:

```
make dev
```

The usual:

```
make clean
```

clean up things.


## statserve

To start this server, do:

```
./bin/statserve
```

It'll be listening on port 7899.

You can connect as a client to the server via `nc`:

```
nc 127.0.0.1 7899
```


### client commands

Recognized commands:


```
/create NAME
/sample NAME
/mean NAME
/dump NAME
/list
/store NAME
/load NAME ANOTHER_NAME
```

The `/store` command stores `NAME` in a persistent database on the disk.

The `/load` command loads the `NAME` from the persistent database into
memory at ANOTHER_NAME.