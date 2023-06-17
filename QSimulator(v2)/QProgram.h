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

		void Init_reg(const long amount);

		void R_x(const double angle, const long qubit_num);
		void R_y(const double angle, const long qubit_num);
		void R_z(const double angle, const long qubit_num);


		void R_x_conj(const double angle, const long qubit_num);
		void R_y_conj(const double angle, const long qubit_num);
		void R_z_conj(const double angle, const long qubit_num);


		void P(const double angle, const long qubit_num);
		void P_conj(const double angle, const long qubit_num);
		void Phase(const double angle, const long qubit_num);
		void Phase_conj(const double angle, const long qubit_num);
		void Cnot(const long qubit_1, const long qubit_2);

		void H(const long qubit_num);
		void X(const long qubit_num);
		void Y(const long qubit_num);
		void Z(const long qubit_num);
		void T(const long qubit_num);
		void T_conj(const long qubit_num);
		void S(const long qubit_num);
		void S_conj(const long qubit_num);
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

	/// <summary>
	/// Method to get answer from simulator
	/// </summary>
	/// <returns> - Bool vector of stationary state</returns>
	std::vector<bool> QProgram::Get_answer() const {
		if (answer.empty()) throw std::exception("Get_answer::ERROR: No measurement has been executed \n\n");
		return answer;
	}

	/// <summary>
	/// Makes empty the queue of commands that you do while writing your program
	/// </summary>
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
	/// <param name="controlling_qubits"> - List of controlling qubits</param>
	/// <param name="auxiliary_qubit_num"> - Auxiliary qubit</param>
	/// <param name="target_qubit_num"> - Target qubit</param>
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
	/// <summary>
	/// Rotation operator about the X axis
	/// </summary>
	/// <param name="angle"> - the double type angle to rotate</param>
	/// <param name="qubit_num"> - number of qubit to be executed on</param>
	void QProgram::R_x(const double angle, const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("R_x::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "R_x(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Rotation operator about the Y axis
	/// </summary>
	/// <param name="angle"> - the double type angle to rotate</param>
	/// <param name="qubit_num"> - number of qubit to be executed on</param>
	void QProgram::R_y(const double angle, const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("R_y::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "R_y(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Rotation operator about the Z axis
	/// </summary>
	/// <param name="angle"> - the double type angle to rotate</param>
	/// <param name="qubit_num"> - number of qubit to be executed on</param>
	void QProgram::R_z(const double angle, const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("R_z::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "R_z(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Rotation adjoint operator about the X axis
	/// </summary>
	/// <param name="angle"> - the double type angle to rotate</param>
	/// <param name="qubit_num"> - number of qubit to be executed on</param>
	void QProgram::R_x_conj(const double angle, const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("R_x_conj::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "R_x_conj(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Rotation adjoint operator about the Y axis
	/// </summary>
	/// <param name="angle"> - the double type angle to rotate</param>
	/// <param name="qubit_num"> - number of qubit to be executed on</param>
	void QProgram::R_y_conj(const double angle, const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("R_y_conj::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "R_y_conj(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Rotation adjoint operator about the Z axis
	/// </summary>
	/// <param name="angle"> - the double type angle to rotate</param>
	/// <param name="qubit_num"> - number of qubit to be executed on</param>
	void QProgram::R_z_conj(const double angle, const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("R_z_conj::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "R_z_conj(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Operates Cnot on qubits in register. Qubits aren't necessary to be adjacent
	/// </summary>
	/// <param name="qubit_1"> - controlling qubit</param>
	/// <param name="qubit_2"> - target qubit</param>
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
	/// <summary>
	/// An Hadamard gate
	/// </summary>
	/// <param name="qubit_num"> - qubit to be executed on</param>
	void QProgram::H(const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("H::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "H(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Quantum 1 qubit Not equivalent
	/// </summary>
	/// <param name="qubit_num"> - qubit to be executed on</param>
	void QProgram::X(const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("X::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "X(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Pauli gate. Rotation about Y on pi radians
	/// </summary>
	/// <param name="qubit_num"> - qubit to be executed on</param>
	void QProgram::Y(const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("Y::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "Y(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Pauli gate. Rotation about Z on pi radians
	/// </summary>
	/// <param name="qubit_num"> - qubit to be executed on</param>
	void QProgram::Z(const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("Z::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "Z(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Gate pi/8. Rotation about Z on pi/4 radians
	/// </summary>
	/// <param name="qubit_num"> - qubit to be executed on</param>
	void QProgram::T(const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("T::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "T(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Gate pi/8 adjoint. Rotation about Z on pi/4 radians
	/// </summary>
	/// <param name="qubit_num"> - qubit to be executed on</param>
	void QProgram::T_conj(const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("T_conj::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "T_conj(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Rotation about Z on pi/2 radians
	/// </summary>
	/// <param name="qubit_num"> - qubit to be executed on</param>
	void QProgram::S(const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("S::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "S(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Rotation adjoint about Z on pi/2 radians
	/// </summary>
	/// <param name="qubit_num"> - qubit to be executed on</param>
	void QProgram::S_conj(const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("S_conj::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "S_conj(" << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Exchanges the qubit's values
	/// </summary>
	/// <param name="qubit_1"> - the first qubit in SWAP pair</param>
	/// <param name="qubit_2"> - the second qubit in SWAP pair</param>
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
	/// <summary>
	/// Toffoli or CCnot gate. Changes the last qubit only if the first 2 qubits are ones
	/// Qubits shouldn't necessarily be adjacent
	/// </summary>
	/// <param name="qubit_1"> - 1 controlling qubit</param>
	/// <param name="qubit_2"> - 2 controlling qubit</param>
	/// <param name="qubit_3"> - target qubit</param>
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
	/// <summary>
	/// Phase shift gate.Rotation about Z axis on phi radians angle.
	/// </summary>
	/// <param name="angle"> - the angle to rotate</param>
	/// <param name="qubit_num"> - the qubit to be executed on</param>
	void QProgram::P(const double angle, const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("P::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "P(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Phase shift adjoint gate.Rotation about Z axis on phi radians angle.
	/// </summary>
	/// <param name="angle"> - the angle to rotate</param>
	/// <param name="qubit_num"> - the qubit to be executed on</param>
	void QProgram::P_conj(const double angle, const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("P_conj::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "P_conj(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Generic phase shift. Can be useful to represent the exact value of gates after Bloch
	/// representation
	/// </summary>
	/// <param name="angle"> - the angle to rotate</param>
	/// <param name="qubit_num"> - the qubit to be executed</param>
	void QProgram::Phase(const double angle, const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("Phase::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "Phase(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// Generic phase shift adjoint. Can be useful to represent the exact value of gates after Bloch
	/// representation
	/// </summary>
	/// <param name="angle"> - the angle to rotate</param>
	/// <param name="qubit_num"> - the qubit to be executed</param>
	void QProgram::Phase_conj(const double angle, const long qubit_num) {
		if (qubit_num <= 0 || qubit_num > qubits_amount) {
			throw std::invalid_argument("Phase_conj::ERROR: target qubit is out of range - " + std::to_string(qubit_num));
		}
		std::stringstream parameters;
		parameters << "Phase_conj(" << angle << "," << qubit_num << ")";
		command_queue.push(parameters.str());
	}
	/// <summary>
	/// The gate of multicontrol rotation about X,Y,Z axes
	/// </summary>
	/// <param name="controlling_qubits"> - the list of controlling qubits</param>
	/// <param name="rotation_type"> - the type of rotation</param>
	/// <param name="angle"> - the angle to rotate on</param>
	/// <param name="qubit_num"> - the qubit to be executed on</param>
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
	/// <summary>
	/// An arbitrary unitary transformation. Use Eigen::MatrixXcd to define unitary matrix of
	/// your own. Make sure it's unitary, though you'll've been told if it's not.
	/// </summary>
	/// <param name="matr"></param>
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
	/// <summary>
	/// Measure operation on exact qubit. 
	/// Note: When you measure the qubit, the register "shrinks" in twice. So, if you wil try to measure
	/// afterwords with the same qubits numbers as they were on the previous step, that'll be another qubits,
	/// as a metter of fact. Thus, you'd better measure carefully
	/// </summary>
	/// <param name="qubit_to_measure">The number of qubit to be measured</param>
	void QProgram::Measure(const long qubit_to_measure) {
		if (qubits_amount == 0) {
			throw std::invalid_argument("Measure::ERROR: Invalid operation. There is nothing to measure \n");
		}
		if (qubit_to_measure <= 0 || qubit_to_measure > qubits_amount) {
			throw std::invalid_argument("Measure::ERROR: target qubit is out of range - " + std::to_string(qubit_to_measure));
		}
		std::stringstream parameters;
		parameters << "Measure(" << qubit_to_measure << ")";
		command_queue.push(parameters.str());
		this->qubits_amount--;
	}
	/// <summary>
	/// Measures the whole register at once
	/// </summary>
	void QProgram::Measure_all() {
		if (qubits_amount == 0) {
			throw std::invalid_argument("Invalid operation: There is nothing to measure \n");
		}
		std::stringstream ss;
		ss << "Measure_all(" << ")";
		command_queue.push(ss.str());
		this->qubits_amount = 0;
	}
	/// <summary>
	/// The call point of the calculation. Execute if you sure that your program is correct and ready to be
	/// send to some executor.
	/// </summary>
	void QProgram::Execute() {
		if (command_queue.empty()) {
			throw std::exception("Execute::ERROR: Invalid operation. The command queue is empty \n");
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