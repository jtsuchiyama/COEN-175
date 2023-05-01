#include "scope.h"

Scope::Scope(Scope *next, Symbols symbols): _next(next), _symbols(symbols)
{} 

void Scope::insert(Symbol *symbol) {
    _symbols.push_back(symbol);
}

void Scope::remove(const string &name) {
    int length = _symbols.size();
    for(int i=0; i<length; i++) {
        if(_symbols[i]->name() == name)
            _symbols.erase(_symbols.begin() + i);
    }
}

Symbol* Scope::find(const string &name) const {
    int length = _symbols.size();
    for(int i=0; i<length; i++) {
        if(_symbols[i]->name() == name)
            return _symbols[i];
    }

    return nullptr;
}

Symbol* Scope::lookup(const string &name) const {
    int length = _symbols.size();
    for(int i=0; i<length; i++) {
        if(_symbols[i]->name() == name)
            return _symbols[i];
    }

    if(_next != nullptr) {
        return _next->lookup(name);
    }

    return nullptr;
}

