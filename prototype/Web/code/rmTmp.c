#include <stdio.h>

int main(int argc, char** argv){
  sleep(2);

  remove(argv[1]);
  return 0;
}
