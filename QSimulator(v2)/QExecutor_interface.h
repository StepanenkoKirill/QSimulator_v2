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
				amplitudes = new std::vector<std::complex<double>>(space_size, 0.);
				amplitudes->at(0) = 1.;
			}
			/*actually we'll have a 1-size space of amps if we set param to 0. That's false in quantum*/
			void Initialize(const long number_of_qubits_to_set) {
				long space_size = 1 << number_of_qubits_to_set;
				this->size = number_of_qubits_to_set;
				delete amplitudes;
				amplitudes = new std::vector<std::complex<double>>(space_size, 0.);
				amplitudes->at(0) = 1.;
			}
			void Add_ancilla_qubit(std::vector<std::complex<double>>& initial_amplitudes) {
				if (initial_amplitudes.size() != 2) {
					std::cout << "ERROR in Add_ancilla_qubit: wrong size of the ancilla bit" << std::endl;

				}
				else if (initial_amplitudes.size() == 2) {
					Tensor_product(initial_amplitudes);
				}
				else {
					std::cout << "ERROR in Add_ancilla_qubit: An undefined behaviour";
				}

			}

			/*copy current amplitudes and resize reg with new ampl-s*/
			void Tensor_product(const QRegister& another_reg) {
				long new_size = size * another_reg.get_size();
				long new_space_size = 1 << new_size;
				std::vector<std::complex<double>>* tmp;
				tmp = new std::vector<std::complex<double>>(size, 0);
				*tmp = (*amplitudes);
				amplitudes->resize(new_space_size);
				for (long i = 0; i < size; ++i) {
					for (long j = 0; j < another_reg.get_size(); ++j) {
						(*amplitudes)[i * j + j] = (*tmp)[i] * (another_reg[j]);
					}
				}
			}
			void Tensor_product(std::vector<std::complex<double>>& another_reg) {
				long new_size = size * another_reg.size();
				long new_space_size = 1 << new_size;
				std::vector<std::complex<double>>* tmp;
				tmp = new std::vector<std::complex<double>>(size, 0);
				*tmp = (*amplitudes);
				amplitudes->resize(new_space_size);
				for (long i = 0; i < size; ++i) {
					for (long j = 0; j < another_reg.size(); ++j) {
						(*amplitudes)[i * j + j] = (*tmp)[i] * (another_reg[j]);
					}
				}
			}

			void Project(const long new_size) {
				//add check for 2^n space_size
				long space_size = 1 << new_size;
				this->size = new_size;
				amplitudes->resize(space_size);
			}
			long get_size() const { return size; }
			std::vector<std::complex<double>> get_amps() const { return *amplitudes; }
			std::complex<double>& operator[](long i) const { return (*amplitudes)[i]; }
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
			std::vector<std::complex<double>>* amplitudes = nullptr;
		};

		virtual void Init_reg(const long qubits_quantity) = 0;
		
		virtual bool Measure(const long qubit_number) = 0;

		virtual std::vector<bool> Measure_all() = 0;

	};
}