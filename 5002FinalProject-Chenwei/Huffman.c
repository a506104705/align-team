/* 
 * CS5002 Final Project
 * Chenwei Yin
 * 12/06/2016
 */

/* 
 * Huffman.c reads in a file, compresses it into a new file
 * and print out the binary code been compressed with a codebook
 * to interpret it.
 */

/*
 * Function (print_tree) is copied from binary_tree.c for debugging
 * Function (free_tree) refers to binary_tree.c for free memory
 * Read and write file method are copied from file_demo.c
 * Write bits method is copied from bitwriter.c
 * Read bits method refers to bitreader.c
 * The method to build a huffman tree refers to Tree.c by Mohamed Ennahdi El Idrissi
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_FILENAME_LENGTH 100
#define MAX_LENGTH 1000000
#define FALSE 0
#define TRUE 1
#define MAX_LETTER 27

struct node
{
    char letter;
    int freq;
    struct node *left;
    struct node *right;
};

// get letters only
char* clean_input(char mystring[]) {

    char *clean_str = malloc(MAX_LENGTH * sizeof(char));;
    for (int i = 0, j = 0; i < strlen(mystring); i++) {

        if ((65 <= mystring[i]) && (mystring[i] <= 90)) {
            clean_str[j] = mystring[i];
            j++;
        } else if ((97 <= mystring[i]) && (mystring[i] <= 122)) {
            clean_str[j] = toupper(mystring[i]);
            j++;
        } 
    }
    free(clean_str);
    return clean_str;
}

// count frequency and number of letters
int count_freq(struct node *arr[MAX_LETTER], char* clean_str, int letter_num) {

    int index;
    for (int j = 0; j < strlen(clean_str); j++) {
        index = clean_str[j] - 65;
        if(arr[index] == NULL) {
            arr[index] = malloc(sizeof(struct node));
            arr[index]->letter = clean_str[j];
            arr[index]->freq = 1;
            arr[index]->left = NULL;
            arr[index]->right = NULL;
            letter_num += 1;
        } else {
            arr[index]->freq += 1;
        }
    }
    return letter_num;
}

// find the min node index
int find_min(struct node *arr[MAX_LETTER], int min) {
    int index = 0;
    int possi_min;
    while (((arr[index] == NULL) || (index == min) || (arr[index]->freq < 0))
            && (index < MAX_LETTER)) {
        index++;
    }
    possi_min = index;
    for (int i = 0; i < MAX_LETTER; i++) {
        if ((arr[i] != NULL) && (arr[i]->freq < arr[possi_min]->freq) && 
            (i != min) && (arr[i]->freq >= 0)) {
            possi_min = i;
        } 
    }
    return possi_min;
}

// create Huffman tree
struct node *create_tree(struct node *arr[MAX_LETTER], int letter_num) {
    int min_temp, min;

    struct node *temp;
    struct node *tree;
    
    
    while (letter_num != 1) {
        
        min_temp = find_min(arr, -1);
        min = find_min(arr, min_temp);
        
        tree = malloc(sizeof(struct node));
        tree->letter = '@';
        tree->freq = arr[min]->freq + arr[min_temp]->freq;
        tree->left = arr[min_temp];
        temp = malloc(sizeof(struct node));
        temp->letter = arr[min]->letter;
        temp->freq = arr[min]->freq;
        temp->left = arr[min]->left;
        temp->right = arr[min]->right;
        tree->right = temp;
        arr[min] = NULL;
        arr[min_temp] = tree;
        
        letter_num--;
    }
    return tree;
}

// traverse the tree and store binary codes into codebook
void traverse_tree(struct node *tree, int depth, char bi_code[], char *codebook[]) {
    if(tree->left == NULL && tree->right == NULL) {
        bi_code[depth] = '\0';
        codebook[tree->letter] = malloc(sizeof(char));
        strcpy(codebook[tree->letter], bi_code);
        printf("Letter: %c\tEncode: %s\n", tree->letter, codebook[tree->letter]);
    } 
    if (tree->left != NULL) {
        bi_code[depth] = '0';
        traverse_tree(tree->left, depth+1, bi_code, codebook);
    }
    if (tree->right != NULL) {
        bi_code[depth] = '1';
        traverse_tree(tree->right, depth+1, bi_code, codebook);
    }
}

// for debug
void print_tree(struct node *tree) {
    if (tree == NULL) {
        printf("x");
    } else {
        printf("(%c", tree->letter);
        printf(" %d ", tree->freq);
        print_tree(tree->left);
        printf(" ");
        print_tree(tree->right);
        printf(")");
    }
}

// free the memory used to store tree
void free_tree(struct node *tree) {
    if (tree != NULL) {
        free_tree(tree->left);
        free_tree(tree->right);
        free(tree);
    }
}

// free the memory used to store codebook
void free_codebook(char *codebook[]) {
    for(int i = 0; i < 256; i++) {
        if (codebook[i] != NULL) {
            free(codebook[i]);
        }
    }
}

int main() {
	char filename[MAX_FILENAME_LENGTH];

	// get file
	printf("Enter name of file to open: ");
    fgets(filename, MAX_LENGTH, stdin);

     /* Trim newline if necessary */
    int len = strlen(filename);
    if (filename[len-1] == '\n') {
        filename[len-1] = '\0';
    }

    FILE *file_in;
    file_in = fopen(filename, "r");
    if (file_in == NULL) {
        printf("No file '%s' found!\n", filename);
    }
 	char file_buffer[MAX_LENGTH];
    int file_index = 0;
    while (TRUE) {
        if (feof(file_in)) {  /* End of file */
            file_buffer[file_index] = '\0';
            break;
        } else {
            file_buffer[file_index] = fgetc(file_in);
            file_index++;
        }
    }
    fclose(file_in);

    //get letters
    char *clean_str = clean_input(file_buffer);
    struct node *arr[MAX_LETTER];
    int letter_num = 0;
    //count frequency
    letter_num = count_freq(arr, clean_str, letter_num);

    //create huffman tree
    struct node *tree = create_tree(arr, letter_num);

    char bi_code[MAX_LENGTH];
    char *codebook[256];
    printf("\nCodebook: (frequency descending)\n");
    // traverse the tree to create codebook
    traverse_tree(tree, 0, bi_code, codebook);
    
    // compression
    printf("\nEnter filename to write to: ");
    fgets(filename, MAX_FILENAME_LENGTH, stdin);
    len = strlen(filename);
    if (filename[len-1] == '\n') {
        filename[len-1] = '\0';
    }

    FILE *file_out = fopen(filename, "w");
    if (file_out == NULL) {
        printf("Couldn't open '%s' for compression!", filename);
        exit(0);
    }

    char bitstring[MAX_LENGTH];
    bitstring[MAX_LENGTH-1] = '\0';
    int bits_to_write;
    
    for (int i = 0; i < strlen(clean_str); i++) {
        if (codebook[clean_str[i]] != NULL) {
             strcat(bitstring, codebook[clean_str[i]]);
        }
    }
    bits_to_write = strlen(bitstring) - 1;

    char buffer = 0;
    fprintf(file_out, "%c", (char) bits_to_write);

    for (int i = 0; i < bits_to_write; i++) {
        /* Assume bitstring is all 1's and 0's. */
        int bit_position = i % 8;  /* How far from the left the bit is */
        if (bitstring[i] == '1') {
            char bit_mask = 1 << (7 - bit_position);
            buffer |= bit_mask;
        }
        if (bit_position == 7) {
            /* We've set all 8 bits; write to file */
            fprintf(file_out, "%c", buffer);
            /* Now zero out the buffer again */
            buffer = 0;
        }
    }
    if (bits_to_write % 8 != 0) {
        /* Number of bits wasn't even multiple of 8, so we need to flush the
         * buffer
         */
        fprintf(file_out, "%c", buffer);
    }

    printf("\nCompressed successfully in %s.\n", filename);
    fclose(file_out);

    // open the file again, read in the binary code and print out
    file_in = fopen(filename, "r");
    printf("\nThe binary code been compressed is: \n");

    /* The first char of the file was the bit count.
     * We'll assume it's accurate and not try to catch the case
     * where the count is too big for the file.
     */
    char bits_to_read = fgetc(file_in);
    char buffer1;
    char code[MAX_LENGTH];
    code[MAX_LENGTH-1] = '\0';

    for (int i = 0; i < strlen(bitstring)-1; i++) {
        if (i % 8 == 0) {
            /* Time to pull a new byte into the buffer. */
            buffer1 = fgetc(file_in);
        }
        int bit_mask = 1 << (7 - (i % 8));  /* Same as writer */
        if (bit_mask & buffer1) {
            strcat(code, "1");
        } else {
            strcat(code, "0");
        }

    }
    printf("%s", code);
    printf("\n");
    fclose(file_in);

    free_tree(tree);
    free_codebook(codebook);
}