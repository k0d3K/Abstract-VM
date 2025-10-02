#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <type_traits>
#include "Operand.hpp"
#include "OperandFactory.hpp"
#include "Exceptions.hpp"

template <typename T>
Operand<T>::Operand() {}

template <typename T>
Operand<T>::Operand(T value) : value_(value), precision_(0)
{
	if (std::is_same<T, int8_t>::value)
		type_ = Int8;
	else if (std::is_same<T, int16_t>::value)
		type_ = Int16;
	else if (std::is_same<T, int32_t>::value)
		type_ = Int32;
	else if (std::is_same<T, float>::value)
	{
		type_ = Float;
		precision_ = std::numeric_limits<float>::digits10;
	}
	else if (std::is_same<T, double>::value)
	{
		type_ = Double;
		precision_ = std::numeric_limits<double>::digits10;
	}
	else
		throw InvalidTypeException("");

	std::ostringstream oss;
	if (type_ < Float)
		oss << std::to_string(value_);
	else
		oss << std::setprecision(precision_) << value_;
	str_ = oss.str();
}

template <typename T>
Operand<T>::Operand(IOperand const & rhs) {*this = rhs;}

template <typename T>
Operand<T>::~Operand() {}

template <typename T>
Operand<T> &Operand<T>::operator=(IOperand const & rhs)
{
	if (&rhs != this)
	{
		type_ = rhs.getType();
		precision_ = rhs.getPrecision();
		
		std::string str = rhs.toString();
		if (type_ == Int8)
			value_ = static_cast<int8_t>(std::stoi(str));
		else if (type_ == Int16)
			value_ = static_cast<int16_t>(std::stoi(str));
		else if (type_ == Int32)
			value_ = std::stoi(str);
		else if (type_ == Float)
			value_ = std::stof(str);
		else
			value_ = std::stod(str);
	}
	return *this;
}

template <typename T>
int Operand<T>::getPrecision(void) const {return precision_;}

template <typename T>
e_OperandType Operand<T>::getType(void) const {return type_;}

template <typename T>
std::string const & Operand<T>::toString(void) const {return str_;}

template <typename T>
IOperand const * Operand<T>::operator+(IOperand const & rhs) const
{
	std::string value;
	e_OperandType e = rhs.getType();

	if (type_ >= e)
		e = type_;

	if (e < Float)
	{
		value = std::to_string(static_cast<int64_t>(value_) + std::stoll(rhs.toString()));
		return (OperandFactory::getInstance().createOperand(e, value));
	}
	else
	{
		value = std::to_string(static_cast<double>(value_) + std::stold(rhs.toString()));
		return (OperandFactory::getInstance().createOperand(e, value));
	}
}

template <typename T>
IOperand const * Operand<T>::operator-(IOperand const & rhs) const
{
	std::string value;
	e_OperandType e = rhs.getType();

	if (type_ >= e)
		e = type_;

	if (e < Float)
	{
		value = std::to_string(static_cast<int64_t>(value_) - std::stoll(rhs.toString()));
		return (OperandFactory::getInstance().createOperand(e, value));
	}
	else
	{
		value = std::to_string(static_cast<double>(value_) - std::stold(rhs.toString()));
		return (OperandFactory::getInstance().createOperand(e, value));
	}
}

template <typename T>
IOperand const * Operand<T>::operator*(IOperand const & rhs) const
{
	std::string value;
	e_OperandType e = rhs.getType();

	if (type_ >= e)
		e = type_;

	if (e < Float)
	{
		value = std::to_string(static_cast<int64_t>(value_) * std::stoll(rhs.toString()));
		return (OperandFactory::getInstance().createOperand(e, value));
	}
	else
	{
		value = std::to_string(static_cast<double>(value_) * std::stold(rhs.toString()));
		return (OperandFactory::getInstance().createOperand(e, value));
	}
}

template <typename T>
IOperand const * Operand<T>::operator/(IOperand const & rhs) const
{
	std::string value;
	e_OperandType e = rhs.getType();

	if (type_ >= e)
		e = type_;

	if (e < Float)
	{
		long long divisor = std::stoll(rhs.toString());
		if (divisor == 0)
			throw DivModByZeroException();
		value = std::to_string(static_cast<int64_t>(value_) / divisor);
	}
	else
	{
		long double divisor = std::stold(rhs.toString());
		if (divisor == 0)
			throw DivModByZeroException();
		value = std::to_string(static_cast<double>(value_) / divisor);
	}
	return (OperandFactory::getInstance().createOperand(e, value));

}

template <typename T>
IOperand const * Operand<T>::operator%(IOperand const & rhs) const
{
	std::string value;
	e_OperandType e = rhs.getType();

	if (type_ >= e)
		e = type_;

	if (e < Float)
	{
		long long divisor = std::stoll(rhs.toString());
		if (divisor == 0)
			throw DivModByZeroException();
		value = std::to_string(static_cast<int64_t>(value_) % divisor);
	}
	else
	{
		long double divisor = std::stold(rhs.toString());
		if (divisor == 0)
			throw DivModByZeroException();
		value = std::to_string(fmod(static_cast<long double>(value_), divisor));
	}
	return (OperandFactory::getInstance().createOperand(e, value));
}

template <typename T>
bool	Operand<T>::operator==(IOperand const & rhs) const
{
	std::string value;
	e_OperandType e = rhs.getType();

	if (type_ >= e)
		e = type_;

	if (e < Float)
		return (static_cast<int32_t>(value_) == std::stoi(rhs.toString()));
	else if (e == Float)
		return (static_cast<float>(value_) == std::stof(rhs.toString()));
	else
		return (static_cast<double>(value_) == std::stod(rhs.toString()));
}

template <typename T>
inline bool Operand<T>::operator<(IOperand const &rhs) const
{
	std::string value;
	e_OperandType e = rhs.getType();

	if (type_ >= e)
		e = type_;

	if (e < Float)
		return (static_cast<int32_t>(value_) < std::stoi(rhs.toString()));
	else if (e == Float)
		return (static_cast<float>(value_) < std::stof(rhs.toString()));
	else
		return (static_cast<double>(value_) < std::stod(rhs.toString()));
}

template <typename T>
inline bool Operand<T>::operator>(IOperand const &rhs) const
{
	std::string value;
	e_OperandType e = rhs.getType();

	if (type_ >= e)
		e = type_;

	if (e < Float)
		return (static_cast<int32_t>(value_) > std::stoi(rhs.toString()));
	else if (e == Float)
		return (static_cast<float>(value_) > std::stof(rhs.toString()));
	else
		return (static_cast<double>(value_) > std::stod(rhs.toString()));
}
