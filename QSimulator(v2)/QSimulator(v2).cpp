#include <iostream>
#include <string>
#include <fstream>
#include "QProgram.h"
#include "QAlgorithms.h"
#include "QSyntax_analyser.h"
#include <list>
#include <complex>
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
long Durr_Hoyer_help_for_min_search(long current, long threshold, std::vector<long> &T) {
	return (T[current] < T[threshold]) ? -1 : 1;
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
		
		//std::cout << "Grover search of the function argument." << std::endl;
		//long size = 4;
		//long space = 1 << size;
		//std::cout << "The size of the search task is: " << space << std::endl;
		//Eigen::MatrixXcd oracle(Eigen::MatrixXcd::Identity(space, space));
		//oracle(9, 9) = -1;
		//std::cout << "The marking oracle is:" << std::endl << oracle << std::endl;
		//long final_iterations = 1;
		//std::cout << std::endl;
		//std::cout << "Calculation start:" << std::endl;
		//std::cout << "--------------------------------------------------------------" << std::endl;
		//long answer = QLab::Grover_Search(oracle, size, final_iterations, 1);
		//std::cout << "Calculation end." << std::endl;
		//std::cout << "--------------------------------------------------------------" << std::endl;
		//std::cout << std::endl;
		//std::cout << "Answer is: " << answer;

		//std::cout << "Durr-Hoyer search of the minimum element in the list." << std::endl;
		//std::vector<long> v{ 1, 44, 15, 2, -1, -44, 125, 112, 33, -2, 100, 72, -32, 22, 10, 722,
		//					 13, 4, 5, 7, -29, 444, 525, -1112, 303, 11, 0, 78, -302, 232, 911, 92 };
		//std::cout << "The list is: " << std::endl;
		//for (int i = 0; i < v.size(); ++i) {
		//	std::cout << v[i] << " ";
		//}
		//std::cout << std::endl;
		////double time1 = std::clock();
		////std:: cout << QLab::Durr_Hoyer_based_search<long, long, long, std::vector<long>&>(Durr_Hoyer_help_for_min_search, v);
		////std::cout << std::endl << "time: " << std::clock() - time1;
		//std::cout << std::endl;
		//std::cout << "Calculation start:" << std::endl;
		//std::cout << "--------------------------------------------------------------" << std::endl;
		//std::vector<int> indexes(32, 0);
		//long an = 0;
		//for (int i = 0; i < 5; ++i) {
		//	an = QLab::Durr_Hoyer_based_search<long, long, long, std::vector<long>&>(Durr_Hoyer_help_for_min_search, v);
		//	indexes[an]++;
		//}
		//std::cout << std::endl;
		//std::cout << "Calculation end." << std::endl;
		//std::cout << "--------------------------------------------------------------" << std::endl << std::endl;
		//std::cout << "The answer is: [ ";
		//for (int i = 0; i < 32; ++i) {
		//	std::cout << indexes[i] << " ";
		//}
		//std::cout << "]";

		//std::cout << "One qubit hashing" << std::endl;
		//long preimage = 10;
		//std::cout << "The preimage is: " << preimage << std::endl;
		//QLab::QProgram qprog = QLab::One_qubit_hash_generator(preimage);
		//qprog.Measure_all();
		//std::cout << "Calculation start:" << std::endl;
		//std::cout << "--------------------------------------------------------------" << std::endl;
		//qprog.Execute();
		//long answer = qprog.Get_answer()[0];
		//std::cout << "Calculation end." << std::endl;
		//std::cout << "--------------------------------------------------------------" << std::endl;
		//std::cout << std::endl;
		//std::cout << "Answer is: " << answer;

		std::cout << "Cyclic group hashing" << std::endl;
		long preimage = 10;
		long size = 3;
		long space_size = (1 << size);
		std::cout << "The preimage is: " << preimage << std::endl;
		Eigen::MatrixXcd Unit(space_size, space_size);
		std::complex<double> w = std::polar(1.0, 0.);
		double angle = 2 * M_PI / space_size;
		double ro = 1.0 / 4;
		for (long i = 0; i < 8; ++i) {
			for (long j = 0; j < 8; ++j) {
				if (i != 0) {
					Unit(i, j) = std::polar(ro, angle * ((10 * j)%space_size));
				}
				else {
					Unit(i, j) = std::polar(ro, 0.);
				}
			}
		}
		QLab::QProgram qprog = QLab::Cyclic_group_hash_generator(U, size);
		qprog.Measure_all();
		std::cout << "Calculation start:" << std::endl;
		std::cout << "--------------------------------------------------------------" << std::endl;
		qprog.Execute();
		std::cout << "Calculation end." << std::endl;
		std::cout << "--------------------------------------------------------------" << std::endl;

	}
	catch (std::exception ex) {
		std::cout << ex.what();
	}
}
