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
		/*describes translation the object file into operators sequence*/
		std::vector<bool> run(){
			std::vector<bool> answer;
			std::string tmp;
			std::stringstream ss;
			long int_par, int_par_2 = 0;
			double real_par = 0.;
			int command_code = 0;
			/*file ordered row by row*/
			while (std::getline(instruction_stream, tmp)) {
				command_code = read_operator(tmp);
				/*have changed tmp for reading params*/
				switch (command_code) {

				case _Init_reg:
					int_par = read_integer_parameter(tmp);
					_sim.Init_reg(int_par);
					break;
				case _R_x:
					real_par = read_real_parameter(tmp);
					int_par = read_integer_parameter(tmp);
					_sim.R_x(real_par, int_par);
					break;
				case _R_y:
					real_par = read_real_parameter(tmp);
					int_par = read_integer_parameter(tmp);
					_sim.R_y(real_par, int_par);
					break;
				case _R_z:
					real_par = read_real_parameter(tmp);
					int_par = read_integer_parameter(tmp);
					_sim.R_z(real_par, int_par);
					break;
				case _SWAP:
					int_par = read_integer_parameter(tmp);
					int_par_2 = read_integer_parameter(tmp);
					_sim.SWAP(int_par, int_par_2);
					break;
				case _Measure:
					int_par = read_integer_parameter(tmp);
					_sim.Measure(int_par);
					break;
				default:
					std::cout << "RUNTIME ERROR: Can't find such operator \n";
				}
			}
			return answer;
		}
		long read_integer_parameter(std::string& str);
		double read_real_parameter(std::string& str);
		int read_operator(std::string& str);
	};
	int QClassic_simulator_handler::read_operator(std::string& str) {
		std::stringstream ss(str);
		int ans = 0;
		std::string oper, params;
		std::getline(ss, oper, '(');
		std::getline(ss, params, ')');
		ans = operators_list.at(oper);
		str = params;
		return ans;
	}
	double read_real_parameter(std::string& str) {
		double answer = 0.;
		std::stringstream ss(str);
		std::string param, remainder;
		char c = '0.';
		/*remake the complex if close*/
		while (ss.get(c)) {
			if (c != ',' && c != '\n' && c != '\0') {
				param.push_back(c);
			}
			else {
				break;
			}
		}
		std::getline(ss, remainder);
		str = remainder;
		answer = std::stod(param);
		return answer;
	}
}