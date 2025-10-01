#pragma once

#include "IOperand.hpp"

class OperandFactory
{
public:
	static OperandFactory& getInstance();

	IOperand const	*createOperand(e_OperandType type, std::string const & value) const;

private:

	OperandFactory	&operator=(OperandFactory const & rhs);
	OperandFactory(OperandFactory const & rhs);
	OperandFactory(void);
	~OperandFactory(void);

	bool	isValidValue(std::string str) const;

	IOperand const	*createInt8(std::string const & value) const;
	IOperand const	*createInt16(std::string const & value) const;
	IOperand const	*createInt32(std::string const & value) const;
	IOperand const	*createFloat(std::string const & value) const;
	IOperand const	*createDouble(std::string const & value) const;

	static OperandFactory * s_instance;
};