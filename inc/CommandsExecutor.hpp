#pragma once

#include "Operand.hpp"
#include "Parser.hpp"

class CommandsExecutor
{
public:
	static CommandsExecutor& getInstance();

	void execute(std::list<t_ParsedInstr> &instructions);

private:

	CommandsExecutor	&operator=(CommandsExecutor const & rhs);
	CommandsExecutor(CommandsExecutor const & rhs);
	CommandsExecutor(void);
	~CommandsExecutor(void);

	void	push(const IOperand *operand);
	void	assert(const IOperand *operand);
	void	pop();
	void	swap();
	void	dump();
	void	add();
	void	sub();
	void	mul();
	void	div();
	void	mod();
	void	print();
	void	sort();
	void	exit();

	std::list<const IOperand *>	stack_;
	bool						exit_;
};