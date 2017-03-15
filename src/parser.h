#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "mpc.h"


#define DEBUG 0
#define SIZE 2048

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval {
	int type;
	double num;

	char *err;
	char *sym;
	char *str;

	lbuiltin builtin;
	lenv *env;
	lval *formals;
	lval *body;

	int count;
	lval **cell;
};

struct lenv {
	lenv *par;
	int count;
	char **syms;
	lval **vals;
};

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_STR, LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

#define LASSERT(args, cond, fmt, ...) \
	if(!(cond)) { lval_del(args); return lval_err(fmt, ##__VA_ARGS__); }

#define LASSERT_TYPE(func, args, index, expect) \
		LASSERT(args, args->cell[index]->type == expect, \
		"Function '%s' passed incorrect type for argument %i. " \
		"Got %s, Expected %s.", \
		func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LASSERT_NUM(func, args, num) \
		LASSERT(args, args->count == num, \
		"Function '%s' passed incorrect number of arguments."  \
		"Got %i, Expected %i.", \
		func, args->count, num)

#define LASSERT_NOT_EMPTY(func, args, index) \
		LASSERT(args, args->cell[index]->count != 0, \
		"Function '%s' passed {} for argument %i.", func, index);

/* evaluate s-expression */
lval* lval_eval_sexpr(lenv *e, lval *v);
lval* lval_eval(lenv *e, lval *v);
lval* lval_call(lenv *e, lval *f, lval *a);
lval* lval_copy(lval *v);
lval* lval_pop(lval *v, int i);
lval* lval_take(lval *v, int i);

lval* builtin_load(lenv* e, lval* a);

lval* builtin_ord(lenv* e, lval* a, char* op);

/* builtins functions */
lval* builtin_op(lenv *e, lval *a, char *op);
char* ltype_name(int t);
lval* builtin_head(lenv *e, lval *a);
lval* builtin_list(lenv *e, lval *a);
lval* builtin_eval(lenv *e, lval *a);
lval* builtin_tail(lenv *e, lval *a);
lval* builtin_join(lenv *e, lval *a);
lval* lval_join(lval *x, lval *y);
lval* builtin_cons(lenv *e, lval *a);
lval* builtin_len(lenv *e, lval *a);
lval* builtin_init(lenv *e, lval *a);
lval* builtin_def(lenv *e, lval *a);
lval* builtin_put(lenv *e, lval *a);
lval* builtin_var(lenv *e, lval *a, char *func);

// arithmetic
lval* builtin_add(lenv *e, lval *a);
lval* builtin_sub(lenv *e, lval *a);
lval* builtin_mul(lenv *e, lval *a);
lval* builtin_div(lenv *e, lval *a);
lval* builtin_mod(lenv *e, lval *a);
lval* builtin_pow(lenv *e, lval *a);
lval* builtin_max(lenv *e, lval *a);
lval* builtin_min(lenv *e, lval *a);

/* read input and construct s-expression */
lval* lval_read_num(mpc_ast_t *t);
lval* lval_read(mpc_ast_t *t);
lval* lval_add(lval *v, lval *x);

/* lval type functions */
lval* lval_num(double x);
lval* lval_err(char *fmt, ...);
lval* lval_sym(char *sym);
lval* lval_str(char *s);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
void lval_del(lval *v);
lval* lval_lambda(lval *formals, lval *body);

/* lenv type functions */
lenv* lenv_new(void);
lenv* lenv_copy(lenv *e);
void lenv_del(lenv *e);
lval* lenv_get(lenv *e, lval *k);
void lenv_put(lenv *e, lval *k, lval *v);
void lenv_def(lenv *e, lval *k, lval *v);
void lenv_add_builtin(lenv *e, char *name, lbuiltin func);
void lenv_add_builtins(lenv *e);

/* lval print functions */
void lval_expr_print(lval *v, char open, char close);
void lval_print(lval *v);
void lval_println(lval *v);

lval* lval_exit(char *input, lenv *e);

int countLeaf(mpc_ast_t *t);
int countBranch(mpc_ast_t *t);
