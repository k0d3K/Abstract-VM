#include "Parser.hpp"
#include <map>

Parser* Parser::s_instance = nullptr;

Parser& Parser::getInstance() {return *s_instance;}

Parser &Parser::operator=(Parser const & rhs) {(void)rhs; return *this;}

Parser::Parser(Parser const & rhs) {(void)rhs;}

Parser::Parser(void) {s_instance = this;}

Parser::~Parser(void) {s_instance = nullptr;}

e_Operation Parser::toOperation(const std::string& opStr) const
{
	static const std::map<std::string, e_Operation> opMap = {
		{"push", PUSH},
		{"pop", POP},
		{"swap", SWAP},
		{"dump", DUMP},
		{"assert", ASSERT},
		{"add", ADD},
		{"sub", SUB},
		{"mul", MUL},
		{"div", DIV},
		{"mod", MOD},
		{"print", PRINT},
		{"sort", SORT},
		{"exit", EXIT}
	};

	auto it = opMap.find(opStr);
	if (it == opMap.end())
		throw UnknownInstructionException(opStr);

	return it->second;
}

e_OperandType Parser::toType(const std::string& type) const
{
	static const std::map<std::string, e_OperandType> typeMap = {
		{"int8", Int8},
		{"int16", Int16},
		{"int32", Int32},
		{"float", Float},
		{"double", Double}
	};

	auto it = typeMap.find(type);
	if (it == typeMap.end())
		throw InvalidTypeException(type);

	return it->second;
}

std::list<t_ParsedInstr> Parser::parse(std::list<t_LexToken> &lexTokens) const
{
	std::list<t_ParsedInstr>	parsTokens;

	for (const t_LexToken& lexToken : lexTokens)
	{
		t_ParsedInstr	parsToken = {NONE, NoType, nullptr, lexToken.line};
		try
		{
			parsToken.instruction = toOperation(lexToken.instruction);
		}
		catch (AVMException &e)
		{
			e.pushError(lexToken.line);
		}
		if (parsToken.instruction <= ASSERT)
		{
			try
			{
				parsToken.operandType = toType(lexToken.operandType);
			}
			catch (AVMException &e)
			{
				e.pushError(lexToken.line);
			}

			try
			{
				parsToken.operand = OperandFactory::getInstance().createOperand(parsToken.operandType, lexToken.literal);;
			}
			catch (AVMException &e)
			{
				e.pushError(lexToken.line);
			}
		}
		else if (!lexToken.operandType.empty() || !lexToken.literal.empty())
		{
			try
			{
				throw NoValueExpectedException();
			}
			catch (AVMException &e)
			{
				e.pushError(lexToken.line);
			}
		}

		parsTokens.push_back(parsToken);
	}
	return parsTokens;
}

void Parser::cleanTokens(const std::list<t_ParsedInstr>& tokens)
{
	for (const t_ParsedInstr& token : tokens)
		if (token.operand != nullptr)
			delete token.operand;
}

