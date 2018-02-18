#include <stdio.h>

typedef struct _Forest{
	int ascii;
	long int frequency;
	struct _Forest* left;
	struct _Forest* right;
	struct _Forest* next;
}Forest;

Forest* createHuffmanTreeBit(char* compressed_file);
void freeForest(Forest* head);
void printCodeBook(Forest* head, int i, char* path, FILE* fp);
Forest* createHuffmanTreeChar(char* compressed_file, int* depth);
Forest* stackPop();
Forest* stackPush(Forest* pusher, Forest** head);
void createOriginalString(char* compressed, char* output, Forest* head);
char getBits(char byte, int left, int right);  