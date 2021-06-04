#define _BSD_SOURCE

#include <stdint.h>
#include <sys/time.h>

#define handle_error(msg) do { perror(msg " error"); exit(EXIT_FAILURE); \
                          } while (0)