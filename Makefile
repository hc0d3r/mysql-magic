CFLAGS+=-Wall -Wextra -Iignotum/src
LDFLAGS+=-lutil

OBJS = src/connection-listen.o src/generic-list.o \
	src/offset-scan.o src/dump-password.o src/mysql-magic.o \
	src/heap.o src/pretty-print.o

all: ignotum/lib/libignotum.a mysql-magic

mysql-magic: $(OBJS) ignotum/lib/libignotum.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@strip mysql-magic

src/%.o: src/%.c

ignotum/lib/libignotum.a:
	$(MAKE) -C ignotum lib/libignotum.a

clean:
	$(MAKE) -C ignotum clean
	rm -f mysql-magic $(OBJS)
