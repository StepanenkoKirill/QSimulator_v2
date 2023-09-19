#pragma once
#include "QProgram.h"
#include "macro.h"
#include <math.h>
//#include <Eigen/Dense>

namespace Work_namespace {

	namespace Algorithms {
		long Grover_Search(const Eigen::Ref<const Eigen::MatrixXcd>& Oracle, long size,
			long& final_iterations_col, long roots) {
			long initial_space_size = 1 << size;
			long space_size = Oracle.cols();
			double complexity = sqrt(space_size / roots);
			std::vector<bool> result;
			long index = 0;
			long iterations = ceil(M_PI * complexity / 4);
			if (initial_space_size != space_size) {
				throw std::exception("Area of function definitions doesn't match Oracle's representation");
			}
			QProgram prog;
			//making R transformation
			Eigen::MatrixXcd R_transform = Eigen::MatrixXcd::Identity(space_size, space_size)*(-1);
			R_transform(0, 0) = 1;
			long counter = 0;
			do {
				if (final_iterations_col > 10 * iterations) { // in case of cycling when there's no marked amplitudes
					index = -1;
					break;
				}
				prog.Init_reg(size);
				//Walsh-Hadamard
				for (long i = 1; i <= size; ++i) {
					prog.H(i);
				}
				for (long i = 0; i < iterations; ++i) {
					//Oracle (marks needed state)
					prog.Arbit_transform(Oracle);
					//Diffuser
					for (long j = 1; j <= size; ++j) {
						prog.H(j);
					}
					prog.Arbit_transform(R_transform);
					for (long k = 1; k <= size; ++k) {
						prog.H(k);
					}
				}
				counter++; final_iterations_col = counter * iterations;
				prog.Measure_all();
				prog.Execute();
				result = prog.Get_answer();
				index = result[0];
				for (int i = 1; i < result.size(); ++i) {
					index = index * 2 + result[i];
				}
			} while (Oracle(index, index) != -1.);
			return index;
		}
	}
	
}

namespace QLab {

	using namespace ::Work_namespace::Algorithms;

}