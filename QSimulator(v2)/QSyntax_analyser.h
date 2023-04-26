#pragma once
#include <fstream>
#include <string>
#include "macro.h"
#include <queue>

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

	bool Analyser(std::queue< std::string>& in) {
		bool answer = false;
		std::string input_instruction, temp;
		int grammar_state = _INITIAL_GRAMMAR_STATE;
		int error_code = 0;
		while (!in.empty()) {
			switch (grammar_state)
			{
			case _INITIAL_GRAMMAR_STATE:
				input_instruction = in.front();
				if (is_start(input_instruction)) {
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
				if (is_operation_list(input_instruction)) {
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
				if (is_end(input_instruction)) {
					grammar_state = _END_STATE;
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
					printf("SYNTAX_ERROR: Wrong initialisation of register: %s \n", in.front());
					printf("----------------------------------------------\n\n");
					break;
				case2:
					printf("SYNTAX_ERROR: Error occured in reading the list of operations: %s \n", in.front());
					printf("-------------------------------------------------------------\n\n");
					break;
				case3:
					printf("SYNTAX_ERROR: Unexpected end of quantum instruction: %s \n", in.front());
					printf("---------------------------------------------------\n\n");
					break;
				case4:
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
		return answer;
	}
}



