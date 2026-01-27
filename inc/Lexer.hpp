#pragma once

#include "Operand.hpp"
#include <list>

typedef struct s_LexToken
{
	std::string	instruction;
	std::string	operandType;
	std::string	literal;
	std::size_t	line;

}	t_LexToken;

class Lexer
{
public:
	static Lexer& getInstance();

	std::list<t_LexToken> lexicalAnalisys(std::istream* input) const;

private:

	Lexer &operator=(Lexer const & rhs);
	Lexer(Lexer const & rhs);
	Lexer(void);
	~Lexer(void);

	void findOperandAndType(t_LexToken *token, std::string rest) const;

	static Lexer * s_instance;
};
