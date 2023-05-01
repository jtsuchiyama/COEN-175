#include "type.h"
#include "assert.h"
using namespace std;

Type::Type(int specifier, unsigned indirection): _specifier(specifier), _indirection(indirection), _declarator(SCALAR)
{}

Type::Type(int specifier, unsigned indirection, unsigned long length): _specifier(specifier), _indirection(indirection), _declarator(ARRAY), _length(length)
{}

Type::Type(int specifier, unsigned indirection, Parameters *parameter): _specifier(specifier), _indirection(indirection), _declarator(FUNCTION), _parameters(parameter)
{}

Type::Type(): _declarator(ERROR)
{}

bool Type::operator != (const Type &rhs) const {
    return !operator == (rhs);
}

bool Type::operator == (const Type &rhs) const {
    if(_declarator != rhs._declarator)
        return false;

    if(_declarator == ERROR)
        return true;

    if(_specifier != rhs._specifier)
        return false;

    if(_indirection != rhs._indirection)
        return false;

    if(_declarator == SCALAR)
        return true;

    if(_declarator == ARRAY)
        return _length == rhs._length;

    assert(_declarator == FUNCTION);

    if(!_parameters || !rhs._parameters)
        return true;

    return *_parameters == *rhs._parameters;
}

std::ostream &operator << (std::ostream &ostr, const Type &type) {
    // Normal properties for Type

    // Specifier
    ostr << type.specifier();

    // Pointers
    unsigned indirection = type.indirection();
    if (indirection > 0) {
        ostr << " ";
        for (int i=0;i<indirection;i++)
            ostr << "*";
    }
    

    // Type-specific
    int declarator = type.declarator();
    if(declarator == 0) { // Array
        ostr << "[" << type.length() << "]";
    }

    else if(type.declarator() == 2) { // Function
        ostr << "(";
        if(type.parameters() != nullptr) 
            ostr << type.parameters()->size(); // If it gets to this point, then the parameters is not a nullptr
        ostr << ")";
    }

    return ostr;
}