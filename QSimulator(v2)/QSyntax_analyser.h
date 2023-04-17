#pragma once
#include <fstream>
#include <string>
#include "macro.h"

namespace Work_namespace {

	bool is_digit(std::string& s) {
		return (s[0] <= '9' && s[0] >= '0') && (s.length() == 1);
	}
	bool is_start(std::string& s) {
		bool answer = false;
		return answer;
	}
	bool is_operation_list(std::string& s) {
		bool answer = false;
		return answer;
	}
	bool is_end(std::string& s) {
		bool answer = false;
		return answer;
	}

	bool Analyser(std::fstream& in) {
		bool answer = false;
		std::string input_instruction, temp;
		int grammar_state = _INITIAL_GRAMMAR_STATE;
		int error_code = 0;
		while (in.peek() != EOF) {
			switch (grammar_state)
			{
			case _INITIAL_GRAMMAR_STATE:
				getline(in, input_instruction, ';');
				if (is_start(input_instruction)) grammar_state = _START_STATE;
				else {
					grammar_state = _ERROR_STATE;
					error_code = 1;
				}
				break;
			case _START_STATE:
				getline(in, input_instruction, ';');
				if (is_operation_list(input_instruction)) grammar_state = _READ_OPERATION_LIST_STATE;
				else {
					grammar_state = _ERROR_STATE;
					error_code = 2;
				}
				break;
			case _READ_OPERATION_LIST_STATE:
				getline(in, input_instruction, '.');
				if (is_end(input_instruction)) grammar_state = _END_STATE;
				else {
					grammar_state = _ERROR_STATE;
					error_code = 3;
				}
				break;
			case _ERROR_STATE:
				switch (error_code)
				{
				case 1:
					printf("SYNTAX_ERROR: Wrong initialisation of register\n");
					printf("----------------------------------------------\n\n");
					break;
				case2:
					printf("SYNTAX_ERROR: Error occured in reading the list of operations\n");
					printf("-------------------------------------------------------------\n\n");
					break;
				case3:
					printf("SYNTAX_ERROR: Unexpected end of quantum instruction\n");
					printf("---------------------------------------------------\n\n");
					break;
				default:
					printf("UNHANDLED_ERROR: Unexpected instruction\n");
					printf("---------------------------------------\n\n");
					break;
				}
				getline(in, input_instruction);
				answer = true;
				break;
			case _END_STATE:
				getline(in, input_instruction);
				answer = true;
				break;
			default:
				std::cout << "Unhandled input instruction." << std::endl;
				printf("----------------------------\n\n");
				break;
			}
		}
		return answer;
	}
}



