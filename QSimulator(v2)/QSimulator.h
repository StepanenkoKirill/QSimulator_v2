#pragma once
#include "QExecutor_interface.h"

namespace Work_namespace {

	class QSimulator : public QExecutor_interface {
		QRegister _reg;
	public:
		virtual void Init_reg(const long qubits_quantity) override {}
		virtual void Measure(const long qubit_number = 0) override {}
		virtual void R_x(const double angle, const long qubit_number = 0) override {}
		virtual void R_y(const double angle, const long qubit_number = 0) override {};
		virtual void R_z(const double angle, const long qubit_number = 0) override {};
		virtual void SWAP(const long first_qubit, const long second_qubit) override {};
		virtual void Cnot(const long first_qubit, const long second_qubit) override {};
	};
}