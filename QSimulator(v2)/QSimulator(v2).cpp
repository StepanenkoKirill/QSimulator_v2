#include <iostream>
#include <string>
#include <fstream>
#include "QProgram.h"
#include "QAlgorithms.h"
#include "QSyntax_analyser.h"
#include <list>
//#include <Eigen/Dense>


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
	Eigen::MatrixXcd U(8,8), U2(4,4), L1(4,4), L0(4, 4), R0(4, 4), R1(4, 4), S(4, 4), C(4, 4);
	long target, new_size;
	target = 0;
	std::complex<double> x(1. / sqrt(2));
	U << x, 0, 0, 0, 0, 0, 0, x,
		0, x, 0, 0, 0, 0, x, 0,
		0, 0, x, 0, 0, x, 0, 0,
		0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 0, 0, 0,
		0, 0, x, 0, 0, -x, 0, 0,
		0, x, 0, 0, 0, 0, -x, 0,
		x, 0, 0, 0, 0, 0, 0, -x;

    QLab::QProgram* program;
    program = new QLab::QProgram;

	try {
		//program->Init_reg(5);
		//for (int i = 1; i <= 3; ++i) {
		//	program->X(i);
		//}
		//program->Multy_X_aux({ 1,2,3 }, 4, 5);
		//program->Measure(5);
		//program->Execute();
		//std::cout << "Answer is:" << program->Get_answer()[0];
		long size = 3;
		long space = 1 << size;
		Eigen::MatrixXcd oracle(Eigen::MatrixXcd::Identity(space, space));
		oracle(1, 1) = -1;
		long final_iterations = 3;
		std::cout << oracle;
		long answer = QLab::Grover_Search(oracle, size, final_iterations, 1);
		std::cout << "Answer is: " << answer;
	}
	catch (std::exception ex) {
		std::cout << ex.what();
	}
}
