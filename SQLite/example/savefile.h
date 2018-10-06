#ifndef _SAVEFILE_H_
#define _SAVEFILE_H_

#include <stdio.h>
#include <stdlib.h>

class myfile {
public:
  myfile() {}
  ~myfile() {}
  void openfile() { file = fopen("file.txt", "a+"); }
  void savefile() {
    if (file == NULL) {
      printf("Error opening file!\n");
      exit(1);
    }
    /* print some text */
    const char *text = "Write this to the file";
    fprintf(file, "Some text: %s\n", text);

    /* print integers and floats */
    int i = 1;
    float py = 3.1415927;
    fprintf(file, "Integer: %d, float: %f\n", i, py);

    /* printing single chatacters */
    char c = 'A';
    fprintf(file, "A character: %c\n", c);
  }
  void closefile() { fclose(file); }

private:
  FILE *file;
};

#endif
