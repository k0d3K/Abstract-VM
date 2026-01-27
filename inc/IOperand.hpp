#pragma once

# include <iostream>
# include <string>
# include <stdint.h>
# include <float.h>

enum	e_OperandType {Int8, Int16, Int32, Float, Double, NoType};

class	IOperand
{
public:
	virtual int getPrecision(void) const = 0; // Precision of the type of the instance
	virtual e_OperandType getType(void) const = 0; // Type of the instance

	virtual IOperand const * operator+(IOperand const & rhs) const = 0; // Sum
	virtual IOperand const * operator-(IOperand const & rhs) const = 0; // Difference
	virtual IOperand const * operator*(IOperand const & rhs) const = 0; // Product
	virtual IOperand const * operator/(IOperand const & rhs) const = 0; // Quotient
	virtual IOperand const * operator%(IOperand const & rhs) const = 0; // Modulo

	virtual bool	operator==(IOperand const & rhs) const = 0; // Assert
	virtual bool	operator<(IOperand const & rhs) const = 0;
	virtual bool	operator>(IOperand const & rhs) const = 0;

	virtual std::string const & toString(void) const = 0; // String representation of the instance

	virtual ~IOperand(void) {}
};
