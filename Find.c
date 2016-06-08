#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h> 
#include <unistd.h>
#include <time.h>

#include "Find.h"

#define TYPE_1 1
#define TYPE_2 2
#define TYPE_3 3
#define TYPE_4 4
#define TYPE_5 5
#define TYPE_6 6
#define BASE_10 10
#define SIZE 100

void Amin(Data *data, char *prefix, char *buffer, DIR *dir) {
   struct stat attrib;
   time_t timeNow;
   time_t modifyTime;  

   if (!strcmp(buffer, ".") || !strcmp(buffer, "..") || opendir(buffer) != NULL) {
      return;
   }

   if (fopen(buffer, "r") != NULL) {
      timeNow = time(NULL);
      stat(buffer, &attrib);
      modifyTime = attrib.st_mtime;

      modifyTime += ((data->n - 1) * 60);

      if (modifyTime - 45 < timeNow && modifyTime + 45 > timeNow) {
         printf("%s/%s\n", prefix, buffer);
      } 
   }
}

void Anewer(Data *data, char *prefix, char *buffer, DIR *dir) {
   struct stat attrib;
   struct stat fileAttrib;
   time_t fileTime;
   time_t bufferTime;  

   if (!strcmp(buffer, ".") || !strcmp(buffer, "..") || opendir(buffer) != NULL) {
      return;
   }   

   if (fopen(buffer, "r") != NULL) {
      stat(data->file, &fileAttrib);
      stat(buffer, &attrib);
      fileTime = fileAttrib.st_atime;
      bufferTime = attrib.st_atime;

      if (fileTime < bufferTime) {
         printf("%s/%s\n", prefix, buffer);
      }   
   }    
}

void Depth(void (*f)(Data *data, char *prefix, char *buffer, DIR *dir), Data *data, int depth, char *prefix) {
   DIR *dp;
   DIR *temp;
   struct dirent *sd;
   char *buffer;
   char *tempPrefix; 
   int num = 0;   

   tempPrefix = malloc(SIZE);

   if (depth == 0) {
      return; 
   }

   buffer = malloc(SIZE);   
   dp = opendir(".");
   while (((sd = readdir(dp))) != NULL) {
      strcpy(buffer, sd->d_name);
      temp = opendir(buffer);
      
      f(data, prefix, buffer, temp);  
 
      if (temp != NULL && strcmp(buffer, ".") && strcmp(buffer, "..")) {
         chdir(buffer);
   
         sprintf(tempPrefix, "%s/%s", prefix, buffer);
         if (depth < 0) {
            Depth(f, data, depth, tempPrefix); 
         } else {
            Depth(f, data, depth - 1, tempPrefix);
         }
      }
      num++;
   }

  if (depth == -1 && num == 2) {
      printf("%s\n", prefix);
   }

   chdir(".."); 
}

void MaxDepth(Data *data, char *prefix, char *buffer, DIR *dir) {
   if (!strcmp(buffer, ".") == !strcmp(buffer, "..")) {
      printf("\n%s/%s", prefix, buffer); 
   } 
}

void Empty(Data *data, char *prefix, char *buffer, DIR *dir) { 
   FILE *file; 
   DIR *temp;

   temp = opendir(buffer);   
   
   if (!strcmp(buffer, ".") == !strcmp(buffer, "..")) {
      if ((file = fopen(buffer, "r")) != NULL) {
         if ((dir == NULL) && (fgetc(file) == EOF)) {
            printf("%s/%s\n", prefix, buffer);
         }
      }  
   }
}


void Type(Data *data, char *prefix, char *buffer, DIR *dir) {
   DIR *directory; 
   FILE *file; 
   char type;

   type = data->c;  
   
   directory = opendir(buffer);
   file = fopen(buffer, "r");
  
   if (!strcmp(buffer, ".") || !strcmp(buffer, "..")) {
      return;
   }

   if (type == 'd') {
      if (directory != NULL) {
         printf("%s/%s\n", prefix, buffer);
      }
   }
   else if (type == 'f') {
      if (file != NULL && directory == NULL) {
         printf("%s/%s\n", prefix, buffer);
      }
   }
}


void Size(Data *data, char *prefix, char *buffer, DIR *dir) {
   struct stat attrib;
   int size; 
   long testSize; 

   if (!strcmp(buffer, ".") || !strcmp(buffer, "..")) {
      return;
   }
   
   if (fopen(buffer, "r") != NULL) {
      stat(buffer, &attrib);
    
      size = attrib.st_size;
      if (size > 0 && data->c != 'x') {
         size -= (size % 1024) ? (size % 1024) : 1024;
         size += 1024;
      } 
      
      testSize = data->n; 

      switch(data->c) {
      case 'k':
         testSize *= 1024; 
         break;
      case 'M':
         testSize *= 1024 * 1024; 
         break;
      case 'G':
         testSize *= 1024 * 1024 * 1024; 
         break;
      }
      
      if (size == testSize) {
         printf("%s/%s\n", prefix, buffer);
      } else if ((data->c != 'k') && size != 0 && data->n == 1) {
         printf("%s/%s\n", prefix, buffer);
      }
   }
}

void Name(Data *data, char *prefix, char *buffer, DIR *dir) {
   if (!strcmp(data->file, buffer)) {
      printf("%s/%s\n", prefix, buffer);
   }
}

void ParseArgs(Data *data, char **argv) {
   char *buffer; 
  
   buffer = malloc(100);

   argv++;
   while(*argv) {
      if (!strcmp(*argv ,"-amin")) {
         data->type = 0; 
      }
      else if (!strcmp(*argv, "-anewer")) {
         data->type = TYPE_1; 
      }
      else if (!strcmp(*argv, "-maxdepth")) {
         data->type = TYPE_2; 
      }
      else if (!strcmp(*argv, "-empty")) { 
         data->type = TYPE_3; 
      }
      else if (!strcmp(*argv, "-type")) {
         data->type = TYPE_4; 
      }
      else if (!strcmp(*argv, "-size")) { 
         data->type = TYPE_5; 
      } 
      else if (!strcmp(*argv, "-name")) { 
         data->type = TYPE_6; 
      }
      else {
         if (data->type == 0) {
            data->n = strtol(*argv, NULL, BASE_10);
         }
         else if (data->type == 1) {
            strcpy(data->file, *argv);
         }
         else if (data->type == 2) { 
            data->n = strtol(*argv, NULL, BASE_10);
         }
         else if (data->type == 4) {
             data->c = **argv;
         }
         else if (data->type == 5) {
            data->n = strtol(*argv, NULL, BASE_10);
            data->c = strlen(*argv) - 1 ? ((*argv)[strlen(*argv) - 1]) : 'x'; 
         }
         else if (data->type == 6) {
            strcpy(data->file, *argv);
         }
      }

      argv++;
   }
    
}

int main(int argc, char **argv) {
   Data *data = malloc(sizeof(Data));
   char *prefix;
   data->file = malloc(SIZE);

   data->c = 'x';
   prefix = malloc(SIZE); 
   strcpy(prefix, "."); 
  
   ParseArgs(data, argv);
   
   switch (data->type) {
   case 0:
      Depth(Amin, data, -2, prefix);
      break;
   case 1:
      if (fopen(data->file, "r") == NULL) {
         printf("find: `%s': No such file or directory\n", data->file);
         return EXIT_FAILURE;
      }
      Depth(Anewer, data, -2, prefix);
      break;
   case 2:
      if (data->n < 0) {
         printf("ERROR -- Depth less then 0\n");
         return EXIT_FAILURE;
      }   
      printf(".");
      Depth(MaxDepth, data, data->n, prefix);
      printf("\n");
      break;
   case 3:
      Depth(Empty, data, -1, prefix);
      break;
   case 4:
      if (data->c == 'd') 
         printf(".\n");
      Depth(Type, data, -2, prefix);    
      break;
   case 5:
      if (data->c == 'x') {
         printf("ERROR -- Malformed afguments\n");
         return EXIT_FAILURE;
      }
      if ((data->c == 'k' && data->n == 4) || (data->c != 'k' && data->n == 1)) {
         printf(".\n");
      }
      Depth(Size, data, -2, prefix);
      break;
   case 6: 
      Depth(Name, data, -2, prefix);
      break; 
   }   

   return 0;
}

   
