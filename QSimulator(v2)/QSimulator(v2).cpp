#include <iostream>
#include <string>
#include <fstream>
#include "QProgram.h"
#include "QAlgorithms.h"
#include <list>
#include <Eigen/Dense>

namespace QLab {
	QProgram* q = new QProgram;
}

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

	Eigen::MatrixXcd U, L1, L0, R0, R1, S, C;
	long target, new_size;
	target = 0;
	U.resize(8, 8);
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
		program->Init_reg(3);
		program->X(1);
		program->X(2);
		program->Arbit_transform(U);
		//program->R_x(M_PI, 3);
		program->UMultycontrol_rotation({ 1,2 }, "R_x", { M_PI / 2, -M_PI / 2, M_PI / 2 , M_PI / 2 }, 3);
		program->Measure_all();
		program->Execute();
		for (auto item : program->Get_answer()) {
			std::cout << item << " ";
		}
	}
	catch (std::exception ex) {
		std::cout << ex.what();
	}
	
}
