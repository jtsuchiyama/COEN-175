#ifndef TYPE_H
#define TYPE_H

#include <vector>
#include <ostream>

typedef std::vector <class Type> Parameters;

class Type {
    int _specifier;
    unsigned _indirection;
    enum Declarator { ARRAY, ERROR, FUNCTION, SCALAR } _declarator;
    unsigned long _length;
    Parameters *_parameters;

    public: 
        Type(int specifier, unsigned indirection=0);
        Type(int specifier, unsigned indirection, unsigned long length);
        Type(int specifier, unsigned indirection, Parameters *parameter);
        Type();

        bool isArray() const { return _declarator == ARRAY; }
        bool isError() const { return _declarator == ERROR; }
        bool isFunction() const { return _declarator == FUNCTION; }
        bool isScalar() const { return _declarator == SCALAR; }

        int specifier() const { return _specifier; }
        unsigned indirection() const { return _indirection; }
        Declarator declarator() const { return _declarator; }
        unsigned long length() const { return _length; }
        Parameters *parameters() const { return _parameters; }

        bool operator == (const Type &rhs) const;
        bool operator != (const Type &rhs) const;
};

#endif

std::ostream &operator << (std::ostream &ostr, const Type &type);