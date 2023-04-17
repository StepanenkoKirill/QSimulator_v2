#pragma once
#include "Matrix.h"
#include <stdexcept>
#include <Queue>
#include "Qsyntax_analyser.h"

namespace Work_namespace {

	class QProgram {
	private:
		std::queue<std::string> command_queue;
		int qubits_amount = 0;
		//	std::vector < std::complex<double>> amplitudes; // в кв. вычислени€х мы не можем просто установить вектор
			// в интересующем нас состо€нии.  аждое состо€ние получаетс€ преобразовани€ми над регистром
	public:
		std::vector<bool> answer;
		void Init_reg(const int amount = 0);

		void R_x(const double theta, const int qubit_num = 0);
		void R_y(const double theta, const int qubit_num = 0);
		void R_z(const double theta, const int qubit_num = 0);
		void R_x_conj(const double theta, const int qubit_num = 0);
		void R_y_conj(const double theta, const int qubit_num = 0);
		void R_z_conj(const double theta, const int qubit_num = 0);
		void Cnot(const int qubit_1, const int qubit_2);

		void H(const int qubit_num = 0) {};
		void X(const int qubit_num = 0);
		void Y(const int qubit_num = 0);
		void Z(const int qubit_num = 0);
		void T(const int qubit_num = 0);
		void T_conj(const int qubit_num = 0);
		void SWAP(const int qubit_1, const int qubit_2);
		void Adjacent_SWAP(const int qubit_1, const int qubit_2);
		void CCnot(const int qubit_1, const int qubit_2, const int qubit_3);

		void Arbit_transform(const Matrix<std::complex<double>>& matr) {};
		void Make_empty_command_queue();


		void Measure(long qubit_to_measure = 0); // = 0 - значение по умол€чанию

		/*¬ случае отправки данных на сервер здесь может быть осуществлен метод execute, осуществл€ющий предачу данных
		и возвращающий некоторое вычисленное значение программы*/

		/*¬озможно этот метод стоит сделать и дл€ клиентского приложени€, потому что по сути нет разделени€
		с симул€тором. ≈го функционал представл€ют классы ркгистра и операций. ѕросто их можн вызывать
		из данного метода, подобно тому, как этот метод будет делать запросы на сервер по соответствующему
		шаблону, обраща€сь к нужным методам и отдава€ нужные параметры дл€ выполнени€ на сервере.

		¬ этом случае становитс€ €сно, почему грамматика идЄт именно с клиента. ≈е примитивный вид тут можно
		и организовать.

		ќстаЄтс€: 1. Ќекоторые непримитивные операторы
				  2. –азбор строки дл€ исполнени€ симул€тором
				  3. ќписание оператора измерени€
				  4. ’от€ бы один алгоритм.*/
	};

	void QProgram::Make_empty_command_queue() {
		while (!command_queue.empty()) {
			command_queue.pop();
		}
	}
	void QProgram::Init_reg(const int amount) {
		if (qubits_amount != 0) {
			Make_empty_command_queue();
			qubits_amount = 0;
		}
		qubits_amount = amount;
		std::stringstream parameters;
		parameters << "Init_reg(" << qubits_amount << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_x(const double theta, const int qubit_num) {
		if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit.");
		}
		std::stringstream parameters;
		parameters << ",R_x(" << theta << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_y(const double theta, const int qubit_num) {
		if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit.");
		}
		std::stringstream parameters;
		parameters << ",R_y(" << theta << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_z(const double theta, const int qubit_num) {
		if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid number of qubit to operate with: " + std::to_string(qubit_num));
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit.");
		}
		std::stringstream parameters;
		parameters << ",R_z(" << theta << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	// сделать качественно и дополнить операторами, в том числе измерени€ помен€ть тип long
	void QProgram::Measure(long qubit_to_measure) {
		return;
	}
}


namespace QLab {

	using ::Work_namespace::QProgram;

}

/*std::string x("1;H,1;"), tmp, command;
			std::stringstream in(x);
			std::map<std::string, int> commands;
			std::vector<bool> result;
			commands["H"] = 1;
			commands["X"] = 2;
			commands["Y"] = 3;
			commands["Z"] = 4;
			commands["Cnot"] = 5;
			commands["SWAP"] = 6;
			commands["adjacent_SWAP"] = 7;
			commands["R_x"] = 8;
			commands["R_y"] = 9;
			commands["R_z"] = 10;
			commands["T"] = 11;
			commands["Measure"] = 12;
			std::getline(in, tmp, ';');
			std::cout << tmp << std::endl;
			int register_size = atoi(tmp.c_str());
			BaseClass::QRegister reg(register_size);
			int command_code = 0;
			int counter = 0;
			long first = 0, second = 0;
			while (std::getline(in, tmp, ';')) {
				std::cout << tmp << std::endl;
				std::stringstream instruction(tmp);
				while (std::getline(instruction, command, ',')) {
					if (counter == 0) {
						command_code = commands[command];
						counter++;
					}
					else if (counter == 1) {
						first = atoi(command.c_str());
						counter++;
					}
					else if (counter == 2) {
						second = atoi(command.c_str());
					}
				}
				if (command_code == 0) {
					throw std::exception("unknown operator");
				}
				else {
					switch (command_code) {
					case 1:
						Operator::H(reg, first);
						break;
					case 2:
						Operator::X(reg, first);
						break;
					case 3:
						Operator::Y(reg, first);
						break;
					case 4:
						Operator::Z(reg, first);
						break;
					case 5:
						QBaseOperator::CNot(reg, first, second);
						break;
					case 6:
						QBaseOperator::adjacent_SWAP(reg, first, second);
						break;
					case 7:
						QBaseOperator::SWAP(reg, first, second);
						break;
					}
				}
			}
			return result;*/