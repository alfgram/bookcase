#include "header.h"
#include "neillsimplescreen.h"

/* Returns 1 if bc array b1 is same as b2 */
bool compare_bcase(bookcase* b1, bookcase* b2);
/* Returns 1 if any duplicates of b appear in list */
bool check_repeat(list* l, bookcase* b);

int main(int argc, char* argv[])
{
   list* bcases = init_list();

   check_args(argc);
   load_file(bcases, argv[1]);
   if (!solve(bcases)) {
      printf("No Solution?\n");
   }
   else {
      print_solution(bcases);
      if (argc == 3 && !strcmp(argv[2], "verbose")) {
         print_verbose_solution(bcases);
      }
   }
   free_list(bcases);
   neillfgcol(white);
   return 0;
}

bool check_repeat(list* l, bookcase* b)
{
   bookcase* current = l->start;
   while (current != b)
   {
      if (compare_bcase(current, b)) {
         return true;
      }
      current = current->next;
   }
   return false;
}

bool solve(list* l)
{
   bookcase* current_p = l->start;
   if (is_happy(current_p)) {
      return true;
   }
   if (is_full(current_p) || colour_width(current_p)) {
      return false;
   }
   do
   {
      if (!check_repeat(l, current_p)) {
         if (generate_children(l, current_p)) {
            return true;
         }
      }
      current_p = current_p->next;
   } while (list_size(l) < ITER_LIMIT && current_p != NULL);
   return false;
}

bool compare_bcase(bookcase* b1, bookcase* b2)
{
   int i, j;
   for (i = 0; i < MAXSIZE; i++)
   {
      for (j = 0; j < MAXSIZE; j++)
      {
         if (b1->bc[i][j] != b2->bc[i][j]) {
            return false;
         }
      }
   }
   return true;
}

bool generate_children(list* l, bookcase* b)
{
   int shelf1, shelf2;
   for (shelf1 = 0; shelf1 < b->h; shelf1++)
   {
      for (shelf2 = 0; shelf2 < b->h; shelf2++)
      {
         if (can_move(b, shelf1, shelf2))
         {
            if (generate_child(l, b, shelf1, shelf2)) {
               return true;
            }
         }
      }
   }
   return false;
}

bool generate_child(list* l, bookcase* b, int shelf1, int shelf2)
{
   int row1_col = get_end_book(b, shelf1);
   int row2_col = get_end_space(b, shelf2);

   swap(&b->bc[shelf1][row1_col], &b->bc[shelf2][row2_col]);
   add_to_list(l, b);
   swap(&b->bc[shelf1][row1_col], &b->bc[shelf2][row2_col]);

   if (is_happy(l->end)) {
      return true;
   }
   return false;
}

bool is_full(bookcase* b)
{
   int shelf, col;
   for (shelf = 0; shelf < b->h; shelf++)
   {
      for (col = 0; col < b->w; col++)
      {
         if (b->bc[shelf][col] == EMPTY) {
            return false;
         }
      }
   }
   return true;
}

bool colour_width(bookcase *b)
{
   int shelf, col;
   int hist[MAXASCII] = {0};
   for (shelf = 0; shelf < b->h; shelf++)
   {
      for (col = 0; col < b->w; col++)
      {
         /* Incriments histogram at index of corresponding ASCII value */
         if (b->bc[shelf][col] != EMPTY && ++hist[(int)b->bc[shelf][col]] > b->w) {
            return true;
         }

      }
   }
   return false;
}

bool can_move(bookcase* b, int shelf1, int shelf2)
{

   if ((shelf1 != shelf2) && (b->bc[shelf1][0] != EMPTY) && (b->bc[shelf2][b->w-1] == EMPTY)) {
      return true;
   }
   return false;
}

int get_end_book(bookcase* b, int shelf)
{
   int col = 0;
   for (col = 0; col < b->w-1; col++)
   {
      if (b->bc[shelf][col+1] == EMPTY) {
         return col;
      }
   }
   return col;
}

int get_end_space(bookcase* b, int shelf)
{
   int col = 0;
   for (col = 0; col < b->w; col++)
   {
      if (b->bc[shelf][col] == EMPTY) {
         return col;
      }
   }
   return col;
}

bool check_rows(bookcase* b)
{
   int shelf, col;
   for (shelf = 0; shelf < b->h; shelf++)
   {
      for (col = 0; col < b->w; col++)
      {
         if ((b->bc[shelf][col] != EMPTY) && (b->bc[shelf][0] != b->bc[shelf][col])) {
            return false;
         }
      }
   }
   return true;
}

bool check_cols(bookcase* b)
{
   int shelf1, shelf2;
   for (shelf1 = 0; shelf1 < b->h-1; shelf1++)
   {
      for (shelf2 = shelf1+1; shelf2 < b->h; shelf2++)
      {
         if (b->bc[shelf1][0] != EMPTY && b->bc[shelf1][0] == b->bc[shelf2][0]) {
            return false;
         }
      }
   }
   return true;
}

bool is_happy(bookcase* b)
{
   if (check_cols(b) && check_rows(b)) {
      return true;
   }
   return false;
}

FILE* open_file(char* filename, char* mode)
{
   FILE* p;
   if ((p = fopen(filename, mode)) == NULL) {
      on_error("Cannot open file\n");
   }
   return p;
}

void load_file(list* l, char* filename)
{
   char buffer[BUF];
   FILE* file = open_file(filename, "r");
   bookcase* b = (bookcase*)a_calloc(sizeof(bookcase), 1);
   if (fgets(buffer, BUF, file) == NULL) {
      on_error("Invalid .bc file\n");
   }
   if (!first_line_valid(b, buffer)) {
      on_error("Invalid .bc file\n");
   }
   load_bookcase(b, file);
   add_to_list(l, b);
   free(b);
}

bool first_line_valid(bookcase* b, const char* string)
{
   size_t line_len = strlen(string);
   if (line_len < 4 || line_len > 7) {
      return false;
   }
   if (sscanf(string,"%d %d", &b->h, &b->w) != 2) {
      return false;
   }
   if (b->h > MAXSIZE || b->h <= 0 || b->w > MAXSIZE || b->w <= 0) {
      return false;
   }
   return true;
}

void load_bookcase(bookcase* b, FILE* file)
{
   int i = 0, j = 0;
   int line_len;
   char buffer[BUF];

   while (fgets(buffer, BUF, file) != NULL)
   {
      line_len = strlen(buffer);
      if (line_len - 1 != b->w) {
         on_error("Invalid .bc file\n");
      }
      for (j = 0; j < line_len; j++)
      {
         b->bc[i][j] = toupper(buffer[j]);
      }
      i++;
   }
   if (i != b->h || !validate_bc(b)) {
      on_error("Invalid .bc file\n");
   }
   fclose(file);
}

bool validate_bc(bookcase* b)
{
   int shelf, col;
   for (shelf = 0; shelf < b->h; shelf++)
   {
      for (col = 0; col < b->w; col++)
      {
         if (!is_valid(b->bc[shelf][col])) {
            return false;
         }
      }
   }
   return true;
}

bool is_valid(char c)
{
   c = toupper(c);
   if (c == BLACK || c == RED || c == GREEN || c == YELLOW ||
   c == BLUE || c == MAGNETA || c == CYAN || c == WHITE || c == EMPTY) {
      return true;
   }
   return false;
}

list* init_list(void)
{
   list* l = (list*) a_calloc(1, sizeof(list));
   return l;
}

void add_to_list(list* l, bookcase* b)
{
   bookcase* child_bc;
   if (l == NULL) {
      return;
   }

   child_bc = duplicate_bc(b);

   if (l->start == NULL) {
      l->start = child_bc;
      l->end = child_bc;
      l->start->parent = NULL;
      l->size = l->size + 1;
      return;
   }

   l->end->next = child_bc;
   l->end = child_bc;
   l->size = l->size + 1;
}

int list_size(list* l)
{
   if (l == NULL || l->start == NULL) {
      return 0;
   }
   return l->size;
}

bool free_list(list* l)
{
   if (l != NULL) {
      bookcase* current = l->start;
      bookcase* tmp;
      while (current != NULL)
      {
         tmp = current;
         current = current->next;
         free(tmp);
      }
      free(l);
   }
   return true;
}

void print_solution(list* l)
{
   int step_count = 0;
   bookcase* current_p = l->end;
   while (current_p != NULL)
   {
      current_p = current_p->parent;
      step_count++;
   }
   printf("%d\n\n", step_count);
}

void print_verbose_solution(list* l)
{
   bookcase* current_p = l->end;
   while (current_p != NULL)
   {
      print_bookcase(current_p);
      current_p = current_p->parent;
   }
}

void print_bookcase(bookcase* b)
{
   int shelf, col;
   for (shelf = 0; shelf < b->h; shelf++)
   {
      for (col = 0; col < b->w; col++)
      {
         colourise(b->bc[shelf][col]);
         printf("%c", b->bc[shelf][col]);
         neillbgcol(black);
      }
      printf("\n");
   }
   printf("\n");
}

void colourise(char c)
{
   switch (c) {
      case BLACK:
      neillbgcol(white);
      neillfgcol(black);
      return;

      case RED:
      neillfgcol(red);
      return;

      case GREEN:
      neillfgcol(green);
      return;

      case YELLOW:
      neillfgcol(yellow);
      return;

      case BLUE:
      neillfgcol(blue);
      return;

      case MAGNETA:
      neillfgcol(magenta);
      return;

      case CYAN:
      neillfgcol(cyan);
      return;

      case WHITE:
      neillfgcol(white);
      return;

      case EMPTY:
      neillfgcol(white);
      return;
   }
}

void* a_calloc(int n, size_t size)
{
   void* v = calloc(n, size);
   if (v == NULL) {
      on_error("Cannot calloc() space");
   }
   return v;
}

void on_error(const char* s)
{
   fprintf(stderr, "%s\n", s);
   exit(EXIT_FAILURE);
}

bool check_args(int arg)
{
   if (arg != 2 && arg != 3) {
      on_error("Incorrent input arguments.\nPlease enter filename. Add optional 'verbose' flag to print bookcase moves.\n");
   }
   return false;
}

void swap(book* a, book* b)
{
   book tmp = *a;
   *a = *b;
   *b = tmp;
}

bookcase* duplicate_bc(bookcase* b)
{
   bookcase* new_bc = (bookcase*)a_calloc(1, sizeof(bookcase));
   *new_bc = *b;
   new_bc->next = NULL;
   new_bc->parent = b;
   return new_bc;
}

bool test_compare(bookcase* b1, bookcase* b2)
{
   int shelf, col;
   for (shelf = 0; shelf < MAXSIZE; shelf++)
   {
      for (col = 0; col < MAXSIZE; col++)
      {
         if (b1->bc[shelf][col] != b2->bc[shelf][col]) {
            return false;
         }
      }
   }
   if (b1->h != b2->h || b1->w != b2->w) {
      return false;
      }
   return true;
}

void test_tostring(bookcase* b, char str[MAXSTR])
{
   int shelf, col;
   char tmp[BUF];
   str[0] = '\0';
   if (b == NULL) {
      return;
   }
   for (shelf = 0; shelf < b->h; shelf++)
   {
      for (col = 0; col < b->w; col++)
      {
         sprintf(tmp, "%c|", b->bc[shelf][col]);
         strcat(str, tmp);
      }
   }
   str[strlen(str)-1] = '\0';
}

void test_copy_array_2_bc(bookcase* b, book a[MAXSIZE][MAXSIZE], int h, int w)
{
   int shelf, col;
   for (shelf = 0; shelf < MAXSIZE; shelf++)
   {
      for (col = 0; col < MAXSIZE; col++)
      {
         b->bc[shelf][col] = a[shelf][col];
      }
   }
   b->h = h;
   b->w = w;
}
