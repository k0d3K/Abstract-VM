#include "Lexer.hpp"

Lexer* Lexer::s_instance = nullptr;

Lexer& Lexer::getInstance() {return *s_instance;}

Lexer &Lexer::operator=(Lexer const & rhs) {(void)rhs; return *this;}

Lexer::Lexer(Lexer const & rhs) {(void)rhs;}

Lexer::Lexer(void) {s_instance = this;}

Lexer::~Lexer(void) {s_instance = nullptr;}

std::list<t_LexToken> Lexer::lexicalAnalisys(std::istream* input) const
{
	std::list<t_LexToken>	tokens;
	std::string				line;
	std::size_t				line_number = 0;

	while (std::getline(*input, line))
	{
		++line_number;
		if (input == &std::cin && line == ";;")
			break;
		if (line.empty() || line[0] == ';')
			continue;

		t_LexToken token;

		token.line = line_number;
		std::string::size_type pos = line.find(' ');
		if (pos == std::string::npos)
			token.instruction = line;
		else
		{
			token.instruction = line.substr(0, pos);
			std::string rest = line.substr(pos + 1);
			try
			{
				findOperandAndType(&token, rest);
			}
			catch (AVMException &e)
			{
				e.pushError(line_number);
			}
		}

		tokens.push_back(token);
	}
	return tokens;
}

void Lexer::findOperandAndType(t_LexToken *token, std::string rest) const
{
	std::size_t parenPos = rest.find('(');
	if (parenPos != std::string::npos)
	{
		token->operandType = rest.substr(0, parenPos);
		std::size_t endParen = rest.find(')', parenPos);
		if (endParen != std::string::npos)
			token->literal = rest.substr(parenPos + 1, endParen - parenPos - 1);
		else
		{
			token->literal = rest.substr(parenPos + 1);
			throw MissingParException(rest);
		}
	}
} 
