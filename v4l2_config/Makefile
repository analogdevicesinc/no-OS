
EXEC = v4l2_config
SYMBOLS = -DINFO
CFLAGS = -Wall -Werror -Wextra -O0 $(SYMBOLS)

all: $(EXEC)

$(EXEC): example.c v4l2_config.c
	$(CC) $+ $(CFLAGS) -o $(EXEC)

clean:
	rm -rf $(EXEC)
