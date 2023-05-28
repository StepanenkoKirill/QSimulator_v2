#pragma once
#include <fstream>
#include <string>
#include "macro.h"
#include <queue>
#include <unordered_map>

namespace Work_namespace {
	std::unordered_map<std::string, const int> operators_list = {
	{"Init_reg", 100},
	{"R_x", 101},
	{"R_y", 102},
	{"R_z", 103},
	{"SWAP", 104},
	{"Cnot", 105},
	{"Measure", 106},
	{"Adjacent_SWAP", 107},
	{"Multycontrol_rotation", 108}
	};
	/*the analising subautomats don't recognize every opp-ty as our input is strict enough
	  except the order of the logic*/
	bool is_start(std::string& s, std::ofstream& out) {
		bool answer = false;
		std::string instruction, parameteres;
		std::stringstream ss(s);
		std::getline(ss, instruction, '(');
		std::getline(ss, parameteres);
		/*if no element .at throws an exception*/
		if (operators_list.at(instruction) == 100) {
			out << instruction << '(' << parameteres << '\n';
			answer = true;
		}
		else {
			std::cout << "SYNTAX ERROR: Initialisation of the register is false. Wrong operator \n ";
		}

		return answer;
	}
	bool is_operation_list(std::string& s, std::ofstream& out) {
		bool answer = false;
		std::string instruction, parameteres, tmp;
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
		case _R_x:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _R_y:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _R_z:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _SWAP:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _Adjacent_SWAP:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _Multycontrol_rotation:
			ss.clear();
			ss >> parameteres;
			std::getline(ss, tmp, ';');
			std::getline(ss, tmp, ';');
			if (!operators_list.count(tmp)) {
				std::cout << "SYNTAX ERROR: Can't find such operator \n";
			}
			else if (!(tmp == "R_x" || tmp == "R_y" || tmp == "R_z")) {
				std::cout << "SYNTAX ERROR: Rotation operator needed \n";
			}
			else {
				out << instruction << '(' << parameteres << '\n';
				answer = true;
			}
			break;
		default:
			std::cout << "SYNTAX ERROR: Can't find such operator \n";
		}
		return answer;
	}
	bool is_end(std::string& s, std::ofstream& out) {
		bool answer = false;
		std::string instruction, parameteres;
		std::stringstream ss(s);
		std::getline(ss, instruction, '(');
		std::getline(ss, parameteres);
		if (operators_list.at(instruction) == 106) {
			out << instruction << '(' << parameteres;
			answer = true;
		}
		else {
			std::cout << "SYNTAX ERROR: Incorrect ending of quantum program.  \n ";
		}
		return answer;
	}

	bool Analyser(std::queue< std::string>& in) {
		bool answer = false;
		std::string input_instruction, temp;
		int grammar_state = _INITIAL_GRAMMAR_STATE;
		int error_code = 0;
		std::ofstream out;
		out.open("Debugging_test_file", std::ios::out | std::ios::app);
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
				if (is_end(input_instruction, out)) {
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

					printf("SYNTAX_ERROR: Wrong initialisation of register: %s \n", (in.front()));
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



