#pragma once
#include "Matrix.h"
namespace Work_namespace {

	class QExecutor_interface {

		//may be it's better to use heap class member for ampl-s
		class QRegister {
		public:
			QRegister(const QRegister& reg) = delete;
			QRegister() : size(0) {}
			QRegister(long size) {
				this->size = size;
				long space_size = 1 << size;
				amplitudes.resize(space_size);
				amplitudes[0] = 1.;
			}
			std::vector<std::complex<double>>& add_ancilla(); // add functionality
			void Project(const long& space_size) {
				//add check for 2^n space_size
				amplitudes.resize(space_size);
			}
			int get_size() const { return size; }
			std::vector<std::complex<double>> get_amps() const { return amplitudes; }
			std::complex<double>& operator[](long i) { return amplitudes[i]; }
			QRegister& operator*(const std::complex<double>& var) {
				for (auto& item : amplitudes) {
					item *= var;
				}
				return *this;
			}
			~QRegister() {}
		private:
			long size = 0;
			std::vector<std::complex<double>> amplitudes;
		};


	};
}