#pragma once
#include "QExecutor_interface.h"
#include <random>
#include <functional>

namespace Work_namespace {

	class QSimulator : public QExecutor_interface {
		QRegister _reg;
	public:
		virtual void Init_reg(const long qubits_quantity) override;
		virtual void Measure(const long qubit_number = 0) override;
		virtual void R_x(const double angle, const long qubit_number = 0) override;
		virtual void R_y(const double angle, const long qubit_number = 0) override;
		virtual void R_z(const double angle, const long qubit_number = 0) override;
		virtual void Ph(const double angle, const long qubit_number = 0) override;
		virtual void Adjacent_SWAP(const long first_qubit, const long second_qubit) override;
		virtual void SWAP(const long first_qubit, const long second_qubit) override;
		virtual void Cnot(const long first_qubit, const long second_qubit) override;

		void Multycontrol_rotation(std::vector<long> controlling_qubits,
			std::function<void(const double, const long)> func, const double param_1, const long param_2);
	};

	/// <summary>
	/// Initialises register
	/// </summary>
	/// <param name="qubits_quantity">Register size</param>
	void QSimulator::Init_reg(const long qubits_quantity) {
		_reg.Initialize(qubits_quantity);
	}

	/// <summary>
	/// Measures register or exact qubit
	/// </summary>
	/// <param name="qubit_number">Default parameter is set to 0</param>
	void QSimulator::Measure(const long qubit_number) {
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
			if (counter % 2 == 1) {
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
		_reg.Project(projected_space_size);
		for (long i = 0; i < projected_space_size; ++i) {
			_reg[i] = temp[i];
		}
		/*for (int n = 0; n < 10000; ++n)
			++m[d(gen) + 1];

		for (auto p : m) {
			std::cout << p.first << " generated " << p.second << " times\n";
		}*/
	}

	/// <summary>
	/// Makes a rotation with angle about X ax on qubit_number qubit
	/// </summary>
	/// <param name="angle">An agle of rotation</param>
	/// <param name="qubit_number">Number of qubit to execute</param>
	void QSimulator::R_x(const double angle, const long qubit_number) {
		long number_of_qubits = _reg.get_size();
		if (!(0 < qubit_number <= number_of_qubits)) {
			throw std::exception("Wrong qubit to execute operation");
		}
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
						std::conj(i_sin) * _reg[(i + 1) * subspace_nm + j];
				}
				else {
					temp[i * subspace_nm + j] = std::conj(i_sin) * _reg[(i - 1) * subspace_nm + j] +
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
		if (!(0 < qubit_number <= number_of_qubits)) {
			throw std::exception("Wrong qubit to execute operation");
		}
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
		if (!(0 < qubit_number <= number_of_qubits)) {
			throw std::exception("Wrong qubit to execute operation");
		}
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
	/// Phase shift of quantum state
	/// </summary>
	/// <param name="first_qubit">Angle to shift</param>
	/// <param name="second_qubit">Qubit to make phase shift</param>
	void QSimulator::Ph(const double angle, const long qubit_number = 0) {
		long number_of_qubits = _reg.get_size();
		if (!(0 < qubit_number <= number_of_qubits)) {
			throw std::exception("Wrong qubit to execute operation");
		}
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
			}
		}
		for (long i = 0; i < space_size; ++i) {
			_reg[i] = temp[i];
		}
	}
	/// <summary>
	/// Swaps to adjacent qubits
	/// </summary>
	/// <param name="first_qubit">First qubit in its order</param>
	/// <param name="second_qubit">Second qubit in its order</param>
	void QSimulator::Adjacent_SWAP(const long first_qubit, const long second_qubit) {
		if (first_qubit == second_qubit) return;
		long number_of_qubits = _reg.get_size();
		if (number_of_qubits < 2) {
			throw std::exception("Not enough qubits to execute");
		}
		if (first_qubit < 0 || second_qubit < 0) {
			throw std::exception("Negative qubit indexing");
		}
		if (std::abs(first_qubit - second_qubit) != 1) {
			throw std::exception("Qubits aren't adjacent");
		}
		long space_size = 1 << number_of_qubits;
		long subspace_nm1 = 1 << (number_of_qubits - second_qubit);
		long times_m1 = 1 << (second_qubit);
		std::vector<std::complex<double>> temp(space_size, 0);
		long pos = 0;
		for (long i = 0; i < times_m1; ++i) {
			pos = i % 4;
			for (long j = 0; j < subspace_nm1; ++j) {
				if (pos == 0) {
					temp[i * subspace_nm1 + j] = _reg[i * subspace_nm1 + j];
				}
				else if (pos == 1) {
					temp[i * subspace_nm1 + j] = _reg[(i + 1) * subspace_nm1 + j];
				}
				else if (pos == 2) {
					temp[i * subspace_nm1 + j] = _reg[(i - 1) * subspace_nm1 + j];
				}
				else if (pos == 3) {
					temp[i * subspace_nm1 + j] = _reg[i * subspace_nm1 + j];
				}
				else throw std::exception("Unexpected 'if-else' result");
			}
		}
		for (long i = 0; i < space_size; ++i) {
			_reg[i] = temp[i];
		}
	};

	/// <summary>
	/// Swaps two selected qubits
	/// </summary>
	/// <param name="first_qubit">First qubit in its order</param>
	/// <param name="second_qubit">Second qubit in its order</param>
	void QSimulator::SWAP(const long first_qubit, const long second_qubit) {
		if (first_qubit == second_qubit) return;
		long number_of_qubits = _reg.get_size();
		if (number_of_qubits < 2) {
			throw std::exception("Not enough qubits to execute");
		}
		if (first_qubit > second_qubit) {
			throw std::exception("Wrong sequence of qubits");
		}
		if (first_qubit < 0 || second_qubit < 0) {
			throw std::exception("Negative qubit indexing");
		}
		long s = second_qubit, f = first_qubit;
		long difference = second_qubit - first_qubit;

		for (long i = 0; i < difference; ++i) {
			Adjacent_SWAP(f + i, f + 1 + i);
		}
		for (long i = 0; i < (difference - 1); ++i) {
			Adjacent_SWAP(s - 2 - i, s - 1 - i);
		}
	}

	/// <summary>
	/// Applyes CNOT gate to qubits in register
	/// </summary>
	/// <param name="first_qubit">Controlling qubit</param>
	/// <param name="second_qubit">Controlled qubit</param>
	void QSimulator::Cnot(const long first_qubit, const long second_qubit) {
		long number_of_qubits = _reg.get_size();
		if (number_of_qubits < 2) {
			throw std::exception("Not enough qubits to execute");
		}
		long fir = first_qubit, sec = second_qubit;
		if (first_qubit > second_qubit) {
			fir = second_qubit;
			sec = first_qubit;
			SWAP(fir, sec);
		}
		SWAP(fir + 1, sec);
		long s = fir + 1;
		long space_size = 1 << number_of_qubits;
		long subspace_nm1 = 1 << (number_of_qubits - s);
		long times_m1 = 1 << (s);
		std::vector<std::complex<double>> temp(space_size, 0);
		long pos = 0;
		for (long i = 0; i < times_m1; ++i) {
			pos = i % 4;
			for (long j = 0; j < subspace_nm1; ++j) {
				if (pos == 0) {
					temp[i * subspace_nm1 + j] = _reg[i * subspace_nm1 + j];
				}
				else if (pos == 1) {
					temp[i * subspace_nm1 + j] = _reg[i * subspace_nm1 + j];
				}
				else if (pos == 2) {
					temp[i * subspace_nm1 + j] = _reg[(i + 1) * subspace_nm1 + j];
				}
				else if (pos == 3) {
					temp[i * subspace_nm1 + j] = _reg[(i - 1) * subspace_nm1 + j];
				}
				else throw std::exception("Unexpected 'if-else' result");
			}
		}
		for (long i = 0; i < space_size; ++i) {
			_reg[i] = temp[i];
		}
		SWAP(fir + 1, sec);
		if (first_qubit > second_qubit) {
			SWAP(fir, sec);
		}
	}

	void QSimulator::Multycontrol_rotation(std::vector<long> controlling_qubits,
		std::function<void(const double, const long)> func, const double param_1, const long param_2) {
		for (const auto item : controlling_qubits) {
			if (item <= 0 || item > _reg.get_size()) {
				std::cout << "RUNTIME ERROR: Controlling qubits out of range \n";
			}
			if (item == param_2) {
				std::cout << "RUNTIME ERROR: Same qubit to control and execute rotation \n";
			}
		}
		long size = _reg.get_size();
		long space_size = 1 << size;
		long control_qubits_number = controlling_qubits.size();
		std::vector<std::complex<double>> temp(_reg.get_amps());
		func(param_1, param_2);

		std::vector<long> temp;
		std::unordered_map<long, std::vector<long>> contents_to_calculate_0_reg_index;
		for (auto item : controlling_qubits) {
			//counter for controling qubit to change group
			contents_to_calculate_0_reg_index[item].push_back(0);
			//number of indexes while in group
			contents_to_calculate_0_reg_index[item].push_back(1 << (size - item));
		}
		long i = 0, j = 0;
		bool flag = true;
		/*here we do checking of cond-n to ensure that all nedded qubits are 0*/
		while (i < space_size) {
			flag = true;
			for (auto& qubit : contents_to_calculate_0_reg_index) {
				j = i;
				if (qubit.second[0] % 2 == 0) {
					++j;
					if (j % qubit.second[1] == 0) {
						++qubit.second[0];
					}
				}
				else {
					++j;
					if (j % qubit.second[1] == 0) {
						++qubit.second[0];
					}
					flag = false;
					break;
				}
			}
			/*here we repair amplitudes that shouldn't have been transformed with func*/
			if (flag) {
				_reg[i] = temp[i];
			}
			++i;
		}
	}


}