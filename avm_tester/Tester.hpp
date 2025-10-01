#ifndef TESTER_HPP
#define TESTER_HPP

#include <iostream>
#include <typeinfo>

class Tester
{
private:
	enum Color { WHITE = 0, BOLT = 1, BLACK = 30, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN };
	static unsigned int num;
	static unsigned int subNum;
	static unsigned int nbCorrectAnswers;
	static unsigned int nbWrongAnswers;
	static const Color colorError = RED;
	static const Color colorSuccess = GREEN;
	static const Color colorTest = YELLOW;
	static const Color colorDiffField = BLUE;
	static const Color colorDiff = MAGENTA;

public:
	static void startTest(const char* testName = NULL);
	static void assertTrue(const bool& condition);
	static void assertFalse(const bool& condition);

	template<typename T>
	static void assertExpectedEqualsActual(const T& expected, const T& actual)
	{
		assertExpectedEqualsActual(expected, actual, true);
	}

	template<typename T>
	static void assertExpectedNotEqualsActual(const T& expected, const T& actual)
	{
		assertExpectedEqualsActual(expected, actual, false);
	}

	template<typename T>
	static void assertNull(const T* ptr)
	{
		assertExpectedEqualsActual((const T*)NULL, ptr);
	}

	template<typename T>
	static void assertNotNull(const T* ptr)
	{
		assertExpectedNotEqualsActual((const T*)NULL, ptr);
	}

	template<typename ExceptionType, typename Func>
	static void assertThrows(Func func)
	{
		try
		{
			func();
			printError(typeid(ExceptionType).name(), ("None"), true);
		}
		catch (const ExceptionType&)
		{
			printSuccess(); // Expected exception thrown
		}
		catch (const std::exception& e)
		{
			printError(typeid(ExceptionType).name(), typeid(e).name(), true); // Wrong exception thrown
		}
		++subNum;
	}

	template<typename Func>
	static void assertDoNotThrows(Func func)
	{
		try
		{
			func();
			printSuccess();
		}
		catch (const std::exception& e)
		{
			printError("None", typeid(e).name(), true); // Exception thrown
		}
		++subNum;
	}

	static void printResults()
	{
		int total = nbCorrectAnswers + nbWrongAnswers;

		std::cout << std::endl;
		std::cout << std::endl;
		setColorText(BLUE);
		setColorText(BOLT);
		std::cout << "═════════════════════════════════════════════════════════" << std::endl;
		
		std::cout << std::endl << "	Score " << nbCorrectAnswers << "/" << total << "  tests successful ✅" << std::endl;
		std::cout << std::endl;
		if (nbWrongAnswers == 0)
		{
			setColorText(GREEN);
			setColorText(BOLT);
			std::cout << "	🎉 Well done !! All tests passed! 🎉 " << std::endl;
		}
		else
		{
			setColorText(RED);
			setColorText(BOLT);
			std::cout << "	❌ FIX every problem right now !!! ❌" << std::endl;
		}

		std::cout << std::endl;
		setColorText(BLUE);
		setColorText(BOLT);
		std::cout << "═════════════════════════════════════════════════════════" << std::endl;
		resetColorText();
	}

private:
	Tester();

	static void setColorText(const Color color);
	static void resetColorText();
	static void printSuccess();

	template<typename T>
	static void assertExpectedEqualsActual(const T& expected, const T& actual, const bool& mustEquals)
	{
		const bool areEquals = (expected == actual);
		if (mustEquals == areEquals)
			printSuccess();
		else
			printError(expected, actual, mustEquals);
		++subNum;
	}

	template<typename T1, typename T2>
	static void printError(const T1& expected, const T2& actual, const bool& mustEquals)
	{
		setColorText(colorError);
		std::cerr << subNum << ".KO" << std::endl;
		setColorText(colorDiffField);
		std::cerr << "  Expected: ";
		setColorText(colorDiff);
		std::cerr << (mustEquals ? "" : "not ") << expected << std::endl;
		setColorText(colorDiffField);
		std::cerr << "  Actual: ";
		setColorText(colorDiff);
		std::cerr << actual << std::endl;
		resetColorText();
		nbWrongAnswers += 1;
	}
};

#endif