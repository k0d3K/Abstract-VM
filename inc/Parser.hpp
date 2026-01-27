#pragma once

#include "Operand.hpp"
#include "Lexer.hpp"
#include <list>

enum e_Operation {PUSH, ASSERT, POP, SWAP, DUMP, ADD, SUB, MUL, DIV, MOD, PRINT, EXIT, SORT, NONE};

typedef struct s_ParsedInstr
{
	e_Operation		instruction;
	e_OperandType	operandType;
	const IOperand	*operand;
	std::size_t	line;

}	t_ParsedInstr;

class Parser
{
public:
	static Parser& getInstance();

	std::list<t_ParsedInstr> parse(std::list<t_LexToken> &lexTokens) const;
	static void cleanTokens(const std::list<t_ParsedInstr>& tokens);

private:

	Parser	&operator=(Parser const & rhs);
	Parser(Parser const & rhs);
	Parser(void);
	~Parser(void);

	e_Operation toOperation(const std::string& opStr) const;
	e_OperandType toType(const std::string& type) const;

	static Parser * s_instance;
};
