#include "header.h"
#include "neillsimplescreen.h"

int main(int argc, char* argv[])
{
   list* bcases = init_list();
   test();

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
      if (generate_children(l, current_p)) {
         return true;
      }
      current_p = current_p->next;
   } while(list_size(l) < ITER_LIMIT && current_p != NULL);
   return false;
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

void test(void)
{
   int i;
   list* list_p;
   char test_str[MAXSTR];
   bookcase* duplicate_p;
   /* t1, t2, t3 and t4 for testing check_cols, check_rows and is_happy */
   book t1[MAXSIZE][MAXSIZE] = {{'G','G','.'}, {'B','B','.'}, {'.','.','.'}};
   book t2[MAXSIZE][MAXSIZE] = {{'g','g','b'}, {'b','b','.'}, {'.','.','.'}};
   book t3[MAXSIZE][MAXSIZE] = {{'G','G','G'}, {'B','B','.'}, {'B','.','.'}};
   book t4[MAXSIZE][MAXSIZE] = {{'.','.','.'}, {'.','.','.'}, {'.','.','.'}};
   /* t5 for testing get_end_space, get_end_book and can_move */
   book t5[MAXSIZE][MAXSIZE] = {{'R','B','G'}, {'R','B','.'}, {'R','.','.'}, {'.','.','.'}};
   /* t6, t7 and t8 for testing validate_bc */
   book t6[MAXSIZE][MAXSIZE] = {{'R','B','G','\0'}, {'R','B','.','\0'}, {'R','.','.','\0'}, {'.','.','.','\0'}};
   book t7[MAXSIZE][MAXSIZE] = {{'R','b','G','G'}, {'r','B','\0','\0'}, {'R','b','G','G'}, {'R','B','G','G'}};
   book t8[MAXSIZE][MAXSIZE] = {{'R','b','G','G'}, {'r','B','Z','.'}, {'R','b','G','G'}, {'R','B','G','G'}};
   /* for testing solve and generate children */
   book t9[MAXSIZE][MAXSIZE] = {{'R','G'}, {'R','G'}, {'R','G'}};
   book t10[MAXSIZE][MAXSIZE] = {{'Y','.', '.'}, {'B','B','Y'}, {'Y','B', '.'}};
   /* for testing colour_width and is_full */
   book t11[MAXSIZE][MAXSIZE] = {{'Y','Y','Y'}, {'B','B','G'}, {'G','G','B'}};
   book t12[MAXSIZE][MAXSIZE] = {{'Y','Y','Y'}, {'Y','.','.'}, {'B','.','.'}};
   bookcase* b1 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   bookcase* b2 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   bookcase* b3 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   bookcase* b4 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   bookcase* b5 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   bookcase* b6 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   bookcase* b7 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   bookcase* b8 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   bookcase* b9 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   bookcase* b10 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   bookcase* b11 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   bookcase* b12 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   bookcase* b13 = (bookcase*)(a_calloc(1, sizeof(bookcase)));
   test_copy_array_2_bc(b1, t1, 3, 3);
   test_copy_array_2_bc(b2, t2, 3, 3);
   test_copy_array_2_bc(b3, t3, 3, 3);
   test_copy_array_2_bc(b4, t4, 3, 3);
   test_copy_array_2_bc(b5, t5, 4, 3);
   test_copy_array_2_bc(b6, t6, 4, 4);
   test_copy_array_2_bc(b7, t7, 4, 3);
   test_copy_array_2_bc(b8, t8, 4, 3);
   test_copy_array_2_bc(b9, t9, 3, 2);
   test_copy_array_2_bc(b10, t10, 3, 3);
   test_copy_array_2_bc(b11, t11, 3, 3);
   test_copy_array_2_bc(b12, t12, 3, 3);
   /* Test first_line_valid */
   assert(first_line_valid(b13, "2 2 2\n"));
   assert(first_line_valid(b13, "2 2 0\n"));
   assert(first_line_valid(b13, "2 2 10\n"));
   assert(first_line_valid(b13, "2 2\n"));

   assert(!first_line_valid(b13, "0 2\n"));
   assert(!first_line_valid(b13, "2 0\n"));
   assert(!first_line_valid(b13, "0 0\n"));
   assert(!first_line_valid(b13, "0 2 2\n"));
   assert(!first_line_valid(b13, "2 0 2\n"));
   assert(!first_line_valid(b13, "0 0 2\n"));
   assert(!first_line_valid(b13, "10 2\n"));
   assert(!first_line_valid(b13, "2 10\n"));
   assert(!first_line_valid(b13, "10 10\n"));
   assert(!first_line_valid(b13, "2\n"));
   assert(!first_line_valid(b13, "2,2\n"));
   assert(!first_line_valid(b13, "a b\n"));
   assert(!first_line_valid(b13, "test\n"));
   assert(!first_line_valid(b13, ""));

   /* Testing colour_width and is_full */
   assert(!colour_width(b1));
   assert(!colour_width(b3));
   assert(!colour_width(b11));
   assert(colour_width(b12));

   assert(!is_full(b1));
   assert(!is_full(b3));
   assert(is_full(b9));
   assert(is_full(b11));

   /* Testing generate_child */
   list_p = init_list();
   assert(!generate_child(list_p, b2, 1, 2));
   swap(&b2->bc[1][1], &b2->bc[2][0]);
   assert(test_compare(list_p->start, b2));
   swap(&b2->bc[1][1], &b2->bc[2][0]);

   assert(!generate_child(list_p, b2, 0, 2));
   swap(&b2->bc[0][2], &b2->bc[2][0]);
   assert(test_compare(list_p->end, b2));
   swap(&b2->bc[0][2], &b2->bc[2][0]);

   assert(generate_child(list_p, b2, 0, 1));
   swap(&b2->bc[0][2], &b2->bc[1][2]);
   assert(test_compare(list_p->end, b2));
   swap(&b2->bc[0][2], &b2->bc[1][2]);
   free_list(list_p);

   /* Test generate_children (no happy bcase in children) */
   list_p = init_list();
   assert(!generate_children(list_p, b10));
   test_tostring(list_p->start, test_str);
   assert(strcmp(test_str, ".|.|.|B|B|Y|Y|B|Y") == 0);
   test_tostring(list_p->start->next, test_str);
   assert(strcmp(test_str, "Y|Y|.|B|B|.|Y|B|.") == 0);
   test_tostring(list_p->start->next->next, test_str);
   assert(strcmp(test_str, "Y|.|.|B|B|.|Y|B|Y") == 0);
   test_tostring(list_p->start->next->next->next, test_str);
   assert(strcmp(test_str, "Y|B|.|B|B|Y|Y|.|.") == 0);
   free_list(list_p);

   /* Test generate_children (happy bcase in children) */
   list_p = init_list();
   assert(generate_children(list_p, b2));
   test_tostring(list_p->start, test_str);
   assert(strcmp(test_str, "g|g|.|b|b|b|.|.|.") == 0);
   free_list(list_p);

   /* Test solve using puzzles which are know to be solvable/unsolvable */
   list_p = init_list();
   add_to_list(list_p, b1);
   assert(solve(list_p));
   free_list(list_p);

   list_p = init_list();
   add_to_list(list_p, b2);
   assert(solve(list_p));
   free_list(list_p);

   list_p = init_list();
   add_to_list(list_p, b3);
   assert(solve(list_p));
   free_list(list_p);

   list_p = init_list();
   add_to_list(list_p, b5);
   assert(solve(list_p));
   free_list(list_p);

   list_p = init_list();
   add_to_list(list_p, b9);
   assert(!solve(list_p));
   free_list(list_p);

   list_p = init_list();
   add_to_list(list_p, b10);
   assert(solve(list_p));
   free_list(list_p);

   /* Testing init_list, add_to_list and free_list */
   list_p = init_list();
   assert(list_p != NULL);
   assert(list_size(list_p) == 0);
   test_tostring(list_p->start, test_str);
   assert(strcmp(test_str, "") == 0);
   add_to_list(list_p, b1);
   assert(list_size(list_p) == 1);
   assert(list_p->start->parent == NULL);
   assert(test_compare(list_p->start, b1));
   add_to_list(list_p, b2);
   assert(list_size(list_p) == 2);
   add_to_list(list_p, b3);
   add_to_list(list_p, b4);
   add_to_list(list_p, b4);
   assert(list_size(list_p) == 5);
   for (i = 1; i < LOOPTEST; i++)
   {
      add_to_list(list_p, b1);
      assert(test_compare(list_p->end, b1));
      assert(list_size(list_p) == i + 5);
   }
   assert(free_list(list_p));
   assert(free_list(NULL));

   /* Test test_compare */
   assert(test_compare(b1, b1));
   assert(test_compare(b3, b3));
   assert(!test_compare(b1, b2));
   assert(!test_compare(b1, b4));

   /* Test duplicate_bc */
   duplicate_p = duplicate_bc(b1);
   assert(test_compare(duplicate_p, b1));
   assert(duplicate_p->parent = b1);
   free(duplicate_p);
   duplicate_p = duplicate_bc(b5);
   assert(test_compare(duplicate_p, b5));
   assert(duplicate_p->parent = b5);
   free(duplicate_p);
   duplicate_p = duplicate_bc(b4);
   assert(test_compare(duplicate_p, b4));
   assert(duplicate_p->parent = b4);
   free(duplicate_p);

   /* Test check_cols */
   assert(check_cols(b1));
   assert(check_cols(b2));
   assert(!check_cols(b3));
   assert(check_cols(b4));
   assert(!check_cols(b5));
   /* Test check_rows */
   assert(check_rows(b1));
   assert(!check_rows(b2));
   assert(check_rows(b3));
   assert(check_rows(b4));
   assert(!check_rows(b5));
   /* Test is_happy */
   assert(is_happy(b1));
   assert(!is_happy(b2));
   assert(!is_happy(b3));
   assert(is_happy(b4));

   /* Test can_move */
   assert(!can_move(b5, 0, 0));
   assert(can_move(b5, 0, 1));
   assert(can_move(b5, 0, 2));
   assert(can_move(b5, 0, 3));

   assert(!can_move(b5, 1, 0));
   assert(!can_move(b5, 1, 1));
   assert(can_move(b5, 1, 2));
   assert(can_move(b5, 1, 3));

   assert(!can_move(b5, 2, 0));
   assert(can_move(b5, 2, 1));
   assert(!can_move(b5, 2, 2));
   assert(can_move(b5, 2, 3));

   assert(!can_move(b5, 3, 0));
   assert(!can_move(b5, 3, 1));
   assert(!can_move(b5, 3, 2));
   assert(!can_move(b5, 3, 3));

   /* Test get_end_space */
   assert(get_end_space(b5, 1) == 2);
   assert(get_end_space(b5, 2) == 1);
   assert(get_end_space(b5, 3) == 0);

   /* Test get_end_book */
   assert(get_end_book(b5, 0) == 2);
   assert(get_end_book(b5, 1) == 1);
   assert(get_end_book(b5, 2) == 0);
   assert(get_end_book(b5, 3) == 0);

   /* Test test_tostring */
   test_tostring(b1, test_str);
   assert(strcmp(test_str, "G|G|.|B|B|.|.|.|.") == 0);
   test_tostring(b2, test_str);
   assert(strcmp(test_str, "g|g|b|b|b|.|.|.|.") == 0);
   test_tostring(b3, test_str);
   assert(strcmp(test_str, "G|G|G|B|B|.|B|.|.") == 0);
   test_tostring(b4, test_str);
   assert(strcmp(test_str, ".|.|.|.|.|.|.|.|.") == 0);
   test_tostring(b5, test_str);
   assert(strcmp(test_str, "R|B|G|R|B|.|R|.|.|.|.|.") == 0);
   test_tostring(NULL, test_str);
   assert(strcmp(test_str, "") == 0);

   /* Test swap */
   swap(&b1->bc[0][0], &b1->bc[1][0]);
   test_tostring(b1, test_str);
   assert(strcmp(test_str, "B|G|.|G|B|.|.|.|.") == 0);

   swap(&b1->bc[0][0], &b1->bc[0][1]);
   test_tostring(b1, test_str);
   assert(strcmp(test_str, "G|B|.|G|B|.|.|.|.") == 0);

   swap(&b1->bc[1][1], &b1->bc[2][0]);
   test_tostring(b1, test_str);
   assert(strcmp(test_str, "G|B|.|G|.|.|B|.|.") == 0);

   swap(&b1->bc[0][0], &b1->bc[1][0]);
   swap(&b1->bc[0][0], &b1->bc[1][0]);
   swap(&b1->bc[0][0], &b1->bc[1][0]);
   swap(&b1->bc[0][0], &b1->bc[1][0]);
   swap(&b1->bc[0][0], &b1->bc[1][0]);
   test_tostring(b1, test_str);
   assert(strcmp(test_str, "G|B|.|G|.|.|B|.|.") == 0);

   test_tostring(NULL, test_str);
   assert(strcmp(test_str, "") == 0);

   /* Test is_valid */
   assert(is_valid('K'));
   assert(is_valid('R'));
   assert(is_valid('G'));
   assert(is_valid('Y'));
   assert(is_valid('B'));
   assert(is_valid('M'));
   assert(is_valid('C'));
   assert(is_valid('W'));
   assert(is_valid('k'));
   assert(is_valid('r'));
   assert(is_valid('g'));
   assert(is_valid('y'));
   assert(is_valid('b'));
   assert(is_valid('m'));
   assert(is_valid('c'));
   assert(is_valid('w'));
   assert(is_valid('.'));
   assert(!is_valid('D'));
   assert(!is_valid('Z'));
   assert(!is_valid('d'));
   assert(!is_valid('z'));
   assert(!is_valid('1'));
   assert(!is_valid('\0'));
   assert(!is_valid(' '));

   /* Test validate_bc */
   assert(validate_bc(b1));
   assert(validate_bc(b2));
   assert(validate_bc(b3));
   assert(validate_bc(b4));
   assert(validate_bc(b5));
   assert(!validate_bc(b6));
   assert(!validate_bc(b7));
   assert(!validate_bc(b8));

   free(b1);
   free(b2);
   free(b3);
   free(b4);
   free(b5);
   free(b6);
   free(b7);
   free(b8);
   free(b9);
   free(b10);
   free(b11);
   free(b12);
   free(b13);
}
