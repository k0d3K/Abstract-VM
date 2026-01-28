#include "CommandsExecutor.hpp"
#include "Exceptions.hpp"
#include <map>

CommandsExecutor& CommandsExecutor::getInstance()
{
	static CommandsExecutor instance;
	return instance;
}

CommandsExecutor &CommandsExecutor::operator=(CommandsExecutor const & rhs) {(void)rhs; return *this;}

CommandsExecutor::CommandsExecutor(CommandsExecutor const & rhs) {(void)rhs;}

CommandsExecutor::CommandsExecutor(void) : exit_(false), right_(nullptr), left_(nullptr) {}

CommandsExecutor::~CommandsExecutor(void)
{
	for (const IOperand * operand : stack_)
	{
		if (operand != nullptr)
			delete operand;
	}
	if (left_ != nullptr)
	{
		delete left_;
		left_ = nullptr;
	}
	if (right_ != nullptr)
	{
		delete right_;
		right_ = nullptr;
	}
}

void CommandsExecutor::push(const IOperand *operand)
{
	stack_.push_back(operand);
}

void CommandsExecutor::assert(const IOperand *operand)
{
	if (stack_.empty())
	{
		delete operand;
		throw EmtpyStackException();
	}
	const IOperand * top = stack_.back();
	if (!(*top == *operand))
	{
		delete operand;
		throw FalseAssertException();
	}
	delete operand;
}

void CommandsExecutor::pop()
{
	if (stack_.empty())
		throw EmtpyStackException();
	const IOperand * del = stack_.back();
	stack_.pop_back();
	delete del;
	del = nullptr;
}

void CommandsExecutor::swap()
{
	if (stack_.size() < 2)
		throw ImpossibleInstructionException("swap");
	right_ = stack_.back();
	stack_.pop_back();
	left_ = stack_.back();
	stack_.pop_back();
	stack_.push_back(right_);
	stack_.push_back(left_);
}

void CommandsExecutor::dump()
{
	for (auto it = stack_.rbegin(); it != stack_.rend(); ++it)
	{
		const IOperand* operand = *it;
		if (operand != nullptr)
			std::cout << operand->toString() << std::endl;
	}
}

void CommandsExecutor::add()
{
	if (stack_.size() < 2)
		throw ImpossibleInstructionException("add");
	right_ = stack_.back();
	stack_.pop_back();
	left_ = stack_.back();
	stack_.pop_back();
	const IOperand * result = *left_ + *right_;
	stack_.push_back(result);
	delete left_;
	left_ = nullptr;
	delete right_;
	right_ = nullptr;
}

void CommandsExecutor::sub()
{
	if (stack_.size() < 2)
		throw ImpossibleInstructionException("sub");
	right_ = stack_.back();
	stack_.pop_back();
	left_ = stack_.back();
	stack_.pop_back();
	const IOperand * result = *left_ - *right_;
	stack_.push_back(result);
	delete left_;
	left_ = nullptr;
	delete right_;
	right_ = nullptr;
}

void CommandsExecutor::mul()
{
	if (stack_.size() < 2)
		throw ImpossibleInstructionException("mul");
	right_ = stack_.back();
	stack_.pop_back();
	left_ = stack_.back();
	stack_.pop_back();
	const IOperand * result = *left_ * *right_;
	stack_.push_back(result);
	delete left_;
	left_ = nullptr;
	delete right_;
	right_ = nullptr;
}

void CommandsExecutor::div()
{
	if (stack_.size() < 2)
		throw ImpossibleInstructionException("div");
	right_ = stack_.back();
	stack_.pop_back();
	left_ = stack_.back();
	stack_.pop_back();
	const IOperand * result = *left_ / *right_;
	stack_.push_back(result);
	delete left_;
	left_ = nullptr;
	delete right_;
	right_ = nullptr;
}

void CommandsExecutor::mod()
{
	if (stack_.size() < 2)
		throw ImpossibleInstructionException("mod");
	right_ = stack_.back();
	stack_.pop_back();
	left_ = stack_.back();
	stack_.pop_back();
	const IOperand * result = *left_ % *right_;
	stack_.push_back(result);
	delete left_;
	left_ = nullptr;
	delete right_;
	right_ = nullptr;
}

void CommandsExecutor::print()
{
	if (stack_.empty())
		throw EmtpyStackException();
	const IOperand * operand = stack_.back();
	if (operand->getType() != Int8)
		throw InvalidPrintException("not an int8");
	int8_t number = static_cast<int8_t>(std::stoi(operand->toString()));
	if (!isprint(number))
		throw InvalidPrintException(operand->toString() + " is not printable");
	std::cout << number << std::endl;
}

static bool compareForStack(const IOperand* a, const IOperand* b)
{
	return *a < *b;
}

void CommandsExecutor::sort()
{
	stack_.sort(compareForStack);
}

void CommandsExecutor::exit()
{
	exit_ = true;
}

void CommandsExecutor::execute(std::list<t_ParsedInstr> &instructions)
{
	static const std::map<e_Operation, void (CommandsExecutor::*)()> noArgOps = {
		{POP, &CommandsExecutor::pop},
		{SWAP, &CommandsExecutor::swap},
		{DUMP, &CommandsExecutor::dump},
		{ADD, &CommandsExecutor::add},
		{SUB, &CommandsExecutor::sub},
		{MUL, &CommandsExecutor::mul},
		{DIV, &CommandsExecutor::div},
		{MOD, &CommandsExecutor::mod},
		{PRINT,&CommandsExecutor::print},
		{SORT,&CommandsExecutor::sort},
		{EXIT, &CommandsExecutor::exit}
	};
	static const std::map<e_Operation, void (CommandsExecutor::*)(const IOperand *operand)> argOps = {
		{PUSH, &CommandsExecutor::push},
		{ASSERT, &CommandsExecutor::assert}
	};

	std::size_t line = 0;
	try
	{
		for (auto& instr : instructions)
		{
			line = instr.line;
			if (argOps.count(instr.instruction))
			{
				const IOperand *tmp = instr.operand;
				auto fn = argOps.at(instr.instruction);
				instr.operand = nullptr;
				(this->*fn)(tmp);
			}
			else if (noArgOps.count(instr.instruction))
			{
				auto fn = noArgOps.at(instr.instruction);
				(this->*fn)();
			}
			instr.operand = nullptr;
			if (exit_)
				return;
		}
		throw NoExitException();
	}
	catch (AVMException& e)
	{
		e.pushError(line);
		std::cerr << e.what() << std::endl;
	}
}
