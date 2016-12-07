#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


#define MAX_TREE_LEN 5000
#define MAX_NAME_LEN 100
#define MAX_FREQ_LEN 256

struct node
{
	int ch;
	int times;
	struct node *left;
	struct node *right;
};

struct queue {
	int head;
	int tail;
};

typedef struct node node;
typedef struct queue queue;

int freq[MAX_FREQ_LEN];
int *code_chart[MAX_FREQ_LEN];
node* heap[MAX_FREQ_LEN + 1];
int heap_top = 1;

// heap_push: node*p -> void
void heap_push(node*p) {
	heap[heap_top] = p;
	int k = heap_top;
	while (k > 1) {
		if ((heap[k / 2]->times) > (heap[k]->times)) {
			node* temp = heap[k];
			heap[k] = heap[k / 2];
			heap[k / 2] = temp;
			k = k / 2;
		}
		else
			break;
	}
	heap_top++;
}

// heap_pop: -> node* p
node* heap_pop() {
	int k = 1;
	node*p = heap[1];
	if (heap_top == 1)
		return heap[1];
	heap[1] = heap[--heap_top];
	while (k * 2 < heap_top) {
		int min = k * 2;
		if (k * 2 + 1 < heap_top && heap[k * 2]->times > heap[k * 2 + 1]->times)
			min = k * 2 + 1;
		if (heap[k]->times > heap[min]->times) {
			node* temp = heap[k];
			heap[k] = heap[min];
			heap[min] = temp;
			k = min;
		}
		else
			break;
	}
	return p;
}


// creat huffman_tree based on heap
node* hufftree() {
	while (heap_top > 2) {
		node* p1 = heap_pop();
		node* p2 = heap_pop();
		node* new_node = (node *)malloc(sizeof(node));
		new_node->left = p1;
		new_node->right = p2;
		new_node->times = p1->times + p2->times;
		new_node->ch = -1;
		heap_push(new_node);
	}
	return heap_pop();
}

// travel: node* htree -> int array
// travel the tree to get a code_chart
void travel(node* tree, int k[], int n) {
	if (tree->left != NULL) {

		k[n] = 0;
		travel(tree->left, k, n + 1);
	}
	if (tree->right != NULL) {
		k[n] = 1;
		travel(tree->right, k, n + 1);
	}
	if (tree->left == NULL && tree->right == NULL) {
		k[n] = 2;
		code_chart[tree->ch] = (int *)malloc(sizeof(int) * (n + 1));
		for (int i = 0; i <= n; i++) {
			code_chart[tree->ch][i] = k[i];
		}
	}
}


// print tree at the begining of the file
void print_tree(FILE *file_out, node* to_print[], queue que) {
	if (que.head >= que.tail) {
		fprintf(file_out, "%d ", -2);
	}
	else {
		node* first = to_print[que.head++];
		fprintf(file_out, "%d ", first->ch);
		if (first->left != NULL)
			to_print[que.tail++] = first->left;
		if (first->right != NULL)
			to_print[que.tail++] = first->right;
		print_tree(file_out, to_print, que);
	}
}

// print binary code for each char
void print_code(FILE *file_out, char* old_file) {
	char input = 0;
	int start = 0;
	unsigned char ch;

	FILE *fl = fopen(old_file, "rb");
	int tmp;
	while ((tmp = fgetc(fl)) != EOF) {
		ch = tmp;
		int *k = code_chart[ch];// k is the code of the character
		int i = 0;
		while (k[i] != 2) {
			//printf("%d", k[i]);
			if (start >= 8) {
				fprintf(file_out, "%c", input);
				input = 0;
				start = 0;
			}
			input = (input << 1) | k[i++];
			start++;
		}
	}
	input = input << (8 - start);
	fprintf(file_out, "%c", input);
	fclose(fl);
}

//file-code: file -> variable-length binary code
// print_tree & print_code
void file_code(char* old_file, node* htree, int sum) {
	char file_name[MAX_NAME_LEN];
	// this part of code committed by Professor Gold
START: printf("Enter filename to write to: \n");
	fgets(file_name, MAX_NAME_LEN, stdin);
	int len = strlen(file_name);
	if (file_name[len - 1] == '\n') {
		file_name[len - 1] = '\0';
	}
	FILE *file_out = fopen(file_name, "wb");
	if (file_out == NULL) {
		printf("Couldn't open '%s' for writing!\n", file_name);
		goto START;
	}

	fprintf(file_out, "%d ", sum);
	// print Huffman tree first
	queue que;
	node* tree[MAX_TREE_LEN];
	tree[0] = htree;
	que.head = 0;
	que.tail = 1;
	print_tree(file_out, tree, que);

	// print coded file
	print_code(file_out, old_file);

	fclose(file_out);
}

void free_tree(node* tree) {
	if (tree != NULL) {
		free_tree(tree->left);
		free_tree(tree->right);
		free(tree);
	}
}

void free_code_chart() {
	for (int i = 0; i < MAX_FREQ_LEN; i++) {
		if (code_chart[i] != NULL)
			free(code_chart[i]);
	}
}

void encode() {
	FILE *fl;
	char file_name[MAX_NAME_LEN];
	unsigned char ch;
	int sum = 0;
START: printf("Please enter the file name:\n");
	fgets(file_name, MAX_NAME_LEN, stdin);

	// this part committed by Professor Gold
	int len = strlen(file_name);
	if (file_name[len - 1] == '\n') {
		file_name[len - 1] = '\0';
	}
	fl = fopen(file_name, "rb");
	if (fl == NULL) {
		printf("File '%s' not found!\n", file_name);
		goto START;
	}

	int tmp;
	while ((tmp = fgetc(fl)) != EOF) {
		// create freq array
		ch = tmp;
		freq[ch]++;
		sum++;
	}
	fclose(fl);

	// create heap and nodes
	for (int i = 0; i < MAX_FREQ_LEN; ++i) {
		if (freq[i] != 0) {
			node* p = (node *)malloc(sizeof(node));
			p->ch = i;
			p->times = freq[i];
			p->left = p->right = NULL;
			heap_push(p);
		}
	}
	int leaf_num = heap_top;

	// create huffman tree
	node* htree = hufftree();

	// tree->code
	int path[MAX_FREQ_LEN];
	travel(htree, path, 0);

	// code file
	file_code(file_name, htree, sum);

	//free memory
	free_tree(htree);
	free_code_chart();
}

/////////////////////////////////////////////////////////////////////////
//decode_tree: char num -> num(boolean)

FILE* decode_tree(FILE* fl, node* to_code[], queue que) {
	int temp;
	fscanf(fl, "%d", &temp);
	if (temp == -2) {
		fscanf(fl, " ");
		return fl;
	}
	node* new_node1 = (node*)malloc(sizeof(node));
	new_node1->ch = temp;
	new_node1->left = new_node1->right = NULL;
	to_code[que.tail++] = new_node1;
	fscanf(fl, "%d", &temp);
	node* new_node2 = (node*)malloc(sizeof(node));
	new_node2->ch = temp;
	new_node2->left = new_node2->right = NULL;
	to_code[que.tail++] = new_node2;

	node* new_node3 = to_code[que.head++];
	while (new_node3->ch != -1) {
		new_node3 = to_code[que.head++];
	}
	new_node3->left = new_node1;
	new_node3->right = new_node2;

	decode_tree(fl, to_code, que);
	return fl;
}

void decode_file(FILE *fl, FILE *file_out, node* htree, int sum) {
	int i = 0;
	unsigned char input = 0;
	int coded_num = 0;
	int bit_code, start;
	node* node_now = htree;
	start = 7;
	input = fgetc(fl);
	while (coded_num < sum) {
		int next = 1;
		while (next) {
			if (start < 0) {
				start = 7;
				input = fgetc(fl);
			}
			bit_code = (input & (1 << start)) >> start;
			start--;
			if (bit_code)
				node_now = node_now->right;
			else
				node_now = node_now->left;
			if (node_now->ch != -1) {
				fprintf(file_out, "%c", (char)node_now->ch);
				next = 0;
				node_now = htree;
			}
		}
		coded_num++;
	}
	fclose(fl);
	fclose(file_out);
}

void decode() {
	FILE *fl;
	char file_name[MAX_NAME_LEN];
	char ch;
	int tree_code = 1;
START: printf("Please enter the file name :\n");
	fgets(file_name, MAX_NAME_LEN, stdin);
	// this part committed by Professor Gold
	int len = strlen(file_name);
	if (file_name[len - 1] == '\n') {
		file_name[len - 1] = '\0';
	}
	fl = fopen(file_name, "rb");
	if (fl == NULL) {
		printf("File '%s' not found!\n", file_name);
		goto START;
	}

	// sum is the total char needed to code
	int sum;
	fscanf(fl, "%d", &sum);

	// create huffman tree first
	node* to_code[MAX_TREE_LEN];
	queue que;
	int code_temp = 0;
	int start = 1;
	que.head = 0;
	que.tail = 1;
	fscanf(fl, "%d", &code_temp);
	to_code[0] = (node*)malloc(sizeof(node));
	to_code[0]->ch = code_temp;
	to_code[0]->left = to_code[0]->right = NULL;
	node* htree = to_code[0];
	fl = decode_tree(fl, to_code, que);

	// start decode
	char file_name2[MAX_NAME_LEN];
START2: printf("Enter filename to write to: \n");
	fgets(file_name2, MAX_NAME_LEN, stdin);
	int len2 = strlen(file_name2);
	if (file_name2[len2 - 1] == '\n') {
		file_name2[len2 - 1] = '\0';
	}
	FILE *file_out = fopen(file_name2, "wb");
	if (file_out == NULL) {
		printf("Couldn't open '%s' for writing!\n", file_name);
		goto START2;
	}

	decode_file(fl, file_out, htree, sum);
	free_tree(htree);
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///main: file -> variable-length binary code/////////////////////
int main() {
	system("dir");
	int k = -1;
	printf("Press 1 for encode, press 0 for decode:\n");
	scanf("%d", &k);
	getchar();
	if (k)
		encode();
	else
		decode();
	return 0;
}

