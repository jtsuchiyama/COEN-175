#include "checker.h"
using namespace std;
#include <iostream>
#include "scope.h"
#include "lexer.h"
#include "tokens.h"

typedef std::vector<Symbol *> Symbols;

Scope* current = nullptr;
Scope* head = nullptr;

const string e1 = "redefinition of '%s'";
const string e2 = "conflicting types for '%s'";
const string e3 = "redeclaration of '%s'";
const string e4 = "'%s' undeclared";
const string e5 = "'%s' has type void";

void openScope() {
    cout << "open scope" << endl;

    Symbols symbols = Symbols();
    Scope* scope = new Scope(current, symbols);

    // Setting of the head pointer
    if(current == nullptr) {
        current = scope;
        head = current;
    }

    else
        current = scope;
}

void closeScope() {
    cout << "close scope" << endl;

    current = current->next();
}

void declareFunction(const std::string &name, const Type &type) {
    cout << name << ": " << type << endl;

    Symbol* symbol = head->find(name);
    if(symbol == nullptr) {
        Symbol* new_symbol = new Symbol(name, type);
        head->insert(new_symbol);
    }

    // After this point, the symbol must already exist
    else if(symbol->type() != type) // E2
        report(e2, name);    
}

void defineFunction(const std::string &name, const Type &type) {
    cout << name << ": " << type << endl;

    Symbol* symbol = head->find(name);
    if(symbol == nullptr) {
        Symbol* new_symbol = new Symbol(name, type);
        head->insert(new_symbol);
    }

    // After this point, the symbol must already exist
    else if(symbol->type().parameters() != nullptr) {// E1 
        report(e1, name);
    }

    else if(symbol->type() != type) // E2
        report(e2, name); 

    else { // Always insert the newest defintion
        head->remove(name);

        Symbol* new_symbol = new Symbol(name, type);
        head->insert(new_symbol);
    }
}

// Array or scalar
void declareVariable(const std::string &name, const Type &type) {
    cout << name << ": " << type << endl;

    Symbol *symbol = current->find(name);
    if(symbol == nullptr) {
        if(type.specifier() == VOID && type.indirection() == 0) { // E5 (type void or array of void)
            report(e5, name);
            return;
        }

        Symbol* new_symbol = new Symbol(name, type);
        current->insert(new_symbol);
    }

    // After this point, the symbol must already exist
    else if(current->next() != nullptr) { // E3
        report(e3, name);
    }

    else if(symbol->type() != type) { // E2
        report(e2, name);
    }
}

void checkIdentifier(const std::string &name) {
    //cout << name << endl;

    Symbol *symbol = current->lookup(name);
    if(symbol == nullptr) {
        report(e4, name);
    }
}