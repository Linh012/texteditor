#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAXCHAR 100 // Number of columns/chars in a single line
#define FNAMELENGTH 259 // File name max length


void deleteFile();
void showChangelog();
void copy(char file[FNAMELENGTH]);
void append(char file[FNAMELENGTH]);
void insert(char file[FNAMELENGTH]);
void show(char file[FNAMELENGTH]);
void showAll(char file[FNAMELENGTH]);
void deleteLine(char file[FNAMELENGTH]);
int lineCount(char file[FNAMELENGTH]);
void record(char file[FNAMELENGTH], char operation[MAXCHAR], int count);
void help();
void setup(char file[FNAMELENGTH]);

int main(int argc, char *argv[]) {
  FILE *fptr, *changelog;
  char operation[20];
  int i;

  if (argc != 2) {
    printf("Usage: ./texteditor [filename]\n");
    return 1;
  }

  setup(argv[1]);

  while (strcmp(operation, "quit") != 0 && (strcmp(operation, "q") != 0)) {
    printf("Enter operation: ");
    scanf("%s", operation);

    i = 0;

    // Lowercase all characters from input - quality of life
    while (operation[i]) {
      if ((65 <= operation[i]) && (operation[i] <= 90)) { // Check if uppercase
        operation[i] = operation[i] + 32; // character arithmetic
      }
      i++;
    }

    // These operations will not be stored in changelog
    if (strcmp(operation, "deletefile") == 0)
    {
    	deleteFile();
    }
    else if ((strcmp(operation, "changelog") == 0))
    {
    	showChangelog();
    }
    else if ((strcmp(operation, "help") == 0) || (strcmp(operation, "h") == 0))
    {
    	help();
    }
    else if ((strcmp(operation, "showall") == 0) || (strcmp(operation, "cat") == 0))
    {
    	showAll(argv[1]);
    }
    else if ((strcmp(operation, "show") == 0) || (strcmp(operation, "s") == 0))
    {
    	show(argv[1]);
    }
    else if ((strcmp(operation, "linecount") == 0))
    {
    	printf("Line count: %d\n", lineCount(argv[1]));
    }
    else // These operation will be stored in changelog
    {
    	if (strcmp(operation, "append") == 0 || (strcmp(operation, "a") == 0))
    	{
    		append(argv[1]);
    	}
    	else if ((strcmp(operation, "insert") == 0) || (strcmp(operation, "i") == 0))
    	{
    		insert(argv[1]);
    	}
    	else if ((strcmp(operation, "copy") == 0))
    	{
    		copy(argv[1]);
    	}
    	else if ((strcmp(operation, "delete") == 0) || (strcmp(operation, "del") == 0))
    	{
    		deleteLine(argv[1]);
    	}

      // Record action to changelog
    	if (strcmp(operation, "quit") != 0 && (strcmp(operation, "q") != 0))
    	{
    		record(argv[1], operation, lineCount(argv[1]));
    	}
    }
  }
  return 0;
}


// Check and make sure file and changelog exists
void setup(char file[FNAMELENGTH]) {
    FILE *fptr, *changelog;
    printf("'help' or 'h' for more help. \n");

  	fptr = fopen(file, "r");
    changelog = fopen("CHANGELOG.md", "r");

    // Create changelog file
    if (changelog == NULL) {
      changelog = fopen("CHANGELOG.md", "w");
      printf("No previous changelog detected. New changelog file created.\n");

      // Add header to changelog
      fprintf(changelog, "# Beginning of changelog  \n");
      fprintf(changelog, "  \n---\n  \n");
    }

    // Create new file
    if (fptr == NULL) {
      fptr = fopen(file, "w");
      printf("File does not exist. New file %s has been created.\n", file);
      record(file, "file creation", 0);
    }


  	if (!fptr) {
      exit(1);
    }
    fclose(fptr);
    fclose(changelog);
}


// Add to end of file
void append(char file[FNAMELENGTH]) {
  char code[MAXCHAR];
  char temp;
  scanf("%c", &temp);
  FILE *fptr;

  fptr = fopen(file, "a");

  printf("Append to file: ");
  scanf("%[^\n]", code); // Take user input
  fprintf(fptr, "%s\n", code); // Write to file

  fclose(fptr);
}


// Add to specific line in a file
void insert(char file[FNAMELENGTH]) {
  FILE *fptr, *tempfile;
  char temp;
  char code[MAXCHAR], newline[MAXCHAR];
  int target;
  int linenumber = 0;


  // Prompt for user input
  printf("Insert at line number: ");
  scanf("%d", &target);
  scanf("%c", &temp);
  printf("Content: ");
  scanf("%[^\n]", newline);


  int lc = lineCount(file);
  if (lc < target) { // Check if other lines need to be shifted
    int i;
    fptr = fopen(file, "a");

    for (i = 0; i < (target - lc); i++) {
      fprintf(fptr, "\n");
    }

    fprintf(fptr, "%s\n", newline);
    fclose(fptr);

  } else {
    fptr = fopen(file, "r");
    tempfile = fopen("tempfile.tmp", "w"); // Create temporary file

    // Copy every line to temporary file
    while (fgets(code, sizeof(code), fptr) != NULL) {
      linenumber++;

      // Insert at specified line
      if (linenumber == target) {
        fprintf(tempfile, "%s\n", newline);
      }

      fprintf(tempfile, "%s", code);
    }

    fclose(fptr);
    fclose(tempfile);
    rename("tempfile.tmp", file);
  }
}


// Delete specific line in file
void deleteLine(char file[FNAMELENGTH]) {
  FILE *fptr, *tempfile;
  char code[MAXCHAR];
  int linenumber = 0;
  int from, to;

  fptr = fopen(file, "r");
  tempfile = fopen("tempfile.tmp", "w"); // Create temporary file

  // Prompt for user input/line number to be deleted
  do {
    printf("Delete from line number: ");
    scanf("%d", &from);
    printf("To line number: ");
    scanf("%d", &to);
  } while (from > to);


  while (fgets(code, sizeof(code), fptr) != NULL) {
    linenumber++;

    // Copy every line to temp file except specified line(s)
    if ((linenumber < from) || (linenumber > to)) {
      fprintf(tempfile, "%s", code);
    }
  }

  fclose(fptr);
  fclose(tempfile);
  rename("tempfile.tmp", file);
}


// Print to terminal entire content of file
void showAll(char file[FNAMELENGTH]) {
  char line[MAXCHAR];
  int linenumber = 0;
  FILE *fptr;

  fptr = fopen(file, "r");

  // Print file content line by line
  while (fgets(line, sizeof(line), fptr) != NULL) {
    linenumber++;
    printf("%d| %s", linenumber, line);
  }

  fclose(fptr);
}


// Print to terminal specific section of file
void show(char file[FNAMELENGTH]) {
  char line[MAXCHAR];
  int linenumber = 0;
  int from, to;
  FILE *fptr;

  fptr = fopen(file, "r");

  // Prompt for user input/lines to be read from file
  do {
    printf("From line number: ");
    scanf("%d", &from);
    printf("To line number: ");
    scanf("%d", &to);
  } while (from > to);

  // Print specific line(s) of file
  while (fgets(line, sizeof(line), fptr) != NULL) {
    linenumber++;
    if ((from <= linenumber) && (linenumber <= to)) {
      printf("%d| %s", linenumber, line);
    }
  }

  fclose(fptr);
}


// Create of a copy of file with a specified name
void copy(char file[FNAMELENGTH]) {
  char filename[FNAMELENGTH];
  char line[MAXCHAR];
  FILE *new, *fptr;

  fptr = fopen(file, "r");

  // Prompt for user input/new file name
  printf("Copying this file.\nEnter new file name: ");
  scanf("%s", filename);

  // Copy contents to new file (if current file name is different from user inputted file name)
  if (strcmp(file, filename) != 0) {
    new = fopen(filename, "w");
    while (fgets(line, sizeof(line), fptr) != NULL) {
      fprintf(new, "%s", line);
    }
    fclose(new);
  }

  fclose(fptr);
}


// Delete specified file in current directory
void deleteFile() {
  char filename[FNAMELENGTH];

  // Prompt for user input/file to delete
  printf("Enter file name: ");
  scanf("%s", filename);

  if (remove(filename) == 0) {
    printf("File has been deleted.\n");
    record(filename, "file deletion", -1); // -1 to indicate file deleted
  } else {
    printf("File has not been deleted.\n");
  }
}


// Return number of lines in a file
int lineCount(char file[FNAMELENGTH]) {
  char line[MAXCHAR];
  int count = 0;
  FILE *fptr;

  fptr = fopen(file, "r");

  // Count lines till end of file
  while (fgets(line, sizeof(line), fptr) != NULL) {
    count++;
  }

  fclose(fptr);
  return count;
}


// Record new change to changelog
void record(char file[FNAMELENGTH], char operation[MAXCHAR], int count) {
  FILE *changelog;
  char buffer[MAXCHAR];
  time_t rawtime;

  time(&rawtime); // Return time in seconds since Unix epoch
  struct tm *info = gmtime(&rawtime); // Fill tm struct

  changelog = fopen("CHANGELOG.md", "a");

  // Write change to changelog file
  fprintf(changelog, "##### Timestamp (UTC): %s  \n", asctime(info));
  fprintf(changelog, "Filename: %s  \n", file);
  fprintf(changelog, "Operation: %s  \n", operation);
  fprintf(changelog, "New line count: %d  \n  \n", count);

  fclose(changelog);
}


// Print changelog to terminal
void showChangelog() {
  char line[MAXCHAR];
  FILE *changelog;

  // Read and print every line until end of file
  changelog = fopen("CHANGELOG.md", "r");
  while (fgets(line, sizeof(line), changelog) != NULL) {
    printf("%s", line);
  }

  fclose(changelog);
}


// Print instructions on how to use editor
void help() {
  printf("--- \n");
  printf("'quit' or 'q' to exit \n");
  printf("'append' or 'a' to append new line of code \n");
  printf("'insert' or 'i' to insert new line of code at a specific line \n");
  printf("'delete' or 'del' to delete specific line(s) of code \n");
  printf("'showall' or 'cat' to show all contents of the file \n");
  printf("'show' or 's' to show contents at particular lines \n");
  printf("'copy' to create a copy of the file with a specified name \n");
  printf("'deletefile' to delete a file with a specified name \n");
  printf("'linecount' to find out number of lines in the file \n");
  printf("'changelog' to show changelog \n");
  printf("--- \n");
}
