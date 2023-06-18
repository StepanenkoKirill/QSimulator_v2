#pragma once
#include "Matrix.h"
namespace Work_namespace {

	/// <summary>
	/// Abstract class for representing the interface for abstract
	/// quantum executor. Class is made to make it possible define different
	/// kinds of simulators.
	/// </summary>
	class QExecutor_interface {
	protected:

		/// <summary>
		/// Inner class of quantum register to make it possible represent register in some other way
		/// and to set it defined inside simulator as it's part
		/// </summary>
		class QRegister {
		public:
			/*copy constructor*/
			QRegister(const QRegister& reg) = delete; 

			QRegister() {}

			QRegister(const long size) {
				this->size = size;
				long space_size = 1 << size;
				/*define |000...0> initial state*/
				amplitudes = new std::vector<std::complex<double>>(space_size, 0.);
				amplitudes->at(0) = 1.;
			}
			
			/// <summary>
			/// Initialises the register size from the inside of the register class as a method
			/// </summary>
			/// <param name="number_of_qubits_to_set"> - Number of qubits in the register</param>
			void Initialize(const long number_of_qubits_to_set) {
				long space_size = 1 << number_of_qubits_to_set;
				this->size = number_of_qubits_to_set;
				delete amplitudes;
				/*define |000...0> initial state*/
				amplitudes = new std::vector<std::complex<double>>(space_size, 0.);
				amplitudes->at(0) = 1.;
			}

			/// <summary>
			/// An ancilla is an extra qubit to make some "handy" intermediate operations
			/// Is necessary for an uncomputation process for example 
			/// </summary>
			/// <param name="initial_amplitudes"> - amplitudes of the ancilla</param>
			void Add_ancilla_qubit(std::vector<std::complex<double>>& initial_amplitudes) {
				if (initial_amplitudes.size() != 2) {
					std::cout << "Add_ancilla_qubit::Runtime ERROR: wrong size of the ancilla bit\n\n" << std::endl;
					std::cout << "---------------------------------------------------------------\n\n";
					throw std::runtime_error("False ancilla size\n\n");
				}
				else if (initial_amplitudes.size() == 2) {
					Tensor_product(initial_amplitudes);
				}
				else {
					std::cout << "Add_ancilla_qubit::Runtime ERROR: undefined behaviour\n\n" << std::endl;
					std::cout << "-----------------------------------------------------\n\n";
					throw std::runtime_error("False state\n\n");
				}

			}

			/// <summary>
			/// Method makes new register making kronecker product with another one
			/// </summary>
			/// <param name="another_reg"> - Anther register object</param>
			void Tensor_product(const QRegister& another_reg) {
				if (another_reg.get_size() % 2 != 0) {
					std::cout << "Tensor_product::Runtime ERROR: Another register is not qubit-oriented\n\n";
					std::cout << "---------------------------------------------------------------------\n\n";
					throw std::runtime_error("False another register size\n\n");
				}
				else if (another_reg.get_amps().empty()) {
					std::cout << "Tensor_product::Runtime ERROR: Another register is empty\n\n";
					std::cout << "---------------------------------------------------------------------\n\n";
					throw std::runtime_error("False another register size\n\n");
				}
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

			/// <summary>
			/// Method of Kronecker product with ampl-s instead register
			/// </summary>
			/// <param name="another_reg"> - Vector of complex amplitudes</param>
			void Tensor_product(std::vector<std::complex<double>>& another_reg) {
				if (another_reg.size() % 2 != 0) {
					std::cout << "Tensor_product::Runtime ERROR: Another register is not qubit-oriented\n\n";
					std::cout << "---------------------------------------------------------------------\n\n";
					throw std::runtime_error("False another register size\n\n");
				}
				else if (another_reg.empty()) {
					std::cout << "Tensor_product::Runtime ERROR: Another register is empty\n\n";
					std::cout << "---------------------------------------------------------------------\n\n";
					throw std::runtime_error("False another register size\n\n");
				}
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

			/// <summary>
			/// Method making a resize of register
			/// </summary>
			/// <param name="new_size"> - new size of register</param>
			void Project(const long new_size) {
				if (new_size % 2 != 0) {
					std::cout << "Project::Runtime ERROR: Projection is not qubit-oriented\n\n";
					std::cout << "--------------------------------------------------------\n\n";
					throw std::runtime_error("False size of projected space size\n\n");
				}
				long space_size = 1 << new_size;
				this->size = new_size;
				amplitudes->resize(space_size);
			}

			/// <summary>
			/// Gets the register size
			/// </summary>
			/// <returns>Returns the value of valid qubits</returns>
			long get_size() const { return size; }
			std::vector<std::complex<double>> get_amps() const { 
				if (amplitudes->empty()) {
					std::cout << "get_size::Runtime ERROR: Empty register\n\n";
					std::cout << "--------------------------------------------------------\n\n";
					throw std::runtime_error("Trying to get an empty register amplitudes\n\n");
				}
				return *amplitudes; 
			}

			/// <summary>
			/// Operator [] to get register easier
			/// </summary>
			/// <param name="i"> - index of element</param>
			/// <returns>Complex amplitude</returns>
			std::complex<double>& operator[](long i) const { return (*amplitudes)[i]; }

			/// <summary>
			/// Product with a complex number
			/// </summary>
			/// <param name="var"> - complex variable</param>
			/// <returns>Returns register reference</returns>
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

		/*Pure virtual functions that every QSimulator should have*/

		virtual void Init_reg(const long qubits_quantity) = 0;
		
		virtual bool Measure(const long qubit_number) = 0;

		virtual std::vector<bool> Measure_all() = 0;

	};
}