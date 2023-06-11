#pragma once
#include "QExecutor_interface.h"
#include <random>
#include <functional>

namespace Work_namespace {

	class QSimulator : public QExecutor_interface {
		QRegister _reg;
	public:
		virtual void Init_reg(const long qubits_quantity) override;
		virtual bool Measure(const long qubit_number = 0) override;
		virtual std::vector<bool> Measure_all() override;
		virtual void R_x(const double angle, const long qubit_number = 0);
		virtual void R_y(const double angle, const long qubit_number = 0);
		virtual void R_z(const double angle, const long qubit_number = 0);
		virtual void P(const double angle, const long qubit_number = 0);
		virtual void Phase(const double angle, const long qubit_number = 0);
		virtual void Cnot(const long first_qubit, const long second_qubit);
		virtual void Debug_reg_content(std::ostream& out);
	};

	/// <summary>
	/// Initialises register
	/// </summary>
	/// <param name="qubits_quantity">Register size</param>
	void QSimulator::Init_reg(const long qubits_quantity) {
		_reg.Initialize(qubits_quantity);
	}

	/// <summary>
	/// Measures exact qubit
	/// </summary>
	/// <param name="qubit_number">Qubit to be measured</param>
	bool QSimulator::Measure(const long qubit_number) {
		/*It's supposed that we do the measurement of 1 qubit from 1 to _reg size
		  It's better to distinguish the measurements of all reg-r and the exact qubit
		  for more clear interface*/
		long number_of_qubits = _reg.get_size();
		long initial_space_size = 1 << number_of_qubits;
		long projected_space_size = 1 << (number_of_qubits - 1);
		std::vector<std::complex<double>> temp(projected_space_size, 0);
		/*  var rows_to_calc is involved in marking subspaces
			where we need to sum pobabilities to compare them
			for further projection */
		long rows_to_calc = 1 << (number_of_qubits - qubit_number);
		std::vector<double> probs(2, 0);
		long i = 0, counter = 0;
		/*	here we sum pobabilities in 2 parts as we measure 1 some qubit */

		while (i < initial_space_size) {
			if (counter % 2 == 0) {
				probs[0] += std::norm(_reg[i]);
				++i;
				if (i % rows_to_calc == 0) {
					++counter;
				}
			}
			else {
				probs[1] += std::norm(_reg[i]);
				++i;
				if (i % rows_to_calc == 0) {
					++counter;
				}
			}
		}
		//random numbers generator
		std::random_device device;
		std::mt19937 gen(device());
		std::discrete_distribution<> d({ probs[0], probs[1] });
		bool answer = d(gen);
		//making a projection
		double normalization_coeff = sqrt(probs[answer]);
		i = 0, counter = 0;
		long j = 0;
		/*	here we make temporary vec to store the result	*/
		while (i < initial_space_size) {
			if (counter % 2 == answer) {
				temp[j] = _reg[i] / normalization_coeff;
				++i; ++j;
				if (i % rows_to_calc == 0) {
					++counter;
				}
			}
			if (counter % 2 != answer) {
				++i;
				if (i % rows_to_calc == 0) {
					++counter;
				}
			}
		}
		/*	here we reduce the size of our reg and replace its content with the correct answer	*/
		_reg.Project(number_of_qubits-1);
		for (long i = 0; i < projected_space_size; ++i) {
			_reg[i] = temp[i];
		}
		/*for (int n = 0; n < 10000; ++n)
			++m[d(gen) + 1];

		for (auto p : m) {
			std::cout << p.first << " generated " << p.second << " times\n";
		}*/
		return answer;
	}
	std::vector<bool> QSimulator::Measure_all() {
		std::vector<bool> answer;
		long number_of_qubits_in_reg = _reg.get_size();
		for (long i = number_of_qubits_in_reg; i >= 1; --i) {
			answer.push_back(Measure(i));
		}
		std::reverse(answer.begin(), answer.end());
		return answer;
	}
	/// <summary>
	/// Makes a rotation with angle about X ax on qubit_number qubit
	/// </summary>
	/// <param name="angle">An agle of rotation</param>
	/// <param name="qubit_number">Number of qubit to execute</param>
	void QSimulator::R_x(const double angle, const long qubit_number) {
		long number_of_qubits = _reg.get_size();
		//if (!(0 < qubit_number <= number_of_qubits)) {
		//	throw std::exception("Wrong qubit to execute operation");
		//}
		std::complex<double> cosin(cos(angle / 2), 0);
		std::complex<double> i_sin(0, -sin(angle / 2));
		long space_size = 1 << number_of_qubits;
		long subspace_nm = 1 << (number_of_qubits - qubit_number);
		long times_m = 1 << (qubit_number);
		std::vector<std::complex<double>> temp(space_size, 0);
		long pos = 0;
		for (long i = 0; i < times_m; ++i) {
			for (long j = 0; j < subspace_nm; ++j) {
				if (i % 2 == 0) {
					temp[i * subspace_nm + j] = cosin * _reg[i * subspace_nm + j] +
						(i_sin) * _reg[(i + 1) * subspace_nm + j];
				}
				else {
					temp[i * subspace_nm + j] = (i_sin) * _reg[(i - 1) * subspace_nm + j] +
						cosin * _reg[i * subspace_nm + j];
				}

			}
		}
		for (long i = 0; i < space_size; ++i) {
			_reg[i] = temp[i];
		}
	}

	/// <summary>
	/// Makes a rotation with angle about Y ax on qubit_number qubit
	/// </summary>
	/// <param name="angle">An agle of rotation</param>
	/// <param name="qubit_number">Number of qubit to execute</param>
	void QSimulator::R_y(const double angle, const long qubit_number) {
		long number_of_qubits = _reg.get_size();
		std::complex<double> cosin(cos(angle / 2), 0);
		std::complex<double> _sin(sin(angle / 2), 0);
		long space_size = 1 << number_of_qubits;
		long subspace_nm = 1 << (number_of_qubits - qubit_number);
		long times_m = 1 << (qubit_number);
		std::vector<std::complex<double>> temp(space_size, 0);
		long pos = 0;
		for (long i = 0; i < times_m; ++i) {
			for (long j = 0; j < subspace_nm; ++j) {
				if (i % 2 == 0) {
					temp[i * subspace_nm + j] = cosin * _reg[i * subspace_nm + j] -
						_sin * _reg[(i + 1) * subspace_nm + j];
				}
				else {
					temp[i * subspace_nm + j] = _sin * _reg[(i - 1) * subspace_nm + j] +
						cosin * _reg[i * subspace_nm + j];
				}

			}
		}
		for (long i = 0; i < space_size; ++i) {
			_reg[i] = temp[i];
		}
	};

	/// <summary>
/// Makes a rotation with angle about Y ax on qubit_number qubit
/// </summary>
/// <param name="angle">An agle of rotation</param>
/// <param name="qubit_number">Number of qubit to execute</param>
	void QSimulator::R_z(const double angle, const long qubit_number) {
		long number_of_qubits = _reg.get_size();
		std::complex<double> a_1 = std::polar(1.0, -angle / 2);
		std::complex<double> a_4 = std::polar(1.0, angle / 2);
		long space_size = 1 << number_of_qubits;
		long subspace_nm = 1 << (number_of_qubits - qubit_number);
		long times_m = 1 << (qubit_number);
		std::vector<std::complex<double>> temp(space_size, 0);
		long pos = 0;
		for (long i = 0; i < times_m; ++i) {
			for (long j = 0; j < subspace_nm; ++j) {
				if (i % 2 == 0) {
					temp[i * subspace_nm + j] = a_1 * _reg[i * subspace_nm + j];
				}
				else {
					temp[i * subspace_nm + j] = a_4 * _reg[i * subspace_nm + j];
				}
			}
		}
		for (long i = 0; i < space_size; ++i) {
			_reg[i] = temp[i];
		}
	};
	/// <summary>
	/// Phase shift about Z ax of quantum state
	/// </summary>
	/// <param name="first_qubit">Angle to shift</param>
	/// <param name="second_qubit">Qubit to make phase shift</param>
	void QSimulator::P(const double angle, const long qubit_number) {
		long number_of_qubits = _reg.get_size();
		std::complex<double> a_4 = std::polar(1.0, angle);
		long space_size = 1 << number_of_qubits;
		long subspace_nm = 1 << (number_of_qubits - qubit_number);
		long times_m = 1 << (qubit_number);
		std::vector<std::complex<double>> temp(space_size, 0);
		long pos = 0;
		for (long i = 0; i < times_m; ++i) {
			for (long j = 0; j < subspace_nm; ++j) {
				if (i % 2 != 0) {
					temp[i * subspace_nm + j] = a_4 * _reg[i * subspace_nm + j];
				}
				else {
					temp[i * subspace_nm + j] = _reg[i * subspace_nm + j];
				}
			}
		}
		for (long i = 0; i < space_size; ++i) {
			_reg[i] = temp[i];
		}

	}
	/// <summary>
	/// Generic phase shift of quantum state
	/// </summary>
	/// <param name="first_qubit">Angle to shift</param>
	/// <param name="second_qubit">Qubit to make generic phase shift</param>
	void QSimulator::Phase(const double angle, const long qubit_number) {
		long number_of_qubits = _reg.get_size();
		std::complex<double> a_4 = std::polar(1.0, angle);
		long space_size = 1 << number_of_qubits;
		long subspace_nm = 1 << (number_of_qubits - qubit_number);
		long times_m = 1 << (qubit_number);
		std::vector<std::complex<double>> temp(space_size, 0);
		long pos = 0;
		for (long i = 0; i < times_m; ++i) {
			for (long j = 0; j < subspace_nm; ++j) {
				if (i % 2 == 0) {
					temp[i * subspace_nm + j] = a_4 * _reg[i * subspace_nm + j];
				}
				else {
					temp[i * subspace_nm + j] = a_4 * _reg[i * subspace_nm + j];
				}
			}
		}
		for (long i = 0; i < space_size; ++i) {
			_reg[i] = temp[i];
		}

	}
	

	void QSimulator::Cnot(const long control_qubit, const long target_qubit) {
		long number_of_qubits = _reg.get_size();
		long space_size = 1 << number_of_qubits;
		long  rows_to_calc = 1 << (number_of_qubits - control_qubit);
		long  rows_to_calc2 = 1 << (number_of_qubits - target_qubit);
		long times_m1 = 1 << (control_qubit);
		long i = 0, j = 0, counter = 0, counter2 = 0;
		bool flag = false;
		/*	here we choose indexes needed to be swapped for controll 1(ones) in target*/
		while (i < space_size) {
			if (counter % 2 == 0) {
				++i;
				if (i % rows_to_calc == 0) {
					++counter;
				}
			}
			if (counter % 2 == 1) {
				j = i;
				if (counter2 % 2 == 0) {
					std::swap(_reg[i], _reg[i ^ (rows_to_calc2)]); // ... and make swap
					++j;
					if (j % rows_to_calc2 == 0) {
						++counter2;
					}
				}
				else {
					++j;
					if (j % rows_to_calc2 == 0) {
						++counter2;
					}
				}
				++i;
				if (i % rows_to_calc == 0) {
					++counter;
				}
			}
		}
	}
	void QSimulator::Debug_reg_content(std::ostream& out) {
		if (_reg.get_size() == 0) {
			throw std::runtime_error("Register::ERROR: Empty register\n\n");
		}
		else {
			long space_size = 1 << _reg.get_size();
			out << "|reg> = ";
			for (long i = 0; i < space_size; ++i) {
				if (i == space_size - 1) {
					out << _reg[i] << "|" << i << ">\n";
				}
				else {
					out << _reg[i] << "|" << i << "> + ";
				}
				
			}
		}

	}
}