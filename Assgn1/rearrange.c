#include <stdio.h>
#include <string.h>
#include <sndfile.h>

#define BUFFER_LENGTH 44100

int main(void) {
   static double data[BUFFER_LENGTH], data2[BUFFER_LENGTH];
   SNDFILE *inFile, *outFile;
   SF_INFO info;
   int readCount, readCount2, secCount = 0;
   int order[] = {1, 30, 2, 29, 3, 28, 4, 27, 5, 26, 6, 25, 7, 24, 8, 23, 9,
         22, 10, 21, 11, 20, 12, 19, 13, 18, 14, 17, 15, 16};
   
   if (!(inFile = sf_open("Mario.wav", SFM_READ, &info))) {
      printf("There was an error opening input file %s.\n", "Mario.wav");
      sf_perror(NULL);
      return 1;
   }

   if (!(outFile = sf_open("New.wav", SFM_WRITE, &info))) {
      printf("There was an error opening output file %s.\n", "New.wav");
      sf_perror(NULL);
      return 1;
   }
   
   
   while ((readCount = sf_read_double(inFile, data, BUFFER_LENGTH)) && secCount < 30) {
      readCount2 = sf_read_double(inFile, data2, BUFFER_LENGTH);
      sf_write_double(outFile, data2, readCount2);
      sf_write_double(outFile, data, readCount);
      secCount++;
   }
   
   
   sf_close(inFile);
   sf_close(outFile);
   return 0;
}