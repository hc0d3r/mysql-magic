# mysql-magic

I made this just for fun, use for illegal purposes are not allowed.

The mysql client read the password, then write this for some malloc'ed memory, and free it, but just because a chunk was freed doesn't mean it will be used again, to ensure that your programs not keep sensitive information in memory you must overwrite the memory.

The main goal is get the password passed through tty, but sometimes it also gets the password passed from command line (-pxxxxxx).

Tested in `mysql  Ver 15.1 Distrib 10.3.13-MariaDB, for Linux (x86_64) using readline 5.1`

# Demo

![](https://media.giphy.com/media/ZYQCJs3873q6kvJcDO/giphy.gif)
