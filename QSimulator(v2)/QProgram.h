#pragma once
#include "Matrix.h"
#include <stdexcept>
#include <Queue>
#include "Qsyntax_analyser.h"
#include "QClassic_simulator_handler.h"
#include <string>
#include <Eigen/Dense>
#include <fstream>


namespace Work_namespace {

	class QProgram {
	private:
		std::vector<bool> answer;
		std::queue<std::string> command_queue;
		long qubits_amount = 0;
	public:

		void Init_reg(const long amount = 0);

		void R_x(const double angle, const long qubit_num = 0);
		void R_y(const double angle, const long qubit_num = 0);
		void R_z(const double angle, const long qubit_num = 0);


		void R_x_conj(const double angle, const long qubit_num = 0);
		void R_y_conj(const double angle, const long qubit_num = 0);
		void R_z_conj(const double angle, const long qubit_num = 0);


		void P(const double angle, const long qubit_num);
		void P_conj(const double angle, const long qubit_num);
		void Phase(const double angle, const long qubit_num);
		void Phase_conj(const double angle, const long qubit_num);
		void Cnot(const long qubit_1, const long qubit_2);

		void H(const long qubit_num = 0);
		void X(const long qubit_num = 0);
		void Y(const long qubit_num = 0);
		void Z(const long qubit_num = 0);
		void T(const long qubit_num = 0);
		void T_conj(const long qubit_num = 0);
		void S(const long qubit_num = 0);
		void S_conj(const long qubit_num = 0);
		void SWAP(const long qubit_1, const long qubit_2);
		void Toffoli(const long qubit_1, const long qubit_2, const long qubit_3);
		void Multy_X_aux(std::initializer_list<long> controlling_qubits, long auxiliary_qubit_num, long target_qubit_num);
		void Arbit_transform(const Eigen::Ref<const Eigen::MatrixXcd>&  matr);
		void Multycontrol_rotation(std::initializer_list<long> controlling_qubits,
			std::string rotation_type,
			const double angle, const long qubit_num);
		void UMultycontrol_rotation(std::initializer_list<long> controlling_qubits,
			std::string rotation_type, std::initializer_list<double> angles, const long qubit_num);
		void Make_empty_command_queue();
		std::vector<bool> Get_answer() const;


		void Measure(long qubit_to_measure);
		void Measure_all();
		void Execute(); // call point for cirquit to be executed
	};

	std::vector<bool> QProgram::Get_answer() const {
		if (answer.empty()) throw std::exception("Get_answer::ERROR: No measurement has been executed \n\n");
		return answer;
	}

	void QProgram::Make_empty_command_queue() {
		while (!command_queue.empty()) {
			command_queue.pop();
		}
	}
	/// <summary>
	/// Makes initialization of considered register
	/// </summary>
	/// <param name="amount">Amount of qubits in register</param>
	void QProgram::Init_reg(const long amount) {
		if (amount == 0) throw std::invalid_argument("Init_reg::ERROR: Register should have at least 1 qubit \n\n");
		else if (amount < 0) {
			throw std::invalid_argument("Init_reg::ERROR: Negative indexing isn't allowed \n\n");
		}
		if (qubits_amount != 0) {
			Make_empty_command_queue();
			qubits_amount = 0;
			answer.clear();
		}
		qubits_amount = amount;
		std::stringstream parameters;
		parameters << "Init_reg(" << qubits_amount << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Multycontrol X(NOT) operation with an auxiliary qubit
	/// The auxiliary qubit can be dirty
	/// So if your reg has size n than you can make the transform with n-2 controlling qubits
	/// Of course, if you have ancilla in reg, using the transformation is still appropriate
	/// </summary>
	/// <param name="controlling_qubits">List of controlling qubits</param>
	/// <param name="auxiliary_qubit_num">Auxiliary qubit</param>
	/// <param name="target_qubit_num">Target qubit</param>
	void QProgram::Multy_X_aux(std::initializer_list<long> controlling_qubits, long auxiliary_qubit_num, long target_qubit_num) {
		if(controlling_qubits.size() > this->qubits_amount - 2) throw std::invalid_argument(
			"Multy_X_aux::ERROR: There  mustn't be more than n-2 controlling qubits\n\n");
		for (const auto item : controlling_qubits) {
			if (item <= 0 || item > this->qubits_amount) {
				throw std::invalid_argument("Multy_X_aux::ERROR: Controlling qubits out of range \n\n");
			}
			if (item == auxiliary_qubit_num) {
				throw std::invalid_argument("Multy_X_aux::ERROR: Same auxiliary and control qubit \n\n");
			}
			if (item == target_qubit_num) {
				throw std::invalid_argument("Multy_X_aux::ERROR: Same target and control qubit \n\n");
			}
		}
		if (auxiliary_qubit_num <= 0 || auxiliary_qubit_num > this->qubits_amount) {
			throw std::invalid_argument("Multy_X_aux::ERROR: auxiliary qubit out of range\n\n");
		}
		if (target_qubit_num <= 0 || target_qubit_num > this->qubits_amount) {
			throw std::invalid_argument("Multy_X_aux::ERROR: target qubit out of range\n\n");
		}
		if (auxiliary_qubit_num == target_qubit_num) {
			throw std::invalid_argument("Multy_X_aux::ERROR: Same auxiliary and target qubits\n\n");
		}
		if (controlling_qubits.size() < 2) {
			throw std::invalid_argument("Multy_X_aux::ERROR: Not enough controling qubits to execute operation\n\n");
		}
		std::stringstream parameters;
		parameters << "Multy_X_aux(";
		std::initializer_list<long>::const_iterator it;
		for (auto it = controlling_qubits.begin(); it != controlling_qubits.end(); ++it) {
			if (it == controlling_qubits.end() - 1) {
				parameters << (*it) << ';';
			}
			else {
				parameters << (*it) << ',';
			}
		}
		parameters << auxiliary_qubit_num << ';' << target_qubit_num << ')';
		command_queue.push(parameters.str());
	}
	void QProgram::R_x(const double angle, const long qubit_num) {
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
		parameters << "R_x(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_y(const double angle, const long qubit_num) {
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
		parameters << "R_y(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_z(const double angle, const long qubit_num) {
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
		parameters << "R_z(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}

	void QProgram::R_x_conj(const double angle, const long qubit_num) {
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
		parameters << "R_x_conj(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_y_conj(const double angle, const long qubit_num) {
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
		parameters << "R_y_conj(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::R_z_conj(const double angle, const long qubit_num) {
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
		parameters << "R_z_conj(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}

	void QProgram::Cnot(const long qubit_1, const long qubit_2) {
		if (qubit_1 <= 0 || qubit_1 > qubits_amount) {
			throw std::invalid_argument("Cnot::ERROR: Invalid controlling qubit number: " + std::to_string(qubit_1));
		}
		else if (qubit_2 <= 0 || qubit_2 > qubits_amount) {
			throw std::invalid_argument("Cnot::ERROR: Invalid target qubit number: " + std::to_string(qubit_2));
		}
		else if (qubit_1 == qubit_2) {
			throw std::invalid_argument("Cnot::ERROR: Same qubit to control and target: " + std::to_string(qubit_1) + " and " + std::to_string(qubit_2));
		}
		if (qubits_amount < 2) {
			throw std::exception("Cnot::ERROR: Can't be executed on current register. Not enough qubits to proceed\n\n");
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
		if (qubit_num <= 0) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num > qubits_amount) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
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

	void QProgram::S(const long qubit_num) {
		if (qubit_num <= -1) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_num));
		}
		else if (qubit_num != 0 && qubit_num > qubits_amount) {
			throw std::invalid_argument(" Invalid number of qubit to operate with: " + qubit_num);
		}
		else if (qubit_num == 0 && qubits_amount > 1) {
			throw std::invalid_argument("Register consists of more than 1 qubit");
		}
		std::stringstream parameters;
		parameters << "S(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::S_conj(const long qubit_num) {
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
		parameters << "S_conj(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}

	void QProgram::SWAP(const long qubit_1, const long qubit_2) {
		if (qubit_1 <= 0 || qubit_2 <= 0 || qubit_1 > qubits_amount || qubit_2 > qubits_amount) {
			throw std::invalid_argument("SWAP::ERROR: Qubits to swap are out of range\n\n");
		}
		else if (qubit_1 == qubit_2) {
			throw std::invalid_argument("SWAP::ERROR: Same qubit numbers: " + std::to_string(qubit_1) + " and " + std::to_string(qubit_2) + "\n\n");
		}
		else if (qubits_amount < 2) {
			throw std::invalid_argument("SWAP::ERROR: Not enough qubits to execute operation\n\n");
		}
		std::stringstream parameters;
		parameters << "SWAP(" << qubit_1 << "," << qubit_2 << ")";
		command_queue.push(parameters.str());
	}
	
	void QProgram::Toffoli(const long qubit_1, const long qubit_2, const long qubit_3) {
		if (qubit_1 <= 0) {
			throw std::invalid_argument("Toffoli::ERROR: Invalid first qubit number: " + std::to_string(qubit_1));
		}
		else if (qubit_2 <= 0) {
			throw std::invalid_argument("Toffoli::ERROR: Invalid second qubit number: " + std::to_string(qubit_2));
		}
		else if (qubit_3 <= 0) {
			throw std::invalid_argument("Toffoli::ERROR: Invalid third qubit number: " + std::to_string(qubit_3));
		}
		else if (qubit_1 == qubit_2 || qubit_1 == qubit_3 || qubit_2 == qubit_3) {
			throw std::invalid_argument("Toffoli::ERROR: Same qubit numbers");
		}
		else if (qubit_1 > qubits_amount || qubit_2 > qubits_amount ||  qubit_3 > qubits_amount) {
			throw std::invalid_argument("Toffoli::ERROR: Invalid number of qubit to operate with: " + std::to_string(qubits_amount));
		}
		else if (qubits_amount < 3) {
			throw std::invalid_argument("Toffoli::ERROR: Not enough qubits to execute operation");
		}
		std::stringstream parameters;
		parameters << "Toffoli(" << qubit_1 << "," << qubit_2 << "," << qubit_3 << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::P(const double angle, const long qubit_num) {
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
		parameters << "P(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::P_conj(const double angle, const long qubit_num) {
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
		parameters << "P_conj(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::Phase(const double angle, const long qubit_num) {
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
		parameters << "Phase(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	void QProgram::Phase_conj(const double angle, const long qubit_num) {
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
		parameters << "Phase_conj(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/*done without considering default 0 meaning for qubit number*/
	void QProgram::Multycontrol_rotation(std::initializer_list<long> controlling_qubits,
		std::string rotation_type,
		const double angle, const long qubit_num) {
		if (controlling_qubits.size() < 2) {
			throw std::invalid_argument("Multycontrol_rotation::ERROR: Not a Multycontrol_rotation operation. Check the number of controlling qubits\n\n");
		}
		for (const auto item : controlling_qubits) {
			if (item <= 0 || item > this->qubits_amount) {
				throw std::invalid_argument("Multycontrol_rotation::ERROR: Controlling qubits out of range \n");
			}
			if (item == qubit_num) {
				throw std::invalid_argument("Multycontrol_rotation::ERROR: Same qubit to control and execute rotation \n");
			}
		}
		if (qubit_num <= 0 || qubit_num > this->qubits_amount) {
			throw std::invalid_argument("Multycontrol_rotation::ERROR: Invalid target qubit: " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "Multycontrol_rotation(";
		std::initializer_list<long>::const_iterator it = controlling_qubits.end() - 1;
		for (auto it = controlling_qubits.begin(); it != controlling_qubits.end(); ++it) {
			if (it == controlling_qubits.end() - 1) {
				parameters << (*it) << ';';
			}
			else {
				parameters << (*it) << ',';
			}
		}
		parameters << rotation_type << ';' << angle << ';' << qubit_num << ')';
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// A uniformly multycontrol rotation about main axes X,Y and Z.
	/// Useful for testing decompositions. Makes rotation on target
	/// depending on the control qubit set meanings. Unlike the operation
	/// of a multicontrolled rotation with an auxiliary qubit, can be done 
	/// up to only 1 controlling qubit.
	/// </summary>
	/// <param name="controlling_qubits"> - A list of controlling qubits</param>
	/// <param name="rotation_type"> - A type of rotation: R_x, R_y or R_z</param>
	/// <param name="angles"> - A vector of angles for every controlling possible set</param>
	/// <param name="qubit_num"> - Target qubit</param>
	void QProgram::UMultycontrol_rotation(std::initializer_list<long> controlling_qubits,
		std::string rotation_type, std::initializer_list<double> angles, const long qubit_num) {
		if (controlling_qubits.size() == 0) {
			throw std::invalid_argument("UMultycontrol_rotation::ERROR: Check the controlling set. It's empty now!\n\n");
		}
		long controlling_states_num = 1 << controlling_qubits.size();
		if (controlling_states_num != angles.size()) {
			throw std::invalid_argument("UMultycontrol_rotation::ERROR: Check for the number of angles to execute operation with\n\n");
		}
		if (controlling_qubits.size() < 2) {
			throw std::invalid_argument("UMultycontrol_rotation::ERROR: Not a Multycontrol_rotation operation. Check the number of controlling qubits\n\n");
		}
		for (const auto item : controlling_qubits) {
			if (item <= 0 || item > this->qubits_amount) {
				throw std::invalid_argument("UMultycontrol_rotation::ERROR: Controlling qubits out of range \n");
			}
			if (item == qubit_num) {
				throw std::invalid_argument("UMultycontrol_rotation::ERROR: Same qubit to control and execute rotation \n");
			}
		}
		if (qubit_num <= 0 || qubit_num > this->qubits_amount) {
			throw std::invalid_argument("UMultycontrol_rotation::ERROR: Invalid target qubit: " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "UMultycontrol_rotation(";
		/*append controling qubits*/
		std::initializer_list<long>::const_iterator it = controlling_qubits.end() - 1;
		for (auto it = controlling_qubits.begin(); it != controlling_qubits.end(); ++it) {
			if (it == controlling_qubits.end() - 1) {
				parameters << (*it) << ';';
			}
			else {
				parameters << (*it) << ',';
			}
		}
		/*append rotation*/
		parameters << rotation_type << ';';
		/*append angles*/
		std::initializer_list<double>::const_iterator it1 = angles.end() - 1;
		for (auto it1 = angles.begin(); it1 != angles.end(); ++it1) {
			if (it1 == angles.end() - 1) {
				parameters << (*it1) << ';';
			}
			else {
				parameters << (*it1) << ',';
			}
		}
		parameters << qubit_num << ')';
		command_queue.push(parameters.str());
	}

	void QProgram::Arbit_transform(const Eigen::Ref<const Eigen::MatrixXcd>&  matr) {
		long space_size = 1 << qubits_amount;
		if (!matr.isUnitary()) {
			throw std::invalid_argument("Arbit_transform::ERROR: The matrix doesnt represent unitary evolution concept\n");
		}
		if (matr.cols() != space_size) {
			throw std::invalid_argument("Arbit_transform::ERROR: The matrix doesnt match the register size\n");
		}
		std::stringstream parameters;
		parameters << "Arbit_transform(" << space_size << ";";
		for (long i = 0; i < matr.rows(); ++i) {
			for (long j = 0; j < matr.rows(); ++j) {
				if (i != matr.rows() - 1 && j == matr.rows() - 1) {
					parameters << matr(i, j) << ";";
				}
				else if (i == matr.rows() - 1 && j == matr.rows() - 1) {
					parameters << matr(i, j) << ")\n";
				}
				else {
					parameters << matr(i, j) << ',';
				}
			}
		}
		command_queue.push(parameters.str());
	}
	void QProgram::Measure(const long qubit_to_measure) {
		if (qubits_amount == 0) {
			throw std::invalid_argument("Invalid operation: There is nothing to measure \n");
		}
		if (qubit_to_measure <= 0 || qubit_to_measure > qubits_amount) {
			throw std::invalid_argument("Invalid qubit number: " + std::to_string(qubit_to_measure));
		}
		std::stringstream parameters;
		parameters << "Measure(" << qubit_to_measure << ")";
		command_queue.push(parameters.str());
		this->qubits_amount--;
	}
	void QProgram::Measure_all() {
		if (qubits_amount == 0) {
			throw std::invalid_argument("Invalid operation: There is nothing to measure \n");
		}
		std::stringstream ss;
		ss << "Measure_all(" << ")";
		command_queue.push(ss.str());
		this->qubits_amount = 0;
	}
	void QProgram::Execute() {
		if (command_queue.empty()) {
			throw std::exception("Invalid operation: There is nothing to execute \n");
		}
		std::ifstream in;
		in.open("Debugging_test_file.txt");
		if (in.is_open()) {
			in.close();
			remove("Debugging_test_file.txt");
		}
		else {
			in.close();
		}
		if (Analyser(command_queue)) {
			in.open("Debugging_test_file.txt");
			QClassic_simulator_handler handler(in);
			answer = handler.Run();
			in.close();
		}
		else {
			std::cout << "An ERROR occured during syntax analisis. Check your QProgram for mistakes\n";
			printf("-------------------------------------------------------------------------\n\n");
		}
		in.close();
	}
}


namespace QLab {

	using ::Work_namespace::QProgram;

}