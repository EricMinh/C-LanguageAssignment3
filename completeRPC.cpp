#include <stdio.h>
#include <stdlib.h>		/* for atof() */
#include <ctype.h>
#include <math.h>
#include <string.h>

#define MAXOP 100		/* max size of operand or operator */
#define NUMBER '0'		/* signal that a number was found */
#define MAXVAL 100		/* maximum depth of val stack */
#define BUFSIZE 100
#define NAME 'n'

char buf[BUFSIZE];		/* buffer for ungetch */
int bufp = 0;			/* next free position in buf */
int sp = 0;				/* next free stack position */
double val[MAXVAL];		/* value stack */

/* push: push f onto value stack */
void push(double f)
{
	if (sp < MAXVAL)
		val[sp++] = f;
	else
		printf("error: stack full, can't push %g\n", f);
}

/* pop: pop and return top value from stack */
double pop(void)
{
	if (sp > 0)
		return val[--sp];
	else {
		printf("error: stack empty\n");
		return 0.0;
	}
}

int getch(void) /* get a (possibly pushed-back) character */
{
	return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) /* push character back on input */
{
	if (bufp >= BUFSIZE)
		printf("ungetch: too many characters\n");
	else
		buf[bufp++] = c;
}

/* getop: get next character or numeric operand */
int getop(char s[])
{
	int i, c;
	while ((s[0] = c = getch()) == ' ' || c == '\t')
		;
	s[1] = '\0';
	i = 0;
	if (islower(c)) {
		while (islower(s[++i] = c = getch()))
			;
		s[i] = '\0';
		if (c != EOF)
			ungetch(c);
		if (strlen(s) > 1)
			return NAME;
		else
			return c;

	}
	if (!isdigit(c) && c != '.' && c != '-')
		return c; /* not a number */
	if (c == '-')
		if (isdigit(c = getch()) || c == '.')
			s[++i] = c;
		else {
			if (c != EOF)
				ungetch(c);
			return '-';
		}

	if (isdigit(c)) /* collect integer part */
		while (isdigit(s[++i] = c = getch()))
			;
	if (c == '.') /* collect fraction part */
		while (isdigit(s[++i] = c = getch()))
			;
	s[i] = '\0';
	if (c != EOF)
		ungetch(c);
	return NUMBER;
}

/* clear: clear the stack */
void clear(void) {

	sp = 0;

}

void mathfnc(char s[]) {

	double op1, op2;

	if (strcmp(s, "sin") == 0)
		push(sin(pop()));
	else if (strcmp(s, "cos") == 0)
		push(cos(pop()));
	else if (strcmp(s, "exp") == 0)
		push(exp(pop()));
	else if (strcmp(s, "pow") == 0) {
		op2 = pop();
		push(pow(pop(), op2));
	}
	else if (strcmp(s, "clear") == 0)
		clear();
	else if (strcmp(s, "dupe") == 0) {
		op2 = pop();
		push(op2);
		push(op2);
	}
	else if (strcmp(s, "swap") == 0) {
		op1 = pop();
		op2 = pop();
		push(op1);
		push(op2);
	}
	else { printf("error: %s is not supported\n", s); }
}

/* reverse Polish calculator */
int main(int argc, const char * argv[])
{
	int type, i, var = 0;
	double op1, op2, v = 10;
	char s[MAXOP];
	double variable[26];

	for (i = 0; i < 26; i++) { variable[i] = 0.0; }

	while ((type = getop(s)) != EOF) {
		switch (type) {
		case NUMBER:
			push(atof(s));
			break;
		case NAME:
			mathfnc(s);
			break;
		case '+':
			push(pop() + pop());
			break;
		case '*':
			push(pop() * pop());
			break;
		case '-':
			op2 = pop();
			push(pop() - op2);
			break;
		case '/':
			op2 = pop();
			if (op2 != 0.0)
				push(pop() / op2);
			else
				printf("error: zero divisor\n");
			break;
		case '%':
			op2 = pop();
			if (op2 != 0.0)
				push(fmod(pop(), op2));
			else
				printf("error: zero divisor\n");
			break;
		case '?':
			op2 = pop();
			printf("\t%.8g\n", op2);
			push(op2);
			break;
		case '=':
			pop();
			if (var >= 'A' && var <= 'Z')
				variable[var - 'A'] = pop();
			else
				printf("error: no variable name\n");
			break;
		case '\n':
			v = pop();
			printf("\t%.8g\n", v);
			break;
		default:
			if (type >= 'A' && type <= 'Z')
				push(variable[type - 'A']);
			else if (type == '!')
				push(v);
			else
				printf("error: unknown command %s\n", s);
			break;
		}
		var = type;
	}
	return 0;
}
