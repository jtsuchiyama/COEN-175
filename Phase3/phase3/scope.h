#ifndef SCOPE_H
#define SCOPE_H
#include <vector>
#include <string>
#include "symbol.h"

class Scope {
    typedef std::vector<Symbol *> Symbols;
    typedef std::string string;
    Scope *_next;
    Symbols _symbols;

    public:
        Scope(Scope *next, Symbols symbols);

        void insert(Symbol *symbol);
        void remove(const string &name);
        Symbol *find(const string &name) const;
        Symbol *lookup(const string &name) const;
        
        Scope *next() { return _next; } 
};

#endif