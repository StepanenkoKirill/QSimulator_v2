#pragma once
#include "QSimulator.h"
#include <unordered_map>
#include "macro.h"

namespace Work_namespace {
	class QClassic_simulator_handler {
	private:
		std::ifstream& instruction_stream;
		QSimulator _sim;
	public:

		QClassic_simulator_handler() = delete;
		QClassic_simulator_handler(std::ifstream& input) : instruction_stream(input) {}
		/*describes translation the object file into operators sequence*/

		std::vector<bool> Run();

		template<typename T>
		T read_parameter(std::string& str, T(*func)(const std::string&));

		template<typename T_1 = long, typename T_2 = double>
		std::vector<bool> run() = delete;

		long read_integer_parameter(std::string& str);
		double read_real_parameter(std::string& str);
		std::string read_string_parameter(std::string& str);
		int read_operator(std::string& str);
		long parameters_counter(std::string& str, char inter_delim, char outer_delim);
		std::function<void(const double, const long)> R_x() {
			return [&](const double x, const long y) {return _sim.R_x(x, y);};
		}
		std::function<void(const double, const long)> R_y() {
			return [&](const double x, const long y) {return _sim.R_y(x, y);};
		}
		std::function<void(const double, const long)> R_z() {
			return [&](const double x, const long y) {return _sim.R_z(x, y);};
		}
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

	long QClassic_simulator_handler::parameters_counter(std::string& str, char inter_delim, char outer_delim) {
		std::stringstream ss(str);
		bool flag = false;
		long counter = 1;
		char c = ' ';
		while (ss.get(c) && c != outer_delim && c != '\0') {
			flag = true;
			if (c == inter_delim) {
				counter++;
			}
		}
		if (!flag) counter = -1;
		return counter;
	}

	double QClassic_simulator_handler::read_real_parameter(std::string& str) {
		double answer = 0.;
		std::stringstream ss(str);
		std::string param, remainder;
		char c = '0.';
		/*remake the complex if close*/
		while (ss.get(c)) {
			if (c != ',' && c != '\0' && c != ';') {
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

	long QClassic_simulator_handler::read_integer_parameter(std::string& str) {
		long answer = 0;
		std::stringstream ss(str);
		std::string param, remainder;
		char c = '0';
		while (ss.get(c)) {
			if (c != ',' && c != '\0' && c != ';') {
				param.push_back(c);
			}
			else {
				break;
			}
		}
		std::getline(ss, remainder);
		str = remainder;
		answer = std::stol(param);
		return answer;
	}

	std::string QClassic_simulator_handler::read_string_parameter(std::string& str) {
		std::stringstream ss(str);
		std::string param, remainder;
		char c = ' ';
		while (ss.get(c)) {
			if (c != ',' && c != '\0' && c != ';') {
				param.push_back(c);
			}
			else {
				break;
			}
		}
		std::getline(ss, remainder);
		str = remainder;
		return param;
	}

	std::vector<bool> QClassic_simulator_handler::Run() {
		std::vector<bool> answer;
		std::string tmp;
		std::stringstream ss;
		std::vector<long> controlling_qubtis;
		int command_code = 0;
		long counter = 0, multycontroll_command = 0;
		/*file ordered row by row*/
		while (std::getline(instruction_stream, tmp)) {
			command_code = read_operator(tmp);
			/*have changed tmp for reading params*/
			switch (command_code) {

			case _Init_reg:
				_sim.Init_reg(read_integer_parameter(tmp));
				break;
			case _R_x:
				_sim.R_x(read_real_parameter(tmp), read_integer_parameter(tmp));
				break;
			case _R_y:
				_sim.R_y(read_real_parameter(tmp), read_integer_parameter(tmp));
				break;
			case _R_z:
				_sim.R_z(read_real_parameter(tmp), read_integer_parameter(tmp));
				break;
			case _SWAP:
				_sim.SWAP(read_integer_parameter(tmp), read_integer_parameter(tmp));
				break;
			case _Adjacent_SWAP:
				_sim.Adjacent_SWAP(read_integer_parameter(tmp), read_integer_parameter(tmp));
				break;
			case _Measure:
				answer.push_back(_sim.Measure(read_integer_parameter(tmp)));
				break;
			case _Measure_all:
				answer = _sim.Measure_all();
				break;
			case _Phase:
				_sim.Phase(read_real_parameter(tmp), read_integer_parameter(tmp));
				break;
			case _P:
				_sim.P(read_real_parameter(tmp), read_integer_parameter(tmp));
				break;
			default:
				std::cout << "RUNTIME ERROR: Can't find such operator \n";
			}
		}
		return answer;
	}

	template<typename T>
	T QClassic_simulator_handler::read_parameter(std::string& str, T(*func)(const std::string&)) {
		T answer;
		std::stringstream ss(str);
		std::string param, remainder;
		char c = '0.';
		/*remake the complex if close*/
		while (ss.get(c)) {
			if (c != ',' && c != '\0') {
				param.push_back(c);
			}
			else {
				break;
			}
		}
		std::getline(ss, remainder);
		str = remainder;
		answer = (*func)(param);
		return answer;
	}

}