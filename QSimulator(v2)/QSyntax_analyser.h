#pragma once
#include <fstream>
#include <string>
#include "macro.h"
#include <queue>
#include <unordered_map>
#include <math.h>

namespace Work_namespace {
	std::unordered_map<std::string, const int> operators_list = {
	{"Init_reg", 100},
	{"R_x", 101},
	{"R_x_conj", 118},
	{"R_y", 102},
	{"R_y_conj", 119},
	{"R_z", 103},
	{"R_z_conj", 120},
	{"SWAP", 104},
	{"Cnot", 105},
	{"Measure", 106},
	{"Measure_all", 126},
	{"Adjacent_SWAP", 107},
	{"Multycontrol_rotation", 108},
	{"Multy_X_aux", 109},
	{"P", 110},
	{"P_conj", 123},
	{"X", 111},
	{"Y", 112},
	{"Z", 113},
	{"H", 114},
	{"Toffoli", 115},
	{"T", 116},
	{"S", 117},
	{"S_conj", 122},
	{"T_conj", 121},
	{"Phase_conj", 125},
	{"Phase", 124}
	};
	static long parsed_register_size = 0;
	void toffoli_decomposition_helper(long _q1, long _q2, long _trgt, std::ofstream& out) {
		out << "Phase(" << M_PI_2 << "," << _trgt << ')' << '\n';
		out << "R_x(" << M_PI_2 << "," << _trgt << ')' << '\n';
		out << "R_z(" << M_PI_2 << "," << _trgt << ')' << '\n';
		out << "R_x(" << M_PI_2 << "," << _trgt << ')' << '\n';
		out << "Cnot(" << _q2 << ',' << _trgt << ')' << '\n';
		out << "P(" << -M_PI_4 << "," << _trgt << ')' << '\n';
		out << "Cnot(" << _q1 << ',' << _trgt << ')' << '\n';
		out << "P(" << M_PI_4 << "," << _trgt << ')' << '\n';
		out << "Cnot(" << _q2 << ',' << _trgt << ')' << '\n';
		out << "P(" << -M_PI_4 << "," << _trgt << ')' << '\n';
		out << "Cnot(" << _q1 << ',' << _trgt << ')' << '\n';
		out << "P(" << M_PI_4 << "," << _trgt << ')' << '\n';
		out << "P(" << M_PI_4 << "," << _trgt << ')' << '\n';
		out << "Cnot(" << _q1 << ',' << _q2 << ')' << '\n';
		out << "Phase(" << M_PI_2 << "," << _trgt << ')' << '\n';
		out << "R_x(" << M_PI_2 << "," << _trgt << ')' << '\n';
		out << "R_z(" << M_PI_2 << "," << _trgt << ')' << '\n';
		out << "R_x(" << M_PI_2 << "," << _trgt << ')' << '\n';
		out << "P(" << M_PI_4 << "," << _q1 << ')' << '\n';
		out << "P(" << -M_PI_4 << "," << _q2 << ')' << '\n';
		out << "Cnot(" << _q1 << ',' << _q2 << ')' << '\n';
	}
	void h_decomposition_helper(long qubit_num, std::ofstream& out) {
		out << "Phase(" << M_PI_2 << "," << qubit_num << ')' << '\n';
		out << "R_x(" << M_PI_2 << "," << qubit_num << ')' << '\n';
		out << "R_z(" << M_PI_2 << "," << qubit_num << ')' << '\n';
		out << "R_x(" << M_PI_2 << "," << qubit_num << ')' << '\n';
	}
	void multy_x_aux_decomposition_helper(std::list<long>& _c_qub_list, long start, long end, long _aux, long _trgt,
			std::ofstream& out) {
		if ((end-start+1) == 2) {
			std::list<long>::const_iterator it_start = _c_qub_list.cbegin();
			std::list<long>::const_iterator it_end = _c_qub_list.cbegin();
			std::advance(it_start, start);
			std::advance(it_end, end);
			toffoli_decomposition_helper(*it_start, *it_end, _trgt, out);
			return;
		}

		else if ((end - start + 1) == 1) {
			std::list<long>::const_iterator it_start = _c_qub_list.cbegin();
			std::advance(it_start, start);
			out << "Cnot(" << *it_start << ',' << _trgt << ')' << '\n';
		}
		else {
			long n = end - start + 1 + 2; // start end are indexes in list of control qubits
			long m1 = n / 2;
			long m2 = n - m1 - 1;
			long _s1 = start;
			long _n1 = start + m1 - 1;
			long _s2 = _n1 + 1; //index of the first control qubit from 2nd group
			long _n2 = end; // index of the last control qubit from 2nd group
			std::list<long>::const_iterator it = _c_qub_list.cbegin();
			std::advance(it, _n2 + 1);
			multy_x_aux_decomposition_helper(_c_qub_list, _s1, _n1, _trgt, _aux, out);
			_c_qub_list.insert(it, _aux);
			multy_x_aux_decomposition_helper(_c_qub_list, _s2, _n2+1, *(_c_qub_list.cbegin()), _trgt, out);
			_c_qub_list.erase(it);
			multy_x_aux_decomposition_helper(_c_qub_list, _s1, _n1, _trgt, _aux, out);
			_c_qub_list.insert(it, _aux);
			multy_x_aux_decomposition_helper(_c_qub_list, _s2, _n2 + 1, *(_c_qub_list.cbegin()), _trgt, out);
			_c_qub_list.erase(it);
		}
	}

	/*the analising subautomats don't recognize every opp-ty as our input is strict enough
	  except the order of the logic*/
	bool is_start(std::string& s, std::ofstream& out) {
		bool answer = false;
		std::string instruction, parameteres;
		std::stringstream ss(s);
		std::getline(ss, instruction, '(');
		/*if no element .at throws an exception*/
		if (operators_list.at(instruction) == 100) {
			std::getline(ss, parameteres, ')');
			parsed_register_size = std::stol(parameteres);
			out << instruction << '(' << parameteres << ')' << '\n';
			answer = true;
		}
		else {
			std::cout << "SYNTAX ERROR: Initialisation of the register is false. Wrong operator \n ";
		}

		return answer;
	}
	bool is_operation_list(std::string& s, std::ofstream& out) {
		bool answer = false;
		std::string instruction, parameteres, tmp, rotation_type;
		double angle = 0., new_angle = 0.;
		std::list<long> c_qub_list;
		std::list<long>::const_iterator it;
		long aux = 0, trgt = 0, q_1 = 0, q_2 = 0, q_3 = 0, k1 = 0, k2 = 0;
		std::stringstream ss(s);
		std::getline(ss, instruction, '(');
		std::getline(ss, parameteres);
		int command_code = 0;
		command_code = operators_list.at(instruction);
		switch (command_code) {

		case _Init_reg:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _Cnot:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _R_x:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
		case _R_x_conj:
			ss.clear();
			ss >> parameteres;
			std::getline(ss, tmp, ',');
			new_angle = -std::stod(tmp);
			std::getline(ss, tmp);
			out << "R_x(" << new_angle << tmp << '\n';
			answer = true;
			break;
		case _R_y:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _R_y_conj:
			ss.clear();
			ss >> parameteres;
			std::getline(ss, tmp, ',');
			new_angle = -std::stod(tmp);
			std::getline(ss, tmp);
			out << "R_y(" << new_angle << tmp << '\n';
			answer = true;
			break;
		case _R_z:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _R_z_conj:
			ss.clear();
			ss >> parameteres;
			std::getline(ss, tmp, ',');
			new_angle = -std::stod(tmp);
			std::getline(ss, tmp);
			out << "R_z(" << new_angle << tmp << '\n';
			answer = true;
			break;
		case _T:
			out << "P(" << M_PI_4 << "," << parameteres << '\n';
			answer = true;
			break;
		case _T_conj:
			out << "P(" << -M_PI_4 << "," << parameteres << '\n';
			answer = true;
			break;
		case _S:
			out << "P(" << M_PI_2 << "," << parameteres << '\n';
			answer = true;
			break;
		case _S_conj:
			out << "P(" << -M_PI_2 << "," << parameteres << '\n';
			answer = true;
			break;
		case _Z:
			out << "P(" << M_PI << "," << parameteres << '\n';
			answer = true;
			break;
		case _Y:
			out << "Phase(" << -M_PI_2 << "," << parameteres << '\n';
			out << "R_y(" << M_PI << "," << parameteres << '\n';
			answer = true;
			break;
		case _X:
			out << "Phase(" << -M_PI_2 << "," << parameteres << '\n';
			out << "R_x(" << M_PI << "," << parameteres << '\n';
			answer = true;
			break;
		case _H:
			ss.clear();
			ss.str(parameteres);
			std::getline(ss, tmp, ')');
			q_3 = std::stol(tmp);
			h_decomposition_helper(q_3, out);
			answer = true;
			break;
		case _Toffoli:
			ss.clear();
			ss >> parameteres;
			std::getline(ss, tmp, ',');
			q_1 = std::stol(tmp);
			std::getline(ss, tmp, ',');
			q_2 = std::stol(tmp);
			std::getline(ss, tmp, ')');
			q_3 = std::stol(tmp);
			toffoli_decomposition_helper(q_1, q_2, q_3, out);
			answer = true;
			break;
		case _Multy_X_aux:
			ss.clear();
			ss >> parameteres;
			while (ss.peek() != ';') {
				std::getline(ss, tmp, ',');
				c_qub_list.push_back(std::stol(tmp));
			}
			ss.get(); // passing ;
			std::getline(ss, tmp, ';');
			aux = std::stol(tmp);
			std::getline(ss, tmp, ')');
			trgt = std::stol(tmp);
			multy_x_aux_decomposition_helper(c_qub_list, 0, c_qub_list.size() - 1, aux, trgt, out);
			c_qub_list.clear();
			break;
		case _SWAP:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _Adjacent_SWAP:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _P:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _P_conj:
			ss.clear();
			ss >> parameteres;
			std::getline(ss, tmp, ',');
			new_angle = -std::stod(tmp);
			std::getline(ss, tmp);
			out << "P(" << new_angle << tmp << '\n';
			answer = true;
			break;
		case _Phase:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _Phase_conj:
			ss.clear();
			ss >> parameteres;
			std::getline(ss, tmp, ',');
			new_angle = -std::stod(tmp);
			std::getline(ss, tmp);
			out << "Phase(" << new_angle << tmp << '\n';
			answer = true;
			break;
		case _Multycontrol_rotation:
			ss.clear();
			ss >> parameteres;
			while (ss.peek() != ';') {
				std::getline(ss, tmp, ',');
				c_qub_list.push_back(std::stol(tmp));
			}
			ss.get(); // passing ;
			std::getline(ss, tmp, ';');
			rotation_type = tmp;
			std::getline(ss, tmp, ';');
			angle = std::stod(tmp);
			std::getline(ss, tmp, ')');
			trgt = std::stol(tmp);
			if (!operators_list.count(tmp)) {
				std::cout << "SYNTAX ERROR: Can't find such operator \n";
			}
			else if (!(tmp == "R_x" || tmp == "R_y" || tmp == "R_z")) {
				std::cout << "SYNTAX ERROR: Rotation operator needed \n";
			}
			else {
				command_code = operators_list.at(rotation_type);
				k1 = c_qub_list.size() / 2;
				k2 = c_qub_list.size() - k1;
				switch (command_code) {
				case _R_x:
					it = c_qub_list.cbegin();
					std::advance(it, k1 + k2 - 1);
					h_decomposition_helper(trgt, out);
					multy_x_aux_decomposition_helper(c_qub_list, 0, k1 - 1, *it, trgt, out);
					out << "R_z(" << angle / 4 << ',' << trgt << ')\n';
					multy_x_aux_decomposition_helper(c_qub_list, k1, k1 + k2 - 1, *c_qub_list.cbegin(), trgt, out);
					out << "R_z(" << -angle / 4 << ',' << trgt << ')\n';
					multy_x_aux_decomposition_helper(c_qub_list, 0, k1 - 1, *it, trgt, out);
					out << "R_z(" << angle / 4 << ',' << trgt << ')\n';
					multy_x_aux_decomposition_helper(c_qub_list, k1, k1 + k2 - 1, *c_qub_list.cbegin(), trgt, out);
					out << "R_z(" << -angle / 4 << ',' << trgt << ')\n';
					h_decomposition_helper(trgt, out);
					break;
				case _R_y:
					it = c_qub_list.cbegin();
					std::advance(it, k1 + k2 - 1);
					multy_x_aux_decomposition_helper(c_qub_list, 0, k1 - 1, *it, trgt, out);
					out << "R_y(" << -angle / 4 << ',' << trgt << ')\n';
					multy_x_aux_decomposition_helper(c_qub_list, k1, k1 + k2 - 1, *c_qub_list.cbegin(), trgt, out);
					out << "R_y(" << angle / 4 << ',' << trgt << ')\n';
					multy_x_aux_decomposition_helper(c_qub_list, 0, k1 - 1, *it, trgt, out);
					out << "R_y(" << -angle / 4 << ',' << trgt << ')\n';
					multy_x_aux_decomposition_helper(c_qub_list, k1, k1 + k2 - 1, *c_qub_list.cbegin(), trgt, out);
					out << "R_y(" << angle / 4 << ',' << trgt << ')\n';
					break;
				case _R_z:
					it = c_qub_list.cbegin();
					std::advance(it, k1 + k2 - 1);
					multy_x_aux_decomposition_helper(c_qub_list, 0, k1 - 1, *it, trgt, out);
					out << "R_z(" << -angle / 4 << ',' << trgt << ')\n';
					multy_x_aux_decomposition_helper(c_qub_list, k1, k1 + k2 - 1, *c_qub_list.cbegin(), trgt, out);
					out << "R_z(" << angle / 4 << ',' << trgt << ')\n';
					multy_x_aux_decomposition_helper(c_qub_list, 0, k1 - 1, *it, trgt, out);
					out << "R_z(" << -angle / 4 << ',' << trgt << ')\n';
					multy_x_aux_decomposition_helper(c_qub_list, k1, k1 + k2 - 1, *c_qub_list.cbegin(), trgt, out);
					out << "R_z(" << angle / 4 << ',' << trgt << ')\n';
					break;
				}
				answer = true;
			}
			break;
		case _Measure:
			if (parsed_register_size > 0) {
				out << "Measure(" << parameteres  << '\n';
				parsed_register_size--;
				answer = true;
			}
			else {
				std::cout << "SYNTAX ERROR: Extra Measure operation. Register has already been measured \n";
			}
			break;
		default:
			std::cout << "SYNTAX ERROR: Can't find such operator \n";
		}
		c_qub_list.clear();
		return answer;
	}
	bool is_end(std::string& s, std::ofstream& out, long command_que_left_elems) {
		bool answer = false;
		std::string instruction, parameteres;
		std::stringstream ss(s);
		std::getline(ss, instruction, '(');
		std::getline(ss, parameteres);
		if (operators_list.at(instruction) == 126) {
			out << instruction << '(' << parameteres << '\n';
			answer = true;
		}
		else if (operators_list.at(instruction) == 106 && parsed_register_size == 1) {
			out << instruction << '(' << parameteres << '\n';
			answer = true;
		}
		else if (operators_list.at(instruction) == 106 && command_que_left_elems == 1) {
			out << instruction << '(' << parameteres << '\n';
			answer = true;
		}
		return answer;
	}

	bool Analyser(std::queue< std::string>& in) {
		bool answer = false;
		std::string input_instruction, temp;
		int grammar_state = _INITIAL_GRAMMAR_STATE;
		int error_code = 0;
		std::ofstream out;
		out.open("Debugging_test_file.txt", std::ios::out | std::ios::app);
		while (!in.empty()) {
			switch (grammar_state)
			{
			case _INITIAL_GRAMMAR_STATE:
				input_instruction = in.front();
				if (is_start(input_instruction, out)) {
					grammar_state = _START_STATE;
					in.pop();
				}
				else {
					grammar_state = _ERROR_STATE;
					error_code = 1;
				}

				break;
			case _START_STATE:
				input_instruction = in.front();
				if (is_operation_list(input_instruction, out)) {
					grammar_state = _READ_OPERATION_LIST_STATE;
					in.pop();
				}
				else {
					grammar_state = _ERROR_STATE;
					error_code = 2;
				}
				break;
			case _READ_OPERATION_LIST_STATE:
				input_instruction = in.front();
				if (is_end(input_instruction, out, in.size())) {
					grammar_state = _END_STATE;
					in.pop();
				}
				else if (is_operation_list(input_instruction, out)) {
					grammar_state = _READ_OPERATION_LIST_STATE;
					in.pop();
				}
				else {
					grammar_state = _ERROR_STATE;
					error_code = 3;
				}
				break;
			case _ERROR_STATE:
				switch (error_code)
				{
				case 1:

					printf("SYNTAX_ERROR: Wrong initialisation of register: %s \n", in.front().c_str());
					printf("----------------------------------------------\n\n");
					break;
				case 2:
					printf("SYNTAX_ERROR: Error occured in reading the list of operations: %s \n", in.front().c_str());
					printf("-------------------------------------------------------------\n\n");
					break;
				case 3:
					printf("SYNTAX_ERROR: Unexpected end of quantum instruction: %s \n", in.front().c_str());
					printf("---------------------------------------------------\n\n");
					break;
				case 4:
					printf("SYNTAX_ERROR: Unexpected end of quantum instruction. Some instructions found after the measurement\n");
					printf("The unexpected end: \n");
					while (!in.empty()) {
						std::cout << in.front() << std::endl;
						in.pop();
					}
					printf("---------------------------------------------------\n\n");
					break;
				default:
					printf("UNHANDLED_ERROR: Unexpected instruction\n");
					printf("---------------------------------------\n\n");
					break;
				}
				/*clear the queue*/
				while (!in.empty()) {
					in.pop();
				}
				break;
				/*can get here only if we've read the end of input instruction but the queue is still not empty*/
			case _END_STATE:
				grammar_state = _ERROR_STATE;
				error_code = 4;

			default:
				std::cout << "Unhandled input instruction." << std::endl;
				printf("----------------------------\n\n");
				break;
			}
		}
		if (grammar_state == _END_STATE) answer = true;
		out.close();
		return answer;
	}
}



