#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char **argv) {
  int ticks = atoi(argv[1]);
  if (ticks <= 0) return 0;
  pause(ticks); // syscall wrapper
  return 0;
}

