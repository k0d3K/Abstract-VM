# Abstract VM -- C++ Project

The purpose of this project is to create a simple virtual machine that can
interpret programs written in a basic assembly language.

## Presentation
AbstractVM is a machine that uses a stack to compute simple arithmetic expressions.
These arithmetic expressions are provided to the machine as basic assembly programs.

### Instructions
As for any assembly language, the language of AbstractVM is composed of a series of instructions, with one instruction per line.
However, AbstractVM’s assembly language has a limited type system, which is a major difference from other real world assembly languages.

- **push v**: Pushes the value v at the top of the stack.
- **pop**: Unstacks the value from the top of the stack.
- **dump**: Displays each value of the stack, from the most recent one to the oldest one
- **assert v**: Asserts that the value at the top of the stack is equal to the one passed as parameter for this instruction. If it is not the case, the program execution stop with an error.
- **add**: Adds the first two values on the stack.
- **sub**: Subtracts the first two values on the stack.
- **mul**: Multiplies the first two values on the stack.
- **div**: Divides the first two values on the stack.
- **mod**: Calculates the modulus of the first two values on the stack.
- **swap**: Swaps the first two values on the stack.
- **sort**: Sort all the value of the stack (greatest on top).
- **print**: Asserts that the value at the top of the stack is an 8-bit integer, prints the corresponding ASCII value.
- **exit**: Terminate the execution of the current program.

### Values
The values must have one of the following form:
- **int8(n)** : Creates an 8-bit integer with value n.
- **int16(n)** : Creates a 16-bit integer with value n.
- **int32(n)** : Creates a 32-bit integer with value n.
- **float(z)** : Creates a float with value z.
- **double(z)** : Creates a double with value z.

### Grammar
The assembly language of AbstractVM is generated from the following grammar (# corresponds to the end of the input, not to the character ’#’):
```
S := INSTR [SEP INSTR]* #

INSTR := <instruction> [value]

N := [-]?[0..9]+

Z := [-]?[0..9]+.[0..9]+

SEP := '\n'+
```

> Note: Any text following `;` on a line is considered a comment and is ignored by the parser.

### Error management
The Abstract VM implements a centralized and non-intrusive error management system based on custom exceptions.

Unlike a traditional throw-and-stop approach, parsing errors are collected, stored, and reported in a controlled way, allowing the program to continue parsing instructions and display all detected parsing errors at once.
Execution errors will terminate the program immediately.

The Abstract VM can detect and report the following errors, grouped by parsing and execution phases:

**Parsing errors**:
- An instruction is unknown
- Overflow on a value
- Underflow on a value
- The program doesn’t have an exit instruction
- Invalid or unsupported operand type.
- Missing or malformed parentheses in an operand.
- Invalid value format for an operand.
- A value was provided for an instruction that does not accept one.

**Execution errors**:
- Operation requires values but the stack is empty.
- An assert instruction is not true
- The stack is composed of strictly less than two values when an arithmetic instruction is executed.
- Division or modulo by zero.
- Instruction is syntactically valid but cannot be executed in the current context.
- Print instruction used on an invalid operand type.

## Execution

### Compilation
- Compile the standard program:
```
make
```
This builds the `avm` executable.

- Compile the debug version:
```
make debug
```
This builds `avm_debug`, which works like avm but also prints parsing and lexing debug information.

- Compile the tester:
```
make test
```
This builds `avm_tester`, used during development to verify program behavior.

### Running
The machine can to run programs from a file passed as a parameter and from the standard input. When reading from the standard input, the end of the program is indicated by the special symbol `;;` otherwise absent.

Examples:
```
$>./avm
push int32(2)
push int32(3)
add
assert int32(5)
dump
exit
;;
5
```
```
1 $>cat exemple.avm
; -------------
; exemple.avm -
; -------------
push int32(42)
push int32(33)
add
push float(44.55)
mul
push double(42.42)
push int32(42)
dump
pop
assert double(42.42)
exit
$>./avm ./exemple.avm
42
42.42
3341.25
```
```
$>./avm
pop
;;
Error line 1: impossible instruction, the stack is empty
```

## The tester

The tester is a development tool used to verify the behavior of the Abstract VM.
It performs comprehensive checks, including:

- **Parsing and lexing** of instructions to ensure the input is correctly interpreted.
- **Individual command** execution, verifying each instruction works as expected.
- **Combined instruction** sequences, to test interactions between multiple operations.
- **Error handling**, confirming that both parsing and execution errors are detected and reported correctly.

You can build the tester with:
```
make test
```
This produces the `avm_tester` executable, which is primarily used during development to catch bugs and validate the implementation.

Then just launch it:
```
./avm_tester
```

## Links
- [Inheritance in C++](https://en.cppreference.com/book/intro/inheritance)
- [Factory Method in C++](https://medium.com/@antwang/factory-method-in-c-the-right-way-e8c5f015fe39)
- [std::stoi, std::stol, std::stoll](https://en.cppreference.com/w/cpp/string/basic_string/stol.html)
- [std::numeric_limits](https://en.cppreference.com/w/cpp/types/numeric_limits.html)
