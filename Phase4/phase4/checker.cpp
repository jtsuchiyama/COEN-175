/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		If a symbol is redeclared, the redeclaration is discarded
 *		and the original declaration is retained.
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
 */

# include <iostream>
# include "lexer.h"
# include "checker.h"
# include "tokens.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"

# include <stdlib.h>


using namespace std;

static Scope *outermost, *toplevel;
static const Type error;

static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string undeclared = "'%s' undeclared";
static string void_object = "'%s' has type void";

static string return_type = "invalid return type";
static string expression_type = "invalid type for test expression";
static string lvalue_required = "lvalue required in expression";
static string operands_binary = "invalid operands to binary %s";
static string operands_unary = "invalid operand to unary %s";
static string not_function = "called object is not a function";
static string arguments = "invalid arguments to called function";

/*
 * Function:	openScope
 *
 * Description:	Create a scope and make it the new top-level scope.
 */

Scope *openScope()
{
    toplevel = new Scope(toplevel);

    if (outermost == nullptr)
	outermost = toplevel;

    return toplevel;
}


/*
 * Function:	closeScope
 *
 * Description:	Remove the top-level scope, and make its enclosing scope
 *		the new top-level scope.
 */

Scope *closeScope()
{
    Scope *old = toplevel;
    toplevel = toplevel->enclosing();
    return old;
}


/*
 * Function:	defineFunction
 *
 * Description:	Define a function with the specified NAME and TYPE.  A
 *		function is always defined in the outermost scope.  This
 *		definition always replaces any previous definition or
 *		declaration.
 */

Symbol *defineFunction(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = outermost->find(name);

    if (symbol != nullptr) {
	if (symbol->type().isFunction() && symbol->type().parameters()) {
	    report(redefined, name);
	    delete symbol->type().parameters();

	} else if (type != symbol->type())
	    report(conflicting, name);

	outermost->remove(name);
	delete symbol;
    }

    symbol = new Symbol(name, type);
    outermost->insert(symbol);
    return symbol;
}


/*
 * Function:	declareFunction
 *
 * Description:	Declare a function with the specified NAME and TYPE.  A
 *		function is always declared in the outermost scope.  Any
 *		redeclaration is discarded.
 */

Symbol *declareFunction(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = outermost->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, type);
	outermost->insert(symbol);

    } else if (type != symbol->type()) {
	report(conflicting, name);
	delete type.parameters();

    } else
	delete type.parameters();

    return symbol;
}


/*
 * Function:	declareVariable
 *
 * Description:	Declare a variable with the specified NAME and TYPE.  Any
 *		redeclaration is discarded.
 */

Symbol *declareVariable(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = toplevel->find(name);

    if (symbol == nullptr) {
	if (type.specifier() == VOID && type.indirection() == 0)
	    report(void_object, name);

	symbol = new Symbol(name, type);
	toplevel->insert(symbol);

    } else if (outermost != toplevel)
	report(redeclared, name);

    else if (type != symbol->type())
	report(conflicting, name);

    return symbol;
}


/*
 * Function:	checkIdentifier
 *
 * Description:	Check if NAME is declared.  If it is undeclared, then
 *		declare it as having the error type in order to eliminate
 *		future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
        report(undeclared, name);
        symbol = new Symbol(name, error);
        toplevel->insert(symbol);
    }

    return symbol;
}

Type checkIf(const Type &expression) {
    if(expression.isError())
        return error;

    if(!expression.isPredicate()) { // E2
        report(expression_type);
        return error;
    }

    return expression;
}

Type checkWhile(const Type &expression) {
    if(expression.isError())
        return error;

    if(!expression.isPredicate()) { // E2
        report(expression_type);
        return error;
    }
    return expression;
}

Type checkFor(const Type &expression) {
    if(expression.isError())
        return error;

    if(!expression.isPredicate()) { // E2
        report(expression_type);
        return error;
    }
    return expression;
}

Type checkLogicalOr(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    Type promo_left = left.promote();
    Type promo_right = right.promote();
    if(!promo_left.isPredicate() || !promo_right.isPredicate()) { // E4
        report(operands_binary, "||");
        return error;
    }
    return Type(INT, 0);
}

Type checkLogicalAnd(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    Type promo_left = left.promote();
    Type promo_right = right.promote();
    if(!promo_left.isPredicate() || !promo_right.isPredicate()) { // E4
        report(operands_binary, "&&");
        return error;
    }
    return Type(INT, 0);
}

Type checkNot(const Type &right) {
    if(right.isError())
        return error;

    if(!right.isPredicate()) { // E5
        report(operands_unary, "!");
        return error;
    }
    return Type(INT, 0);
}

Type checkMul(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    if(!left.isNumeric() || !right.isNumeric()) { // E4
        report(operands_binary, "*");
        return error;
    }
    if(left.specifier() == LONG || right.specifier() == LONG)
        return Type(LONG, 0);
    else
        return Type(INT, 0);
}

Type checkDiv(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    if(!left.isNumeric() || !right.isNumeric()) { // E4
        report(operands_binary, "/");
        return error;
    }
    if(left.specifier() == LONG || right.specifier() == LONG)
        return Type(LONG, 0);
    else
        return Type(INT, 0);
}

Type checkRem(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    if(!left.isNumeric() || !right.isNumeric()) { // E4
        report(operands_binary, "%");
        return error;
    }
    if(left.specifier() == LONG || right.specifier() == LONG)
        return Type(LONG, 0);
    else
        return Type(INT, 0);
}

Type checkNeg(const Type &right) {
    if(right.isError())
        return error;

    Type promo_right = right.promote();
    if(!promo_right.isNumeric()) { // E5    
        report(operands_unary, "-");
        return error;
    }
    return promo_right;
}

Type checkLtn(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    Type promo_left = left.promote();
    Type promo_right = right.promote();
    if(promo_left.isNumeric() && promo_right.isNumeric())
        return Type(INT, 0);
    if(promo_left.isPredicate() && promo_right.isPredicate() && left == right)
        return Type(INT, 0);
    report(operands_binary, "<");
    return error;
}

Type checkGtn(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    Type promo_left = left.promote();
    Type promo_right = right.promote();
    if(promo_left.isNumeric() && promo_right.isNumeric())
        return Type(INT, 0);
    if(promo_left.isPredicate() && promo_right.isPredicate() && left == right)
        return Type(INT, 0);
    report(operands_binary, ">");
    return error;
}

Type checkLeq(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    Type promo_left = left.promote();
    Type promo_right = right.promote();
    if(promo_left.isNumeric() && promo_right.isNumeric())
        return Type(INT, 0);
    if(promo_left.isPredicate() && promo_right.isPredicate() && left == right)
        return Type(INT, 0);
    report(operands_binary, "<=");
    return error;
}

Type checkGeq(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    Type promo_left = left.promote();
    Type promo_right = right.promote();
    if(promo_left.isNumeric() && promo_right.isNumeric())
        return Type(INT, 0);
    if(promo_left.isPredicate() && promo_right.isPredicate() && left == right)
        return Type(INT, 0);
    report(operands_binary, ">=");
    return error;
}

Type checkEql(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    if(!left.isCompatibleWith(right)) { // E4
        report(operands_binary, "==");
        return error;
    }
    return Type(INT, 0);
}

Type checkNeq(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    if(!left.isCompatibleWith(right)) { // E4
        report(operands_binary, "!=");
        return error;
    }

    return Type(INT, 0);
}

Type checkAdd(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    Type promo_left = left.promote();
    Type promo_right = right.promote();
    if(promo_left.isNumeric() && promo_right.isNumeric()) {
        if(promo_left.specifier() == LONG || promo_right.specifier() == LONG)
            return Type(LONG, 0);
        else 
            return Type(INT, 0);
    }

    else if(promo_left.isPointerNotVoid() && promo_right.isNumeric())
        return Type(promo_left.specifier(), promo_left.indirection());

    else if(promo_left.isNumeric() && promo_right.isPointerNotVoid())
        return Type(promo_right.specifier(), promo_right.indirection());

    else { // E4
        report(operands_binary, "+");
        return error;
    }

}

Type checkSub(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    Type promo_left = left.promote();
    Type promo_right = right.promote();
    if(promo_left.isNumeric() && promo_right.isNumeric()) {
        if(promo_left.specifier() == LONG || promo_right.specifier() == LONG)
            return Type(LONG, 0);
        else 
            return Type(INT, 0);
    }

    else if(promo_left.isPointerNotVoid() && promo_right.isNumeric())
       return Type(promo_left.specifier(), promo_left.indirection());

    else if(promo_left.isPointerNotVoid() && promo_right.isPointerNotVoid() && promo_left.specifier() == promo_right.specifier() && promo_left.indirection() == promo_right.indirection())
        return Type(LONG, 0);

    else { // E4
        report(operands_binary, "-");
        return error;
    }
}

Type checkDeref(const Type &right) {
    if(right.isError())
        return error;

    Type promo_right = right.promote();
    if(!promo_right.isPointerNotVoid()) {
        report(operands_unary, "*");
        return error;
    }
        
    return Type(promo_right.specifier(), promo_right.indirection()-1);
}

Type checkIndex(const Type &left, const Type &right) {
    if(left.isError() || right.isError())
        return error;

    Type promo_left = left.promote();
    Type promo_right = right.promote();
    if(promo_left.isPointerNotVoid() && promo_right.isNumeric())
        return Type(promo_left.specifier(), promo_left.indirection()-1);
        
    report(operands_binary, "[]");
    return error;
}

Type checkAddr(const Type &right, const bool &lvalue) {
    if(right.isError())
        return error;

    if(!lvalue) {
        report(lvalue_required);
        return error;
    }
        
    return Type(right.specifier(), right.indirection()+1);
}

Type checkSizeof(const Type &right) {
    if(right.isError())
        return error;

    if(!right.isPredicate()) {
        report(operands_unary, "sizeof");
        return error;
    }
        
    return Type(LONG, 0);
}

Type checkFunction(const Type &left, std::vector<Type> *args) {
    if(left.isError())
        return error;

    // Check for arguments of type error
    for(int i=0;i<args->size();i++) {
        if(args->at(i).isError()) 
            return error;
    }

    if(!left.isFunction()) { // E6
		report(not_function, "");
        return error;
    }

    if(left.parameters() == nullptr) { // Undefined parameters -> Function declared
        for(int i=0;i<args->size();i++) {
            Type arg_type = args->at(i);
            if(!arg_type.isPredicate()) { // E7
                report(arguments,"");
                return error;
            }      
        }
    }

    else { // Defined parameters -> Function defined
        if(left.parameters()->size() != args->size()) { // E7
            report(arguments,"");
            return error;
        }

        for(int i=0;i<args->size();i++) {
            Type arg_type = args->at(i);
            Type param_type = left.parameters()->at(i);
            cout << arg_type << endl;
            cout << param_type << endl;

            if(!arg_type.isCompatibleWith(param_type)) {// E7
                report(arguments,"");
                return error;
            }
        }
    }

    return Type(left.specifier(), left.indirection());
}

Type checkReturn(const Type &defined, const Type &returned) {
    if(defined.isError() || returned.isError())
        return error;

    if(!defined.isCompatibleWith(returned)) {
        report(return_type, "");
        return error;
    }

    return returned;
}

Type checkAssignment(const Type &left, const Type &right, const bool &lvalue) {
    if(left.isError() || right.isError())
        return error;

    if(!lvalue) { // E3
        report(lvalue_required, "");
        return error;
    }

    if(!left.isCompatibleWith(right)) { // E4
        report(operands_binary, "=");
        return error;
    }

    return left;
}