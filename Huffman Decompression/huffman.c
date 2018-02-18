#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "huffman.h"

char setBits(char byte, char replacements) {	// bytes looking to be replaced 						
	char new_byte = byte | replacements;		// will always be least significants
	return new_byte;							// and always zero
}

Forest* createHuffmanTreeBit(char* compressed_file) {
	FILE* fp_compress = fopen(compressed_file, "r");

	if (fp_compress == NULL) {
		return NULL;
	}

	long int char_in_header = 0;
	fread(&char_in_header, sizeof(char_in_header), 1, fp_compress);
	fseek(fp_compress, 3 * sizeof(long int), SEEK_SET);
	
	Forest* top_tree = NULL;
	int i = 0;
	int bitIndex = 7;
	char bit = 0;
	unsigned char byte = 0;
	unsigned char new_byte1 = 0;
	unsigned char new_byte2 = 0;

	fread(&byte, sizeof(byte), 1, fp_compress);
	while (i < char_in_header) {
		bit = getBits(byte, bitIndex, -1);					// get 1 or 0 (from 1g1e01o1h...)
		bitIndex--;	

		if (bitIndex == -1) {								// if bit was the last bit in the byte being read,
			bitIndex = 7;									// reset j, read next byte
			fread(&byte, sizeof(byte), 1, fp_compress);		
		}

		if (bit == 1) {
			new_byte1 = getBits(byte, bitIndex, 0);			// get remaining bits from original byte
			fread(&byte, sizeof(byte), 1, fp_compress);		// read next byte						
			
			if (7 - bitIndex == 1) {						
				new_byte2 = getBits(byte, bitIndex+1, -1);	// -1 means, get bit at position bitIndex+1
				new_byte1 = new_byte1 << 1;
				new_byte1 = setBits(new_byte1, new_byte2);
			}

			else {											// if reading more than one bit from new byte
				new_byte2 = getBits(byte, 7, bitIndex + 1);
				new_byte1 = new_byte1 << (7 - bitIndex);
				new_byte1 = setBits(new_byte1, new_byte2);
			}

			i++;
			Forest* new_tree = malloc(sizeof(*new_tree));
			new_tree->ascii = new_byte1;
			new_tree->frequency = 1;
			new_tree->next = NULL;
			new_tree->left = NULL;
			new_tree->right = NULL;
			top_tree = stackPush(new_tree, &top_tree);
		}
		else if (bit == 0) {
			if (top_tree->next != NULL) {
				Forest* T1 = stackPop(&top_tree);
				Forest* T2 = stackPop(&top_tree);
				Forest* new_forest = malloc(sizeof(*new_forest));
				new_forest->ascii = -1;
				new_forest->frequency = 0;
				new_forest->next = NULL;
				new_forest->left = T2;
				new_forest->right = T1;
				stackPush(new_forest, &top_tree);
			}
			else {
				break;
			}
		}

	}

	fclose(fp_compress);
	return(top_tree);
}

void freeForest(Forest* head) {
	if (head == NULL) {
		return;
	}
	freeForest(head->left);
	freeForest(head->right);
	free(head);
}

void createOriginalString(char* compressed, char* output, Forest* head) {
	FILE* fp_compress = fopen(compressed, "r");
	FILE* fp_output = fopen(output, "w");

	long int char_in_compress = 0;
	long int char_in_header = 0;
	long int char_in_original = 0;

	fread(&char_in_compress, sizeof(long int), 1, fp_compress);
	fread(&char_in_header, sizeof(long int), 1, fp_compress);
	fread(&char_in_original, sizeof(long int), 1, fp_compress);

	fseek(fp_compress, 3*sizeof(long int) + char_in_header, SEEK_SET);
	Forest* temp = head;
	int j = 0;

	while (j < char_in_original) {
		unsigned char ch = 0;
		fread(&ch, sizeof(ch), 1, fp_compress);
		for (int i = 7; i >= 0; i--) {
			char bit = getBits(ch, i, -1);
			if (bit == 1) {
				temp = temp->right;
				if (temp == NULL) {
					break;
				}
				if (temp->ascii != -1) {
					fprintf(fp_output, "%c", temp->ascii);
					j++;
					if (j == char_in_original) {
						break;
					}
					temp = head;
				}
			}
			else if (bit == 0) {
				temp = temp->left;
				if (temp == NULL) {
					break;
				}
				if (temp->ascii != -1) {
					fprintf(fp_output, "%c", temp->ascii);
					j++;
					if (j == char_in_original) {
						break;
					}
					temp = head;
				}
			}
		}
		if (temp == NULL) {
			break;
		}
	}
	fclose(fp_compress);
	fclose(fp_output);
}

Forest* createHuffmanTreeChar (char* compressed_file, int* depth) {
	FILE* fp_compress = fopen(compressed_file, "r");

	if (fp_compress == NULL) {
		return NULL;
	}

	long int char_in_header = 0;
	fread(&char_in_header, sizeof(char_in_header), 1, fp_compress);
	fseek(fp_compress, 3 * sizeof(long int), SEEK_SET);

	Forest* top_tree = NULL;
	int i = 0;
	unsigned char temp = 0;

	while(i < char_in_header) {
		fread(&temp, sizeof(temp), 1, fp_compress);
		i++;

		if (temp == '1') {
			fread(&temp, sizeof(temp), 1, fp_compress);
			i++;
			Forest* new_tree = malloc(sizeof(*new_tree));
			new_tree->ascii = temp;
			new_tree->frequency = 1;
			new_tree->next = NULL;
			new_tree->left = NULL;
			new_tree->right = NULL;
			top_tree = stackPush(new_tree, &top_tree);
		}

		else if (temp == '0') {
			if (top_tree->next != NULL) {
				Forest* T1 = stackPop(&top_tree);
				Forest* T2 = stackPop(&top_tree);
				Forest* new_forest = malloc(sizeof(*new_forest));
				*depth += 1;
				new_forest->ascii = -1;
				new_forest->frequency = 0;
				new_forest->next = NULL;
				new_forest->left = T2;
				new_forest->right = T1;
				stackPush(new_forest, &top_tree);
			}
			else {
				break;
			}
		}
	}

	fclose(fp_compress);
	return(top_tree);
}

void printCodeBook(Forest* head, int i, char* path, FILE* fp) {
	if (head->ascii != -1) {
		fprintf(fp, "%c:", head->ascii);
		path[i] = '\0';
		fprintf(fp, "%s\n", path);
		return;
	}
	else {
		path[i] = '0';
		printCodeBook(head->left, i + 1, path, fp);
		path[i] = '1';
		printCodeBook(head->right, i + 1, path, fp);
	}
	
}

Forest* stackPop(Forest** head) {
	if (*head == NULL) {
		return NULL;
	}
	Forest* popped_tree = *head;
	*head = (*head)->next;
	popped_tree->next = NULL;
	return popped_tree;
}

Forest* stackPush(Forest* pusher, Forest** head) {
	pusher->next = *head;
	*head = pusher;
	return pusher;
}

char getBits (char byte, int left, int right) { //if right == -1 means get bit @ position left
	if (right == -1) {
		char bit = byte >> left;
		bit = bit & 1;
		return bit;
	}
	char bit = byte >> right;
	int i = pow(2, left - right + 1) - 1;
	bit = bit & i;
	return bit;
}
