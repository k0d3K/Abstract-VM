#include "Tester.hpp"

unsigned int Tester::num = 1;
unsigned int Tester::subNum = 1;
unsigned int Tester::nbCorrectAnswers = 0;
unsigned int Tester::nbWrongAnswers = 0;

void Tester::startTest(const char* testName)
{
	setColorText(colorTest);
	std::cout << std::endl << std::endl
		<< "===== Test " << num
		<< (testName == NULL ? "" : ": " + std::string(testName))
		<< " ====="
		<< std::endl;
	resetColorText();
	++num;
	subNum = 1;
}

void Tester::assertTrue(const bool& condition)
{
	assertExpectedEqualsActual(condition, true);
}

void Tester::assertFalse(const bool& condition)
{
	assertTrue(!condition);
}

void Tester::setColorText(const Color color)
{
	std::cout << "\033[" << color << "m";
}

void Tester::resetColorText()
{
	setColorText(WHITE);
}

void Tester::printSuccess()
{
	setColorText(colorSuccess);
	std::cout << subNum << ".OK " << std::flush;
	resetColorText();
	nbCorrectAnswers += 1;
}
