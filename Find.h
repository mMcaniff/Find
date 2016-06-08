#ifndef FIND_H
#define FIND_H

typedef struct Data { 
   int type; 
   int n;
   char c; 
   char *file; 
} Data; 

void Amin(Data *data, char *prefix, char *buffer, DIR *dir);
void Anewer(Data *data, char *prefix, char *buffer, DIR *dir);
void MaxDepth(Data *data, char *prefix, char *buffer, DIR *dir);
void Empty(Data *data, char *prefix, char *buffer, DIR *dir);
void Type(Data *data, char *prefix, char *buffer, DIR *dir);
void Size(Data *data, char *prefix, char *buffer, DIR *dir);
void Name(Data *data, char *prefix, char *buffer, DIR *dir);
void ParseArgs(Data *data, char **argv);
void Depth(void (*f)(Data *data, char *prefix, char *buffer, DIR *dir), Data *data, int depth, char *prefix);

#endif 
