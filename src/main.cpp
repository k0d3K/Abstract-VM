#include <fstream>
#include <iostream>
#include "OperandFactory.hpp"
#include "Operand.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "CommandsExecutor.hpp"

#ifdef DEBUG
void printTokens(const std::list<t_LexToken>& tokens)
{
	int nb = 1;
	std::cout << "##### Lexer output #####" << std::endl;
	std::cout << "------------------------" << std::endl;
	for (const t_LexToken& token : tokens)
	{
		std::cout << nb++ << ":\n";
		std::cout << "  instruction: " << token.instruction << std::endl;
		std::cout << "  operandType: " << (token.operandType.empty() ? "<none>" : token.operandType) << std::endl;
		std::cout << "  literal:     " << (token.literal.empty() ? "<none>" : token.literal) << std::endl;
		std::cout << "------------------------" << std::endl;
	}
}

void printTokens(const std::list<t_ParsedInstr>& tokens)
{
	int nb = 1;
	std::cout << "##### Parser output #####" << std::endl;
	std::cout << "-------------------------" << std::endl;
	for (const t_ParsedInstr& token : tokens)
	{
		std::cout << nb++ << ":\n";
		std::cout << "  instruction: " << token.instruction << std::endl;
		std::cout << "  operandType: " << token.operandType << std::endl;
		std::cout << "  operand:     " << (token.operand == nullptr ? "<none>" : token.operand->toString()) << std::endl;
		std::cout << "-------------------------" << std::endl;
	}
}
#endif

int main(int argc, char **argv)
{
	std::istream* input = &std::cin;
	std::ifstream inFile;

	if (argc > 2)
	{
		std::cout << "Usage: ./avm [file]" << std::endl;
		return 1;
	}
	if (argc == 2)
	{
		if (std::string(argv[1]).empty())
		{
			std::cout << "Error: file argument must not be empty." << std::endl;
			return 1;
		}
		inFile.open(argv[1]);
		if (!inFile.is_open())
		{
			std::cout << "Error: could not open file " << argv[1] << std::endl;
			return 1;
		}
		input = &inFile;
	}

	std::list<t_LexToken> lexTokens = Lexer::getInstance().lexicalAnalisys(input);
	std::list<t_ParsedInstr> parstokens = Parser::getInstance().parse(lexTokens);
#ifdef DEBUG
	printTokens(lexTokens);
	std::cout << std::endl;
	printTokens(parstokens);
	std::cout << std::endl << "##### Program output #####" << std::endl << std::endl;
#endif
	if (AVMException::isError())
		AVMException::printErrors();
	else
		CommandsExecutor::getInstance().execute(parstokens);
	Parser::cleanTokens(parstokens);
	if (inFile.is_open())
		inFile.close();
	return 0;
}
