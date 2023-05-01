# include <iostream>
# include "tokens.h"
# include "lexer.h"

using namespace std;

int lookahead;

void expression();
void statements();
void global_declarator();
void declarations();

void match(int t) {
	if(lookahead == t) {
		lookahead = yylex();
	}
	else {
		exit(EXIT_FAILURE);

	}
}

void specifier() {
	if(lookahead == INT)
		match(INT);

	else if(lookahead == CHAR)
		match(CHAR);
	
	else if(lookahead == LONG)
		match(LONG);

	else if(lookahead == VOID)
		match(VOID);
}

void pointers() {
	while(1) {
		if(lookahead == STAR)
			match(STAR);

		else
			break;
	}
}

void global_declarator() {
	pointers();
	match(ID);
	if(lookahead == LPAREN) {
		match(LPAREN);
		match(RPAREN);
	}

	else if(lookahead == LBRACKET) {
		match(LBRACKET);
		match(NUM);
		match(RBRACKET);
	}
}

void remainingDecls() {
	if(lookahead == SEMICOLON) {
		match(SEMICOLON);
	}

	else if (lookahead == COMMA){
		match(COMMA);
		global_declarator();
		remainingDecls();
	}
}

void parameter() {
	specifier();
	pointers();
	match(ID);
}

void parameter_list() {
	parameter();
	if(lookahead == COMMA) {
		match(COMMA);
		parameter_list();
	}
}

void void_parameter() {
	pointers();
	match(ID);
	if(lookahead == COMMA) {
		parameter_list();
	}
}

void parameters() {
	if(lookahead == VOID) {
		match(VOID);
		if(lookahead == STAR || lookahead == ID) {
			void_parameter();
		}
	}

	else {
		parameter_list();
	}
}

void functionOrGlobal() {
	specifier();
	pointers();
	match(ID);

	if(lookahead == LPAREN) {
		match(LPAREN);
		if(lookahead == RPAREN) {
			match(RPAREN);
			remainingDecls();
		}

		else {
			parameters();
			match(RPAREN);
			match(LBRACE);
			declarations();
			statements();
			match(RBRACE);
		}
	}

	else if(lookahead == LBRACKET) {
		match(LBRACKET);
		match(NUM);
		match(RBRACKET);
		remainingDecls();
	}

	else
		remainingDecls();
}

void declarator() {
	pointers();
	match(ID);
	if(lookahead == LBRACKET) {
		match(LBRACKET);
		match(NUM);
		match(RBRACKET);
	}
}

void declarator_list() {
	declarator();
	if(lookahead == COMMA) {
		match(COMMA);
		declarator_list();
	}
}

void declaration() {
	specifier();
	declarator_list();
	match(SEMICOLON);
}

void declarations() {
	while(lookahead == INT || lookahead == CHAR || lookahead == LONG || lookahead == VOID) {
		declaration();
	}
}

void assignment() {
	expression();
	if(lookahead == ASSIGN) {
		match(ASSIGN);
		expression();
	}
}

void statement() {
	if(lookahead == LBRACE) {
		match(LBRACE);
		declarations();
		statements();
		match(RBRACE);
	}

	else if(lookahead == RETURN) {
		match(RETURN);
		expression();
		match(SEMICOLON);
	}

	else if(lookahead == WHILE) {
		match(WHILE);
		match(LPAREN);
		expression();
		match(RPAREN);
		statement();
	}

	else if(lookahead == FOR) {
		match(FOR);
		match(LPAREN);
		assignment();
		match(SEMICOLON);
		expression();
		match(SEMICOLON);
		assignment();
		match(RPAREN);
		statement();
	}

	else if(lookahead == IF) {
		match(IF);
		match(LPAREN);
		expression();
		match(RPAREN);
		statement();

		if(lookahead == ELSE) {
			match(ELSE);
			statement();
		}
	}

	else {
		assignment();
		match(SEMICOLON);
	}
}

void statements() {
	if(lookahead != RBRACE) {
		statement();
		statements();
	}
}

void extra_expressions() {
	while(1) {
		if(lookahead == NUM) {
			match(NUM);
		}

		else if(lookahead == STRING) {
			match(STRING);
		}

		else if(lookahead == CHARACTER) {
			match(CHARACTER);
		}

		else if(lookahead == LPAREN) {
			match(LPAREN);
			expression();
			match(RPAREN);
		}

		else
			break;
	}
}

void expression_list() {
	expression();
	while(1) {
		if(lookahead == COMMA) {
			match(COMMA);
			expression_list();
		}
		
		else
			break;
	}
}

void id_expression() {
	while(1) {
		if(lookahead == ID) {
			match(ID);

			if(lookahead == LPAREN) {
				match(LPAREN);
				if(lookahead != RPAREN) { // Expression list
					expression_list();
				}

				if(lookahead == RPAREN) {
					match(RPAREN);
				}
			}
		}

		else
			break;
	}

	extra_expressions();
}

void bracket_expression() {
	id_expression();

	while(1) {
		if(lookahead == LBRACKET) {
			match(LBRACKET);
			expression();
			match(RBRACKET);
			cout << "index" << endl;
		}

		else
			break;
	}
}

void unary_expression() {
	if(lookahead == ADDRESS || lookahead == STAR || lookahead == NOT || lookahead == MINUS || lookahead == SIZEOF) {
		int temp = lookahead; // since lookahead could change when recursing
		match(lookahead);
		unary_expression();
		if(temp == ADDRESS) {
			cout << "addr" << endl;
		}

		else if(temp == STAR) {
			cout << "deref" << endl;
		}

		else if(temp == NOT) {
			cout << "not" << endl;
		}

		else if(temp == MINUS) {
			cout << "neg" << endl;
		}

		else if(temp == SIZEOF) {
			if (lookahead == LPAREN) {
                match(LPAREN);
                expression();
                match(RPAREN);
            }
			cout << "sizeof" << endl;
		}
	}

	bracket_expression();
}

void mul_div_expression() {
	unary_expression();

	while(1) {
		if(lookahead == STAR) {
			match(STAR);
			unary_expression();
			cout << "mul" << endl;
		}

		else if(lookahead == DIV) {
			match(DIV);
			unary_expression();
			cout << "div" << endl;
		}

		else if(lookahead == MOD) {
			match(MOD);
			unary_expression();
			cout << "rem" << endl;
		}

		else
			break;
	}
}

void add_sub_expression() {
	mul_div_expression();

	while(1) {
		if(lookahead == ADD) {
			match(ADD);
			mul_div_expression();
			cout << "add" << endl;
		}

		else if(lookahead == MINUS) {
			match(MINUS);
			mul_div_expression();
			cout << "sub" << endl;
		}

		else
			break;
	}
}

void compare_expression() {
	add_sub_expression();

	while(1) {
		if(lookahead == LESSER) {
			match(LESSER);
			add_sub_expression();
			cout << "ltn" << endl;
		}

		else if(lookahead == GREATER) {
			match(GREATER);
			add_sub_expression();	
			cout << "gtn" << endl;
		}

		else if(lookahead == LEQ) {
			match(LEQ);
			add_sub_expression();
			cout << "leq" << endl;
		}

		else if(lookahead == GEQ) {
			match(GEQ);
			add_sub_expression();
			cout << "geq" << endl;
		}

		else
			break;
	}
}

void equality_expression() {
	compare_expression();	

	while(1) {
		if(lookahead == EQL) {
			match(EQL);
			compare_expression();
			cout << "eql" << endl;
		}

		else if(lookahead == NEQ) {
			match(NEQ);
			compare_expression();
			cout << "neq" << endl;
		}

		else
			break;
	}
}

void and_expression() {
	equality_expression();

	while(1) {
		if(lookahead == AND) {
			match(AND);
			equality_expression();
			cout << "and" << endl;
		}

		else
			break;
	}
}

void expression() {
	and_expression();

	while(1) {
		if(lookahead == OR) {
			match(OR);
			and_expression();
			cout << "or" << endl;
		}

		else 
			break;
	}
}

int main()
{
    lookahead = yylex();
	while(lookahead != DONE) {
		functionOrGlobal();
	}
		
	return 0;
}
