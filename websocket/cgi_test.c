#include "stdio.h"
#include <unistd.h>
#include <fcntl.h>

#define BUFF_SIZE 24

int main(void) {
  int i = 0;
  int c;
  char line[BUFF_SIZE];
  FILE * sensor;
  //sensor = fopen("/home/root/text.txt",O_RDONLY);
  sensor = fopen("/dev/ttyACM0","r");
  if(sensor)
  {
    while(1)
    {
      printf( "Content-Type: text/plain\n\n" );
      fread(line, sizeof(char), BUFF_SIZE, sensor);
      line[BUFF_SIZE-1] = 0;
      printf("%s\n\r",line);
    }
    fclose(sensor);
  }
   
  return 0;
}
