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
		int qubits_amount = 0;
	public:
		
		void Init_reg(const long amount = 0);

		void R_x(const double theta, const long qubit_num = 0);
		void R_y(const double theta, const long qubit_num = 0);
		void R_z(const double theta, const long qubit_num = 0);
		void R_x_conj(const double theta, const long qubit_num = 0);
		void R_y_conj(const double theta, const long qubit_num = 0);
		void R_z_conj(const double theta, const long qubit_num = 0);
		void Cnot(const long qubit_1, const long qubit_2);

		void H(const long qubit_num = 0) {};
		void X(const long qubit_num = 0);
		void Y(const long qubit_num = 0);
		void Z(const long qubit_num = 0);
		void T(const long qubit_num = 0);
		void T_conj(const long qubit_num = 0);
		void SWAP(const long qubit_1, const long qubit_2);
		void Adjacent_SWAP(const long qubit_1, const long qubit_2);
		void CCnot(const long qubit_1, const long qubit_2, const long qubit_3);

		void Arbit_transform(const Matrix<std::complex<double>>& matr) {};
		void Make_empty_command_queue();
		std::vector<bool> Get_answer() const;


		void Measure(long qubit_to_measure = 0); // = 0 - значение по умолячанию
	};

	std::vector<bool> QProgram::Get_answer() const{
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
		if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit.");
		}
		std::stringstream parameters;
		parameters << "R_x(" << theta << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_y(const double theta, const long qubit_num) {
		if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit.");
		}
		std::stringstream parameters;
		parameters << "R_y(" << theta << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_z(const double theta, const long qubit_num) {
		if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit.");
		}
		std::stringstream parameters;
		parameters << "R_z(" << theta << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	// сделать качественно и дополнить операторами, в том числе измерения поменять тип long
	void QProgram::Measure(const long qubit_to_measure) {
		if (qubit_to_measure != 0 && qubit_to_measure > qubits_amount) {
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