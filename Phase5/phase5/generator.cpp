#include "generator.h"
#include <iostream>

using namespace std;

static vector<string> registers {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};

static ostream &operator<<(ostream &ostr, Expression *expr) {
    expr->operand(ostr);
    return ostr;
}

void Block::generate() {
    for(auto stmt: _stmts) {
        stmt->generate();
    }
}

void Simple::generate() {
    _expr->generate();
}

void Expression::operand(ostream &ostr) const {}

void Number::operand(ostream &ostr) const {
    ostr << "$" << _value;
}

void Identifier::operand(ostream &ostr) const {
    int offset = _symbol->_offset;
    if(offset == 0)
        ostr << _symbol->name() << endl;
    else
        ostr << offset << "(%rbp)";
}

void generateGlobals(Scope *scope) {
    Symbols symbols = scope->symbols();
    for(unsigned int i=0;i<symbols.size();i++) {
        Symbol* symbol = symbols.at(i);
        Type type = symbol->type();

        if(!type.isFunction())
            cout << ".comm\t" << symbol->name() << ", " << type.size() << endl;
    }
}

void Function::generate() {
    // Allocating for all of the symbols declared within the function
    int offsetCounter = 0;
    Symbols symbols = _body->declarations()->symbols();
    for(unsigned int i=0;i<symbols.size();i++) {
        Symbol* symbol = symbols.at(i);
        Type type = symbol->type();

        offsetCounter -= type.size();
        symbol->_offset = offsetCounter;
    }

    // Ensure that the stack is 16-byte aligned after allocating for symbols
    while(offsetCounter % 16 != 0)
        offsetCounter--;

    // Output the label
    cout << _id->name() << ":" << endl;

    // Generate the function prologue
    cout << "#Generating function prologue" << endl;
    cout << "pushq\t%rbp" << endl;
    cout << "movq\t%rsp, %rbp" << endl;
    cout << "subq\t$" << -offsetCounter << ", %rsp" << endl;

    // Spill parameters
    cout << "#Spilling parameters" << endl;
    for(unsigned int i=0;i<_id->type().parameters()->size();i++) {
        Symbol* symbol = symbols[i];
        cout << "movl\t" << registers[i] << ", " << symbol->_offset << "(%rbp)" << endl;
    }

    // Generate body
    cout << "#Generating body" << endl;
    _body->generate();

    // Generate the function epilogue
    cout << "#Generating function epilogue" << endl;
    cout << "movq\t%rbp, %rsp" << endl;
    cout << "popq\t%rbp" << endl;
    cout << "ret" << endl;

    // .global directive
    cout << "#Outputting global directive" << endl;
    cout << ".globl\t" << _id->name() << endl;
}

void Assignment::generate() {
    cout << "movl\t" << _right << ", " << _left << endl;
}

void Call::generate() {
    for(unsigned int i=0; i<_args.size(); i++) {
        cout << "movl\t" << _args[i] << ", " << registers[i] << endl;
    }

    cout << "call\t" << _id->name() << endl;
}