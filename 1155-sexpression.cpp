#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum op { ADD, SUB, MUL, DIV, LT, GT, EQ, IF, LET };

struct sexp {
	enum {ATOM_I, ATOM_B, ATOM_N, SEXP} type;
	union {
		int atom_i;
		bool atom_b;
		char atom_n[16];
		struct {
			enum op op;
			struct sexp *s1, *s2, *s3;
		} sexp;
	};
};

struct value {
	enum { VINT, VBOOL, VERR } type;
	union {
		int v;
		bool b;
	};
};

struct env {
	struct {
		char name[16];
		struct value v;
	} tab[30];
	int top;
};

struct env env0()
{
	struct env e;
	e.top = 0;
	return e;
}

struct env env_add(struct env e, char *name, struct value v)
{
	struct env e1 = e;
	strcpy(e1.tab[e1.top].name, name);
	e1.tab[e1.top].v = v;
	e1.top++;
	return e1;
}

struct value env_lookup(struct env e, char *name)
{
	int i;
	struct value err = {.type = VERR, .v = 3};
	for(i = e.top - 1; i >= 0; i--)
	{
		if(strcmp(name, e.tab[i].name) == 0)
			return e.tab[i].v;
	}
	return err;
}

struct value value_int(int v)
{
	struct value x = {.type = VINT, .v = v};
	return x;
}

struct value value_bool(bool b)
{
	struct value x = {.type = VBOOL, .b = b};
	return x;
}

struct value value_err(struct value v1, struct value v2, int e)
{
	struct value x = {.type = VERR, .v = e};
	if(v1.type == VERR)
		return v1;
	if(v2.type == VERR)
		return v2;
	return x;
}

struct sexp *atom_i(int v)
{
	struct sexp *s = malloc(sizeof(struct sexp));
	s->type = ATOM_I;
	s->atom_i = v;
	return s;
}

struct sexp *atom_b(bool v)
{
	struct sexp *s = malloc(sizeof(struct sexp));
	s->type = ATOM_B;
	s->atom_b = v;
	return s;
}

struct sexp *atom_n(char *n)
{
	struct sexp *s = malloc(sizeof(struct sexp));
	s->type = ATOM_N;
	strcpy(s->atom_n, n);
	return s;
}

struct sexp *sexp(enum op op, struct sexp *s1, struct sexp *s2, struct sexp *s3)
{
	struct sexp *s = malloc(sizeof(struct sexp));
	s->type = SEXP;
	s->sexp.op = op;
	s->sexp.s1 = s1;
	s->sexp.s2 = s2;
	s->sexp.s3 = s3;
	return s;
}

struct value eval(struct env env, struct sexp *s)
{
	struct value v1, v2, v3;
	switch(s->type)
	{
	case ATOM_B:
		return value_bool(s->atom_b);
	case ATOM_I:
		return value_int(s->atom_i);
	case ATOM_N:
		return env_lookup(env, s->atom_n);
	default:
		switch(s->sexp.op) {
		case ADD:
			v1 = eval(env, s->sexp.s1);
			v2 = eval(env, s->sexp.s2);
			if(v1.type == VINT &&
			   v2.type == VINT)
				return value_int(v1.v + v2.v);
			else
				return value_err(v1, v2, 1);
		case SUB:
			v1 = eval(env, s->sexp.s1);
			v2 = eval(env, s->sexp.s2);
			if(v1.type == VINT &&
			   v2.type == VINT)
				return value_int(v1.v - v2.v);
			else
				return value_err(v1, v2, 1);
		case MUL:
			v1 = eval(env, s->sexp.s1);
			v2 = eval(env, s->sexp.s2);
			if(v1.type == VINT &&
			   v2.type == VINT)
				return value_int(v1.v * v2.v);
			else
				return value_err(v1, v2, 1);
		case DIV:
			v1 = eval(env, s->sexp.s1);
			v2 = eval(env, s->sexp.s2);
			if(v1.type == VINT &&
			   v2.type == VINT)
				if(v2.v == 0)
					return value_err(v1, v1, 2);
				else
					return value_int(v1.v / v2.v);
			else
				return value_err(v1, v2, 1);
		case LT:
			v1 = eval(env, s->sexp.s1);
			v2 = eval(env, s->sexp.s2);
			if(v1.type == VINT &&
			   v2.type == VINT)
				return value_bool(v1.v < v2.v);
			else
				return value_err(v1, v2, 1);
		case GT:
			v1 = eval(env, s->sexp.s1);
			v2 = eval(env, s->sexp.s2);
			if(v1.type == VINT &&
			   v2.type == VINT)
				return value_bool(v1.v > v2.v);
			else
				return value_err(v1, v2, 1);
		case EQ:
			v1 = eval(env, s->sexp.s1);
			v2 = eval(env, s->sexp.s2);
			if(v1.type == VINT &&
			   v2.type == VINT)
				return value_bool(v1.v == v2.v);
			else
				return value_err(v1, v2, 1);
		case IF:
			v1 = eval(env, s->sexp.s1);
			if(v1.type == VBOOL)
				if(v1.b)
					return eval(env, s->sexp.s2);
				else
					return eval(env, s->sexp.s3);
			else
				return value_err(v1, v1, 1);
		case LET:
			v1 = eval(env, s->sexp.s2);
			if (v1.type == VERR) {
				return v1;
			}
			return eval(env_add(env, s->sexp.s1->atom_n, eval(env, s->sexp.s2)), s->sexp.s3);
		}
	}
}

char line[256];
int pos;
char tok[16];

void run(struct sexp *s)
{
	struct value v = eval(env0(), s);
	if(v.type == VINT)
		printf("%d\n", v.v);
	else if(v.type == VBOOL)
		printf("%s\n", v.b ? "true" : "false");
	else {
		if(v.v == 1)
			printf("Type Mismatch\n");
		else if (v.v == 2)
			printf("Division By Zero\n");
		else
			printf("Unbound Identifier\n");
	}
}

void lex()
{
	int i;
	tok[0] = 0;
	while(line[pos] == ' ')
		pos++;
	switch(line[pos]) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '<':
	case '>':
	case '=':
		tok[0] = line[pos];
		tok[1] = 0;
		pos++;
		return;
	default:
		i = 0;
		if(line[pos] >= '0' && line[pos] <= '9') {
			while(line[pos] >= '0' && line[pos] <= '9')
				tok[i++] = line[pos++];
			tok[i] = 0;
			return;
		}
		if(line[pos] >= 'a' && line[pos] <= 'z') {
			while(line[pos] >= 'a' && line[pos] <= 'z')
				tok[i++] = line[pos++];
			tok[i] = 0;
			return;
		}
	}
}

struct sexp *parse()
{
	struct sexp *s, *s1, *s2, *s3;
	lex();
	while(tok[0]) {
		if(strcmp(tok, "(") == 0) {
			lex();
			if(strcmp(tok, "+") == 0) {
				s1 = parse();
				s2 = parse();
				s = sexp(ADD, s1, s2, NULL);
			} else if (strcmp(tok, "-") == 0) {
				s1 = parse();
				s2 = parse();
				s = sexp(SUB, s1, s2, NULL);
			} else if (strcmp(tok, "*") == 0) {
				s1 = parse();
				s2 = parse();
				s = sexp(MUL, s1, s2, NULL);
			} else if (strcmp(tok, "/") == 0) {
				s1 = parse();
				s2 = parse();
				s = sexp(DIV, s1, s2, NULL);
			} else if (strcmp(tok, "<") == 0) {
				s1 = parse();
				s2 = parse();
				s = sexp(LT, s1, s2, NULL);
			} else if (strcmp(tok, ">") == 0) {
				s1 = parse();
				s2 = parse();
				s = sexp(GT, s1, s2, NULL);
			} else if (strcmp(tok, "=") == 0) {
				s1 = parse();
				s2 = parse();
				s = sexp(EQ, s1, s2, NULL);
			} else if (strcmp(tok, "if") == 0) {
				s1 = parse();
				s2 = parse();
				s3 = parse();
				s = sexp(IF, s1, s2, s3);
			} else if (strcmp(tok, "let") == 0) {
				lex(); //(
				s1 = parse();
				s2 = parse();
				lex(); //)
				s3 = parse();
				s = sexp(LET, s1, s2, s3);
			}
			lex(); //)
			return s;
		} else if(strcmp(tok, "true") == 0) {
			return atom_b(true);
		} else if(strcmp(tok, "false") == 0) {
			return atom_b(false);
		} else if(tok[0] >= 'a' && tok[0] <= 'z') {
			return atom_n(tok);
		} else if(tok[0] >= '0' && tok[0] <= '9') {
			int v;
			sscanf(tok, "%d", &v);
			return atom_i(v);
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	struct sexp *s; //*s = sexp(SUB, atom_i(3), atom_i(2), NULL);
	int i, t;
	gets(line);
	sscanf(line, "%d", &t);
	for(i = 0; i < t; i++) {
		gets(line);
		pos = 0;
		s = parse();
		if(s)
			run(s);
		else
			printf("0\n");
	}

	return 0;
}
