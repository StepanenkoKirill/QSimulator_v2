#pragma once
#include "QSimulator.h"
#include <unordered_map>

namespace Work_namespace {

	class QClassic_simulator_handler {
	private:
		std::unordered_map<std::string, int> operators_list = {
			{"Init_reg", 1},
			{"R_x", 2},
			{"R_y", 3},
			{"R_z", 4},
			{"SWAP", 5},
			{"Cnot", 6},
			{"Measure", 7}
		};
		std::ifstream& instruction_stream;
		QSimulator _sim;
	public:
		QClassic_simulator_handler() = delete;
		QClassic_simulator_handler(std::ifstream& input) : instruction_stream(input){}
		std::vector<bool> run(){
			std::vector<bool> answer;
			std::string tmp;
			int command_code = 0;
			while (std::getline(instruction_stream, tmp)) {
				switch (command_code) {
					operators_list[tmp];
				case 0:

				}
			}
			return answer;
		}
		void read_integer(std::string& str);
		void read_real(std::string& str);
		void read_operator(std::string& str);
		void read_parameters(std::string& str);
	};
}