#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>

#define MAXSIZE 9
#define ITER_LIMIT 100000000
#define BUF 50
#define MAXSTR MAXSIZE*MAXSIZE*2
#define LOOPTEST 10000
/* Largest standard ASCII value */
#define MAXASCII 127

typedef enum book {
    BLACK = (int)'K',
    RED = (int)'R',
    GREEN = (int)'G',
    YELLOW = (int)'Y',
    BLUE = (int)'B',
    MAGNETA = (int)'M',
    CYAN = (int)'C',
    WHITE = (int)'W',
    EMPTY = (int)'.'
} book;

struct bookcase {
   book bc[MAXSIZE][MAXSIZE];
   struct bookcase* next;
   struct bookcase* parent;
   int h;
   int w;
};
typedef struct bookcase bookcase;

struct list {
   bookcase* start;
   bookcase* end;
   int size;
};
typedef struct list list;

typedef enum bool {false,true} bool;

/* Return true if bcase has any empty spaces */
bool is_full(bookcase* b);
/* Returns true if count of any colour exceeds the width */
bool colour_width(bookcase *b);
/* Iterates through list, returns 1 if happy bcase is found */
bool solve(list* l);
/* Generates all possible children from bcase b and adds to list l,
returns 1 if happy bcase is found */
bool generate_children(list* l, bookcase* b);
/* Generates child by swapping end book of shelf1 with end space of
shelf2 returns 1 if happy bcase is found */
bool generate_child(list* l, bookcase* b, int shelf1, int shelf2);

/* Returns 1 if both check_cols and check_rows are true */
bool is_happy(bookcase* b);
/* Returns 1 if first col of each shelf is different (ignoring empty) */
bool check_cols(bookcase* b);
/* Return 1 if members of every shelf are the same (ignoring empty) */
bool check_rows(bookcase* b);

/* Returns 1 if there is space on shelf2 to move end book of shelf1 */
bool can_move(bookcase* b, int shelf1, int shelf2);
/* Returns col of end book of given shelf */
int get_end_book(bookcase* b, int shelf);
/* Returns col of end space of given shelf */
int get_end_space(bookcase* b, int shelf);

/* Opens file ensuring a NULL pointer isn't returned */
FILE* open_file(char* filename, char* mode);
/* Verifies first line of .bc contains 2 or 3 numbers below MAXSIZE */
bool first_line_valid(bookcase* b, const char* string);
/* Loads bcase from file into start of list l */
void load_file(list* l, char* filename);
/* Reads bcase into array and verifies it is valid */
void load_bookcase(bookcase* b, FILE* file);
/* Verifies all books of array are valid */
bool validate_bc(bookcase* b);
/* Returns 1 if c is a valid colour or a space */
bool is_valid(char c);

/* Allocate memory to initiate list */
list* init_list(void);
/* Adds bcase b to end of list l */
void add_to_list(list* l, bookcase* b);
/* Returns the size of the list */
int list_size(list* l);
/* Frees list */
bool free_list(list* l);

/* Prints the number of steps to soluton */
void print_solution(list* l);
/* Prints bcase for each step in colour */
void print_verbose_solution(list* l);
/* Prints individual bookcase */
void print_bookcase(bookcase* b);
/* Changes colour of text to corresponding book colour */
void colourise(char c);

/* Same as Calloc but exits and prints error if memory cannot be allocated */
void* a_calloc(int n, size_t size);
/* Prints string s and exits programme with EXIT_FAILURE */
void on_error(const char* s);
/* Checks correct number of input arguments */
bool check_args(int arg);
/* Swaps a with b */
void swap(book* a, book* b); /* TESTED */
/* Returns a pointer to a duplicate of bcase b */
bookcase* duplicate_bc(bookcase* b);

/* Compares bcase b1 with b2, returns 1 if their bcase array, h and w are equal */
bool test_compare(bookcase* b1, bookcase* b2);
/* Copies array a into b->bc */
void test_copy_array_2_bc(bookcase* b, book a[MAXSIZE][MAXSIZE], int h, int w);
/* Stringifies the bcase matrix */
void test_tostring(bookcase* b, char str[MAXSTR]);

void test(void);
