#include <iostream>
#include <string>
#include <fstream>
#include "QProgram.h"
#include "QAlgorithms.h"
#include <list>


void f(std::vector<long>& _c_qub_list, long _aux, long _trgt, std::ostream& out) {
	if (_c_qub_list.size() == 2) {
		for (auto item : _c_qub_list) {
			out << item << " ";
		}
		out << '\n';
	}
	else if (_c_qub_list.size() == 1) {
		out << 2 << '\n';
	}
	else {
		_c_qub_list.pop_back();
		f(_c_qub_list, _trgt, _aux, out);
	}
}
int main()
{
    QLab::QProgram* program;
    program = new QLab::QProgram;
	program->Init_reg(2);
	program->H(1);
	program->Measure_all();
	program->Execute();
	std::cout << program->Get_answer()[0];
}
