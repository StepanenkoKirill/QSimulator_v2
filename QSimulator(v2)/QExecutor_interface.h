#pragma once
#include "Matrix.h"
namespace Work_namespace {

	class QExecutor_interface {
	protected:

		class QRegister {
		public:
			QRegister(const QRegister& reg) = delete; // we can't copy reg-r
			QRegister() {}
			QRegister(const long size) {
				this->size = size;
				long space_size = 1 << size;
				amplitudes = new std::vector<std::complex<double>>;
				amplitudes->resize(space_size);
				amplitudes->at(0) = 1.;
			}
			void Init_reg(const long number_of_qubits_to_set) {
				long space_size = 1 << number_of_qubits_to_set;
				this->size = number_of_qubits_to_set;
				if (this->size != 0) {
					amplitudes->clear();
					amplitudes->resize(space_size);
					amplitudes->at(0) = 1.;
				}
				else if (this->size == 0){
					amplitudes = new std::vector<std::complex<double>>;
					amplitudes->resize(space_size);
					amplitudes->at(0) = 1.;
				}
				else {
					std::cout << "An undefined behaviour";
				}
			}
			void Add_ancilla(); // add functionality

			/*copy current amplitudes and resize reg with new ampl-s*/
			void Tensor_product(const QRegister& another_reg) {
				long new_size = size * another_reg.get_size();
				long new_space_size = 1 << new_size;
				std::vector<std::complex<double>>* tmp;
				tmp = new std::vector<std::complex<double>>(size, 0);
				*tmp = (*amplitudes);
				amplitudes->resize(new_space_size);
				for (long i = 0; i < size; ++i) {
					for (long j = 0; j < another_reg.size; ++j) {
						(*amplitudes)[i * j + j] = (*tmp)[i] * (another_reg[j]);
					}
				}
			}

			void Project(const long& space_size) {
				//add check for 2^n space_size
				amplitudes->resize(space_size);
			}
			long get_size() const { return size; } //? why need
			std::vector<std::complex<double>> get_amps() const { return *amplitudes; }
			std::complex<double>& operator[](long i) const  { return (*amplitudes)[i]; }
			QRegister& operator*(const std::complex<double>& var) {
				for (auto& item : *amplitudes) {
					item *= var;
				}
				return *this;
			}
			virtual ~QRegister() {
				this->size = 0;
				delete this->amplitudes;
			}
		private:
			long size = 0;
			std::vector<std::complex<double>> *amplitudes = nullptr;
		};

		/*Standart Basis*/
		virtual void R_x(const double angle) = 0 {};
		virtual void R_y(const double angle) = 0 {};
		virtual void R_z(const double angle) = 0 {};
		virtual void SWAP(const long first_qubit, const long second_qubit) = 0 {};
		virtual void Cnot(const long first_qubit, const long second_qubit) = 0 {};

	};
}