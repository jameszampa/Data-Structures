#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

int main(int argc, char* argv[]) {
	if (argc < 4) {
		return EXIT_FAILURE;
	}
	if (strcmp(argv[1], "-c") == 0) {
		if (argc == 5) {
			FILE* fp = fopen(argv[3], "w");
			int tree_depth = 0;
			Forest* head = createHuffmanTreeChar(argv[2], &tree_depth);
			char* path = malloc(sizeof(*path)*tree_depth);
			printCodeBook(head, 0, path, fp);
			fclose(fp);
			createOriginalString(argv[2], argv[4], head);
			freeForest(head);
			free(path);
			return EXIT_SUCCESS;
		}
	}
	if (strcmp(argv[1], "-b") == 0) {
		if (argc == 4) {
			FILE* fp = fopen(argv[3], "w");
			Forest* head = createHuffmanTreeBit(argv[2]);
			createOriginalString(argv[2], argv[3], head);
			fclose(fp);
			freeForest(head);
			return EXIT_SUCCESS;
		}
	}
	
	return EXIT_FAILURE;
}