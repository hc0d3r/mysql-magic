# mysql-magic

I made this just for fun, use for illegal purposes are not allowed.

The mysql client read the password, then write this for some malloc'ed memory, and free it, but just because a chunk was freed doesn't mean it will be used again, to ensure that your programs not keep sensitive information in memory you must overwrite the memory.

The main goal is get the password passed through tty, but sometimes it also gets the password passed from command line (-pxxxxxx).

Tested in `mysql  Ver 15.1 Distrib 10.3.13-MariaDB, for Linux (x86_64) using readline 5.1`

# Compiling

First clone the project using `git clone --recurse-submodules https://github.com/hc0d3r/mysql-magic`.

for compile you only need to run `make`, if you want add some flags, for any reason, you can do that with `CFLAGS=myflags make`.

if the system mysql client is not placed at `/usr/bin/mysql` you'll need compile with
`CFLAGS=-DMYSQLCLI=/path/to/mysql make`

# Options

```
mysql-magic [options] <pid>
 -o <offset>  comma-separated list of offset
 -s            search the memory for passwords and get offset
 -d <dir>      Write heap to the folder
 -l            Listen mode, wait for outgoing ipv4 connections on port 3306
 -r <secs>     Sleep time (Default: 3 seconds)
 -p            Use process_vm_readv instead of /proc/pid/mem
```

pass `-d` is a good thing to do, the password, and some informations like old queries, can remain in memory, so you can analyse this and maybe build a wordlist based on it, if the password don't are in the offset

# Demo

![](https://raw.githubusercontent.com/hc0d3r/mysql-magic/demo/demo.gif)
