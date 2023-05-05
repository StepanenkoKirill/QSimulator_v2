#pragma once
#define _USE_MATH_DEFINES

#define _INITIAL_GRAMMAR_STATE 0
#define _START_STATE 1
#define _ERROR_STATE 99
#define _READ_OPERATION_LIST_STATE 2
#define _END_STATE 3

#define _Init_reg 100
#define _R_x 101
#define _R_y 102
#define _R_z 103
#define _SWAP 104
#define _Cnot 105
#define _Measure 106
#define _Adjacent_SWAP 107

std::unordered_map<std::string, const int> operators_list = {
			{"Init_reg", 100},
			{"R_x", 101},
			{"R_y", 102},
			{"R_z", 103},
			{"SWAP", 104},
			{"Cnot", 105},
			{"Measure", 106},
			{"Adjacent_SWAP", 107}
};



