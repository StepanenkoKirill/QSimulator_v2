#include <iostream>
#include <string>
#include <fstream>
#include "QProgram.h"
#include "QAlgorithms.h"

int main()
{
    QLab::QProgram* program;
    program = new QLab::QProgram;
	std::string input_instruction;
	int grammar_state = 0;
	std::ifstream in;
	int num = 0;
	in.open("Debugging_test_file.txt");
	while (getline(in, input_instruction, ';')) {
		std::cout << input_instruction << std::endl;
		//std::cout << input_instruction[input_instruction.length() - 1] << std::endl;
		++num;
	}
	std::cout << num << std::endl;
	in.close();
	std::vector<int>* x = new std::vector<int>;
	x->push_back(5);
}
