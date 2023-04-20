#pragma once
#include "Matrix.h"
namespace Work_namespace {

	class QExecutor_interface {
	protected:

		class QRegister {
		public:
			QRegister(const QRegister& reg) = delete; // we can't copy reg-r
			QRegister() {}
			QRegister(long size) {
				this->size = size;
				long space_size = 1 << size;
				amplitudes = new std::vector<std::complex<double>>;
				amplitudes->resize(space_size);
				amplitudes->at(0) = 1.;
			}
			void Add_ancilla(); // add functionality
			void Tensor_product(const QRegister& another_reg);

			void Project(const long& space_size) {
				//add check for 2^n space_size
				amplitudes->resize(space_size);
			}
			int get_size() const { return size; }
			std::vector<std::complex<double>> get_amps() const { return *amplitudes; }
			std::complex<double>& operator[](long i) { return (*amplitudes)[i]; }
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