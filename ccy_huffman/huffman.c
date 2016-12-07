/*working reading and writing to file */
/*fix end of file when decompressed the file */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ASCII_TABLE_LENGTH 256
#define INT_MAX	2147483647
#define MAX_BIT_LENGTH_UNSIGNED_SHORT 16
#define MAX_NUMBER_UNSIGNED_SHORT 65535
#define MAX_INPUT_LENGTH 100;
#define MAX_FILENAME_LENGTH 100
/*hex editor*/

/**************************************************************************************************/
/*Creating Data Type*/
/**************************************************************************************************/
/*Define Tree Node data type*/
typedef struct tree_node {
    int character;             /*un-coded character*/
    unsigned long freq;        /*frequency of the character on the imported file*/
    struct tree_node *left;    /*left tree_node */
    struct tree_node *right;   /*right tree_node*/
} node;


/*Define Node for queue*/
typedef struct queue_node {
    struct tree_node *t_node;
    struct queue_node *next;
} queue_node;

/*Define Queue*/
typedef struct queue {
    struct queue_node *head;
    struct queue_node *last;
} queue;

/**************************************************************************************************/
/*global variable */
/**************************************************************************************************/
/*alphabet_table[] stores the frequency of each charactor from input file*/
unsigned long alphabet_table[ASCII_TABLE_LENGTH] ={0};

int character_reading_buffer;/*buffer for for reading char from file*/

int remain_bit = MAX_BIT_LENGTH_UNSIGNED_SHORT; 
unsigned short writing_buffer = 0;

unsigned short reading_buffer = 0;






/**************************************************************************************************/
/*Function Prototype*/
/**************************************************************************************************/
/*Debugging function */
long decimalToBinary(int  n);
void print_queue(struct queue *q);
void print_tree(struct tree_node *n);
void print_alphabet_table();
/*queue function */
struct queue *new_queue();
struct queue_node *dequeue(struct queue *q);

/*File I/O functions*/
int import_file_to_alphabet_table(FILE *fp, char filename[]);
unsigned short get_writing_buffer(int output_val, int output_bit);

/*unknowned functions */
int get_index_of_min_freq (int *table);
void fill_depth_and_binary_table (struct tree_node *root, int *depth_table, int *binary_value_table, int depth, int val);


/**************************************************************************************************/
/*Binary search tree fuctions*/
/**************************************************************************************************/

/*Define constructor of Tree_Node*/
struct tree_node *new_tree_node(int character, int freq){

	struct tree_node *n =malloc(sizeof(struct tree_node));
	n->character = character;
	n->freq = freq;
	n->left = NULL;
	n->right = NULL;
	return n;
}


/**************************************************************************************************/
/*Define queue functions*/
/**************************************************************************************************/

/*Creating Queue*/
struct queue *new_queue() {
    struct queue *q = malloc(sizeof(struct queue));
    q->head = NULL;
    q->last = NULL;
    return q;
}

/*fuction for enqueue_minQ*/
struct queue *enqueue_minQ(struct tree_node *t_node, struct queue *q) {
    // printf("t_node contain %c appear %lu times\n", t_node->character, t_node->freq);

    struct queue_node *new_node = malloc(sizeof(struct queue_node));
    new_node->t_node = t_node;
    new_node->next = NULL;
    
    if (q->head == NULL) {
        q->head = new_node;
        q->last = new_node;
        //printf("q->head is NULL\n");
        return q;
    } else {
        struct queue_node *tmp_low_bound;
        struct queue_node *tmp_height_bound;
        tmp_low_bound = q->head;
        /*t_node is less than head*/
        if(t_node->freq < tmp_low_bound->t_node->freq){
            new_node->next = q->head;
            q->last = q->head;
            q->head = new_node;
            return q;
        }
        /*t_node is in between */
        while(tmp_low_bound->next != NULL){
            tmp_height_bound = tmp_low_bound->next;
            if (t_node->freq < tmp_height_bound->t_node->freq){
                new_node->next = tmp_height_bound;
                tmp_low_bound->next = new_node;
                // print_queue(q);
                return q;
            }
            tmp_low_bound = tmp_height_bound;
        }
        /*t_node is greater than last*/
        q->last->next = new_node;
        q->last = new_node;
        // print_queue(q);

        return q;
    }
    // printf("new_node contain %c appear %lu times\n", new_node->t_node->character, new_node->t_node->freq);
}


struct queue *enqueue(struct tree_node *t_node, struct queue *q) {
    struct queue_node *new_node = malloc(sizeof(struct queue_node));
    new_node->t_node = t_node;
    new_node->next = NULL;
    if (q->head == NULL) {
        q->head = new_node;
        q->last = new_node;
        return q;
    } else {
        q->last->next = new_node;
        q->last = new_node;
        return q;
    }
}

/*fuction for dequeue*/
struct queue_node *dequeue(struct queue *q) {
    if (q->head == NULL) {
        return NULL;
    }
    struct queue_node *to_return = q->head;

    q->head = q->head->next;

    if (q->head == NULL) {
        q->last = NULL;
    }
    return to_return;
}

int is_empty(struct queue *q) {
    if (q->head == NULL)
        return 1;
    return 0;

}


/**************************************************************************************************/
/* helper fuctions for debugging*/
/**************************************************************************************************/

/*A helper function to convert decimal to binary for debugging purpose*/
long decimalToBinary(int  n) {
    int remainder; 
    long binary = 0, i = 1;
    while(n != 0) {
        remainder = n%2;
        n = n/2;
        binary= binary + (remainder*i);
        i = i*10;
    }
    return binary;
}

void print_queue(struct queue *q){
    printf("\nprint_queue: \n");
    struct queue_node *tmp_queue_node =q->head;
    while (tmp_queue_node != NULL){
        printf("enqueue_minQ test contain %d appear %lu times\n", tmp_queue_node->t_node->character, tmp_queue_node->t_node->freq);
        tmp_queue_node = tmp_queue_node->next;
    }
    printf("\n");
}

void print_tree(struct tree_node *n){
    //     if (n == NULL)
    //     return;
    // printf("node %c-%d %lu times has ",n->character,n->character, n->freq);
    // if (n->left == NULL && n->right == NULL)
    //     printf("no chridren\n");
    // else if (n->left != NULL && n->right == NULL)
    //     printf("left node %c-%d %lu times and no right node \n", n->left->character, n->left->character,n->left->freq);
    // else if (n->left == NULL && n->right != NULL)
    //     printf("no left node and has right node %c-%d %lu times and \n", n->right->character, n->right->character,n->right->freq);
    // else if (n->left != NULL && n->right != NULL)
    //     printf("left node %c-%d %lu times and has right node %c-%d %lu times\n", n->left->character,n->left->character,n->left->freq, n->right->character,n->right->character,n->right->freq);
    // else{
    //     printf("error\n");
    // }
    if (n == NULL)
        return;
    printf("node %d times has ",n->character);
    if (n->left == NULL && n->right == NULL)
        printf("no chridren\n");
    else if (n->left != NULL && n->right == NULL)
        printf("left node %d  and no right node \n", n->left->character);
    else if (n->left == NULL && n->right != NULL)
        printf("no left node and has right node %d times and \n", n->right->character);
    else if (n->left != NULL && n->right != NULL)
        printf("left node %d times and has right node %d times\n", n->left->character, n->right->character);
    else{
        printf("error\n");
    }
    print_tree(n->left);
    print_tree(n->right);
}

void print_alphabet_table(){
    for (int i =0 ; i<ASCII_TABLE_LENGTH ; i++){
        if (alphabet_table[i] !=0 ){
            printf("file contain ascii number %d-%c appear %lu times\n", i,i, alphabet_table[i]);
        }
    }
    printf("\n");
}


/**************************************************************************************************/
/*File I/O*/
/**************************************************************************************************/

int import_file_to_alphabet_table(FILE *fp, char filename[]){
    /* opening file for reading */
    fp = fopen(filename, "r");
    if(fp == NULL) 
    {
        return(1);
    }
    while (character_reading_buffer != EOF){
        character_reading_buffer = fgetc(fp);
        alphabet_table [character_reading_buffer] ++;
    }
    fclose(fp);
    return 0;   
}

unsigned short get_writing_buffer(int output_val, int output_bit){
    if(output_bit <= remain_bit){
        writing_buffer <<= output_bit;
        writing_buffer |= output_val;
        remain_bit = remain_bit - output_bit;
        //printf("enough space get_writing_buffer binary %lu\n", decimalToBinary(output_val));
        return 0;
    }

    // printf("not enougth bits: output_bit %d\t", output_bit );
    // printf("remain_bit %d\n", remain_bit );
    writing_buffer = ( (writing_buffer << remain_bit) | (output_val>>(output_bit-remain_bit)) );
    // printf("out of bits: %hu\n",writing_buffer );

    unsigned short full_buffer = writing_buffer;
    
    writing_buffer = 0;
    output_bit -= remain_bit;
    remain_bit = MAX_BIT_LENGTH_UNSIGNED_SHORT; 

    // printf("output_bit %d \n",output_bit );
    // printf("remain_bit - output_bit : %d\n", remain_bit - output_bit );
    output_val = ((output_val<<(remain_bit - output_bit))& MAX_NUMBER_UNSIGNED_SHORT)>>(remain_bit - output_bit);
    // printf("out of bits, output_val %d \t output_bit %d\n",output_val,output_bit);

    get_writing_buffer(output_val, output_bit);
    
    return full_buffer;
}

void serialize(struct tree_node *root,FILE *fp){
    if(root == NULL){
        fputc(3,fp);
        return;
    }
    fputc(root->character,fp);
    serialize(root->right, fp);

    serialize(root->left, fp);
}


struct tree_node* de_serialize(struct tree_node *root, FILE *fp){
    int input_buffer;
    input_buffer = fgetc(fp);
    if(input_buffer == EOF)
        return NULL;
    if(input_buffer == 3)
        return NULL;
    root =  malloc(sizeof(struct tree_node));

    root->character = input_buffer;
    root->right = de_serialize (root->right,fp);
    root->left = de_serialize (root->left,fp);

    return root;
}

/**************************************************************************************************/
/*Unknowned function*/
/**************************************************************************************************/

/*Go through table, get the index of element with min value of frequency
 * return -1 if no miniment found 
 */
int get_index_of_min_freq (int *table){
    int min_freq = INT_MAX;
    int min_index = -1;
    for (int i = 0; i< ASCII_TABLE_LENGTH; i++){
        if (table[i] > 0 && table[i] < min_freq){
            min_index = i;
            min_freq = table[i];
        }
    }

    if (min_freq == INT_MAX){
        return -1; /* did not find the min*/
    }
    table [min_index] = 0;
    return min_index;
}



/*Traverse thorugh tree, assign depth and binanry representation for each tree node*/
void fill_depth_and_binary_table (struct tree_node *root, int depth_table[256], int binary_value_table[256], int depth, int val){
    if(root->left == NULL && root->right == NULL){
        depth_table[root->character] = depth;
        binary_value_table[root->character] =val;
        return;
    }

    else if(root->left != NULL && root->right == NULL){
        fill_depth_and_binary_table (root->left, depth_table,binary_value_table,depth+1,(val<<1));
        return;
    }
    else if(root->left == NULL && root->right != NULL){
        fill_depth_and_binary_table (root->right, depth_table,binary_value_table,depth+1,(val<<1)+1);
        return;
    } 

    else if(root->left != NULL && root->right != NULL) {
        fill_depth_and_binary_table (root->left, depth_table,binary_value_table,depth+1,(val<<1));
        fill_depth_and_binary_table (root->right, depth_table,binary_value_table,depth+1,(val<<1)+1);

    }

}

void find_null_and_replace (int new_character, struct tree_node *root){
    if (root == NULL){
        return;
    }
    if (root->left !=NULL && root->left->left == NULL && root->left->character == 0){
        root->left = new_tree_node(new_character,0);
        return;
    }
    if (root->right !=NULL &&root->right->right == NULL && root->right->character == 0){
        root->right = new_tree_node(new_character,0);
        return;
    }
    find_null_and_replace(new_character,root->left);
    find_null_and_replace(new_character,root->right);
    return;
}

/**************************************************************************************************/
/* Main function*/
/**************************************************************************************************/

int main (){

    int option =99;
    char input_filename[MAX_FILENAME_LENGTH];
    char output_filename[MAX_FILENAME_LENGTH];
    printf("Welcome to Huffman Code, please choose what you want to do:\n");
    while (option){

        start: printf("Enter 0 for exit the program\n");
        printf("Enter 1 for compress a file\n");
        printf("Enter 2 for decompressing a file\n");
        scanf("%d%*c", &option);/*%*c read extra character without process it */


        if (option == 1){

            /*Initial global variable*/
            for(int i = 0; i< ASCII_TABLE_LENGTH; i++)
                alphabet_table[i] = 0 ;
            character_reading_buffer = 0;/*buffer for for reading char from file*/
            remain_bit = MAX_BIT_LENGTH_UNSIGNED_SHORT; 
            writing_buffer = 0;
            reading_buffer = 0;

            printf("Enter filename to compress: ");
            fgets(input_filename, MAX_FILENAME_LENGTH-1, stdin);
            input_filename[strlen(input_filename)-1] = '\0';
            // strncpy(input_filename, "test_input_file1.txt", MAX_FILENAME_LENGTH);

            printf("Enter output filename to compress: ");
            fgets(output_filename, MAX_FILENAME_LENGTH-1, stdin);
            output_filename[strlen(output_filename)-1] = '\0';
            // strcat(output_filename, "comp");
            // strncpy(output_filename, "./output/test_input_file.txt", MAX_FILENAME_LENGTH);


            /*input file name*/
            //char input_filename[] = "test_input_file1.txt";
            FILE *fp;
            if (import_file_to_alphabet_table(fp,&input_filename[0])){
                perror("Error opening file: ");
                goto start;
            }

            alphabet_table[0] =1;/*adding NULL to indicate end of reading buffer during decompressing process*/
            /*Adding end of file charactor*/
            
            printf("\nBegin compressing:\n");
            printf("Counting frequency of characters...\n");
            /**debugging*/
            print_alphabet_table();


            /*make a buffer table*/
            /*creating a copy of alphabet_table*/
            int buffer_table[ASCII_TABLE_LENGTH];
            for(int i = 0 ; i< ASCII_TABLE_LENGTH; i++){
                buffer_table[i] = alphabet_table[i]; 
            }

            /* Build min-queue */
            int index;
            int number_of_char = 0;
            struct queue *q = new_queue();
            for (int i =0 ; i<ASCII_TABLE_LENGTH ; i++){
                index = get_index_of_min_freq(&buffer_table[0]);
                if(index < 0)/*found a valid index, index is -1 if min is not found*/
                break;
                /*Debug printing*/
                //printf("min-enqueue_minQ: found min freq of character %c-%d appear %lu times\n", index, index, alphabet_table[index]);
                enqueue_minQ(new_tree_node(index,alphabet_table[index]), q);
                number_of_char++;
            }

             /* Build huffman codes tree*/
            struct tree_node *tmp_tree_node;
            /*Looping start with 1 because first tree contain two elements in the array*/
            for (int i=1 ; i<number_of_char ; i++){
                tmp_tree_node = malloc(sizeof(struct tree_node));
                tmp_tree_node->left = dequeue(q)->t_node;
                tmp_tree_node->right =dequeue(q)->t_node;
                tmp_tree_node->freq = tmp_tree_node->left->freq + tmp_tree_node->right->freq;
                enqueue_minQ(tmp_tree_node,q);
            /* Debuging messages*/
            //print_queue(q);
            }

            struct tree_node *root = dequeue(q)->t_node;

            // /*Debugging message*/
            // printf("Printing Tree\n");
            // print_tree(root);


             /*Find left most node*/
            struct tree_node *left_most_parent =root;
            while (left_most_parent->left->left != NULL){
                left_most_parent = left_most_parent->left;
            }
            /*Find parent of of NULL tree node
            *replace it with left most tree node
            */

            find_null_and_replace(left_most_parent->left->character,root);
            left_most_parent->left = new_tree_node(0,0);

            int depth_table [ASCII_TABLE_LENGTH] ={0}; /*Array to store the depth of each each element in the tree*/
            int binary_value_table [ASCII_TABLE_LENGTH] ={0};/* Array to store the binanry representation in the tree*/
            fill_depth_and_binary_table(root, depth_table,binary_value_table,0,0);

            /* Debugging messaging
             * Printing Binanry representation of each character
             */
            printf("\nAssign binanry representation... \n");
            for (int i = 0; i< ASCII_TABLE_LENGTH; i++){
                if(alphabet_table[i]){
                    printf("ascii character %d-%c has frequency:%lu, depth:%d, bianary representation: %ld\n",
                        i, i, alphabet_table[i],
                        depth_table[i],
                        decimalToBinary(binary_value_table[i]));
                }
            }

    //**********************************************************************************************
            printf("\nSaving Huffman Code Tree to file...\n");
            FILE *serialize_file;
            serialize_file = fopen(output_filename,"w");
            serialize(root,serialize_file);
            fclose(serialize_file);


            // /* convert and save the file */
            printf("\nCompressing...\n");
            FILE *write_b_ptr;
            write_b_ptr = fopen(output_filename,"a");  // w for write, b for binary
            fp = fopen(input_filename , "r");/*open original file*/
            if(fp == NULL) /*check if file is found in the directory*/
            {
                perror("Error opening file");
                return(-1);
            }

            unsigned short tmp_unsigned_short;
            int i =0;
            do{
                character_reading_buffer=0; /*initialize reading buffer for reading one char from file*/
                character_reading_buffer = fgetc(fp);
                tmp_unsigned_short = get_writing_buffer(binary_value_table[character_reading_buffer], depth_table[character_reading_buffer]);
       // printf("character_reading_buffer: %c i %d,tmp_unsigned_short %ld \n",character_reading_buffer, i ,decimalToBinary(tmp_unsigned_short));
                if (tmp_unsigned_short != 0 ){
                    fwrite(&tmp_unsigned_short, sizeof(unsigned short), 1, write_b_ptr);
                    printf("writing : %10d \t %016ld \n",tmp_unsigned_short, decimalToBinary(tmp_unsigned_short));
                }
            }
            while (character_reading_buffer != EOF);

        /*save not full buffer*/
            writing_buffer <<= remain_bit;
            if (writing_buffer != 0){
                fwrite(&writing_buffer, sizeof(unsigned short), 1, write_b_ptr);
                printf("writing : %10d \t %016ld \n",writing_buffer, decimalToBinary(writing_buffer));
            }
            writing_buffer = 0;
            fclose(write_b_ptr);
            fclose(fp);
            printf("Compressing sucessful. save to %s\n\n\n\n\n",output_filename);
        }

        else if(option == 2){

            /*Initial global variable*/


            character_reading_buffer = 0;/*buffer for for reading char from file*/
            remain_bit = MAX_BIT_LENGTH_UNSIGNED_SHORT; 
            writing_buffer = 0;
            reading_buffer = 0;
            struct tree_node *root;
            struct tree_node *tmp_tree_node;

            printf("Enter filename to decompress: ");
            fgets(input_filename, MAX_FILENAME_LENGTH-1, stdin);
            input_filename[strlen(input_filename)-1] = '\0';
            // strncpy(input_filename, "./output/test_input_file.txt", MAX_FILENAME_LENGTH);

            printf("Enter filename for output file: ");
            fgets(output_filename, MAX_FILENAME_LENGTH-1, stdin);
            output_filename[strlen(output_filename)-1] = '\0';
            // strncpy(output_filename, "./output/test_input_file_decompressed.txt", MAX_FILENAME_LENGTH);

            FILE *de_serialize_file_read;
            de_serialize_file_read = fopen(input_filename,"r");
            struct tree_node *de_serialize_root;
            // printf("start deserialize\n");
            de_serialize_root = de_serialize(de_serialize_root,de_serialize_file_read);
            // printf("\nend deserialize\n");
            //fclose(de_serialize_file_read);

            printf("de_serialize tree\n");
            print_tree(de_serialize_root);

            int depth_table_DS[256];
            int binary_value_table_DS[256];
            for(int i =0; i<256; i++){
                depth_table_DS [i] = 0; /*Array to store the depth of each each element in the tree*/
                binary_value_table_DS [i] = 0;/* Array to store the binanry representation in the tree*/
            }

            printf("\nRetriving Huffman Tree ...\n");
            fill_depth_and_binary_table(de_serialize_root, depth_table_DS,binary_value_table_DS,0,0);
            for (int i = 0; i< ASCII_TABLE_LENGTH; i++){
                if(alphabet_table[i]){
                    printf("%c-%d has frequency:%lu, depth:%d, bianary representation: %ld\n",
                        i, i, alphabet_table[i],
                        depth_table_DS[i],
                        decimalToBinary(binary_value_table_DS[i]));
                }
            }

   // **********************************************************************************************
    /*read the binary file and convert it to txt*/
            printf("\nDecompressing file\n");
            int read_count = 0;
            FILE *read_b_ptr;
            FILE *decompressed;

            decompressed = fopen(output_filename,"w");
            read_b_ptr = de_serialize_file_read;
            tmp_tree_node = de_serialize_root;
            if (read_b_ptr == NULL)
            {
                printf("Error to open file!");
                return -1;
            }
            int loop_control =0;
            while(1){
                if (!fread(&reading_buffer,2,1,read_b_ptr)){
                    printf("found end of file\n");
                    printf("\nDecompressing complete. save file to %s \n\n\n",output_filename);
                    break;
                }
                read_count++;
            // printf("read_count %d reading_buff %016ld\n",read_count, decimalToBinary(reading_buffer));
                for(int i = 0 ; i< MAX_BIT_LENGTH_UNSIGNED_SHORT; i++){
                    int bit = ((reading_buffer<<1) & (MAX_NUMBER_UNSIGNED_SHORT+1))>> MAX_BIT_LENGTH_UNSIGNED_SHORT;
                    if (tmp_tree_node->left == NULL || tmp_tree_node->right == NULL){
                        printf(" writing %c\n",tmp_tree_node->character);
                        fputc(tmp_tree_node->character,decompressed);

                        tmp_tree_node = de_serialize_root;
                    }
                    if(bit){
                        tmp_tree_node = tmp_tree_node->right;
                    }else{
                        tmp_tree_node = tmp_tree_node->left;
                    }
                    reading_buffer<<=1;
                }
                loop_control++;
            }
            fclose(read_b_ptr);
            fclose(decompressed);
        }
        else if (option == 0){
            printf("Exited. Thanks to using Huffman Coding program. \n");

        }
        else{
            printf("option %d is not a valid option, please re-enter a valid option: \n",option );
        }


   //  //**********************************************************************************************
   //  // print depth_table 
   //  // for (int i = 0; i< ASCII_TABLE_LENGTH; i++){
   //  //     if(depth_table[i]>0)
   //  //         printf("depth table %d has depth of %d \n",i, depth_table[i]);
   //  // }

   //  // /*print binary_value_table */
   //  // for (int i = 0; i< ASCII_TABLE_LENGTH; i++){
   //  //     if(binary_value_table[i]>0)
   //  //         printf("binary_value_table %d has value of %ld \n",i, decimalToBinary(binary_value_table[i]));
   //  // }


   //  /*test for enqueue_minQ*/
   //  // print_queue(q);

   //  // /**test for dequeue*/
   //  // struct queue_node *tmp_queue_node;
   //  // for (int i=0; i<number_of_char ; i++){
   //  //  tmp_queue_node = dequeue(q);
   //  //  printf("tmp_queue_node contain %c appear %lu times\n", tmp_queue_node->t_node->character, tmp_queue_node->t_node->freq);
   //  // }

    }
    return 0;
}


