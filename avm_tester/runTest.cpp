#include <iostream>
#include <memory>
#include <array>
#include <fstream>
#include <string>
#include <cstdio>
#include <stdexcept>
#include <algorithm>

#include "Tester.hpp"

struct AVMResult
{
	std::string stdoutStr;
	std::string stderrStr;
};

AVMResult exec(const std::string& cmd)
{
	std::string safeCmd;
	for (char c : cmd)
	{
		if (c == '\\')
			safeCmd += "\\";
		else
			safeCmd += c;
	}
	std::string tmpErrFile = "/tmp/avm_stderr.txt";
	std::string fullCmd = "printf \"" + safeCmd + "\" | ./avm" + " 2> " + tmpErrFile;

	std::array<char, 128> buffer;
	std::string out;

	struct PcloseDeleter
	{
		void operator()(FILE* f) const
		{
			if (f)
				pclose(f);
		}
	};

	std::unique_ptr<FILE, PcloseDeleter> pipe(popen(fullCmd.c_str(), "r"));
	if (!pipe)
	{
		std::cerr << "popen() failed!" << std::endl;
		return {"", ""};
	}

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
		out += buffer.data();

	std::ifstream errFile(tmpErrFile);
	std::string err((std::istreambuf_iterator<char>(errFile)), std::istreambuf_iterator<char>());

	std::remove(tmpErrFile.c_str());
	return { out, err };
}

void AssertResult(std::string command, std::string result)
{
	AVMResult res = exec(command);
	Tester::assertExpectedEqualsActual(result, res.stdoutStr);
	Tester::assertExpectedEqualsActual(std::string(""), res.stderrStr);
}

void AssertError(std::string command, std::string result)
{
	AVMResult res = exec(command); 

	std::string s1 = res.stderrStr;
	std::string s2 = result;

	std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
	std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
	
	std::size_t found = s1.find(s2);
	if (found==std::string::npos)
		Tester::assertExpectedEqualsActual(result, res.stderrStr);
	else
		Tester::assertTrue(true);
	Tester::assertExpectedEqualsActual(std::string(""), res.stdoutStr);
}

void AssertBoth(std::string command, std::string std, std::string err)
{
	AVMResult res = exec(command);

	std::string s1 = res.stderrStr;
	std::string s2 = err;

	std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
	std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
	
	std::size_t found = s1.find(s2);
	if (found==std::string::npos)
		Tester::assertExpectedEqualsActual(err, res.stderrStr);
	else
		Tester::assertTrue(true);
	Tester::assertExpectedEqualsActual(std, res.stdoutStr);
}


void push_test()
{
	Tester::startTest("push");

	// Single int8 push + assert
	AssertResult("push int8(1)\nassert int8(1)\nexit\n", "");

	// Push multiple int8 values and dump (top of stack first)
	AssertResult("push int8(1)\npush int8(2)\npush int8(3)\ndump\nexit\n", "3\n2\n1\n");

	// Different integer types
	AssertResult("push int8(42)\npush int16(300)\npush int32(100000)\ndump\nexit\n", "100000\n300\n42\n");

	// Negative integers
	AssertResult("push int8(-5)\npush int16(-123)\ndump\nexit\n", "-123\n-5\n");

	// Zero
	AssertResult("push int32(0)\ndump\nexit\n", "0\n");

	// Max/min int8
	AssertResult("push int8(127)\npush int8(-128)\ndump\nexit\n", "-128\n127\n");

	// Float and double push, top of stack first
	AssertResult("push float(3.14)\npush double(2.71828)\ndump\nexit\n", "2.71828\n3.14\n");

	// Push with negative floats
	AssertResult("push float(-42.42)\npush double(-0.001)\ndump\nexit\n", "-0.001\n-42.42\n");

	// Mix integer and floats
	AssertResult("push int32(42)\npush float(1.5)\npush double(2.25)\ndump\nexit\n", "2.25\n1.5\n42\n");

	// Assert float/double value
	AssertResult("push float(3.14)\nassert float(3.14)\nexit\n", "");
	AssertResult("push double(2.71828)\nassert double(2.71828)\nexit\n", "");

	Tester::startTest("push errors");

	// Overflow int8
	AssertError("push int8(128)\nexit\n", "overflow");

	// Underflow int8
	AssertError("push int8(-129)\nexit\n", "underflow");

	// Invalid integer format
	AssertError("push int32(abc)\nexit\n", "value format");

	// Invalid float format
	AssertError("push float(3.14.15)\nexit\n", "value format");

	// Push then assert wrong value
	AssertError("push int8(1)\nassert int8(2)\nexit\n", "assert");

	// Push and then illegal command generates both stdout and stderr
	AssertBoth("push int32(42)\ndump\nunknowncmd\nexit\n", "", "unknown instruction");
}

void assert_test()
{
	Tester::startTest("assert");

	// Assert exact integer values
	AssertResult("push int8(10)\nassert int8(10)\nexit\n", "");
	AssertResult("push int16(1000)\nassert int16(1000)\nexit\n", "");
	AssertResult("push int32(-50000)\nassert int32(-50000)\nexit\n", "");

	// Assert float and double exact match
	AssertResult("push float(3.14159)\nassert float(3.14159)\nexit\n", "");
	AssertResult("push double(2.718281828)\nassert double(2.718281828)\nexit\n", "");
	AssertResult("push int8(42)\nassert float(42.0)\nexit\n", "");

	// Assert after multiple pushes (top of stack)
	AssertResult("push int8(1)\npush int8(2)\nassert int8(2)\nexit\n", "");
	AssertResult("push int32(12345)\npush double(0.99)\nassert double(0.99)\nexit\n", "");

	Tester::startTest("assert errors");

	// Assert fails due to different integer value
	AssertError("push int8(5)\nassert int8(6)\nexit\n", "assert");

	// Assert fails due to different float/double value
	AssertError("push float(1.23)\nassert float(1.24)\nexit\n", "assert");
	AssertError("push double(9.8765)\nassert double(9.8764)\nexit\n", "assert");

	// Assert fails due to type mismatch
	AssertError("push float(3.14)\nassert int32(3)\nexit\n", "assert");

	// Assert on empty stack
	AssertError("assert int8(0)\nexit\n", "empty");
}

void pop_test()
{
	Tester::startTest("pop");

	// Pop single element
	AssertResult("push int8(1)\npop\ndump\nexit\n", "");

	// Pop multiple elements, check remaining stack
	AssertResult("push int8(1)\npush int8(2)\npush int8(3)\npop\ndump\nexit\n", "2\n1\n");
	AssertResult("push int32(42)\npush float(3.14)\npop\ndump\nexit\n", "42\n");

	// Pop until stack empty
	AssertResult("push int8(5)\npop\npush int16(10)\npop\nexit\n", "");

	Tester::startTest("pop errors");

	// Pop on empty stack
	AssertError("pop\nexit\n", "empty");

	// Multiple pops exceeding stack size
	AssertError("push int8(1)\npop\npop\nexit\n", "empty");

	// Push and pop in sequence
	AssertResult("push int32(100)\npop\npush float(1.23)\npop\nexit\n", "");
}

void dump_test()
{
	Tester::startTest("dump");

	// Dump single int
	AssertResult("push int8(42)\ndump\nexit\n", "42\n");

	// Dump multiple integers
	AssertResult("push int8(1)\npush int16(200)\npush int32(3000)\ndump\nexit\n", "3000\n200\n1\n");

	// Dump floats and doubles
	AssertResult("push float(3.14)\npush double(2.71828)\ndump\nexit\n", "2.71828\n3.14\n");

	// Dump negative numbers and zero
	AssertResult("push int8(-5)\npush int16(0)\npush float(-3.14)\ndump\nexit\n", "-3.14\n0\n-5\n");

	// Dump after pop
	AssertResult("push int32(100)\npush float(1.5)\npop\ndump\nexit\n", "100\n");

	// Dump with mix of integer and float types
	AssertResult("push int8(10)\npush float(2.2)\npush double(3.3)\ndump\nexit\n", "3.3\n2.2\n10\n");

	// Dump edge integers
	AssertResult("push int8(127)\npush int8(-128)\ndump\nexit\n", "-128\n127\n");

	// Dump empty stack
	AssertResult("dump\nexit\n", "");

	Tester::startTest("dump errors");

	// Push then pop all, then dump (empty stack)
	AssertResult("push int16(5)\npop\ndump\nexit\n", "");

	// Multiple pops and dump
	AssertResult("push int8(1)\npush int8(2)\npop\npop\ndump\nexit\n", "");
}

void add_test()
{
	Tester::startTest("add");

	// Simple integer addition
	AssertResult("push int8(10)\npush int8(20)\nadd\ndump\nexit\n", "30\n");
	AssertResult("push int16(1000)\npush int16(2000)\nadd\ndump\nexit\n", "3000\n");
	AssertResult("push int32(50000)\npush int32(50000)\nadd\ndump\nexit\n", "100000\n");

	AssertResult("push int8(-5)\npush int8(-10)\nadd\ndump\nexit\n", "-15\n");
	AssertResult("push int16(-100)\npush int16(50)\nadd\ndump\nexit\n", "-50\n");

	// Float addition
	AssertResult("push float(1.5)\npush float(2.5)\nadd\ndump\nexit\n", "4\n");
	AssertResult("push double(2.718)\npush double(3.14159)\nadd\ndump\nexit\n", "5.85959\n");

	// Mixed float and int
	AssertResult("push int32(10)\npush float(2.5)\nadd\ndump\nexit\n", "12.5\n");

	// Edge integer values
	AssertResult("push int8(127)\npush int8(0)\nadd\ndump\nexit\n", "127\n");
	AssertResult("push int8(-128)\npush int8(0)\nadd\ndump\nexit\n", "-128\n");

	Tester::startTest("add errors");

	// Less than 2 elements
	AssertError("push int8(1)\nadd\nexit\n", "add");
 
	// Overflow / underflow 
	AssertError("push int8(127)\npush int8(1)\nadd\nexit\n", "overflow");
	AssertError("push int8(-128)\npush int8(-1)\nadd\nexit\n", "underflow");
	AssertError("push int16(32767)\npush int16(1)\nadd\nexit\n", "overflow");
	AssertError("push int16(-32768)\npush int16(-1)\nadd\nexit\n", "underflow");
	AssertError("push int32(2147483647)\npush int32(1)\nadd\nexit\n", "overflow");
	AssertError("push int32(-2147483648)\npush int32(-1)\nadd\nexit\n", "underflow");
	AssertError("push float(3.4028235e38)\npush float(1e38)\nadd\nexit\n", "overflow");
	AssertError("push float(-3.4028235e38)\npush float(-1e38)\nadd\nexit\n", "underflow");
	AssertError("push double(1.7976931348623157e308)\npush double(1e308)\nadd\nexit\n", "overflow");
	AssertError("push double(-1.7976931348623157e308)\npush double(-1e308)\nadd\nexit\n", "underflow");
}

void sub_test()
{
	Tester::startTest("sub");

	// Normal integer subtraction
	AssertResult("push int8(20)\npush int8(10)\nsub\ndump\nexit\n", "10\n");
	AssertResult("push int16(2000)\npush int16(1000)\nsub\ndump\nexit\n", "1000\n");
	AssertResult("push int32(50000)\npush int32(10000)\nsub\ndump\nexit\n", "40000\n");

	// Negative results
	AssertResult("push int8(10)\npush int8(20)\nsub\ndump\nexit\n", "-10\n");
	AssertResult("push int16(50)\npush int16(-100)\nsub\ndump\nexit\n", "150\n");

	// Float / double subtraction
	AssertResult("push float(3.5)\npush float(1.2)\nsub\ndump\nexit\n", "2.3\n");
	AssertResult("push double(5.718)\npush double(3.14159)\nsub\ndump\nexit\n", "2.57641\n");

	// Mixed int / float
	AssertResult("push int32(10)\npush float(2.5)\nsub\ndump\nexit\n", "7.5\n");
	AssertResult("push float(2.5)\npush int32(10)\nsub\ndump\nexit\n", "-7.5\n");

	// Edge integer values
	AssertResult("push int8(127)\npush int8(0)\nsub\ndump\nexit\n", "127\n");
	AssertResult("push int8(-128)\npush int8(0)\nsub\ndump\nexit\n", "-128\n");

	Tester::startTest("sub errors");

	// Less than 2 elements
	AssertError("push int8(1)\nsub\nexit\n", "sub");

	// Overflow / underflow 
	AssertError("push int8(127)\npush int8(-1)\nsub\nexit\n", "overflow");
	AssertError("push int8(-128)\npush int8(1)\nsub\nexit\n", "underflow");
	AssertError("push int16(32767)\npush int16(-1)\nsub\nexit\n", "overflow");
	AssertError("push int16(-32768)\npush int16(1)\nsub\nexit\n", "underflow");
	AssertError("push int32(2147483647)\npush int32(-1)\nsub\nexit\n", "overflow");
	AssertError("push int32(-2147483648)\npush int32(1)\nsub\nexit\n", "underflow");
	AssertError("push float(3.4028235e38)\npush float(-1e38)\nsub\nexit\n", "overflow");
	AssertError("push float(-3.4028235e38)\npush float(1e38)\nsub\nexit\n", "underflow");
	AssertError("push double(1.7976931348623157e308)\npush double(-1e308)\nsub\nexit\n", "overflow");
	AssertError("push double(-1.7976931348623157e308)\npush double(1e308)\nsub\nexit\n", "underflow");
}

void mul_test()
{
	Tester::startTest("mul");

	// Simple integer multiplication
	AssertResult("push int8(2)\npush int8(5)\nmul\ndump\nexit\n", "10\n");
	AssertResult("push int16(100)\npush int16(20)\nmul\ndump\nexit\n", "2000\n");
	AssertResult("push int32(5000)\npush int32(10)\nmul\ndump\nexit\n", "50000\n");

	// Negative numbers
	AssertResult("push int8(-2)\npush int8(5)\nmul\ndump\nexit\n", "-10\n");
	AssertResult("push int16(-10)\npush int16(-20)\nmul\ndump\nexit\n", "200\n");

	// Float / double multiplication
	AssertResult("push float(1.5)\npush float(2.0)\nmul\ndump\nexit\n", "3\n");
	AssertResult("push double(2.5)\npush double(4.0)\nmul\ndump\nexit\n", "10\n");

	// Mixed float and int
	AssertResult("push int32(10)\npush float(2.5)\nmul\ndump\nexit\n", "25\n");
	AssertResult("push float(2.5)\npush int32(10)\nmul\ndump\nexit\n", "25\n");

	// Zeros
	AssertResult("push int8(0)\npush int8(100)\nmul\ndump\nexit\n", "0\n");
	AssertResult("push float(0.0)\npush double(5.5)\nmul\ndump\nexit\n", "0\n");

	Tester::startTest("mul errors");

	// Less than 2 elements
	AssertError("push int8(1)\nmul\nexit\n", "mul");

	// Overflow / underflow
	AssertError("push int8(127)\npush int8(2)\nmul\nexit\n", "overflow");
	AssertError("push int8(-128)\npush int8(2)\nmul\nexit\n", "underflow");
	AssertError("push int16(32767)\npush int16(2)\nmul\nexit\n", "overflow");
	AssertError("push int16(-32768)\npush int16(2)\nmul\nexit\n", "underflow");
	AssertError("push int32(2147483647)\npush int32(2)\nmul\nexit\n", "overflow");
	AssertError("push int32(-2147483648)\npush int32(2)\nmul\nexit\n", "underflow");
	AssertError("push float(3.4e38)\npush float(2.0)\nmul\nexit\n", "overflow");
	AssertError("push float(-3.4e38)\npush float(2.0)\nmul\nexit\n", "underflow");
	AssertError("push double(1.7e308)\npush double(2.0)\nmul\nexit\n", "overflow");
	AssertError("push double(-1.7e308)\npush double(2.0)\nmul\nexit\n", "underflow");
}

void div_test()
{
	Tester::startTest("div");

	// Simple integer division
	AssertResult("push int8(20)\npush int8(5)\ndiv\ndump\nexit\n", "4\n");
	AssertResult("push int16(2000)\npush int16(10)\ndiv\ndump\nexit\n", "200\n");
	AssertResult("push int32(50000)\npush int32(10000)\ndiv\ndump\nexit\n", "5\n");

	// Negative numbers
	AssertResult("push int8(-20)\npush int8(5)\ndiv\ndump\nexit\n", "-4\n");
	AssertResult("push int16(-100)\npush int16(-25)\ndiv\ndump\nexit\n", "4\n");

	// Float / double division
	AssertResult("push float(3.5)\npush float(1.4)\ndiv\ndump\nexit\n", "2.5\n");
	AssertResult("push double(5.718)\npush double(2.0)\ndiv\ndump\nexit\n", "2.859\n");

	// Mixed float and int
	AssertResult("push int32(10)\npush float(2.5)\ndiv\ndump\nexit\n", "4\n");
	AssertResult("push float(2.5)\npush int32(10)\ndiv\ndump\nexit\n", "0.25\n");

	Tester::startTest("div errors");

	// Less than 2 elements
	AssertError("push int8(1)\ndiv\nexit\n", "div");

	// Division by zero
	AssertError("push int8(10)\npush int8(0)\ndiv\nexit\n", "division");
	AssertError("push float(3.14)\npush float(0.0)\ndiv\nexit\n", "division");
	AssertError("push double(2.718)\npush double(0.0)\ndiv\nexit\n", "division");

	// Integer overflow (INT_MIN / -1)
	AssertError("push int8(-128)\npush int8(-1)\ndiv\nexit\n", "overflow");
	AssertError("push int16(-32768)\npush int16(-1)\ndiv\nexit\n", "overflow");
	AssertError("push int32(-2147483648)\npush int32(-1)\ndiv\nexit\n", "overflow");
}

void mod_test()
{
	Tester::startTest("mod");

	// Simple integer modulo
	AssertResult("push int8(20)\npush int8(6)\nmod\ndump\nexit\n", "2\n");
	AssertResult("push int16(2000)\npush int16(300)\nmod\ndump\nexit\n", "200\n");
	AssertResult("push int32(50000)\npush int32(3000)\nmod\ndump\nexit\n", "2000\n");

	// Negative numbers
	AssertResult("push int8(-20)\npush int8(6)\nmod\ndump\nexit\n", "-2\n");
	AssertResult("push int16(200)\npush int16(-30)\nmod\ndump\nexit\n", "20\n");

	// (INT_MIN % -1)
	AssertResult("push int8(-128)\npush int8(-1)\nmod\ndump\nexit\n", "0\n");
	AssertResult("push int16(-32768)\npush int16(-1)\nmod\ndump\nexit\n", "0\n");
	AssertResult("push int32(-2147483648)\npush int32(-1)\nmod\ndump\nexit\n", "0\n");

	Tester::startTest("mod errors");

	// Less than 2 elements
	AssertError("push int8(1)\nmod\nexit\n", "mod");

	// Modulo by zero
	AssertError("push int8(10)\npush int8(0)\nmod\nexit\n", "modulo");
	AssertError("push int16(100)\npush int16(0)\nmod\nexit\n", "modulo");
	AssertError("push int32(1000)\npush int32(0)\nmod\nexit\n", "modulo");
}

void print_test()
{
	Tester::startTest("print");

	// Basic ASCII letters
	AssertResult("push int8(65)\nprint\nexit\n", "A\n");
	AssertResult("push int8(90)\nprint\nexit\n", "Z\n");
	AssertResult("push int8(97)\nprint\nexit\n", "a\n");
	AssertResult("push int8(122)\nprint\nexit\n", "z\n");

	// Digits and symbols
	AssertResult("push int8(48)\nprint\nexit\n", "0\n");
	AssertResult("push int8(57)\nprint\nexit\n", "9\n");
	AssertResult("push int8(33)\nprint\nexit\n", "!\n");
	AssertResult("push int8(63)\nprint\nexit\n", "?\n");

	// Multiple prints in a row
	AssertResult("push int8(72)\nprint\npush int8(101)\nprint\npush int8(108)\nprint\npush int8(108)\nprint\npush int8(111)\nprint\nexit\n", "H\ne\nl\nl\no\n");

	// Print after arithmetic
	AssertResult("push int8(60)\npush int8(5)\nadd\nprint\nexit\n", "A\n");
	AssertResult("push int8(100)\npush int8(1)\nsub\nprint\nexit\n", "c\n");

	Tester::startTest("print errors");

	// Empty stack
	AssertError("print\nexit\n", "empty");

	// Wrong types
	AssertError("push int16(65)\nprint\nexit\n", "print");
	AssertError("push int32(65)\nprint\nexit\n", "print");
	AssertError("push float(65.0)\nprint\nexit\n", "print");
	AssertError("push double(65.0)\nprint\nexit\n", "print");

	// Out of ASCII printable range
	AssertError("push int8(-1)\nprint\nexit\n", "print");
	AssertError("push int8(0)\nprint\nexit\n", "print");   // NUL
	AssertError("push int8(10)\nprint\nexit\n", "print");  // LF
	AssertError("push int8(31)\nprint\nexit\n", "print");  // unit separator
	AssertError("push int8(127)\nprint\nexit\n", "print"); // DEL non-printable
}

void exit_test()
{
	Tester::startTest("exit");

	// Simple program with exit at the end
	AssertResult("push int8(42)\nexit\n", "");

	// Exit after multiple instructions
	AssertResult("push int8(1)\npush int8(2)\nadd\nexit\n", "");

	Tester::startTest("exit errors");

	// Missing exit
	AssertError("push int8(42)\n", "exit");

	// Instructions after exit should be ignored and should not leak ;)
	AssertResult("push int8(5)\nexit\npush int8(10)\ndump\nexit\n", "");

	// Program with only exit
	AssertResult("exit\nexit\n", "");
}

void more_fun()
{
	Tester::startTest("more tests");

	AssertResult("push int8(10)\npush int16(3000)\npush int32(1000000)\nadd\nmul\ndump\nexit\n", "10030000\n");
	AssertError("push int8(127)\npush int16(32760)\nadd\nexit\n", "overflow");
	AssertError("push int8(-128)\npush int16(-32760)\nadd\nexit\n", "underflow");

	AssertResult("push int8(10)\npush int8(5)\nadd\npush int8(2)\nsub\npush int8(3)\nmul\npush int8(2)\ndiv\npush int8(3)\nmod\ndump\nexit\n", "1\n");
	AssertError("push int8(10)\npush int8(0)\ndiv\npush int8(5)\nadd\nexit\n", "division");

	AssertResult("push int8(2)\npush int8(3)\nadd\npush int8(4)\nmul\nassert int8(20)\nexit\n", "");
	AssertError("push int8(2)\npush int8(3)\nadd\npush int8(4)\nmul\nassert int8(21)\nexit\n", "assert");

	AssertError("pop\nassert int8(1)\npush int8(128)\npush int8(0)\ndiv\nexit\n", "overflow");
	AssertError("push int8(10)\npush int16(3000)\nadd\npush int8(2)\ndiv\npush int8(0)\nmod\nassert int32(1)\nexit\n", "modulo");

	AssertError("push int8(1)\npush int8(2)\nadd\npush int8(3)\nmul\npush int8(4)\nsub\npush int16(1000)\nadd\npush int32(50000)\nmul\npush int8(0)\ndiv\nexit\n", "division");

	AssertResult("push int8(2)\npush int8(3)\nadd\npush int8(4)\nsub\npush int8(5)\nmul\npush int8(2)\ndiv\npush int8(3)\nmod\ndump\nexit\n", "2\n");

	AssertResult("push int8(10)\npush int16(3000)\nadd\npush int32(100000)\nmul\npush float(3.5)\ndiv\npush double(2.5)\nmul\ndump\nexit\n", "215000000\n");
	AssertResult("push float(4.2)\npush int32(3000)\nmul\npush int16(239)\ndiv\npush int8(3)\nadd\ndump\nexit\n", "55.7197\n");
}

void test_comments()
{
	Tester::startTest("comments");

	AssertResult(";something\nexit\n", "");
	AssertResult(";something\n;exit\nexit\n", "");
	AssertError(";something\n;exit\n", "exit");
	AssertResult("push int8(42)\n;something\ndump\nexit\n", "42\n");
	AssertResult(";;;push int8(42)\n;something\ndump\nexit\n", "");
	AssertResult(";;;push int8(42)\n;something\ndump\nexit\n", "");
	AssertResult(";;;\nexit\n", "");

	AssertResult("push int32(8)\ndump\nexit\n;;\npush float(5.8)\ndump\n", "8\n");
	AssertBoth("push int32(8)\ndump\n;;\npush float(5.8)\ndump\n", "8\n", "exit");

}

void test_swap()
{
	Tester::startTest("swap");

	// Basic swap
	AssertResult("push int8(1)\npush int8(2)\nswap\ndump\nexit\n", "1\n2\n");

	// Swap + add
	AssertResult("push int8(3)\npush int8(5)\nswap\nadd\ndump\nexit\n", "8\n");

	// Swap + sub
	AssertResult("push int8(10)\npush int8(4)\nswap\nsub\ndump\nexit\n", "-6\n");

	// Swap + mul
	AssertResult("push int8(2)\npush int8(7)\nswap\nmul\ndump\nexit\n", "14\n");

	// Swap + div (integer division)
	AssertResult("push int8(3)\npush int8(10)\nswap\ndiv\ndump\nexit\n", "3\n");

	// Swap + mod
	AssertResult("push int8(10)\npush int8(3)\nswap\nmod\ndump\nexit\n", "3\n");

	// Swap + assert
	AssertResult("push int8(5)\npush int8(7)\nswap\nassert int8(5)\ndump\nexit\n", "5\n7\n");

	// Swap + complex combination
	AssertResult("push int8(2)\npush int8(3)\npush int8(4)\nswap\nadd\nswap\nmul\ndump\nexit\n", "14\n");

	Tester::startTest("swap");

	// Swap with overflow check
	AssertError("push int8(127)\npush int8(1)\nswap\nadd\nexit\n", "overflow");

	// Swap on undersized stack
	AssertError("swap\nexit\n", "stack");
	AssertError("push int8(5)\nswap\nexit\n", "stack");
}

int main()
{
	//parsing test
	push_test();
	assert_test();
	pop_test();
	dump_test();
	add_test();
	sub_test();
	sub_test();
	mul_test();
	div_test();
	mod_test();
	print_test();
	exit_test();
	more_fun();
	test_comments();
	std::cout << std::endl << std::endl << "########## BONUS PART ##########" << std::flush;
	test_swap();
	//test multi errors
	Tester::printResults();
	return 0;
}
