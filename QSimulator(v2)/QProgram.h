#pragma once
#include "Matrix.h"
#include <stdexcept>
#include <Queue>
#include "Qsyntax_analyser.h"
#include "QClassic_simulator_handler.h"
#include <string>
#include <fstream>


namespace Work_namespace {

	class QProgram {
	private:
		std::vector<bool> answer;
		std::queue<std::string> command_queue;
		long qubits_amount = 0;
	public:

		void Init_reg(const long amount = 0);

		void R_x(const double theta, const long qubit_num = 0);
		void R_y(const double theta, const long qubit_num = 0);
		void R_z(const double theta, const long qubit_num = 0);
		void R_x_conj(const double theta, const long qubit_num = 0);
		void R_y_conj(const double theta, const long qubit_num = 0);
		void R_z_conj(const double theta, const long qubit_num = 0);
		void Cnot(const long qubit_1, const long qubit_2);

		void H(const long qubit_num = 0);
		void X(const long qubit_num = 0);
		void Y(const long qubit_num = 0);
		void Z(const long qubit_num = 0);
		void T(const long qubit_num = 0);
		void T_conj(const long qubit_num = 0);
		void SWAP(const long qubit_1, const long qubit_2);
		void Adjacent_SWAP(const long qubit_1, const long qubit_2);
		void CCnot(const long qubit_1, const long qubit_2, const long qubit_3);

		void Arbit_transform(const Matrix<std::complex<double>>& matr) {};
		void Multycontrol_rotation(std::initializer_list<long> controlling_qubits,
			std::string rotation_type,
			const double theta, const long qubit_num);
		void Make_empty_command_queue();
		std::vector<bool> Get_answer() const;


		void Measure(long qubit_to_measure = 0); // = 0 - значение по умолчанию
	};

	std::vector<bool> QProgram::Get_answer() const {
		return answer;
	}

	void QProgram::Make_empty_command_queue() {
		while (!command_queue.empty()) {
			command_queue.pop();
		}
	}
	void QProgram::Init_reg(const long amount) {
		if (qubits_amount != 0) {
			Make_empty_command_queue();
			qubits_amount = 0;
		}
		qubits_amount = amount;
		std::stringstream parameters;
		parameters << "Init_reg(" << qubits_amount << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_x(const double theta, const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "R_x(" << theta << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_y(const double theta, const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "R_y(" << theta << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_z(const double theta, const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "R_z(" << theta << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_x_conj(const double theta, const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "R_x_conj(" << theta << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_y_conj(const double theta, const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "R_y_conj(" << theta << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_z_conj(const double theta, const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "R_z_conj(" << theta << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::Cnot(const long qubit_1, const long qubit_2) {
		if (qubit_1 <= -1) {
			throw std::invalid_argument("Invalid first qubit number: " + std::to_string(qubit_1));
		}
		else if (qubit_2 <= -1) {
			throw std::invalid_argument("Invalid second qubit number: " + std::to_string(qubit_2));
		}
		else if (qubit_1 == qubit_2) {
			throw std::invalid_argument("Same qubit numbers: " + std::to_string(qubit_1) + " and " + std::to_string(qubit_2));
		}
		else if (qubit_1 != 0 && qubit_1 > qubits_amount || qubit_2 != 0 && qubit_2 > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubits_amount));
		}
		else if (qubit_1 == 0 || qubit_2 == 0) {
			throw std::invalid_argument("Qubit number zero does not exist");
		}
		std::stringstream parameters;
		parameters << "Cnot(" << qubit_1 << "," << qubit_2 << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::H(const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "H(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::X(const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "X(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::Y(const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "Y(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::Z(const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "Z(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::T(const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "T(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::T_conj(const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "T_conj(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::SWAP(const long qubit_1, const long qubit_2) {
		if (qubit_1 <= -1) {
			throw std::invalid_argument("Invalid first qubit number: " + std::to_string(qubit_1));
		}
		else if (qubit_2 <= -1) {
			throw std::invalid_argument("Invalid second qubit number: " + std::to_string(qubit_2));
		}
		else if (qubit_1 == qubit_2) {
			throw std::invalid_argument("Same qubit numbers: " + std::to_string(qubit_1) + " and " + std::to_string(qubit_2));
		}
		else if (qubit_1 != 0 && qubit_1 > qubits_amount || qubit_2 != 0 && qubit_2 > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubits_amount));
		}
		else if (qubit_1 == 0 || qubit_2 == 0) {
			throw std::invalid_argument("Qubit number zero does not exist");
		}
		std::stringstream parameters;
		parameters << "SWAP(" << qubit_1 << "," << qubit_2 << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::Adjacent_SWAP(const long qubit_1, const long qubit_2) {
		if (qubit_1 <= -1) {
			throw std::invalid_argument("Invalid first qubit number: " + std::to_string(qubit_1));
		}
		else if (qubit_2 <= -1) {
			throw std::invalid_argument("Invalid second qubit number: " + std::to_string(qubit_2));
		}
		else if (qubit_1 == qubit_2) {
			throw std::invalid_argument("Same qubit numbers: " + std::to_string(qubit_1) + " and " + std::to_string(qubit_2));
		}
		else if (qubit_1 != 0 && qubit_1 > qubits_amount || qubit_2 != 0 && qubit_2 > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubits_amount));
		}
		else if (qubit_1 == 0 || qubit_2 == 0) {
			throw std::invalid_argument("Qubit number zero does not exist");
		}
		std::stringstream parameters;
		parameters << "Adjacent_SWAP(" << qubit_1 << "," << qubit_2 << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::CCnot(const long qubit_1, const long qubit_2, const long qubit_3) {
		if (qubit_1 <= -1) {
			throw std::invalid_argument("Invalid first qubit number: " + std::to_string(qubit_1));
		}
		else if (qubit_2 <= -1) {
			throw std::invalid_argument("Invalid second qubit number: " + std::to_string(qubit_2));
		}
		else if (qubit_3 <= -1) {
			throw std::invalid_argument("Invalid second qubit number: " + std::to_string(qubit_3));
		}
		else if (qubit_1 == qubit_2 || qubit_1 == qubit_3 || qubit_2 == qubit_3) {
			throw std::invalid_argument("Same qubit numbers");
		}
		else if (qubit_1 != 0 && qubit_1 > qubits_amount || qubit_2 != 0 && qubit_2 > qubits_amount || qubit_3 != 0 && qubit_3 > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubits_amount));
		}
		else if (qubit_1 == 0 || qubit_2 == 0 || qubit_3 == 0) {
			throw std::invalid_argument("Qubit number zero does not exist");
		}
		std::stringstream parameters;
		parameters << "CCnot(" << qubit_1 << "," << qubit_2 << "," << qubit_3 << ")";
		command_queue.push(parameters.str());
	}
	/*done without considering default 0 meaning for qubit number*/
	void QProgram::Multycontrol_rotation(std::initializer_list<long> controlling_qubits,
		std::string rotation_type,
		const double theta, const long qubit_num) {
		for (const auto item : controlling_qubits) {
			if (item <= 0 || item > this->qubits_amount) {
				throw std::invalid_argument("Controlling qubits out of range \n");
			}
			if (item == qubit_num) {
				throw std::invalid_argument("Same qubit to control and execute rotation \n");
			}
		}
		if (qubit_num <= 0 || qubit_num > this->qubits_amount) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "Multycontrol_rotation(";
		std::initializer_list<long>::const_iterator it, it = controlling_qubits.end() - 1;
		for (auto it = controlling_qubits.begin(); it != controlling_qubits.end(); ++it) {
			if (it == controlling_qubits.end() - 1) {
				parameters << (*it) << ';';
			}
			else {
				parameters << (*it) << ',';
			}
		}
		parameters << rotation_type << ';' << theta << ';' << qubit_num << ')';
		command_queue.push(parameters.str());
	}

	// сделать качественно и дополнить операторами, в том числе измерения поменять тип long
	void QProgram::Measure(const long qubit_to_measure) {
		if (qubit_to_measure <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_to_measure));
		}
		else if (qubit_to_measure != 0 && qubit_to_measure > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_to_measure));
		}
		std::stringstream parameters;
		parameters << "Measure(" << qubit_to_measure << ")";
		command_queue.push(parameters.str());
		if (Analyser(command_queue)) {
			std::ifstream in;
			in.open("Debugging_test_file.txt");
			QClassic_simulator_handler handler(in);
			answer = handler.Run();
			in.close();
			remove("Debugging_test_file.txt");
		}
		else {
			std::cout << "An ERROR occured during syntax analisis. Check your QProgram for mistakes" << "\n";
		}
		return;
	}
}


namespace QLab {

	using ::Work_namespace::QProgram;

}