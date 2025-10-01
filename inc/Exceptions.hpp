#pragma once

#include <exception>

#include <iostream>
#include <list>

enum class e_ErrorType {
	InvalidTypeException,			\
	MissingParException,			\
	UnknownInstructionException,	\
	OverflowException,				\
	UnderflowException,				\
	EmtpyStackException,			\
	DivModByZeroException,			\
	NoExitException,				\
	FalseAssertException,			\
	ImpossibleInstructionException,	\
	InvalidValueFormatException,	\
	NoValueExpectedException,		\
	InvalidPrintException
};

struct Error
{
	std::size_t	line;
	e_ErrorType	type;
	std::string	error_part;
};

class AVMException : public std::exception
{
public:
	AVMException(e_ErrorType type, std::string error_part);
	AVMException(AVMException const & rhs);
	virtual ~AVMException() noexcept;

	static bool isError();
	static void printErrors();
	const char* what() const noexcept;

	void pushError(std::size_t line);

protected:
	std::string	current_msg_;

private:
	AVMException();
	AVMException	&operator=(AVMException const & rhs);

	static void sortErrors();

	static std::list<Error>	errors_;
	Error					current_errors_;
};

class InvalidTypeException : public AVMException
{
public:
	InvalidTypeException(std::string error_part);
	virtual ~InvalidTypeException() noexcept {};
};

class MissingParException : public AVMException
{
public:
	MissingParException(std::string error_part);
	virtual ~MissingParException() noexcept {};
};

class UnknownInstructionException : public AVMException
{
public:
	UnknownInstructionException(std::string error_part);
	virtual ~UnknownInstructionException() noexcept {};
};

class OverflowException : public AVMException
{
public:
	OverflowException(std::string error_part);
	virtual ~OverflowException() noexcept {};
};

class UnderflowException : public AVMException
{
public:
	UnderflowException(std::string error_part);
	virtual ~UnderflowException() noexcept {};
};

class EmtpyStackException : public AVMException
{
public:
	EmtpyStackException();
	virtual ~EmtpyStackException() noexcept {};
};

class DivModByZeroException : public AVMException
{
public:
	DivModByZeroException();
	virtual ~DivModByZeroException() noexcept {};
};

class NoExitException : public AVMException
{
public:
	NoExitException();
	virtual ~NoExitException() noexcept {};
};

class FalseAssertException : public AVMException
{
public:
	FalseAssertException();
	virtual ~FalseAssertException() noexcept {};
};

class ImpossibleInstructionException : public AVMException
{
public:
	ImpossibleInstructionException(std::string error_part);
	virtual ~ImpossibleInstructionException() noexcept {};
};

class InvalidValueFormatException : public AVMException
{
public:
	InvalidValueFormatException(std::string error_part);
	virtual ~InvalidValueFormatException() noexcept {};
};

class NoValueExpectedException : public AVMException
{
public:
	NoValueExpectedException();
	virtual ~NoValueExpectedException() noexcept {};
};

class InvalidPrintException : public AVMException
{
public:
	InvalidPrintException(std::string error_part);
	virtual ~InvalidPrintException() noexcept {};
};
