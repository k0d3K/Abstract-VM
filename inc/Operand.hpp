#pragma once

#include "IOperand.hpp"

template<typename T>
class	Operand :	public IOperand
{
public:
	Operand(T value);
	Operand(IOperand const & rhs);
	~Operand ();

	Operand &operator=(IOperand const & rhs);

	IOperand const * operator+(IOperand const & rhs) const; // Sum
	IOperand const * operator-(IOperand const & rhs) const; // Difference
	IOperand const * operator*(IOperand const & rhs) const; // Product
	IOperand const * operator/(IOperand const & rhs) const; // Quotient
	IOperand const * operator%(IOperand const & rhs) const; // Modulo

	bool operator==(IOperand const & rhs) const;
	bool operator<(IOperand const & rhs) const;
	bool operator>(IOperand const & rhs) const;

	int getPrecision(void) const;
	e_OperandType getType(void) const;
	std::string const & toString(void) const;

private:

	Operand();

	T				value_;
	std::string		str_;
	e_OperandType	type_;
	int				precision_;

};

#include "Operand.tpp"
