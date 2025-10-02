#include "Exceptions.hpp"
#include "OperandFactory.hpp"
#include "Operand.hpp"

OperandFactory* OperandFactory::s_instance = nullptr;

OperandFactory& OperandFactory::getInstance() {return *s_instance;}

IOperand const *OperandFactory::createOperand(e_OperandType type, std::string const & value) const
{
	if (value.empty())
		throw InvalidValueFormatException("cannot be empty");
	if (!isValidValue(type, value))
		throw InvalidValueFormatException(value);
	switch (type)
	{
		case Int8:
			return createInt8(value);
		case Int16:
			return createInt16(value);
		case Int32:
			return createInt32(value);
		case Float:
			return createFloat(value);
		case Double:
			return createDouble(value);
		default:
			break;
	}
	return nullptr; // impossible case
}

OperandFactory &OperandFactory::operator=(OperandFactory const & rhs) {(void)rhs; return *this;}

OperandFactory::OperandFactory(OperandFactory const & rhs) {(void)rhs;}

OperandFactory::OperandFactory(void) {s_instance = this;}

OperandFactory::~OperandFactory(void) {s_instance = nullptr;}

bool OperandFactory::isValidValue(e_OperandType type, std::string str) const
{
	if (str.empty())
		return false;

	size_t i = 0;
	bool hasDot = (type < Float);
	bool hasExp = (type < Float);

	if (str[0] == '+' || str[0] == '-')
		i++;

	if (i == str.size())
		return false;

	for (; i < str.size(); ++i)
	{
		if (str[i] == '.')
		{
			if (hasDot || hasExp)
				return false;
			hasDot = true;
		}
		else if (str[i] == 'e')
		{
			if (hasExp)
				return false;
			hasExp = true;
		}
		else if (!std::isdigit(str[i]))
			return false;
	}

	return true;
}

IOperand const	*OperandFactory::createInt8(std::string const & value) const
{
	int64_t num = 0;
	try
	{
		num = std::stoll(value);
	}
	catch (const std::invalid_argument&)
	{
		throw InvalidValueFormatException(value);
	}
	catch (const std::out_of_range&)
	{
		throw OverflowException(value + " is not int8 type");
	}

	if (num > std::numeric_limits<int8_t>::max())
		throw  OverflowException(value + " is not int8 type");
	else if (num < std::numeric_limits<int8_t>::min())
		throw  UnderflowException(value + " is not int8 type");
	else
		return (new Operand<int8_t>(static_cast<int8_t>(num)));
}

IOperand const	*OperandFactory::createInt16(std::string const & value) const
{
	int64_t num = 0;
	try
	{
		num = std::stoll(value);
	}
	catch (const std::invalid_argument&)
	{
		throw InvalidValueFormatException(value);
	}
	catch (const std::out_of_range&)
	{
		throw OverflowException(value + " is not int16 type");
	}

	if (num > std::numeric_limits<int16_t>::max())
		throw OverflowException(value + " is not int16 type");
	else if (num < std::numeric_limits<int16_t>::min())
		throw UnderflowException(value + " is not int16 type");
	else
		return (new Operand<int16_t>(static_cast<int16_t>(num)));
}

IOperand const	*OperandFactory::createInt32(std::string const & value) const
{
	int64_t num = 0;
	try
	{
		num = std::stoll(value);
	}
	catch (const std::invalid_argument&)
	{
		throw InvalidValueFormatException(value);
	}
	catch (const std::out_of_range&)
	{
		throw OverflowException(value + " is not int32 type");
	}

	if (num > std::numeric_limits<int32_t>::max())
		throw OverflowException(value + " is not int32 type");
	else if (num < std::numeric_limits<int32_t>::min())
		throw UnderflowException(value + " is not int32 type");
	else
		return (new Operand<int32_t>(static_cast<int32_t>(num)));
}

IOperand const	*OperandFactory::createFloat(std::string const & value) const
{
	long double num = 0;
	try
	{
		num = std::stold(value);
	}
	catch (const std::invalid_argument&)
	{
		throw InvalidValueFormatException(value);
	}
	catch (const std::out_of_range&)
	{
		throw OverflowException(value + " is not float type");
	}

	if (num > std::numeric_limits<float>::max())
		throw OverflowException(value + " is not float type");
	else if (num < std::numeric_limits<float>::lowest())
		throw UnderflowException(value + " is not float type");
	else
		return (new Operand<float>(static_cast<float>(num)));
}	

IOperand const	*OperandFactory::createDouble(std::string const & value) const
{
	long double num = 0;
	try
	{
		num = std::stold(value);
	}
	catch (const std::invalid_argument&)
	{
		throw InvalidValueFormatException(value);
	}
	catch (const std::out_of_range&)
	{
		throw OverflowException(value + " is not double type");
	}

	if (num > std::numeric_limits<double>::max())
		throw OverflowException(value + " is not double type");
	else if (num < std::numeric_limits<double>::lowest())
		throw UnderflowException(value + " is not double type");
	else
		return (new Operand<double>(static_cast<double>(num)));
}

