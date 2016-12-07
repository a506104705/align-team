#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define A 911
#define B 1994827
#define M 15374911
#define MAX_LENGTH 101
#define TRUE 1
#define PI 3.14159

long long x;

long long my_random(long b);

void test_random();

void generate_random();

void compute_pi();

void permutation();

void choice_random();

int *my_sort(int *b, int r);

double random_01();

double normal_distribution();

void test_normal_distribution();

void c14_decay();

int main() {
	// initialize the seed with current time.
	time_t t;
	x = time(&t);
	
	
	char c = 0;

	while(TRUE){
		printf("*************************************************************************\n");
		printf("t: test my random function.\n");
		printf("r: generate some random numbers.\n");
		printf("i: compute Pi.\n");
		printf("p: generate a random permutation.\n");
		printf("c: generate a random choice of r things from n things.\n");
		printf("n: test my random number generator with a standard normal distribution.\n");
		printf("s: simulate the decay of C-14.\n");
		printf("q: quit.\n");
		printf("Pleace enter a character: ");
		fflush(stdin);
		scanf("%c", &c);
		printf("\n");
		if(c == 't'){
			test_random();
		}else if(c == 'r'){
			generate_random();
		}else if(c == 'i'){
			compute_pi();
		}else if(c == 'p'){
			permutation();
		}else if(c == 'c'){
			choice_random();
		}else if(c == 'n'){
			test_normal_distribution();
		}else if(c == 's'){
			c14_decay();
		}else if(c == 'q'){
			break;
		}
	}

	return 0;
}

long long my_random(long b){
	// the linear congruential generator
	// A, B and M are three prime number I choose arbitarily.
	x = (A*x + B) % M;
	// return a pseudo-random integer within [0,b).
	return x % b;
}

void test_random(){
	// test 2, 11, 101, 1001, 10001 for 10000 times
	int i;
	long long sum[5] = {0};
	double average[5] = {0};
	double d[5] = {0.5,5,50,500,5000};
	for(i = 0; i < 10000; i++){
		sum[0] += my_random(2);
		sum[1] += my_random(11);
		sum[2] += my_random(101);
		sum[3] += my_random(1001);
		sum[4] += my_random(10001);
	}
	printf("Test my random number function for 5 different cases, 10,000 times each.\n\n");
	printf("Case:     2\t  11\t  101\t  1001\t  10001\n");
	printf("average:  ");
	for(i = 0; i < 5; i++){
		average[i] = (double) sum[i] / 10000;
		printf("%-7.3f ", average[i]);
	}
	printf("\n");
	printf("Expected: 0.5     5       50      500     5000    \n");
	printf("Deviation:");
	for(i = 0; i < 5; i++){
		d[i] = fabs(d[i] - average[i]) / d[i] * 100;
		printf("%-2.2f%%   ", d[i]);
	}
	printf("\n\n");
}

void generate_random(){
	// this function is to generate n random number within [0, b), and print them.
	int i;
	long b, n;
	long long output;
	printf("Enter the upper boundary of the random number: ");
    scanf("%ld", &b);
	printf("Enter the number of random number you want: ");
    scanf("%ld", &n);
	
	for(i = 0; i < n; i++){
		output = my_random(b);
		printf("%lld\n", output);
	}
	printf("\n");
}

void compute_pi(){
	printf("Throw some beans to a square area with an insribed circle.\n");
	printf("The number of beans that happen to locate within the circle represents the area of the circle.\n");
	printf("We can use it to compute Pi roughly.\n\n");
	printf("Please enter how many beans you want to throw: ");
	long n;
	double count = 0;
	// The radius of the circle is r. Thus, the side length of the square is 2r.
	long r = 5000;
	int x, y, i;
	scanf("%ld", &n);
	
	for(i = 0; i < n; i++){
		x = my_random(2*r - 1) - r;
		y = my_random(2*r - 1) - r;
		if(x*x + y*y <= r*r){
			count++;
		}
	}
	printf("%f\n\n", 4 * count/n);
	// Using this method to compute Pi requires too many computations, but the accuracy is still vary low.
}

void permutation(){
	int i, n;
	printf("This function is to make a random permutation of some strings.\n");
	printf("Please enter the number of strings you want to permutate: ");
	scanf("%d", &n);
	printf("Then enter the strings, each finishes with an enter.\ninput:\n");
	fflush(stdin);
	// a[n][] stores the n string.
	// b[n] stores the random permutation.
	// c[n] helps to generate b[n].
	char a[n][MAX_LENGTH];
	int b[n], c[n];
	for(i = 0; i < n; i++){
		fgets(a[i], MAX_LENGTH, stdin);
	}
	int k, temp;
	for(i = 0; i < n; i++){
		c[i] = i;
	}
	// c[n] is originally: 0, 1, 2, 3 ... n-1
	// Each time we pick a number from the sequence and put it into b[i].
	// And then we replace the picked number with the last number of the sequence,
	// so that we won't pick that number again.
	// Finally, the sequnce of b[n] represents the permutation.
	for(i = 0; i < n; i++){
		k = my_random(n-i);
		b[i] = c[k];
		c[k] = c[n-i-1];
	}
	printf("\noutput:\n");
	for(i = 0; i < n; i++){
		printf("%s", a[b[i]]);
	}
	printf("\n\n");
}

void choice_random(){
	// This function is much like the permutation one.
	// The major difference is that I need to sort the output with the order of input.
	int i, n, r;
	printf("This function is to make a random choice of r strings from n strings.\n");
	printf("Please enter the number of strings you want to choose from: n = ");
	scanf("%d", &n);
	printf("Please enter the number of strings you want: r = ");
	scanf("%d", &r);
	printf("Then enter the strings, each finishes with an enter.\ninput:\n");
	fflush(stdin);
	
	char a[n][MAX_LENGTH];
	int b[r], c[n];
	for(i = 0; i < n; i++){
		fgets(a[i], MAX_LENGTH, stdin);
	}
	int k, temp;
	for(i = 0; i < n; i++){
		c[i] = i;
	}
	for(i = 0; i < r; i++){
		k = my_random(n-i);
		b[i] = c[k];
		c[k] = c[n-i-1];
	}
	my_sort(b, r);
	
	printf("\noutput:\n");
	for(i = 0; i < r; i++){
		printf("%s", a[b[i]]);
	}
	printf("\n\n");
}

int *my_sort(int *b, int r){
	// bubble sort with recursion
	int i, temp;
	int *p = b;
	if(r == 1){
		return b;
	}
	for(i = 0; i < r-1; i++){
		if(*p > *(p+1)){
			temp = *p;
			*p = *(p+1);
			*(p+1) = temp;
		}
		p++;
	}
	my_sort(b, r-1);
}

double random_01(){
	// to generate a random number within (0,1).
	long long a = my_random(9999) + 1;
	double output = (double) a / 10000;
	return output;
}

double normal_distribution(){
	// generate a random number with a standard normal distribution.
	double u, v, z;
	u = random_01();
	v = random_01();
	z = sqrt(-2*log(u)) * cos(2*PI*v);
	return z;	
}

void test_normal_distribution(){
	// test my normal distribution random number generator.
	printf("This function is to test whether the random numbers obey the standard normal distribution.\n");
	printf("Please enter the number of random number you want to test:\n");
	printf("n = ");
	double z;
	long a[6] = {0};
	double b[6];
	long i;
	long n;
	scanf("%ld", &n);
	printf("\n");
	for(i = 0; i < n; i++){
		z = normal_distribution();
		if(z < -2){
			a[0]++;
		}else if(z < -1){
			a[1]++;
		}else if(z < 0){
			a[2]++;
		}else if(z < 1){
			a[3]++;
		}else if(z < 2){
			a[4]++;
		}else{
			a[5]++;
		}
	}
	printf("interval: < -2    [-2,-1)  [-1,0)   [0,1)    [1,2)    > 2\n");
	printf("result:   ");
	for(i = 0; i < 6; i++){
		b[i] = (double)a[i] / n * 100;
		printf("%-0.2f%%   ", b[i]);
	}
	printf("\nexpected: 2.3%%    13.5%%    34.2%%    34.2%%    13.5%%    2.3%%\n");
	printf("\nWhen n is larger than 10,000, the result is very satisfying.\n ");
	printf("\n\n");
	
}

void c14_decay(){
	printf("Archaeologists use a method called radiocarbon dating to determine the age of an organic object.\n");
	printf("Living things collect C-14 when they are alive.\n");
	printf("After death, the amount of C-14 in their body cannot change but decay.\n");
	printf("Thus we can evaluate when did they died by examining the amount of C-14 remaining in their body.\n");
	printf("\nThis functiong is to simulate the decay of C-14.\n");
	printf("Beginning with 1 million C-14 atoms, after a period of time, how many of them remains?\n");
	int hl = 5730; // the half-life period of c14 is 5730 years.
	long long n = 1000000;
	long long count = 0;
	long y, i, j, t;
	printf("Please enter the length of time in year: ");
	scanf("%ld", &y);
	t = y / hl;
	for(i = 0; i < t; i++){
		for(j = 0; j < n; j++){
			if(my_random(2) == 1){
				count++;
			}
		}
		n = count;
		count = 0;
	}
	printf("We got 1,000,000 atoms.\n");
	printf("After %ld years, ", y);
	printf("only %lld left.\n", n);
	printf("\n\n");
}















