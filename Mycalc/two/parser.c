#include <stdio.h>
#include <stdlib.h>
#include "token.h"

#define LINE_BUF_SIZE (1024)

static Token st_look_ahead_token;
static int st_look_ahead_token_exists;

static void 
my_get_token(Token *token)
{
	if (st_look_ahead_token_exists) {
		*token = st_look_ahead_token;
		st_look_ahead_token_exists = 0;
	} else {
		get_token(token);
	}
}

static void 
unget_token(Token *token)
{
	st_look_ahead_token = *token;
	st_look_ahead_token_exists = 1;
}

double parse_expression(void);
	
static double
parse_primary_expression()
{
	Token token;
	double value = 0.0;
	int minus_flag = 0;

	my_get_token(&token);
	if (SUB_OPERATOR_TOKEN == token.kind){
		minus_flag = 1;
	} else {
		unget_token(&token);
	}

	my_get_token(&token);
	if (NUMBER_TOKEN == token.kind) {
		value = token.value;
	} else if (LEFT_PAREN_TOKEN == token.kind){
		value = parse_expression();
		my_get_token(&token);
		if (RIGHT_PAREN_TOKEN != token.kind) {
			fprintf(stderr, "missing ')' error.\n");
			exit(1);
		}
	} else {
		unget_token(&token);
	}
	if (minus_flag) {
		value = -value;
	}
	return value;
}

static double
parse_term()
{
	double v1;
	double v2;
	Token token;

	v1 = parse_primary_expression();
	for (;;) {
		my_get_token(&token);
		if (MUL_OPERATOR_TOKEN != token.kind && DIV_OPERATOR_TOKEN != token.kind) {
			unget_token(&token);
			break;
		}
		v2 = parse_primary_expression();
		if (MUL_OPERATOR_TOKEN == token.kind){
			v1 *= v2;
		} else if (DIV_OPERATOR_TOKEN == token.kind){
			v1 /= v2;
		}
	}
	return v1;
}

double
parse_expression()
{
	double v1;
	double v2;
	Token token;

	v1 = parse_term();
	for (;;) {
		my_get_token(&token);
		if (ADD_OPERATOR_TOKEN != token.kind && SUB_OPERATOR_TOKEN != token.kind) {
			unget_token(&token);
			break;
		}
		v2 = parse_term();
		if (ADD_OPERATOR_TOKEN == token.kind) {
			v1 += v2;
		} else if (SUB_OPERATOR_TOKEN == token.kind) {
			v1 -= v2;
		} else {
			unget_token(&token);
		}
	}
	return v1;
}

double
parse_line(void)
{
	double value;

	st_look_ahead_token_exists = 0;
	value = parse_expression();

	return value;
}

int
main(int argc, char **argv)
{
	char line[LINE_BUF_SIZE];
	double value;

	while (NULL != fgets(line, LINE_BUF_SIZE, stdin)) {
		set_line(line);
		value = parse_line();
		printf(">>%f\n", value);
	}

	return 0;
}
