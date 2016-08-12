#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "hash.h"

/*
  742:
  Expand procedures such as quoted? and make-procedure into sequences
  of elementary list-structure memory operations, and implement these
  operations on our register machine.

  However, this would make our evaluator very long, obscuring the
  basic structure with details.  To clarify the presentation, we will
  include as primitive operations of the register machine the syntax
  procedures given in Section 4.1.2 and the procedures for
  representing environments and other run-time data given in sections
  Section 4.1.3 and Section 4.1.4.

  In order to completely specify an evaluator that could be programmed
  in a low-level machine language or implemented in hardware, we would
  replace these operations by more elementary operations, using the
  list-structure implemention we described in Section 5.3.
*/

typedef void (*voidFunction)(void);

/*
  Setting debugging level.

  Most of functions have a printline(2), i.e. debugLevel for printline
  is 2.  When debugging a piece of code, set the local debugging level
  lower, i.e. to 1, then after debugging is done set it to level 3 and
  above.  You may locally #define a debugging level for each of the
  sections, and centrally control here.
*/

#define debugLevel 2
#define debug(level,etc) {if (level <= debugLevel) etc}
#define printline(level) debug(level, printf("%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);)
#define printfn(s) printf("%s,%d:%s:",__FUNCTION__,__LINE__,s);
#define printfnr(s,r) printfn(s);reg_print(r);newline();
#define hline() { printf("----------------------------------------------------------------------\n"); }
#define reg_set(A,B,C) A.type=B; A.val=(REG_DATA) C;
#define goto_reg(A) ((voidFunction) A.val)();
void newline () { printf("\n"); }

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define red()     { printf(ANSI_COLOR_RED); }
#define green()   { printf(ANSI_COLOR_GREEN); }
#define yellow()  { printf(ANSI_COLOR_YELLOW); }
#define blue()    { printf(ANSI_COLOR_BLUE); }
#define magenta() { printf(ANSI_COLOR_MAGENTA); }
#define cyan()    { printf(ANSI_COLOR_CYAN); }
#define reset()   { printf(ANSI_COLOR_RESET); }

// 501
//----------------------------------------------------------------------
// typedef
//----------------------------------------------------------------------
// Define a boolean
typedef enum { false, true } bool;

// Define a general data type
typedef enum {
  p_string,
  p_symbol,
  p_label,
  p_null,
  p_pair,
  p_bool,
  p_primitive,
  p_integer,     // number
  p_float,       // number
  p_read_skip,   // used in parsing
  p_read_quote,  // used in parsing
  p_read_comment // used in parsing
} p_type;
typedef unsigned long REG_DATA;

typedef struct {
  p_type type;
  REG_DATA val;
} REG;

typedef struct {
  REG *list; size_t count; size_t max_elements;
} REG_array_info_t;

//----------------------------------------------------------------------
// error
void error (char *s) { printf ("%s\n",s); exit (1); }
REG reg_print ();
void reg_error (char *s, REG r) {
  printf ("%s: ", s); reg_print (r); newline(); exit (1); }

//----------------------------------------------------------------------
// Forward declarations

bool is_pair ();
REG  car ();
REG  cdr ();

bool              reg_array_find_element ();
REG_array_info_t *reg_array_info_new ();
void              reg_array_info_delete ();
void              reg_array_add ();

void reg_print_pair_val (REG r) {
  printf ("(pair,%ld)",r.val);
}

void reg_print_pair_simple (REG r) {
  printf ("(pair,");
  reg_print (car (r));
  printf (",");
  reg_print (cdr (r));
  printf (")");
}

void reg_print_pair_internal (REG r, REG_array_info_t *a_ptr, int depth, bool iscdr) { printline(4);
  
  debug (4, printf("(depth,%d)",depth););
  if (depth == 0) {printf("(depth,%d)",depth); return;}

  if (reg_array_find_element (a_ptr, r))
    printf ("(...)");
  else {
    reg_array_add (a_ptr, r);
    REG car_r = car (r);
    REG cdr_r = cdr (r);
    if (!(iscdr)) printf("(");
    if (is_pair (car_r)) {
      debug (4, printf ("(printing car, depth=%d)", depth););
      reg_print_pair_internal (car_r, a_ptr, depth-1, false);
    } else {
      reg_print (car_r);
    }
    printf (",");
    if (is_pair (cdr_r)) {
      debug (4, printf ("(pair:%ld)", cdr_r.val););
      reg_print_pair_internal (cdr_r, a_ptr, depth, true);
    } else {
      reg_print (cdr_r);
    }
    if (!(iscdr)) printf (")");
  }
}

void reg_print_pair (REG r) { printline(4);
  // Keep track of circular lists
  int depth = 10;
  REG_array_info_t *a_ptr = reg_array_info_new ();
  reg_print_pair_internal (r, a_ptr, depth, false);
  reg_array_info_delete (a_ptr);
}

//----------------------------------------------------------------------
// Conversion

double to_float (REG_DATA n) { double *d = (double *) &n; return *d; }
long int to_int (REG_DATA n) { long int *d = (long int *) &n; return *d; }
REG_DATA int_to_reg_data (long int n) {
  REG_DATA *d = (REG_DATA *) &n; return *d; }
REG_DATA float_to_reg_data (double n) {
  REG_DATA *d = (REG_DATA *) &n; return *d; }

REG s_ok        = { p_symbol, (REG_DATA) "ok"};
REG reg_print (REG r) { printline(4);
  switch (r.type) {
  case (p_string):      printf ("(p_string,%s)", (char *) r.val); break;
  case (p_symbol):      printf ("(p_symbol,%s)", (char *) r.val); break;
  case (p_label):       printf ("(p_label,%s)", (char *) r.val); break;
  case (p_null):        printf ("(p_null)"); break;
  case (p_pair):        reg_print_pair (r); break;
  case (p_bool):        printf ("(p_bool,%c)", (r.val == true) ? 't':'f'); break;
  case (p_primitive):   printf ("(p_primitive)"); break;
  case (p_integer):     printf ("(p_integer,%ld)", (long int) r.val); break;
  case (p_float):       printf ("(p_float,%lf)", to_float (r.val)); break;
  case (p_read_skip):   printf ("(p_read_skip)"); break;
  case (p_read_quote):  printf ("(p_read_quote)"); break;
  case (p_read_comment): printf ("(p_read_comment)"); break;
  default:
    break;
  }
  return s_ok;
}

//----------------------------------------------------------------------
// registers
//----------------------------------------------------------------------
// Register globals
REG reg_exp, reg_val, reg_continue, reg_env, reg_unev, reg_argl, reg_proc;
REG *reg_new() { return (REG *) calloc(1,sizeof(REG)); }

//----------------------------------------------------------------------
// constants/symbols
//----------------------------------------------------------------------
REG the_empty_environment = {p_null, 0};
REG reg_null = {p_null,0};
REG reg_skip = {p_read_skip,0};
REG reg_comment = {p_read_comment,0};
REG reg_quote = {p_read_quote, 0};
REG reg_false = {p_bool, 0};
REG reg_true = {p_bool, 1};
REG reg_one = {p_integer, 1};
REG reg_zero = {p_integer, 0};

// Reserved symbols
REG s_quote     = { p_symbol, (REG_DATA) "quote"};
REG s_set       = { p_symbol, (REG_DATA) "set!"};
REG s_define    = { p_symbol, (REG_DATA) "define"};
REG s_if        = { p_symbol, (REG_DATA) "if"};
REG s_lambda    = { p_symbol, (REG_DATA) "lambda"};
REG s_begin     = { p_symbol, (REG_DATA) "begin"};
REG s_false     = { p_symbol, (REG_DATA) "false"};
REG s_true      = { p_symbol, (REG_DATA) "true"};
REG s_procedure = { p_symbol, (REG_DATA) "procedure"}; 
REG s_primitive = { p_symbol, (REG_DATA) "primitive"};
REG s_unknown_procedure_type_error = { p_symbol, (REG_DATA) "unknown-procedure-type"};

// Add new symbols below
REG **s_reserved_set;
size_t s_reserved_set_size;

void s_reserved_set_init () {

  REG *s[] = {&s_quote, &s_set, &s_define, &s_if, &s_lambda, &s_begin,
	      &s_ok, &s_false, &s_true, &s_procedure, &s_primitive,
	      &s_unknown_procedure_type_error
  };

  s_reserved_set_size = sizeof(s) / sizeof(REG *);
  debug (3, printf("Reserved set size: %ld\n", s_reserved_set_size););
  s_reserved_set = (REG **) calloc(s_reserved_set_size,sizeof(REG *));
  for (size_t i = 0; i < s_reserved_set_size; i++)
    s_reserved_set[i] = s[i];
}

#define HASHTABLE_SIZE 65536
hashtable_t *st_p; // symbol table pointer

// Caster for reg
void ht_set_reg (hashtable_t *ht_p, hashtable_key_t *key_p, REG *r) {
  ht_set (ht_p, key_p, (hashtable_value_t) r); }

REG *ht_get_reg (hashtable_t *ht_p, char *s) {
  hashtable_key_t key = {strlen(s),s};
  return (REG *) ht_get (ht_p, &key);
}

// Create symbol table
void st_create () {
  s_reserved_set_init ();

  st_p = ht_create (HASHTABLE_SIZE, NULL, NULL);
  // Insert symbols
  for (size_t i = 0; i < s_reserved_set_size; i++) {
    debug (3, printf("Inserting %s\n",(char *) s_reserved_set[i]->val););
    ht_set_reg (st_p, ht_new_key ((char *) s_reserved_set[i]->val), s_reserved_set[i]);
  }
}

void st_print () {
  for (size_t i = 0; i < s_reserved_set_size; i++) {
    debug (3, printf("Looking for %s\n", (char *) s_reserved_set[i]->val););
    REG *r = ht_get_reg (st_p, (char *) s_reserved_set[i]->val);
    if (r) {
      debug (3, printf("Found %s\n",(char *) r->val););
    } else {
      debug (3, printf("Not in table\n"););
    }
  }
}

//----------------------------------------------------------------------
// Memory
//----------------------------------------------------------------------
#define MEMORY_SIZE 10240
REG the_cars[MEMORY_SIZE];
REG the_cdrs[MEMORY_SIZE];

size_t free_idx;

// Could add a check if it is a pair?
REG car    (REG r) { return the_cars [r.val]; }
REG cdr    (REG r) { return the_cdrs [r.val]; }
REG caar   (REG r) { return car (car (r)); }
REG cdar   (REG r) { return cdr (car (r)); }
REG cadr   (REG r) { return car (cdr (r)); }
REG cddr   (REG r) { return cdr (cdr (r)); }
REG caadr  (REG r) { return car (car (cdr (r))); }
REG caddr  (REG r) { return car (cdr (cdr (r))); }
REG cdadr  (REG r) { return cdr (car (cdr (r))); }
REG cadddr (REG r) { return car (cdr (cdr (cdr (r)))); }

void set_car (REG out, REG in) { the_cars [out.val] = in; }
void set_cdr (REG out, REG in) { the_cdrs [out.val] = in; }

REG cons (REG a, REG b) { printline(3);
  REG n;
  the_cars [free_idx] = a;
  the_cdrs [free_idx] = b;
  reg_set (n, p_pair, free_idx);
  free_idx++;
  debug (3, printf("(cons,%ld)",free_idx););
  if (free_idx >= MEMORY_SIZE) {
    error ("Out of memory");
    exit(1);
  }
  return n;
}

bool is_null (REG r);

REG append (REG x, REG y) { return is_null (x) ? y : cons (car (x), append (cdr (x), y)); }

// Why does this work?
// (define list (lambda x x))
// ((lambda x x) 2 3 5 7 11)
// (2 3 5 7 11)

REG empty_arglist () { return reg_null; }

bool is_list (REG r) { return (is_null (r) || is_list (cdr (r))); }

REG list (int count, ...) { printline (3);
  va_list va;
  REG *ptr = (REG *) calloc (count, sizeof (REG));
  va_start (va, count);
  for (int i=0; i < count;i++) {
    ptr[i] = va_arg (va, REG);
  }
  va_end (va);
  REG r = reg_null;
  for (long int i = (count -1); i >= 0; i--) {
    debug (4, reg_print (ptr[i]););
    r = cons ( ptr[i], r );
  }
  free (ptr);
  return r;
}

REG map (REG (*fn)(REG), REG l) {
  if (is_null (l))
    return reg_null;
  else
    return cons (fn (car (l)), map (fn, cdr (l)));
}

size_t length (REG r) {
  if (! (is_list (r))) {
    reg_error ("In length: register is not a list.", r);
    exit (1);
  }
  size_t temp = 0;
  while (true) {
    if (is_null(r)) {
      break;
    } else {
      r = cdr (r);
      temp++;
    }
  }
  return temp;
}

//----------------------------------------------------------------------
// Stack
//----------------------------------------------------------------------
// 713 : stacks needs to make a copy of register

#define STACK_SIZE 1024
REG stack[STACK_SIZE];
size_t stack_idx;
REG restore () { printline (1);
  debug (1,
	 {
	   printf ("Restoring: ");
	   reg_print (stack[stack_idx-1]); newline();
	 });
  if (stack_idx == 0) { error ("Stack is empty"); exit(1); }
  return  stack[--stack_idx];
}

void save (REG r) { printline (1);
  debug (1,
	 {
	   printf ("Saving: ");
	   reg_print (r); newline();
	 });
  if (stack_idx >= STACK_SIZE) { error ("Stack is full"); exit(1); }
  stack[stack_idx++] = r;
}

//----------------------------------------------------------------------
// Forward declarations
//----------------------------------------------------------------------
bool is_self_evaluating (REG);
bool is_variable (REG);
bool is_quoted (REG);
bool is_assignment (REG);
bool is_definition (REG);
bool is_if (REG);
bool is_lambda (REG);
bool is_begin (REG);
bool is_application (REG);
void ev_self_eval ();
void ev_variable ();
void ev_quoted ();
void ev_assignment ();
void ev_definition ();
void ev_if ();
void ev_lambda ();
void ev_begin ();
void ev_application ();
void unknown_expression_type ();

//----------------------------------------------------------------------
// eval-dispatch
//----------------------------------------------------------------------
void eval_dispatch() { printline (1);
  if (is_self_evaluating    (reg_exp)) {   ev_self_eval();
  } else if (is_variable    (reg_exp)) {    ev_variable();
  } else if (is_quoted      (reg_exp)) {      ev_quoted();
  } else if (is_assignment  (reg_exp)) {  ev_assignment();
  } else if (is_definition  (reg_exp)) {  ev_definition();
  } else if (is_if          (reg_exp)) {          ev_if();
  } else if (is_lambda      (reg_exp)) {      ev_lambda();
  } else if (is_begin       (reg_exp)) {       ev_begin();
  } else if (is_application (reg_exp)) { ev_application();
  } else                               { unknown_expression_type();
  }
  // goto_reg ( reg_continue );
  // The calling function will determine where to go next
}

//----------------------------------------------------------------------
// is_quoted etc
//----------------------------------------------------------------------
bool is_eq (REG a, REG b)    {
  if (a.type == b.type)
    if (a.type == p_symbol)
      if ((strcmp ((const char *) a.val, (const char *) b.val) == 0))
	return true;
      else // strcmp
	return false;
    else // not symbol
      if (a.val == b.val)
	return true;
      else
	return false;
  else // type
    return false;
}

REG reg_is_eq (REG a, REG b) {
  return is_eq (a,b) ? reg_true : reg_false;
}

// The only difference between the two
// is that for pairs, we check the content
// i.e. two lists which have same contents are
// "equal", even if they may have different
// references
bool is_equal (REG a, REG b) {
  if (a.type == b.type) {
    if (a.type == p_symbol) {
      if ((strcmp ((const char *) a.val, (const char *) b.val) == 0))
	return true;
      else // strcmp
	return false;
    } else { // not symbol
      if (a.type == p_pair)
	return (is_equal (car (a), car (b)) && is_equal (cdr (a), cdr (b)));
      else { // not pair
	if (a.val == b.val)
	  return true;
	else
	  return false;
      }
    }
  } else // type
    return false;
}

REG reg_is_equal (REG a, REG b) {
  return is_equal (a,b) ? reg_true : reg_false;
}

bool is_null        (REG r)     { return (r.type == p_null)    ? true : false; }
bool is_number      (REG r)     { return (r.type == p_integer) || (r.type == p_float) ? true : false; }
bool is_integer     (REG r)     { return (r.type == p_integer); }
bool is_float       (REG r)     { return (r.type == p_float); }
bool is_string      (REG r)     { return (r.type == p_string)  ? true : false; }
bool is_symbol      (REG r)     { return (r.type == p_symbol)  ? true : false; }
bool is_pair        (REG r)     { return (r.type == p_pair)    ? true : false; }
bool is_tagged_list (REG r, REG s) {
  return is_pair (r) ? (is_eq (car (r), s) ? true : false) : false;
}

bool is_self_evaluating (REG r) {
  return (is_number (r) || is_string (r)) ? true : false;
}
bool is_variable    (REG r)     { return is_symbol (r) ? true :false; }
bool is_quoted      (REG r)     { return is_tagged_list (r, s_quote); }
bool is_assignment  (REG r)     { return is_tagged_list (r, s_set); }
bool is_definition  (REG r)     { return is_tagged_list (r, s_define); }
bool is_if          (REG r)     { return is_tagged_list (r, s_if); }
bool is_lambda      (REG r)     { return is_tagged_list (r, s_lambda); }
bool is_begin       (REG r)     { return is_tagged_list (r, s_begin); }
bool is_application (REG r)     { return is_pair (r); }
bool is_bool        (REG r)     { return r.type == p_bool; }
bool is_true        (REG r)     { return is_bool(r) && r.val == (bool) true; }

// For primitive usage
REG  reg_is_null    (REG r)     { return (r.type == p_null)    ? reg_true : reg_false; }

//----------------------------------------------------------------------
// self-evaluating
//----------------------------------------------------------------------
void ev_self_eval() { printline(1); reg_val = reg_exp; }

//----------------------------------------------------------------------
// variable and env functions
//----------------------------------------------------------------------
REG enclosing_environment (REG env)   { return cdr (env); }
REG first_frame           (REG env)   { return car (env); }
REG frame_variables       (REG frame) { return car (frame); }
REG frame_values          (REG frame) { return cdr (frame); }

void add_binding_to_frame (REG var, REG val, REG frame) {
  set_car (frame, cons (var, (car (frame))));
  set_cdr (frame, cons (val, (cdr (frame))));
}

REG make_frame (REG variables, REG values) { return cons (variables, values); }

REG  extend_environment (REG vars, REG vals, REG base_env) {
  if (length (vars) == length (vals))
    return cons (make_frame (vars, vals), base_env);
 else
   if (length (vars) < length(vals)) {
     reg_error ("Too many arguments supplied, vars", vars);
     reg_error ("                             vals", vals);
   } else {
     reg_error ("Too few arguments supplied, vars", vars);
     reg_error ("                            vals", vals);
   }
}

REG env_loop ( REG var, REG env );

REG scan ( REG var, REG vars, REG vals, REG env ) { printline (2);
  debug (1,
	 { printfnr("Looking for:",var);
	   printfnr("vars:",vars);
	   printfnr("vals:",vals);
	 });
  if (is_null (vars)) {
    return env_loop ( var, enclosing_environment ( env ) );
  } else if (is_eq (var, car (vars))) {
    return car (vals);
  } else {
    return scan ( var, cdr (vars), cdr (vals), env );
  }
}

REG env_loop (REG var, REG env) { printline (2);
  debug (1, {
      printfnr("Looking for:",var);
      printfnr("env:",env);
    });
  if (is_eq (env, the_empty_environment)) {
    reg_error ("Unbound variable", var);
  } else {
    REG frame = first_frame (env);
    debug (1, { printfnr("Frame:",frame); });
    return
      scan (var,
	    frame_variables (frame),
	    frame_values (frame),
	    env);
  }
}

REG lookup_variable_value (REG var, REG env) { printline(1);
  return env_loop (var, env);}

void set_variable_value (REG var, REG val, REG env) { printline (2);
  REG frame, vars, vals;
  bool found = false;
  while (!(found)) {
    if (is_eq (env, the_empty_environment)) {
      reg_error ("Unbound variable: SET!", var);
      break;
    }
    frame = first_frame (env);
    vars = frame_variables (frame);
    vals = frame_values (frame);

    // scan vars vals
    while (true) {
      if (is_null (vars)) {  // until there are no more vars left
	env = enclosing_environment (env);
	break;
      } else {
	if (is_eq (var, car(vars))) {
	  set_car (vals, val);
	  found = true;
	  break;
	} else {
	  vars = cdr (vars);
	  vals = cdr (vals);
	}
      }
    }
  }
}

void ev_variable (REG r) { printline(1);
  reg_val = lookup_variable_value (reg_exp, reg_env);
}

//----------------------------------------------------------------------
// quoteed
//----------------------------------------------------------------------
REG text_of_quotation (REG r) { return cadr (r); }
void ev_quoted () {  reg_val = text_of_quotation (reg_exp); }
REG symbol (const char *s) {
  REG tmp;
  reg_set (tmp, p_symbol, s);
  return tmp;
}

REG quote (const char *s) {
  return cons (s_quote, (cons (symbol (s), reg_null)));
}

//----------------------------------------------------------------------
// assignment
//----------------------------------------------------------------------
REG assignment_variable ();
REG assignment_value ();
void ev_assignment_1();

void ev_assignment() { printline (2);
  reg_unev = assignment_variable (reg_exp);
  save (reg_unev);
  reg_exp = assignment_value (reg_exp);
  save (reg_env);
  // save (reg_continue);
  // reg_continue = &ev_assignment_1;
  eval_dispatch ();
  ev_assignment_1 ();
}

void ev_assignment_1() { printline (2);
  // reg_continue = restore ();
  reg_env = restore ();
  reg_unev = restore ();
  set_variable_value (reg_unev, reg_val, reg_env);
  reg_val = s_ok;
}

REG assignment_variable (REG r) { return cadr (r); }
REG assignment_value (REG r) { return caddr (r); }

//----------------------------------------------------------------------
// definition
//----------------------------------------------------------------------
void define_variable ();
REG definition_variable ();
REG definition_value ();

void ev_definition_1() { printline(2);
  // Since we did not save it, we won't bother
  // restoring it..
  // reg_continue = restore ();
  reg_env = restore ();
  reg_unev = restore ();
  debug (1,
	 {
	   printf("B4 definition to env:"); newline();
	   reg_print (reg_unev); newline();
	   reg_print (reg_env);  newline();
	 });

  define_variable (reg_unev, reg_val, reg_env);
  debug (1,
	 {
	   printf("Added to definition to env:"); newline();
	   reg_print (reg_unev); newline();
	   reg_print (reg_val);  newline();
	   reg_print (reg_env);  newline();
	 });
  reg_val = s_ok;
}

void ev_definition() { printline(1);
  debug (1,
	 {
	   printf("In definition: ");
	   reg_print (reg_exp); newline ();
	 });
  reg_unev = definition_variable (reg_exp);
  save (reg_unev);
  debug (1,
	 {
	   printf("B4 eval Variable: ");
	   reg_print (reg_unev); newline ();
	 });
  reg_exp = definition_value (reg_exp);
  save (reg_env);
  debug (1,
	 {
	   printf("B4 eval Value: ");
	   reg_print (reg_exp); newline ();
	 });
  
  // reg_set (reg_continue, p_label, &ev_definition_1);
  // This evals the definition_value
  eval_dispatch ();
  debug (1,
	 {
	   printf("Definition value: ");
	   reg_print(reg_val); newline ();
	 });
  ev_definition_1();
}

REG definition_variable (REG r) { return is_symbol (cadr (r)) ? cadr (r) : caadr (r); }

REG make_lambda();
REG definition_value    (REG r) {
  if (is_symbol (cadr (r))) 
    return caddr (r);
  else     
    return make_lambda (cdadr (r), cddr (r));
}

void define_variable (REG var, REG val, REG env) { printline(2);

  REG frame = first_frame (env);
  REG vars = frame_variables (frame);
  REG vals = frame_values (frame);

  while (true) {
    if (is_null (vars)) {
      add_binding_to_frame (var, val, frame);
      break;
    } else if (is_eq (var, car (vars))) {
      set_car (vals, val);
      break;
    } else {
      vars = cdr (vars);
      vals = cdr (vals);
    }
  }
}

//----------------------------------------------------------------------
// if
//----------------------------------------------------------------------
REG if_predicate ();
void ev_if_decide ();
void ev_if_alternative ();
void ev_if_consequent ();
REG if_alternative ();
REG if_consequent ();

void ev_if () {
  save (reg_exp);
  save (reg_env);
  // save (reg_continue);
  // reg_continue = &ev_if_decide;
  reg_exp = if_predicate (reg_exp);
  eval_dispatch();
  ev_if_decide();
}

void ev_if_decide () {
  // reg_continue = retore();
  reg_env = restore ();
  reg_exp = restore ();
  if (is_true (reg_val))
    ev_if_consequent();
  else 
    ev_if_alternative();
}

void ev_if_alternative () {
  reg_exp = if_alternative (reg_exp);
  eval_dispatch();
}

void ev_if_consequent () {
  reg_exp = if_consequent (reg_exp);
  eval_dispatch();
}

REG if_predicate (REG r) { return cadr (r); }
REG if_alternative (REG r) { return is_null (cddr (r)) ? s_false : cadddr (r); }
REG if_consequent (REG r) { return caddr(r); }

//----------------------------------------------------------------------
// lambda
//----------------------------------------------------------------------
REG lambda_parameters();
REG lambda_body();
REG make_procedure();

void ev_lambda () {
  reg_unev = lambda_parameters (reg_exp);
  reg_exp = lambda_body (reg_exp);
  reg_val = make_procedure (reg_unev, reg_exp, reg_env);
}

REG lambda_parameters (REG r) { return cadr (r); }
REG lambda_body (REG r) { return cddr (r); }
REG make_lambda (REG parameters, REG body) { printline(2); return cons (s_lambda, cons (parameters, body)); }
REG make_procedure (REG parameters, REG body, REG env) {
  // list ('procedure parameters body env)
  return cons (s_procedure, cons (parameters, cons (body, cons (env, reg_null))));
}

//----------------------------------------------------------------------
// begin
//----------------------------------------------------------------------
REG begin_actions ();
void ev_sequence ();
void ev_sequence_continue ();
void ev_sequence_last_exp ();
REG first_exp ();
REG rest_exps ();
bool is_last_exp ();

void ev_begin () {
  reg_unev = begin_actions (reg_exp);
  ev_sequence ();
}

REG begin_actions (REG r) { return cdr (r); }

// 753
void ev_sequence () { printline(2);
  reg_exp = first_exp (reg_unev);
  while (!(is_last_exp (reg_unev))) {
    save (reg_unev);
    save (reg_env);
    // reg_continue = &ev_sequence_continue;
    eval_dispatch ();
    ev_sequence_continue ();
  }
  ev_sequence_last_exp ();
}

void ev_sequence_continue () {
  reg_env = restore ();
  reg_unev = restore ();
  reg_unev = rest_exps (reg_unev);
  // ev_sequence ();
  reg_exp = first_exp (reg_unev);
}

void ev_sequence_last_exp () {
  eval_dispatch ();
}

REG first_exp (REG seq) { return car (seq); }
REG rest_exps (REG seq) { return cdr (seq); }
bool is_last_exp (REG seq) { return is_null (cdr (seq)); }
  
// 755 for tail-cot optimization
  
//----------------------------------------------------------------------
// is_application
//----------------------------------------------------------------------

REG   operands ();
REG   operator ();
bool  is_no_operands ();
void  apply_dispatch ();
REG   first_operand ();
bool  is_last_operand ();
REG   adjoin_arg ();
REG   rest_operands ();
bool  is_primitive_procedure ();
bool  is_compound_procedure ();
void  primitive_apply ();
void  compound_apply ();
void  unknown_procedure_type ();
REG   apply_primitive_procedure ();
REG   procedure_parameters ();
REG   procedure_environment ();
REG   extend_environment ();
REG   procedure_body ();

void ev_application () { printline(2);

  // save (reg_continue);
  save (reg_env);
  reg_unev = operands (reg_exp);
  save (reg_unev);
  reg_exp = operator (reg_exp);

  debug (1,
	 { printf ("Evaluating the operator."); newline(); });

  // Evaluate the operator
  eval_dispatch ();

  // Evaluate each operand
  debug (1,
	 { printf ("Evaluating each operand."); newline(); });

  // ev-appl-did-operator
  reg_unev = restore (); // Restores the operands
  reg_env = restore ();  
  reg_argl = empty_arglist ();
  reg_proc = reg_val;
  if (is_no_operands (reg_unev)) {
    apply_dispatch ();
    return;
  }
  save (reg_proc);

  // ev-appl-operand-loop
  while (true) {
    save (reg_argl);
    reg_exp = first_operand (reg_unev);  // Get one operand
    if (is_last_operand (reg_unev)) {
      // ev_appl_last_arg ();
      break;
    }
    save (reg_env);
    save (reg_unev);
    // reg_continue = &ev_appl_accumulate_arg;
    eval_dispatch();                     // Evaluate it

    // ev-appl-accumulate-arg
    reg_unev = restore ();
    reg_env = restore ();
    reg_argl = restore ();
    reg_argl = adjoin_arg (reg_val, reg_argl); // Accumulate results
    reg_unev = rest_operands (reg_unev);       // Update unevaluated register
    // ev_appl_operand_loop ();
  }

  // ev-appl-last-arg
  eval_dispatch ();

  // ev_appl_accum_last_arg
  reg_argl = restore ();
  reg_argl = adjoin_arg (reg_val, reg_argl);
  reg_proc = restore ();

  debug (1,
	 { printf ("B4 apply dispatch: "); newline ();
	   printf ("Procedure: "); reg_print (reg_proc); newline ();
	   printf ("Arguments: "); reg_print (reg_argl); newline ();
	 });

  apply_dispatch ();
}

void apply_dispatch () { printline (2);
  debug (1,
	 {
	   printf ("%s: B4 if :",__FUNCTION__);
	   reg_print (reg_proc); newline ();
	 });
  if (is_primitive_procedure (reg_proc)) {
    debug (1,
	   {
	     printf ("%s: primitive procedure\n",__FUNCTION__);
	   });
    primitive_apply ();
  } else {
    if (is_compound_procedure (reg_proc)) {
      debug (1,
	     {
	       printf ("%s: compound procedure\n",__FUNCTION__);
	     });

      compound_apply ();
    } else {
      unknown_procedure_type ();
    }
  }
}

void primitive_apply () { printline (2);
  debug (1,
	 {
	   printf ("B4 apply: "); newline ();
	   printf ("proc: ");
	   reg_print (reg_proc); newline ();
	   printf ("args: ");
	   reg_print (reg_argl); newline();
	 });
  reg_val = apply_primitive_procedure (reg_proc, reg_argl);
}

void compound_apply () { printline (2);
  reg_unev = procedure_parameters (reg_proc);
  reg_env = procedure_environment (reg_proc);
  reg_env = extend_environment (reg_unev, reg_argl, reg_env);
  reg_unev = procedure_body (reg_proc);
  ev_sequence ();
}

REG  operator (REG r)                  { return car (r); }
REG  operands (REG r)                  { return cdr (r); }
bool is_no_operands (REG r)            { return is_null (r); }
REG  first_operand (REG r)             { return car (r); }
REG  rest_operands (REG r)             { return cdr (r); }
bool is_last_operand (REG r)           { return is_null (cdr (r)); }
REG  adjoin_arg (REG arg, REG arglist) { return append (arglist, cons (arg, reg_null)); }
// bool is_primitive_procedure (REG r)    { return is_tagged_list (r, s_primitive); }
bool is_primitive_procedure (REG r)    { return (r.type == p_primitive); }
bool is_compound_procedure (REG r)     { return is_tagged_list (r, s_procedure); }
REG  procedure_parameters (REG r)      { return cadr (r); }
REG  procedure_environment (REG r)     { return cadddr (r); }
REG  procedure_body (REG r)            { return caddr (r); }
void unknown_procedure_type ()         { reg_val = s_unknown_procedure_type_error; }

REG primitive_implementation (REG proc) { return cadr (proc); }

REG apply_primitive_procedure (REG proc, REG args) { printline (1);
  // REG tmp = primitive_implementation (proc);
  REG (*fn)(REG) = (REG (*)(REG)) proc.val;
  return fn(args);
}

//----------------------------------------------------------------------
// unknown_expression_type()
//----------------------------------------------------------------------
void unknown_expression_type () {}

/*----------------------------------------------------------------------*/

int done () { printline(1); }

//----------------------------------------------------------------------
// reader and environment
//----------------------------------------------------------------------

REG primitive_procedures,
  primitive_procedure_names,
  primitive_procedure_objects;

REG primitive (REG (*fn)(REG)) {
  REG tmp;
  reg_set (tmp, p_primitive, fn);
  return tmp;
}

REG primitive2 ( REG (*fn)(REG,REG) ) {
  REG tmp;
  reg_set (tmp, p_primitive, fn);
  return tmp;
}

#define binary_number_op(op,a,b)				\
  {								\
    REG r;							\
    if (is_number (a) && is_number (b)) {			\
      if (is_float (a) && is_float (b)) {			\
	double res = to_float (a.val) op to_float (b.val);	\
	reg_set (r, p_float, float_to_reg_data (res));		\
	return r;						\
      } else if (is_float (a) && is_integer (b)) {		\
	double res = to_float (a.val) op to_int (b.val);	\
	reg_set (r, p_float, float_to_reg_data (res));		\
	return r;						\
      } else if (is_integer (a) && is_float (b)) {		\
	double res = to_int (a.val) op to_float (b.val);	\
	reg_set (r, p_float, float_to_reg_data (res));		\
	return r;						\
      } else if (is_integer(a) && is_integer (b)) {		\
	long int res = to_int (a.val) op to_int (b.val);	\
	reg_set (r, p_integer, int_to_reg_data (res));		\
	return r;						\
      } else {							\
	printf("%s: ",__FUNCTION__);				\
	error ("numbers must be int or float");			\
      }								\
    } else {							\
      printf("%s: ",__FUNCTION__);				\
      error ("both inputs must be numbers.");	\
    }								\
  }
  
REG multiply (REG a, REG b) { binary_number_op (*,a,b); }
REG divide (REG a, REG b) { binary_number_op (/,a,b); }
REG subtract (REG a, REG b) { binary_number_op (-,a,b); }
REG add (REG a, REG b) { binary_number_op (+,a,b); }
REG ge (REG a, REG b) { binary_number_op (>=,a,b); }
REG le (REG a, REG b) { binary_number_op (<=,a,b); }
REG lt (REG a, REG b) { binary_number_op (<,a,b); }
REG gt (REG a, REG b) { binary_number_op (>,a,b); }

#define binary_list_op(op,op_list,a,b)				\
  {								\
    REG r;							\
    if (is_null (a)) {						\
      return b;							\
    } else {							\
      return op (car (a), op_list (cdr (a)));			\
    }								\
  }

REG multiply_list (REG a) {binary_list_op(multiply,multiply_list,a,reg_one);}
REG divide_list (REG a) {binary_list_op(divide, divide_list,a,reg_one);}
REG subtract_list (REG a) {binary_list_op(subtract,subtract_list,a,reg_zero);}
REG add_list (REG a) {binary_list_op(add,add_list,a,reg_zero);}
REG not (REG a) { return (is_true (a) ? reg_false : reg_true); }

#define check_null(a,b) \
  {									\
    if (is_null (a)) {							\
      printf("%s: ",__FUNCTION__);					\
      error("operator takes one argument");				\
    } else {								\
      return b;								\
    }									\
  }

#define check_binary(a,b)						\
  {									\
    if (is_null (a) || is_null (cdr (a))) {				\
      printf("%s: ",__FUNCTION__);					\
      error("operator takes two arguments");				\
    } else {								\
      return b;								\
    }									\
  }
    
// Take only the first item : (car (e1 e2 e3)) = (car e1)
REG car_list (REG a)     { check_null (a, caar (a)); }
REG cdr_list (REG a)     { check_null (a, cdar (a)); }
REG is_null_list (REG a) { check_null (a, reg_is_null (car (a))); }
REG not_list (REG a)     { check_null (a, not (car (a))); }
REG pp_list (REG a)      { check_null (a, reg_print (car (a))); }

// Take only the first two items : (cons (e1 e2)) = (cons e1 e2)
REG cons_list (REG a)     { check_binary ( a, cons (car (a), cadr(a))); }
REG is_eq_list (REG a)    { check_binary ( a, reg_is_eq (car (a), cadr (a))); }
REG is_equal_list (REG a) { check_binary ( a, reg_is_equal (car (a), cadr (a))); }
REG gt_list (REG a) { check_binary (a, gt (car (a), cadr (a))); }
REG lt_list (REG a) { check_binary (a, lt (car (a), cadr (a))); }
REG le_list (REG a) { check_binary (a, le (car (a), cadr (a))); }
REG ge_list (REG a) { check_binary (a, ge (car (a), cadr (a))); }

  
void init_primitive () { printline(2);
  primitive_procedures = 
    list (17,
	  list (2, // 1
		symbol ("car"), primitive (&car_list)),
	  list (2,
		symbol ("cdr"), primitive (&cdr_list)),
	  list (2,
		symbol ("cons"), primitive (&cons_list)),
	  list (2,
		symbol ("null?"), primitive (&is_null_list)),
	  list (2, // 5
		symbol ("eq?"), primitive (&is_eq_list)),
	  list (2,
		symbol ("equal?"), primitive (&is_equal_list)),
	  list (2,
		symbol ("not"), primitive (&not_list)),
	  list (2,
		symbol ("-"), primitive (&subtract_list)),
	  list (2,
		symbol ("+"), primitive (&add_list)),
	  list (2, // 10
		symbol ("/"), primitive (&divide_list)),
	  list (2, 
		symbol ("*"), primitive (&multiply_list)),
	  list (2,
		symbol ("="), primitive (&is_equal_list)),
	  list (2,
		symbol ("<"), primitive (&lt_list)),
	  list (2,
		symbol (">"), primitive (&gt_list)),
	  list (2, // 15
		symbol ("<="), primitive (&le_list)),
	  list (2, 
		symbol (">="), primitive (&ge_list)),
	  list (2,
		symbol ("pp"), primitive (&pp_list))
	);
  primitive_procedure_names =
    map (&car, primitive_procedures);
  primitive_procedure_objects =
    map (&cadr, primitive_procedures);
}

REG setup_environment () { printline(2);
  init_primitive ();
  REG initial_env = extend_environment ( primitive_procedure_names,
					 primitive_procedure_objects,
					 the_empty_environment );
  define_variable (s_true, reg_true, initial_env);
  define_variable (s_false, reg_false, initial_env);
  return initial_env;
}

//----------------------------------------------------------------------
// reader / parser
//----------------------------------------------------------------------
// docs.racket-lang.org/reference/reader.html

#define BUFFER_MAX_LENGTH 1024
char buffer[BUFFER_MAX_LENGTH];
REG read_line () { fgets(buffer, sizeof(buffer), stdin); }

#define TOKEN_MAX_LENGTH 256
typedef struct {
  FILE *in;                         // input file pointer
  size_t count;                     // character count
  size_t lines;                     // number of lines
  size_t lc;                        // line count
  size_t plevel;                    // parenthesis level
  size_t tc;                        // token count
  char c;                           // currently read char
  char *token;                      // store one token
  size_t tc_max;                    // max token length
} readInfo;

// Macros: Add indentation, file location
#define RL_ind print_indent(info->plevel);
#define RL_loc printf("%ld:%ld:%c",info->lines,info->lc-1,info->c);
#define RL_nl  printf("\n");
#define print_line_level printf("Line: %ld Paren level: %ld\n",info->lines,info->plevel)

void print_indent(int n) { for (int i=0;i<n;i++) printf(" "); }

 
REG read_number (char *s) { printline(2);
  long int i;
  double d; 
  int r, n;
  r = sscanf (s, "%ld%n", &i, &n);
  if (r == 1 && n == strlen(s)) {
    return (REG){p_integer, int_to_reg_data (i)};
  } else {
    r = sscanf (s, "%lf", &d);
    if (0) { // Note do not cast (double) i !!!
      double *x = (double *) &i; printf ("Read floast %s %lf\n", s, *x); }
    if (r == 1)
      return (REG){p_float, float_to_reg_data (d)};
    else {
      error ("Error in read_number");
      exit(1);
    }
  }
}

char *str_alloc_copy (char *s) { printline(2);
  debug (3, printf("String len: %ld", strlen(s)););
  char *ns = (char *) calloc(strlen(s), sizeof(char));
  strcpy(ns, s);
  return ns;
}

REG read_symbol (char *s) { printline(2);
  REG *rp;
  debug (4, printf("Reading symbol: %s\n",s););
  if ((rp = ht_get_reg (st_p, s)) == NULL)  { // New token
    char *scopy = str_alloc_copy (s);
    rp = reg_new();
    *rp = (REG){p_symbol, (REG_DATA) scopy};
    hashtable_key_t *kp = ht_new_key (scopy);
    ht_set_reg (st_p, kp, rp);
  }
  return *rp;
}

REG read_bool (char *s) { printline(2);
  if (strlen(s) != 2) error ("Error: read in read_bool");
  else
    if (s[1] == 't')
      return s_true;
    else
      return s_false;
}

bool is_digit (char c) { return ((c >= (char) 48) && (c <= (char) 57)); }

REG read_token_internal (char *s) {

  size_t slen = strlen (s);
  REG r;
  
  switch (s[0]) {
  case ';':
    r = reg_comment; break;
  case '.':
    if (slen == 1 || (!is_digit (s[1]))) {
      r = read_symbol (s); break; }
  case '-':
  case '+':
    if (!(s[1] == '.')) {
      r = read_symbol (s); break; }
  case '0' ... '9':
    r = read_number (s); break;

  case '#':
    r = read_bool (s); break;

  case '/':
  case '*':
  case 'a' ... 'z':
  case 'A' ... 'Z':
    r = read_symbol (s); break;

  case (char) 39: // single quote followed by symbol
    if (slen == 1)
      r = s_quote;
    else {
      r = read_token_internal (&s[1]);
      if (is_symbol (r)) {
	debug (3,
	       {printf("Read symbol: %s",&s[1]);
		 reg_print (s_quote);
		 reg_print (r);
	       });
	r = list (2, s_quote, r);
	debug (3, {newline (); reg_print (r); newline ();});
      }
    }
    break;
 
  default:
    printf ("Unrecognized character\n");
    r = reg_skip;
  }

  return r;
}

REG read_token (readInfo* info) { printline(1);

  if (info->tc == 0)             // Skip white spaces
    return reg_skip;

  info->token[info->tc] = 0;     // Add terminating null char
  char *s = &info->token[0];
  debug (3, {RL_ind; printf("%s : ",s);});

  REG r;
  r = read_token_internal (s);

  info->tc=0;
  return r;

}

#define REG_ARRAY_INFO_MAX_ELEMENTS 16

REG *reg_array_resize (size_t new_count, REG *old_array, size_t old_count) {
  REG *r = (REG *)  calloc (new_count, sizeof (REG));
  if (r && (old_array != NULL)) {
    for (size_t i = 0; i <= old_count; i++) 
      r[i] = old_array[i];
    free (old_array);
  }
  return r;
}

REG_array_info_t *reg_array_info_new () {
  REG_array_info_t *a_ptr = (REG_array_info_t *) calloc (1, sizeof(REG_array_info_t));
  a_ptr->max_elements = REG_ARRAY_INFO_MAX_ELEMENTS;
  a_ptr->count = 0;
  a_ptr->list = reg_array_resize (a_ptr->max_elements, NULL, 0);
}

void reg_array_info_delete (REG_array_info_t *a_ptr) { free (a_ptr->list); free (a_ptr); }

void reg_array_add (REG_array_info_t *a_ptr, REG r) {
  if (!(is_eq (r, reg_skip) || is_eq (r, reg_comment))) {
    a_ptr->list[a_ptr->count++] = r;
    if (a_ptr->count == a_ptr->max_elements) {
      a_ptr->list = reg_array_resize (a_ptr->max_elements * 2, a_ptr->list,
				      a_ptr->max_elements);
      a_ptr->max_elements *= 2;
    }
  }
}

bool reg_array_find_element (REG_array_info_t *a_ptr, REG r) {
  bool found = false;
  for (size_t i = 0; i < a_ptr->count; i++)
    if (is_eq (r, a_ptr->list[i])) found = true;
  return found;
}

REG reg_array_to_reg (REG_array_info_t *a_ptr) { printline(1);
  REG r = reg_null;
  debug (3, printf("(a_prt->count, %ld)",a_ptr->count););
  for (long int i = (long int) a_ptr->count-1; i >= 0; i--) {
    debug (3, printf("(i, %ld)",i););
    r = cons ( a_ptr->list[i], r );
  }
  return r;
}

void read_to_eol (readInfo* info) { printline (2);
  while ((info->c = fgetc(info->in)) != EOF) {
    info->count++; info->lc++;
    if (info->c == '\n') {
      info->lines++; info->lc = 0;
      break;
    }
  }
}

// on '('
REG_array_info_t *read_list (readInfo* info) { printline (1);

  debug (3 , {RL_ind; RL_loc; RL_nl;});

  // size_t begin_count = info->count;
  // size_t begin_line  = info->lines;

  REG_array_info_t *a_ptr = reg_array_info_new ();
  bool end_list = false;
  
  info->tc = 0;
  REG r;
  while ((end_list == false) && ((info->c = fgetc(info->in)) != EOF)) { // Order important
    switch (info->c) {
    case ')' :
      info->count++; info->lc++;
      reg_array_add (a_ptr, r = read_token (info));
      if (is_eq (r, reg_comment)){
	read_to_eol (info);
      } else {
	info->plevel--; end_list = true;
      }
      break;
    case '(' :
      info->plevel++; info->count++; info->lc++;
      bool quote = is_eq (s_quote, r = read_token (info));
      // By choice: ignore 'symbol'(...) , only consider '(...)
      if (is_eq (r, reg_comment)) {
	read_to_eol (info);
	info->plevel--;
      } else {
	REG_array_info_t  *l_ptr = read_list (info);
	debug (3, RL_ind; printf ("Finished read_list."););
	if (l_ptr->count == 0) {
	  if (quote)
	    r = reg_null;
	  else
	    error ("Unquoted zero length list.");
	} else {
	  r = reg_array_to_reg (l_ptr);
	  if (quote)
	    r = list (2, s_quote, r);
	}
	reg_array_add (a_ptr, r);
      } // not comment
      break;
    case '\n' :
    case ' ' :
      info->count++; info->lc++;
      bool newln = (info->c == '\n');
      if (newln) { debug (3 , printf("newline\n"););
	info->lines++; info->lc = 0; }
      reg_array_add (a_ptr, r = read_token (info));
      if (!(newln) && is_eq (r, reg_comment)) read_to_eol (info);
      break;
    default:
      debug (3 , printf("(%c,%d,%ld)",info->c,(int)info->c,info->lc);) ;
      info->count++; info->lc++; info->token[info->tc] = info->c; info->tc++;
      if (info->tc >= info->tc_max) { error ("Read input parser: increase tc_max\n"); }
    } // switch
  } // while
  if (end_list == false) {
    error ("unclosed parenthesis");
  } else {
    debug (3, { print_indent (info->plevel + 1); printf("Ending on %ld:%ld:%c\n", info->lines, info->lc-1,info->c); });
    return a_ptr;
  }
}

REG read_file (char *path) { printline(1);

  readInfo infoStruct;
  readInfo *info = &infoStruct;
  info->tc_max = TOKEN_MAX_LENGTH;
  info->token = (char *) calloc (info->tc_max, sizeof (char));
  info->count = 0;
  info->lines = 0;
  info->lc = 0;
  info->plevel = 0;
  info->in = fopen(path, "r");
  info->tc = 0;
  if (!info->in) error ("read_file: error file could not be opened");

  REG_array_info_t *a_ptr = reg_array_info_new ();

  REG r;
  while ((info->c = fgetc(info->in)) != EOF) {
    switch (info->c) {
    case '(' :  
      info->plevel++; info->count++; info->lc++;
      // By choice: ignore 'symbol'(...) , only consider '(...)
      bool quote = is_eq (s_quote, r = read_token (info));
      if (is_eq (r, reg_comment)) {
	read_to_eol (info);
	info->plevel--;
      } else {
	REG_array_info_t  *l_ptr = read_list (info);
	debug (3, RL_ind; printf ("Finished read_list."););
	if (l_ptr->count == 0) {
	  if (quote)
	    r = reg_null;
	  else
	    error ("Unquoted zero length list");
	} else {
	  r = reg_array_to_reg (l_ptr);
	  if (quote)
	    r = list (2, s_quote, r);
	}
	reg_array_add (a_ptr, r);
      } // not comment
      break;
    case '\n' : // Copied from above
    case ' ' :
      info->count++; info->lc++;
      bool newln = (info->c == '\n');
      if (newln) { debug (3 , printf("newline\n"););
	info->lines++; info->lc = 0; }
      reg_array_add (a_ptr, read_token (info));
      if (!(newln) && is_eq (r,reg_comment)) read_to_eol (info);
      break;
    default:
      debug (3 , printf("(%c,%d,%ld)",info->c,(int)info->c,info->lc);) ;
      info->count++; info->lc++; info->token[info->tc] = info->c; info->tc++;
      if (info->tc >= info->tc_max) { error ("Read input parser: increase tc_max\n"); }
    } // switch
  } // while
  fclose (info->in);
  free (info->token);

  if (a_ptr->count == 0)
    return reg_null;
  else
    return reg_array_to_reg (a_ptr);
}

//----------------------------------------------------------------------
// driver loop
//----------------------------------------------------------------------

char *input_prompt  = ";;; M-Eval input:";
char *output_prompt = ";;; M-Eval value:";

void prompt_for_input ();

void read ();
void announce_output ();
void user_print ();

void driver_loop () {
  while (true) {
    prompt_for_input (input_prompt);
    reg_exp = read_line ();
    eval_dispatch ();
    announce_output (output_prompt);
    user_print (reg_val);
  }
}

void prompt_for_input (char *s) { newline (); newline (); printf("%s",s); newline (); }
void announce_output (char *s) { newline (); printf("%s",s); newline (); }
void user_print (REG out) {}
void read () {}

//----------------------------------------------------------------------
// test
//----------------------------------------------------------------------
void test_cons () {
  REG h = {p_symbol, (REG_DATA) "hello"};
  REG g = {p_symbol, (REG_DATA) "goodbye"};
  REG hg = cons (h, g);
  reg_print (h); newline ();
  reg_print (g); newline ();
  reg_print (hg); newline ();
}

void test_dispatch () {
  // reg_set ( reg_continue, p_label, &done );
  // set exp to 1, and evalute
  reg_set ( reg_exp, p_integer, 1 );
  eval_dispatch();
  // set exp to "", and evalute
  reg_set ( reg_exp, p_string, "" );
  eval_dispatch();
  reg_set ( reg_exp, p_string, "" );
}

void test_parser () {
  debug(1, printf("Printing symbol table\n"););
  debug(1, st_print (););
  REG r = read_file ("test.scm");
  hline();
  reg_print (r); newline ();
}

void read_eval_print_loop (REG r) {
  reg_env = setup_environment ();
  while (!(is_null (r))) {
    reg_exp = car (r);
    yellow(); reg_print(reg_exp); reset(); newline ();
    eval_dispatch ();
    green(); reg_print (reg_val); reset(); newline ();
    r = cdr (r);
  }
}

void test_scm (char *s) {
  REG r = read_file (s);
  newline (); green(); reg_print (r); reset(); newline ();
  read_eval_print_loop (r);
}

//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------
int main(int argc, char **argv){
  debug(1, printf("Creating symbol table\n"););
  st_create ();

  if (argc>=1){
    printf ("Reading from %s\n",argv[1]);
    test_scm (argv[1]);
  } else {
    // test_scm ("test_ev-definition.scm");
    // test_scm ("test_ev-assignment.scm");
    // test_scm ("test_ev-quoted.scm");
    test_scm ("test_ev-lambda.scm");
    // test_scm ("test_primitives.scm");
    // test_scm ("test_fib.scm");
  }
}
