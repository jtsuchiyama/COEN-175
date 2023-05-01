/*
 * File:	generator.cpp
 *
 * Description:	This file contains the public and member function
 *		definitions for the code generator for Simple C.
 *
 *		Extra functionality:
 *		- putting all the global declarations at the end
 */

# include <vector>
# include <cassert>
# include <iostream>
# include <map>
# include "generator.h"
# include "machine.h"
# include "Tree.h"
# include "string.h"

using namespace std;

static int offset;
static string funcname;
static string suffix(Expression *expr);
static ostream &operator <<(ostream &ostr, Expression *expr);

static Register *rax = new Register("%rax", "%eax", "%al");
static Register *rbx = new Register("%rbx", "%ebx", "%bl");
static Register *rcx = new Register("%rcx", "%ecx", "%cl");
static Register *rdx = new Register("%rdx", "%edx", "%dl");
static Register *rsi = new Register("%rsi", "%esi", "%sil");
static Register *rdi = new Register("%rdi", "%edi", "%dil");
static Register *r8 = new Register("%r8", "%r8d", "%r8b");
static Register *r9 = new Register("%r9", "%r9d", "%r9b");
static Register *r10 = new Register("%r10", "%r10d", "%r10b");
static Register *r11 = new Register("%r11", "%r11d", "%r11b");
static Register *r12 = new Register("%r12", "%r12d", "%r12b");
static Register *r13 = new Register("%r13", "%r13d", "%r13b");
static Register *r14 = new Register("%r14", "%r14d", "%r14b");
static Register *r15 = new Register("%r15", "%r15d", "%r15b");

static vector<Register *> parameters = {rdi, rsi, rdx, rcx, r8, r9};
static vector<Register *> registers = {rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11};

static map<string, Label> strings;

/* These will be replaced with functions in the next phase.  They are here
   as placeholders so that Call::generate() is finished. */

static void assign(Expression *expr, Register *reg) {

    if(expr != nullptr) {
        // Severing the links if they need to be reassigned
        if(expr->_register != nullptr)
            expr->_register->_node = nullptr;
        

        // Reassign
        expr->_register = reg;
    }

    if(reg != nullptr) {
        // Severing the links if they need to be reassigned
        if(reg->_node != nullptr)
            reg->_node->_register = nullptr;

        // Reassign
        reg->_node = expr;
    }
}

static void load(Expression *expr, Register *reg)
{
    if(reg->_node != expr) {
        // Spill if the register is already allocated
        if(reg->_node != nullptr) {
            unsigned size = reg->_node->type().size();
            offset -= size;
            reg->_node->_offset = offset;
            cout << "\tmov" << suffix(reg->_node);
            cout << reg->name(size) << ", ";
            cout << offset << "(%rbp)" << endl;
        }

        // Load into the register
        if(expr != nullptr) {
            unsigned size = expr->type().size();
            cout << "\tmov" << suffix(expr) << expr;
            cout << ", " << reg->name(size) << endl;
        }

        // Assign expr to reg
        assign(expr, reg);
    }
}

// Returns the next available register
static Register *getreg() {
    for(auto reg : registers)
        if(reg->_node == nullptr)
            return reg;

    // If all the registers are full, spill the first register
    load(nullptr, registers[0]);
    return registers[0];
}

/*
 * Function:	suffix (private)
 *
 * Description:	Return the suffix for an opcode based on the given size.
 */

static string suffix(unsigned long size)
{
    return size == 1 ? "b\t" : (size == 4 ? "l\t" : "q\t");
}


/*
 * Function:	suffix (private)
 *
 * Description:	Return the suffix for an opcode based on the size of the
 *		given expression.
 */

static string suffix(Expression *expr)
{
    return suffix(expr->type().size());
}


/*
 * Function:	align (private)
 *
 * Description:	Return the number of bytes necessary to align the given
 *		offset on the stack.
 */

static int align(int offset)
{
    if (offset % STACK_ALIGNMENT == 0)
	return 0;

    return STACK_ALIGNMENT - (abs(offset) % STACK_ALIGNMENT);
}


/*
 * Function:	operator << (private)
 *
 * Description:	Convenience function for writing the operand of an
 *		expression using the output stream operator.
 */

static ostream &operator <<(ostream &ostr, Expression *expr)
{
    if (expr->_register != nullptr)
	return ostr << expr->_register;

    expr->operand(ostr);
    return ostr;
}


/*
 * Function:	Expression::operand
 *
 * Description:	Write an expression as an operand to the specified stream.
 */

void Expression::operand(ostream &ostr) const
{
    assert(_offset != 0);
    ostr << _offset << "(%rbp)";
}


/*
 * Function:	Identifier::operand
 *
 * Description:	Write an identifier as an operand to the specified stream.
 */

void Identifier::operand(ostream &ostr) const
{
    if (_symbol->_offset == 0)
	ostr << global_prefix << _symbol->name() << global_suffix;
    else
	ostr << _symbol->_offset << "(%rbp)";
}


/*
 * Function:	Number::operand
 *
 * Description:	Write a number as an operand to the specified stream.
 */

void Number::operand(ostream &ostr) const
{
    ostr << "$" << _value;
}


/*
 * Function:	Call::generate
 *
 * Description:	Generate code for a function call expression.
 *
 *		On a 64-bit platform, the stack needs to be aligned on a
 *		16-byte boundary.  So, if the stack will not be aligned
 *		after pushing any arguments, we first adjust the stack
 *		pointer.
 *
 *		Since all arguments are 8-bytes wide, we could simply do:
 *
 *		    if (args.size() > 6 && args.size() % 2 != 0)
 *			subq $8, %rsp
 */

void Call::generate()
{
    unsigned numBytes;


    /* Generate code for the arguments first. */ 

    numBytes = 0;

    for (int i = _args.size() - 1; i >= 0; i --)
	_args[i]->generate();


    /* Adjust the stack if necessary */

    if (_args.size() > NUM_PARAM_REGS) {
	numBytes = align((_args.size() - NUM_PARAM_REGS) * SIZEOF_PARAM);

	if (numBytes > 0)
	    cout << "\tsubq\t$" << numBytes << ", %rsp" << endl;
    }


    /* Move the arguments into the correct registers or memory locations. */

    for (int i = _args.size() - 1; i >= 0; i --) {
	if (i >= NUM_PARAM_REGS) {
	    numBytes += SIZEOF_PARAM;
	    load(_args[i], rax);
	    cout << "\tpushq\t%rax" << endl;

	} else
	    load(_args[i], parameters[i]);

	assign(_args[i], nullptr);
    }


    /* Call the function and then reclaim the stack space.  Technically, we
       only need to assign the number of floating point arguments passed in
       vector registers to %eax if the function being called takes a
       variable number of arguments.  But, it never hurts. */

    for (auto reg : registers)
	load(nullptr, reg);

    if (_id->type().parameters() == nullptr)
	cout << "\tmovl\t$0, %eax" << endl;

    cout << "\tcall\t" << global_prefix << _id->name() << endl;

    if (numBytes > 0)
	cout << "\taddq\t$" << numBytes << ", %rsp" << endl;

    assign(this, rax);
}


/*
 * Function:	Block::generate
 *
 * Description:	Generate code for this block, which simply means we
 *		generate code for each statement within the block.
 */

void Block::generate()
{
    for (auto stmt : _stmts) {
	stmt->generate();

	for (auto reg : registers)
	    assert(reg->_node == nullptr);
    }
}


/*
 * Function:	Simple::generate
 *
 * Description:	Generate code for a simple (expression) statement, which
 *		means simply generating code for the expression.
 */

void Simple::generate()
{
    _expr->generate();
    assign(_expr, nullptr);
}


/*
 * Function:	Function::generate
 *
 * Description:	Generate code for this function, which entails allocating
 *		space for local variables, then emitting our prologue, the
 *		body of the function, and the epilogue.
 */

void Function::generate()
{
    int param_offset;
    unsigned size;
    Parameters *params;
    Symbols symbols;


    /* Assign offsets to the parameters and local variables. */

    param_offset = 2 * SIZEOF_REG;
    offset = param_offset;
    allocate(offset);


    /* Generate our prologue. */

    funcname = _id->name();
    cout << global_prefix << funcname << ":" << endl;
    cout << "\tpushq\t%rbp" << endl;
    cout << "\tmovq\t%rsp, %rbp" << endl;
    cout << "\tmovl\t$" << funcname << ".size, %eax" << endl;
    cout << "\tsubq\t%rax, %rsp" << endl;


    /* Spill any parameters. */

    params = _id->type().parameters();
    symbols = _body->declarations()->symbols();

    for (unsigned i = 0; i < NUM_PARAM_REGS; i ++)
	if (i < params->size()) {
	    size = symbols[i]->type().size();
	    cout << "\tmov" << suffix(size) << parameters[i]->name(size);
	    cout << ", " << symbols[i]->_offset << "(%rbp)" << endl;
	} else
	    break;


    /* Generate the body of this function. */

    _body->generate();


    /* Generate our epilogue. */

    cout << endl << global_prefix << funcname << ".exit:" << endl;
    cout << "\tmovq\t%rbp, %rsp" << endl;
    cout << "\tpopq\t%rbp" << endl;
    cout << "\tret" << endl << endl;

    offset -= align(offset - param_offset);
    cout << "\t.set\t" << funcname << ".size, " << -offset << endl;
    cout << "\t.globl\t" << global_prefix << funcname << endl << endl;
}


/*
 * Function:	generateGlobals
 *
 * Description:	Generate code for any global variable declarations.
 */

void generateGlobals(Scope *scope)
{
    const Symbols &symbols = scope->symbols();

    // Generating global symbols
    for (auto symbol : symbols)
        if (!symbol->type().isFunction()) {
            cout << "\t.comm\t" << global_prefix << symbol->name() << ", ";
            cout << symbol->type().size() << endl;
        }

    cout << "\t.data" << endl;

    // Print all strings in map
    map<string, Label>::iterator it;
    for(it=strings.begin(); it!=strings.end(); it++) {
        cout << it->second << ":\t.asciz\t\"" << escapeString(it->first) << "\"" << endl;
    }
}


/*
 * Function:	Assignment::generate
 *
 * Description:	Generate code for an assignment statement.
 *
 *		NOT FINISHED: Only works if the right-hand side is an
 *		integer literal and the left-hand side is an integer
 *		scalar.
 */

void Assignment::generate()
{
    cout << "#Assignment::generate" << endl;

    Expression *pointer;

    // Generate right
    _right->generate();

    // If LHS is a dereference
    if(_left->isDereference(pointer)) {
        // Generate pointer
        pointer->generate();

        // Load right into a register
        unsigned long val;
        if(!(_right->isNumber(val)))
        load(_right, getreg());

        // Load pointer into a register
        load(pointer, getreg());

        // Move right into pointer
        cout << "\tmov" << suffix(_left) << _right << ", (" << pointer << ")" << endl;

        // Unassign all registers
        assign(pointer, nullptr);
        assign(_right, nullptr);
    }

    // If LHS is NOT a deference
    else {
        // Load right into a register
        load(_right, getreg());

        // Move right into left
        cout << "\tmov" << suffix(_left) << _right << ", " << _left << endl;

        // Unassign all registers
        assign(_left, nullptr);
        assign(_right, nullptr);
    }    
}

void Add::generate() {
    cout << "#Add::generate" << endl;

    // Generate left and right
    _left->generate();
    _right->generate();

    // Load left (if not in register)
    if(_left->_register == nullptr)
        load(_left, getreg());
        
    // Run the operation
    cout << "\tadd" << suffix(_left) << _right << ", " << _left << endl;

    // Unassign right register
    assign(_right, nullptr);

    // Assign left register to this expression
    assign(this, _left->_register);
}

void Subtract::generate() {
    cout << "#Subtract::generate" << endl;

    // Generate left and right
    _left->generate();
    _right->generate();

    // Load left (if not in register)
    if(_left->_register == nullptr)
        load(_left, getreg());
        
    // Run the operation
    cout << "\tsub" << suffix(_left) << _right << ", " << _left << endl;

    // Unassign right register
    assign(_right, nullptr);

    // Assign left register to this expression
    assign(this, _left->_register);
}

void Multiply::generate() {
    cout << "#Multiply::generate" << endl;

    // Generate left and right
    _left->generate();
    _right->generate();

    // Load left (if not in register)
    if(_left->_register == nullptr)
        load(_left, getreg());
        
    // Run the operation
    cout << "\timul" << suffix(_left) << _right << ", " << _left << endl;

    // Unassign right register
    assign(_right, nullptr);

    // Assign left register to this expression
    assign(this, _left->_register);
}

void Divide::generate() {
    cout << "#Divide::generate" << endl;

    // Generate left and right
    _left->generate();
    _right->generate();

    // Load left into rax
    load(_left, rax);

    // Unload rdx
    load(nullptr, rdx);

    // Load right into rcx
    load(_right, rcx);

    // Sign extend rax into rdx
    if(rax->_node->type().size() == 4)
        cout << "\tcltd" << endl;
    else
        cout << "\tcqto" << endl;

    // idiv
    cout << "\tidiv" << suffix(_right) << _right << endl;

    // Unassign left and register registers
    assign(_left, nullptr);
    assign(_right, nullptr);

    // Assign this to correct register
    assign(this, rax);
}

void Remainder::generate() {
    cout << "#Remainder::generate" << endl;

    // Generate left and right
    _left->generate();
    _right->generate();

    // Load left into rax
    load(_left, rax);

    // Unload rdx
    load(nullptr, rdx);

    // Load right into rcx
    load(_right, rcx);

    // Sign extend rax into rdx
    if(rax->_node->type().size() == 4)
        cout << "\tcltd" << endl;
    else
        cout << "\tcqto" << endl;

    // idiv
    cout << "\tidiv" << suffix(_right) << _right << endl;

    // Unassign left and register registers
    assign(_left, nullptr);
    assign(_right, nullptr);

    // Assign this to correct register
    assign(this, rdx);
}

void LessThan::generate() {
    cout << "#LessThan::generate" << endl;

    // Generate left and right
    _left->generate();
    _right->generate();

    // Load left
    load(_left, getreg());

    // Compare left and right
    cout << "\tcmp" << suffix(_left) << _right << ", " << _left << endl;

    // Unassign left and right
    assign(_left, nullptr);
    assign(_right, nullptr);

    // Assign this to a register
    assign(this, getreg());

    // Store result of condition code in byte register
    cout << "\tsetl\t%al" << endl;

    // Zero-extend byte to long
    cout << "\tmovzbl\t%al, " << this->_register << endl;
}

void GreaterThan::generate() {
    cout << "#GreaterThan::generate" << endl;

    // Generate left and right
    _left->generate();
    _right->generate();

    // Load left
    load(_left, getreg());

    // Compare left and right
    cout << "\tcmp" << suffix(_left) << _right << ", " << _left << endl;

    // Unassign left and right
    assign(_left, nullptr);
    assign(_right, nullptr);

    // Assign this to a register
    assign(this, getreg());

    // Store result of condition code in byte register
    cout << "\tsetg\t%al" << endl;

    // Zero-extend byte to long
    cout << "\tmovzbl\t%al, " << this->_register << endl;
}

void LessOrEqual::generate() {
    cout << "#LessOrEqual::generate" << endl;

    // Generate left and right
    _left->generate();
    _right->generate();

    // Load left
    load(_left, getreg());

    // Compare left and right
    cout << "\tcmp" << suffix(_left) << _right << ", " << _left << endl;

    // Unassign left and right
    assign(_left, nullptr);
    assign(_right, nullptr);

    // Assign this to a register
    assign(this, getreg());

    // Store result of condition code in byte register
    cout << "\tsetle\t%al" << endl;

    // Zero-extend byte to long
    cout << "\tmovzbl\t%al, " << this->_register << endl;
}

void GreaterOrEqual::generate() {
    cout << "#GreaterOrEqual::generate" << endl;

    // Generate left and right
    _left->generate();
    _right->generate();

    // Load left
    load(_left, getreg());

    // Compare left and right
    cout << "\tcmp" << suffix(_left) << _right << ", " << _left << endl;

    // Unassign left and right
    assign(_left, nullptr);
    assign(_right, nullptr);

    // Assign this to a register
    assign(this, getreg());

    // Store result of condition code in byte register
    cout << "\tsetge\t%al" << endl;

    // Zero-extend byte to long
    cout << "\tmovzbl\t%al, " << this->_register << endl;
}

void Equal::generate() {
    cout << "#Equal::generate" << endl;

    // Generate left and right
    _left->generate();
    _right->generate();

    // Load left
    load(_left, getreg());

    // Compare left and right
    cout << "\tcmp" << suffix(_left) << _right << ", " << _left << endl;

    // Unassign left and right
    assign(_left, nullptr);
    assign(_right, nullptr);

    // Assign this to a register
    assign(this, getreg());

    // Store result of condition code in byte register
    cout << "\tsete\t%al" << endl;

    // Zero-extend byte to long
    cout << "\tmovzbl\t%al, " << this->_register << endl;
}

void NotEqual::generate() {
    cout << "#NotEqual::generate" << endl;

    // Generate left and right
    _left->generate();
    _right->generate();

    // Load left
    load(_left, getreg());

    // Compare left and right
    cout << "\tcmp" << suffix(_left) << _right << ", " << _left << endl;

    // Unassign left and right
    assign(_left, nullptr);
    assign(_right, nullptr);

    // Assign this to a register
    assign(this, getreg());

    // Store result of condition code in byte register
    cout << "\tsetne\t%al" << endl;

    // Zero-extend byte to long
    cout << "\tmovzbl\t%al, " << this->_register << endl;
}

void Not::generate() {
    cout << "#Not::generate" << endl;

    // Generate
    _expr->generate();

    // Load
    load(_expr, getreg());

    // Compare
    cout << "\tcmp" << suffix(_expr) << "$0, " << _expr << endl;

    // Set
    cout << "\tsete\t" << _expr->_register->byte() << endl;

    // Zero-extend
    cout << "\tmovzbl\t" << _expr->_register->byte() << ", " << _expr->_register << endl;

    // Assign result
    assign(this, _expr->_register);
}

void Negate::generate() {
    cout << "#Negate::generate" << endl;

    // Generate
    _expr->generate();

    // Load
    load(_expr, getreg());

    // Negate
    cout << "\tneg" << suffix(_expr) << _expr->_register << endl;

    // Assign result
    assign(this, _expr->_register);
}

void Expression::test(const Label &label, bool ifTrue) {
    cout << "#Expression::test" << endl;

    // Generate code for expression
    generate();

    // Load into register if not in register
    if(_register == nullptr)
        load(this, getreg());

    // Setup loop depending on the bool "ifTrue"
    cout << "\tcmp" << suffix(this) << "$0, " << this << endl;
    cout << (ifTrue ? "\tjne\t" : "\tje\t") << label << endl;

    // Deallocate this
    assign(this, nullptr);
}

void While::generate() {
    cout << "#While::generate" << endl;

    Label loop, exit;

    // Beginning of loop
    cout << loop << ":" << endl;

    // Test loop confdition
    _expr->test(exit, false);

    // Generate statements if condition satisfied
    _stmt->generate();

    // Jump back to loop
    cout << "\tjmp\t" << loop << endl;

    // End of While loop
    cout << exit << ":" << endl;
}

void Address::generate() {
    cout << "#Address::generate" << endl;

    Expression *pointer;

    if(_expr->isDereference(pointer)) {
        pointer->generate();

        if(pointer->_register == nullptr)
            load(pointer, getreg());
        
        assign(this, pointer->_register);
    }

    else {
        assign(this, getreg());
        cout << "\tleaq\t" << _expr << ", " << this << endl;
    }
}

void Dereference::generate() {
    cout << "#Dereference::generate" << endl;

    _expr->generate();
    

    if(_expr->_register == nullptr)
        load(_expr, getreg());

    cout << "\tmov" << suffix(this) << "\t(" << _expr << "), ";
    
    assign(this, _expr->_register);

    cout << this << endl;
}

void String::operand(ostream &ostr) const {
    map<string, Label>::iterator it;
    Label label;

    // Search for string key in map
    it = strings.find(_value);

    // If the string is not in the map
    if(it == strings.end()) {
        label = Label();
        strings.insert({_value, label});
    }

    // If the string is in the map
    else {
        label = it->second;
    }

    // Write label to ostr
    ostr << label;
}

void Return::generate() {
    cout << "#Return::generate" << endl;

    // Generate the return value
    _expr->generate();

    // Load into rax
    load(_expr, rax);

    // Jump to return label
    cout << "\tjmp\t" << funcname << ".exit" << endl;

    // Unassign register bound to _expr
    assign(_expr, nullptr);
}

// Generate the assembly code for casting an object to a different type
void Cast::generate() {
    cout << "#Cast::generate" << endl;

    Register *reg;
    unsigned source, target;

    source = _expr->type().size();
    target = _type.size();
    _expr->generate();

    if(source >= target) {
        // Load _expr into a register
        load(_expr, getreg());

        // Assign this to the expression
        assign(this, _expr->_register);
    }

    else {
        load(_expr, getreg());

        reg = getreg();

        // Byte to long
        if(source == 1 && target == 4) {
            cout << "\tmovsbl\t" << _expr->_register->name(source) << ", " << reg->name(target) << endl;
        }

        // Byte to quad
        else if(source == 1 && target == 8) {
            cout << "\tmovsbq\t" << _expr->_register->name(source) << ", " << reg->name(target) << endl;
        }

        // Long to quad
        else { // if(source == 4 && target == 8)
            cout << "\tmovslq\t" << _expr->_register->name(source) << ", " << reg->name(target) << endl;
        }

        // Bind this to the register with the casted value
        assign(this, reg);
    }

    // Unassign register bound to _expr
    assign(_expr, nullptr);
}

void LogicalAnd::generate() {
    cout << "#LogicalAnd::generate" << endl;

    Label L1, L2;

    // Test the left and right
    _left->test(L1, false);
    _right->test(L1, false);

    assign(this, getreg());

    // If here, then it is true
    cout << "\tmovl\t$1, " << this << endl;
    cout << "\tjmp\t" << L2 << endl;

    // L1; If here, then it is false
    cout << L1 << ":" << endl;
    cout << "\tmovl\t$0, " << this << endl;

    cout << L2 << ":" << endl;
}

void LogicalOr::generate() {
    cout << "#LogicalOr::generate" << endl;

    Label L1, L2;

    // Test the left and right
    _left->test(L1, true);
    _right->test(L1, true);

    assign(this, getreg());

    // If here, then it is false
    cout << "\tmovl\t$0, " << this << endl;
    cout << "\tjmp\t" << L2 << endl;

    // L1; If here, then it is true
    cout << L1 << ":" << endl;
    cout << "\tmovl\t$1, " << this << endl;

    cout << L2 << ":" << endl;
}

void For::generate() {
    cout << "#For::generate" << endl;

    Label loop, exit;

    // Generate initialization
    _init->generate();

    cout << loop << ":" << endl;

    // Test loop condition
    _expr->test(exit, false);

    // Generate statements and increment if condition satisfied
    _stmt->generate();
    _incr->generate();

    // Jump back to the loop
    cout << "\tjmp\t" << loop << endl;

    // End of For loop
    cout << exit << ":" << endl;
}

void If::generate() {
    cout << "#If::generate" << endl;

    Label elseLbl, exit;

    // Test loop condition
    _expr->test(elseLbl, false);

    // Generate Then statements if condition satisfied
    _thenStmt->generate();

    if(_elseStmt != nullptr) {
         cout << "\tjmp\t" << exit << endl;

        // Else label
        cout << elseLbl << ":" << endl;

        _elseStmt->generate();

        cout << exit << ":" << endl;
    }

    else {
        cout << elseLbl << ":" << endl;
    }
}