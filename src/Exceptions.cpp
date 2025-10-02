#include <map>
#include <sstream>

#include "Exceptions.hpp"

// Definition of the static member
std::list<Error> AVMException::errors_;

AVMException::AVMException() {}

AVMException::AVMException(e_ErrorType type, std::string error_part)
{
	current_errors_.type = type;
	current_errors_.error_part = error_part;
}

AVMException::AVMException(AVMException const & rhs) {(void)rhs;}

AVMException::~AVMException() noexcept {}

AVMException	&AVMException::operator=(AVMException const & rhs) {(void)rhs; return *this;}
 
bool AVMException::isError() {return !errors_.empty();}

static bool compareByLine(const Error& a, const Error& b)
{
	return a.line < b.line;
}

void AVMException::sortErrors()
{
	errors_.sort(compareByLine);
}

static std::string builtErrorMessage(Error &error)
{
	std::string	errorMessage;
	const char*	explanation = "Unknown error";

	static const std::map<e_ErrorType, const char *> explain = {
		{e_ErrorType::InvalidTypeException, "syntax error : unknown type"},
		{e_ErrorType::MissingParException, "syntax error : missing parenthesis"},
		{e_ErrorType::UnknownInstructionException, " unknown instruction"},
		{e_ErrorType::OverflowException, "overflow"},
		{e_ErrorType::UnderflowException, "underflow"},
		{e_ErrorType::EmtpyStackException, "impossible instruction, the stack is empty"},
		{e_ErrorType::DivModByZeroException, "division or modulo by 0"},
		{e_ErrorType::NoExitException, "no exit instruction at the end of the program"},
		{e_ErrorType::FalseAssertException,"the execution stoped because of a false assertion"},
		{e_ErrorType::ImpossibleInstructionException, "the stack is composed of strictly less that two values when an arithmetic instruction is executed"},
		{e_ErrorType::InvalidValueFormatException, "invalid value format fot the given type"},
		{e_ErrorType::NoValueExpectedException, "no value expected for this instruction"},
		{e_ErrorType::InvalidPrintException, "impossible to print"}
	};

	auto it = explain.find(error.type);
	if (it != explain.end())
		explanation = it->second;

	std::ostringstream oss;
	oss << "Error line " << error.line << ": " << explanation ;
	if (!error.error_part.empty())
		oss << " --> " << error.error_part;
	errorMessage = oss.str();

	return errorMessage;
}

void AVMException::printErrors()
{
	sortErrors();

	for (Error& error : errors_)
	{
		std::cerr << builtErrorMessage(error) << std::endl;
	}
}

void AVMException::pushError(std::size_t line)
{
	current_errors_.line = line;

	errors_.push_back(current_errors_);
	current_msg_ = builtErrorMessage(errors_.back());

}

const char* AVMException::what() const noexcept
{
	return current_msg_.c_str();
}

InvalidTypeException::InvalidTypeException(std::string error_part) : AVMException(e_ErrorType::InvalidTypeException, error_part) {}

MissingParException::MissingParException(std::string error_part) : AVMException(e_ErrorType::MissingParException, error_part) {}

UnknownInstructionException::UnknownInstructionException(std::string error_part) : AVMException(e_ErrorType::UnknownInstructionException, error_part) {}

OverflowException::OverflowException(std::string error_part) : AVMException(e_ErrorType::OverflowException, error_part) {}

UnderflowException::UnderflowException(std::string error_part) : AVMException(e_ErrorType::UnderflowException, error_part) {}

EmtpyStackException::EmtpyStackException() : AVMException(e_ErrorType::EmtpyStackException, "") {}

DivModByZeroException::DivModByZeroException() : AVMException(e_ErrorType::DivModByZeroException, "") {}

NoExitException::NoExitException() : AVMException(e_ErrorType::NoExitException, "") {}

FalseAssertException::FalseAssertException() : AVMException(e_ErrorType::FalseAssertException, "") {}

ImpossibleInstructionException::ImpossibleInstructionException(std::string error_part) : AVMException(e_ErrorType::ImpossibleInstructionException, error_part) {}

InvalidValueFormatException::InvalidValueFormatException(std::string error_part) : AVMException(e_ErrorType::InvalidValueFormatException, error_part) {}

NoValueExpectedException::NoValueExpectedException() : AVMException(e_ErrorType::NoValueExpectedException, "") {}

InvalidPrintException::InvalidPrintException(std::string error_part) : AVMException(e_ErrorType::InvalidPrintException, error_part) {}