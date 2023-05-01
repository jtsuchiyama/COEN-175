/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include "Scope.h"

Scope *openScope();
Scope *closeScope();

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *declareFunction(const std::string &name, const Type &type);
Symbol *declareVariable(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);

Type checkIf(const Type &expression);
Type checkWhile(const Type &expression);
Type checkFor(const Type &expression);
Type checkLogicalOr(const Type &left, const Type &right);
Type checkLogicalAnd(const Type &left, const Type &right);
Type checkNot(const Type &left);

Type checkMul(const Type &left, const Type &right);
Type checkDiv(const Type &left, const Type &right);
Type checkRem(const Type &left, const Type &right);
Type checkNeg(const Type &right);

Type checkLtn(const Type &left, const Type &right);
Type checkGtn(const Type &left, const Type &right);
Type checkLeq(const Type &left, const Type &right);
Type checkGeq(const Type &left, const Type &right);

Type checkEql(const Type &left, const Type &right);
Type checkNeq(const Type &left, const Type &right);

Type checkAdd(const Type &left, const Type &right);
Type checkSub(const Type &left, const Type &right);

Type checkDeref(const Type &right);
Type checkIndex(const Type &left, const Type &right);
Type checkAddr(const Type &right, const bool &lvalue);

Type checkSizeof(const Type &right);

Type checkFunction(const Type &left, std::vector<Type> *args);
Type checkReturn(const Type &defined, const Type &returned);
Type checkAssignment(const Type &left, const Type &right, const bool &lvalue);

# endif /* CHECKER_H */
