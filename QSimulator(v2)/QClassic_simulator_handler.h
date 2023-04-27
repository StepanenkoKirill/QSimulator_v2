#pragma once
#include "QSimulator.h"
#include <unordered_map>
#include "macro.h"

namespace Work_namespace {

	class QClassic_simulator_handler {
	private:
		std::unordered_map<std::string, const int> operators_list = {
			{"Init_reg", 100},
			{"R_x", 101},
			{"R_y", 102},
			{"R_z", 103},
			{"SWAP", 104},
			{"Cnot", 105},
			{"Measure", 106}
		};
		std::ifstream& instruction_stream;
		QSimulator _sim;
	public:
		QClassic_simulator_handler() = delete;
		QClassic_simulator_handler(std::ifstream& input) : instruction_stream(input){}
		std::vector<bool> run(){
			std::vector<bool> answer;
			std::string tmp;
			std::stringstream ss;
			int command_code = 0;
			/*file ordered row by row*/
			while (std::getline(instruction_stream, tmp)) {
				command_code = read_operator(tmp);
				/*have changed tmp for reading params*/
				switch (command_code) {

				case _Init_reg:

				}
			}
			return answer;
		}
		void read_integer_parameter(std::string& str);
		void read_real(std::string& str);
		int read_operator(std::string& str);
		void read_parameters(std::string& str);
	};
	int QClassic_simulator_handler::read_operator(std::string& str) {
		std::stringstream ss(str);
		int ans = 0;
		std::string oper, params;
		std::getline(ss, oper, '(');
		std::getline(ss, params, ')');
		ans = operators_list.at(oper);
		return ans;
	}
}