#pragma once
#include <fstream>
#include <string>
#include "macro.h"
#include <queue>
#include <unordered_map>
#include <math.h>

#include <Eigen/Dense>
//#include <Eigen/Eigen>
//#include <unsupported/Eigen/MatrixFunctions>

namespace Work_namespace {
	/// <summary>
	/// table of provided operators
	/// </summary>
	std::unordered_map<std::string, const int> operators_list = {
	{"Init_reg", 100},
	{"R_x", 101},
	{"R_x_conj", 118},
	{"R_y", 102},
	{"R_y_conj", 119},
	{"R_z", 103},
	{"R_z_conj", 120},
	{"SWAP", 104},
	{"Cnot", 105},
	{"Measure", 106},
	{"Measure_all", 126},
	{"Multycontrol_rotation", 108},
	{"UMultycontrol_rotation", 127},
	{"Arbit_transform", 128},
	{"Multy_X_aux", 109},
	{"P", 110},
	{"P_conj", 123},
	{"X", 111},
	{"Y", 112},
	{"Z", 113},
	{"H", 114},
	{"Toffoli", 115},
	{"T", 116},
	{"S", 117},
	{"S_conj", 122},
	{"T_conj", 121},
	{"Phase_conj", 125},
	{"Phase", 124}
	};

	/// <summary>
	/// current register size during compilation of quantum program
	/// </summary>
	static long parsed_register_size = 0;

	/// <summary>
	/// helper function for Hadamard decomposition
	/// </summary>
	/// <param name="qubit_num"> - qubit to execute</param>
	/// <param name="out"> - stream to be put in</param>
	void h_decomposition_helper(long qubit_num, std::ofstream& out) {
		out << "R_x(" << M_PI_2 << "," << qubit_num << ')' << '\n';
		out << "R_z(" << M_PI_2 << "," << qubit_num << ')' << '\n';
		out << "R_x(" << M_PI_2 << "," << qubit_num << ')' << '\n';
		out << "Phase(" << M_PI_2 << "," << qubit_num << ')' << '\n';
	}

	/// <summary>
	/// helper function for Toffoli decomposition
	/// </summary>
	/// <param name="_q1"> - 1 controlling qubit</param>
	/// <param name="_q2"> - 2 controlling qubit</param>
	/// <param name="_trgt"> - target qubit</param>
	/// <param name="out"> - stream to be put in</param>
	void toffoli_decomposition_helper(long _q1, long _q2, long _trgt, std::ofstream& out) {
		h_decomposition_helper(_trgt, out);
		out << "Cnot(" << _q2 << ',' << _trgt << ')' << '\n';
		out << "P(" << -M_PI_4 << "," << _trgt << ')' << '\n';
		out << "Cnot(" << _q1 << ',' << _trgt << ')' << '\n';
		out << "P(" << M_PI_4 << "," << _trgt << ')' << '\n';
		out << "Cnot(" << _q2 << ',' << _trgt << ')' << '\n';
		out << "P(" << -M_PI_4 << "," << _trgt << ')' << '\n';
		out << "Cnot(" << _q1 << ',' << _trgt << ')' << '\n';
		out << "P(" << -M_PI_4 << "," << _q2 << ')' << '\n';
		out << "P(" << M_PI_4 << "," << _trgt << ')' << '\n';
		out << "Cnot(" << _q1 << ',' << _q2 << ')' << '\n';
		h_decomposition_helper(_trgt, out);
		out << "P(" << -M_PI_4 << "," << _q2 << ')' << '\n';
		out << "Cnot(" << _q1 << ',' << _q2 << ')' << '\n';
		out << "P(" << M_PI_4 << "," << _q1 << ')' << '\n';
		out << "P(" << M_PI_2 << "," << _q2 << ')' << '\n';
	}

	/// <summary>
	/// function to read complex numbers
	/// </summary>
	/// <param name="str"> - complex value as string</param>
	/// <returns>Returns complex</returns>
	std::complex<double> read_complex(std::string& str) {
		std::stringstream ss(str);
		std::complex<double> complex;
		ss >> complex;
		return complex;
	}

	/// <summary>
	/// helper function for Multy_X_aux
	/// </summary>
	/// <param name="_c_qub_list"> - list of controlling qubits</param>
	/// <param name="start"> - index in the list where we start from</param>
	/// <param name="end"> - index in the list where we end</param>
	/// <param name="_aux"> - number of auxiliary qubit</param>
	/// <param name="_trgt"> - target qubit</param>
	/// <param name="out"> -  - stream to be put in</param>
	void multy_x_aux_decomposition_helper(std::list<long>& _c_qub_list, long start, long end, long _aux, long _trgt,
			std::ofstream& out) {

		if ((end-start+1) == 2) {
			std::list<long>::const_iterator it_start = _c_qub_list.cbegin();
			std::list<long>::const_iterator it_end = _c_qub_list.cbegin();
			std::advance(it_start, start);
			std::advance(it_end, end);
			toffoli_decomposition_helper(*it_start, *it_end, _trgt, out);
		}

		else if ((end - start + 1) == 1) {
			std::list<long>::const_iterator it_start = _c_qub_list.cbegin();
			std::advance(it_start, start);
			out << "Cnot(" << *it_start << ',' << _trgt << ')' << '\n';
		}
		else {
			long n = end - start + 1 + 2; // start end are indexes in list of control qubits
			long m1 = n / 2;
			long m2 = n - m1 - 1;
			long _s1 = start;
			long _n1 = start + m1 - 1;
			long _s2 = _n1 + 1; //index of the first control qubit from 2nd group
			long _n2 = end; // index of the last control qubit from 2nd group
			std::list<long>::const_iterator it = _c_qub_list.cbegin();
			std::advance(it, _n2);
			multy_x_aux_decomposition_helper(_c_qub_list, _s1, _n1, _trgt, _aux, out);
			_c_qub_list.insert(it, _aux);
			multy_x_aux_decomposition_helper(_c_qub_list, _s2, _n2, *(_c_qub_list.cbegin()), _trgt, out);
			it = _c_qub_list.cbegin();
			std::advance(it, _n2);
			_c_qub_list.erase(it);
			it = _c_qub_list.cbegin();
			std::advance(it, _n2);
			multy_x_aux_decomposition_helper(_c_qub_list, _s1, _n1, _trgt, _aux, out);
			_c_qub_list.insert(it, _aux);
			multy_x_aux_decomposition_helper(_c_qub_list, _s2, _n2, *(_c_qub_list.cbegin()), _trgt, out);
			it = _c_qub_list.cbegin();
			std::advance(it, _n2);
			_c_qub_list.erase(it);
		}
	}
	long get_bit_index(long number, long max_index) {
		long index = 0;
		while (number >= 0) {
			if (number % 2 == 1) {
				break;
			}
			number = number / 2;
			index+=1;
		}
		return (max_index - index);
	}
	/// <summary>
	/// helper function for UMultycontrol_rotation
	/// </summary>
	/// <param name="c_qub"> - list of controlling qubits</param>
	/// <param name="ang"> - list of angles for rotation</param>
	/// <param name="rotation"> - type of rotation</param>
	/// <param name="trg"> - target qubit</param>
	/// <param name="out"> - stream to be put in</param>
	void umultycontrol_rotation_decomposition_helper(const std::deque<long>& c_qub, const Eigen::VectorXd& ang,
		std::string rotation, const long trg, std::ofstream& out) {
		long gray_prev = 0, gray_cur = 0, control_index = 0;
		long c_index_max = 0, tmp = 0, j = 0, control_qubit = 0;
		c_index_max = log2(ang.rows()) - 1;

		for (long i = 1; i < ang.rows(); ++i) {
			gray_cur = i ^ (i >> 1);
			control_index = get_bit_index(gray_cur ^ gray_prev, c_index_max);
			if (control_index < 0 || control_index >= ang.rows()) {
				throw std::exception("COMPILE ERROR: Wrong size of deque with qubits to control");
			}
			out << rotation << '(' << ang[i - 1] << ',' << trg << ")\n";
			out << "Cnot(" << c_qub[control_index] << ',' << trg << ")\n";
			gray_prev = gray_cur;
		}
		control_index = get_bit_index(gray_cur, c_index_max);
		out << rotation << '(' << ang[c_index_max] << ',' << trg << ")\n";
		out << "Cnot(" << c_qub[control_index] << ',' << trg << ")\n";
	}
	/// <summary>
	/// helper function for UMultycontrol_rotation
	/// </summary>
	/// <param name="c_qub"> - list of controlling qubits</param>
	/// <param name="ang"> - list of angles for rotation</param>
	/// <param name="rotation"> - type of rotation</param>
	/// <param name="trg"> - target qubit</param>
	/// <param name="out"> - stream to be put in</param>
	void umultycontrol_rotation_decomposition_helper(const std::deque<long>& c_qub, const std::deque<double>& ang,
		std::string rotation, const long trg, std::ofstream& out) {
		long gray_prev = 0, gray_cur = 0, control_index = 0;
		long c_index_max = 0, tmp = 0, j = 0, control_qubit = 0;
		c_index_max = log2(ang.size()) - 1;

		for (long i = 1; i < ang.size(); ++i) {
			gray_cur = i ^ (i >> 1);
			control_index = get_bit_index(gray_cur ^ gray_prev, c_index_max);
			if (control_index < 0 || control_index >= ang.size()) {
				throw std::exception("COMPILE ERROR: Wrong size of deque with qubits to control");
			}
			out << rotation << '(' << ang[i - 1] << ',' << trg << ")\n";
			out << "Cnot(" << c_qub[control_index] << ',' << trg << ")\n";
			gray_prev = gray_cur;
		}
		control_index = get_bit_index(gray_cur, c_index_max);
		out << rotation << '(' << ang[c_index_max] << ',' << trg << ")\n";
		out << "Cnot(" << c_qub[control_index] << ',' << trg << ")\n";
	}
	/// <summary>
	/// helper function for UMultycontrol_rotation
	/// </summary>
	/// <param name="c_qub"> - list of controlling qubits</param>
	/// <param name="ang"> - list of angles for rotation</param>
	/// <param name="rotation"> - type of rotation</param>
	/// <param name="trg"> - target qubit</param>
	/// <param name="out"> - stream to be put in</param>
	void umultycontrol_rotation_decomposition_helper_wrong(const std::deque<long>& c_qub, const Eigen::VectorXd& ang,
		std::string rotation, const long trg, std::ofstream& out) {
		long gray_0 = 0, gray_1 = 0, c_index = 0;
		long c_index_max = 0, tmp = 0, j = 0, control_qubit = 0;
		c_index_max = c_qub.size() - 1;

		for (j = 1; j < ang.size(); ++j) {
			gray_1 = (j ^ (j >> 1));
			tmp = gray_0 ^ gray_1;
			c_index = 0;
			while (tmp > 0) {
				if (tmp % 2 == 1) {
					break;
				}
				tmp = tmp / 2;
				c_index++;
			}
			out << rotation << '(' << ang[j - 1] << ',' << trg << ")\n";
			out << "Cnot(" << c_qub[c_index_max - c_index] << ',' << trg << ")\n";
			gray_0 = gray_1;
		}
		tmp = gray_1;
		c_index = 0;
		while (tmp > 0) {
			if (tmp % 2 == 1) {
				break;
			}
			tmp = tmp / 2;
			c_index++;
		}
		out << rotation << '(' << ang[j - 1] << ',' << trg << ")\n";
		out << "Cnot(" << c_qub[c_index_max - c_index] << ',' << trg << ")\n";
	}
	/// <summary>
/// helper function for UMultycontrol_rotation for deque angles
/// </summary>
/// <param name="c_qub"> - list of controlling qubits</param>
/// <param name="ang"> - list of angles for rotation</param>
/// <param name="rotation"> - type of rotation</param>
/// <param name="trg"> - target qubit</param>
/// <param name="out"> - stream to be put in</param>
	void umultycontrol_rotation_decomposition_helper_wrong(const std::deque<long>& c_qub, const std::deque<double>& ang,
		std::string rotation, const long trg, std::ofstream& out) {
		long gray_0 = 0, gray_1 = 0, c_index = 0;
		long c_index_max = 0, tmp = 0, j = 0, control_qubit = 0;
		c_index_max = c_qub.size() - 1;

		for (j = 1; j < ang.size(); ++j) {
			gray_1 = (j ^ (j >> 1));
			tmp = gray_0 ^ gray_1;
			c_index = 0;
			while (tmp > 0) {
				if (tmp % 2 == 1) {
					break;
				}
				tmp = tmp / 2;
				c_index++;
			}
			out << rotation << '(' << ang[j - 1] << ',' << trg << ")\n";
			out << "Cnot(" << c_qub[c_index_max - c_index] << ',' << trg << ")\n";
			gray_0 = gray_1;
		}
		tmp = gray_1;
		c_index = 0;
		while (tmp > 0) {
			if (tmp % 2 == 1) {
				break;
			}
			tmp = tmp / 2;
			c_index++;
		}
		out << rotation << '(' << ang[j - 1] << ',' << trg << ")\n";
		out << "Cnot(" << c_qub[c_index_max - c_index] << ',' << trg << ")\n";
	}

	/// <summary>
	/// Makes matrix to be used to get the final angles of rotation
	/// </summary>
	/// <param name="M_k"> - Complex matrix</param>
	/// <param name="size"> - Matrix size</param>
	void make_M_k_matrix(Eigen::MatrixXcd& M_k, long size) {
		long gray = 0;
		for (long i = 0; i < size; ++i) {
			for (long j = 0; j < size; ++j) {
				gray = (j ^ (j >> 1));
				M_k(i, j) = std::pow(-1, i & gray);
			}
		}
	}

	/// <summary>
	/// cosine-sine decomposition; 
	/// it's up to the theorem that you can decompose a unitary into U=RDL*; 
	/// R = R_0 0 0 R_1; L = L_0 0 0 L_1; D = C -S S C; 
	/// C^2 + S^2 = 1, (R,L)_i, D are also Unitary; 
	/// S is sufficient in parameteres as the angles in �,S are supposed to be the same
	/// </summary>
	/// <param name="U"> - Initial matrix</param>
	/// <param name="L0"> - top left corner L matrix</param>
	/// <param name="L1"> - bottom  right coremer L matrix</param>
	/// <param name="R0"> - top left corner R matrix</param>
	/// <param name="R1"> - bottom  right coremer R matrix</param>
	/// <param name="S"> - S matrix</param>
	/// <returns></returns>
	bool cosine_sine_decomposition_helper(const Eigen::Ref<const Eigen::MatrixXcd>& U, Eigen::Ref<Eigen::MatrixXcd> L0, Eigen::Ref<Eigen::MatrixXcd> L1, 
		Eigen::Ref<Eigen::MatrixXcd> R0, Eigen::Ref<Eigen::MatrixXcd> R1, Eigen::Ref<Eigen::MatrixXcd> S) {

		long size = U.rows();
		Eigen::BDCSVD<Eigen::MatrixXcd> svd(size / 2, size / 2);
		svd.compute(U.topLeftCorner(size / 2, size / 2), Eigen::ComputeThinU | Eigen::ComputeThinV);
		long new_size = size / 2;
		Eigen::MatrixXcd c(svd.singularValues().reverse().asDiagonal());
		L0.noalias() = svd.matrixU().rowwise().reverse();
		R0.noalias() = svd.matrixV().rowwise().reverse();

		Eigen::MatrixXcd q2 = U.bottomLeftCorner(new_size, new_size) * R0;
		long index = 0;
		for (long j = 1; j < new_size; j++)
		{
			if (c(j, j).real() <= 1. / sqrt(2)) {
				index = j;
			}
		}
		Eigen::HouseholderQR<Eigen::MatrixXcd> find_unitary(new_size, index + 1);
		find_unitary.compute(q2.block(0, 0, new_size, index + 1));
		L1 = find_unitary.householderQ();
		S.noalias() = L1.adjoint() * q2;
		if (index < new_size - 1)
		{
			index += 1;
			Eigen::BDCSVD<Eigen::MatrixXcd> svd2(new_size - index, new_size - index);
			svd2.compute(S.block(index, index, new_size - index, new_size - index), Eigen::ComputeThinU | Eigen::
				ComputeThinV);
			S.block(index, index, new_size - index, new_size - index) = svd2.singularValues().asDiagonal();
			c.block(0, index, new_size, new_size - index) = c.block(0, index, new_size, new_size - index) * svd2.matrixV();
			L1.block(0, index, new_size, new_size - index) = L1.block(0, index, new_size, new_size - index) * svd2.matrixU();
			R0.block(0, index, new_size, new_size - index) = R0.block(0, index, new_size, new_size - index) * svd2.matrixV();

			Eigen::HouseholderQR<Eigen::MatrixXcd> QR_dec(new_size - index, new_size - index);

			QR_dec.compute(c.block(index, index, new_size - index, new_size - index));
			c.block(index, index, new_size - index, new_size - index) = QR_dec.matrixQR().triangularView<Eigen::Upper>();
			L0.block(0, index, new_size, new_size - index) = L0.block(0, index, new_size, new_size - index) * QR_dec.householderQ();
		}

		std::vector<int> c_ind;
		std::vector<int> s_ind;
		for (long j = 0; j < new_size; j++) {
			if (c(j, j).real() < 0) {
				c_ind.push_back(j);
			}
			if (S(j, j).real() < 0) {
				s_ind.push_back(j);
			}
		}

		c(c_ind, c_ind) = -c(c_ind, c_ind);
		L0(Eigen::all, c_ind) = -L0(Eigen::all, c_ind);
		S(s_ind, s_ind) = -S(s_ind, s_ind);
		L1(Eigen::all, s_ind) = -L1(Eigen::all, s_ind);
		Eigen::MatrixXcd checker1 = L0 * c * R0.adjoint();
		Eigen::MatrixXcd checker2 = L1 * S * R0.adjoint();
		if (!U.topLeftCorner(new_size, new_size).isApprox(checker1, 10e-8)) {
			std::cout << "COMPILE ERROR:: False U11 to be represented!\n";
			std::cout << "U11: \n" << U.topLeftCorner(new_size, new_size);
			std::cout << "U11_new: \n" << checker1;
			return false;
		}
		else if (!U.bottomLeftCorner(new_size, new_size).isApprox(checker2, 10e-8)) {
			std::cout << "COMPILE ERROR:: False U21 to be represented!\n";
			std::cout << "U21: \n" << U.topLeftCorner(new_size, new_size);
			std::cout << "U21_new: \n" << checker2;
			return false;
		}
		R0.adjointInPlace(); S = -S;
		Eigen::MatrixXcd tmp_s = L0.adjoint() * U.topRightCorner(new_size, new_size);
		Eigen::MatrixXcd tmp_c = L1.adjoint() * U.bottomRightCorner(new_size, new_size);
		for (long i = 0; i < new_size; i++)
		{
			if (std::abs(S(i, i)) > std::abs(c(i, i)))
			{
				R1.row(i).noalias() = tmp_s.row(i) / S(i, i);
			}
			else
			{
				R1.row(i).noalias() = tmp_c.row(i) / c(i, i);
			}
		}
		/*Eigen::MatrixXcd tmp(size, size);
		tmp.topLeftCorner(new_size, new_size) = L0 * c * R0;
		tmp.bottomLeftCorner(new_size, new_size) = -L1 * S * R0;
		tmp.topRightCorner(new_size, new_size) = L0 * S * R1;
		tmp.bottomRightCorner(new_size, new_size) = L1 * c * R1;
		if (!tmp.isApprox(U, 10e-2))
		{
			std::cout << "COMPILE ERROR::Wrong CSD!\n";
			return false;
		}*/
		return true;
	}
	
	/// <summary>
	/// Demultiplexing arbitrary helper function: G = V R_z W; 
	/// </summary>
	/// <param name="G"> - Initial matrix</param>
	/// <param name="V"> - one of matrices in decomposition</param>
	/// <param name="W"> - one of matrices in decomposition</param>
	/// <param name="R_z_matrix"> - one of matrices in decomposition corresponding to R_z multiplexor</param>
	/// <returns></returns>
	bool UMulticontroled_arbitrary_decomposition_helper(const Eigen::Ref<const Eigen::MatrixXcd>& G,
		Eigen::Ref<Eigen::MatrixXcd> V, Eigen::Ref<Eigen::MatrixXcd> W, Eigen::Ref<Eigen::MatrixXcd> R_z_matrix) {
		long new_size = G.cols() >> 1;
		Eigen::MatrixXcd G0_G1adj = G.topLeftCorner(new_size, new_size) * G.bottomRightCorner(new_size, new_size).adjoint();
		Eigen::VectorXcd tmp_sqrt(new_size);
		Eigen::MatrixXcd D_sqrt(new_size, new_size);
		D_sqrt.setZero(new_size, new_size);
		//if (G0_G1adj == G0_G1adj.adjoint()) {
		//	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXcd> eigensolver(G0_G1adj);
		//	if (eigensolver.info() != Eigen::Success) {
		//		std::cout << "UMulticontroled_arbitrary_decomposition_helper::COMPILE ERROR: Failed to make eigen decomposition\n";
		//		std::cout << "-------------------------------------------------------------------------------------------------\n";
		//		return false;
		//	}
		//	V = eigensolver.eigenvectors();
		//	tmp_sqrt = eigensolver.eigenvalues().array().sqrt(); //!!!
		//}
		Eigen::ComplexEigenSolver<Eigen::MatrixXcd> cmplx_eigensolver(G0_G1adj);
		if (cmplx_eigensolver.info() != Eigen::Success) {
			std::cout << "UMulticontroled_arbitrary_decomposition_helper::COMPILE ERROR: Failed to make eigen decomposition\n";
			std::cout << "-------------------------------------------------------------------------------------------------\n";
			return false;
		}
		V = cmplx_eigensolver.eigenvectors();
		tmp_sqrt = cmplx_eigensolver.eigenvalues().array().sqrt();
		for (long j = 0; j < new_size; ++j) {
			D_sqrt(j, j) = tmp_sqrt(j);
		}
		R_z_matrix.topLeftCorner(new_size, new_size) = D_sqrt;
		R_z_matrix.bottomRightCorner(new_size, new_size) = D_sqrt.adjoint();
		W = D_sqrt * V.adjoint() * G.bottomRightCorner(new_size, new_size);

		/*Eigen::MatrixXcd checker(G.cols(), G.cols());
		checker.setZero(G.cols(), G.cols());
		checker.topLeftCorner(new_size, new_size) = V * R_z_matrix.topLeftCorner(new_size, new_size) * W;
		checker.bottomRightCorner(new_size, new_size) = V * R_z_matrix.bottomRightCorner(new_size, new_size) * W;
		if (!checker.isApprox(G, 10e-2))
		{
			std::cout << "COMPILE ERROR::Wrong demultiplexing!\n";
			return false;
		}*/
		return true;
	}

	/// <summary>
	/// function to get angles to be substituted in Uniformly multicontrolled rotation
	/// </summary>
	/// <param name="D"> - diagonal matrix for angles to be made from</param>
	/// <param name="M_k"> - matrix to select correct signes before the angles</param>
	/// <returns></returns>
	Eigen::VectorXd make_real_angles_R_z(const Eigen::Ref<const Eigen::MatrixXcd>& D, const Eigen::Ref<const Eigen::MatrixXcd>& M_k) {
		std::complex<double> i(0, 1);
		Eigen::VectorXcd alpha = D.diagonal();
		alpha = alpha.array().log();
		alpha = alpha.array()* i* (-2);
		Eigen::VectorXcd theta = M_k.colPivHouseholderQr().solve(alpha);
		return theta.array().real();
	}

	/// <summary>
	/// function to get angles to be substituted in Uniformly multicontrolled rotation
	/// </summary>
	/// <param name="D"> - diagonal matrix for angles to be made from</param>
	/// <param name="M_k"> - matrix to select correct signes before the angles</param>
	/// <returns></returns>
	Eigen::VectorXd make_real_angles_R_y(const Eigen::Ref<const Eigen::MatrixXcd>& S, const Eigen::Ref<const Eigen::MatrixXcd>& M_k) {
		Eigen::VectorXcd alpha = S.diagonal();
		alpha = alpha.array().asin();
		alpha = alpha.array()*2;
		Eigen::VectorXcd theta = M_k.colPivHouseholderQr().solve(alpha);
		return theta.array().real();
	}
	/// <summary>
	/// Z-Y-Z decomposition
	/// </summary>
	/// <param name="matrix"> - initial one-qubit matrix</param>
	/// <param name="qubit_num"> - target qubit</param>
	/// <param name="out"> - stream to be put in</param>
	void one_qubit_arbitrary_decomposition_helper(const Eigen::Ref<const Eigen::MatrixXcd>& matrix, 
		const long qubit_num, std::ofstream& out) {
		std::complex<double>  det = matrix.determinant();
		double delta = atan2(det.imag(), det.real()) / matrix.rows();
		std::complex<double> A = exp(std::complex<double>(0, -1) * delta) * matrix(0, 0);
		std::complex<double> B = exp(std::complex<double>(0, -1) * delta) * matrix(0, 1); 
		double s = sqrt(pow((double)B.imag(), 2) + pow((double)B.real(), 2) +
			pow((double)A.imag(), 2));
		double x = 0, y = 0, z = 0;

		if (s > 0)		{
			x = B.imag() / s;
			y = B.real() / s;
			z = A.imag() / s;
		}
		double t1 = atan2(A.imag(), A.real()), t2 = atan2(B.imag(), B.real());
		double alpha = t1 + t2, gamma = t1 - t2;
		double beta = 2 * atan2(s * sqrt(pow((double)x, 2) + pow((double)y, 2)), sqrt(pow
		((double)A.real(), 2) + pow((z * s), 2)));
		out << "R_z(" << -gamma << "," << qubit_num << ")\n";
		out << "R_y(" << -beta << "," << qubit_num << ")\n";
		out << "R_z(" << -alpha << "," << qubit_num << ")\n";
	}
	
	/// <summary>
	/// Quantum Shannon decomposition; 
	/// Decomposes arbitrary complex transformation into basic operators
	/// </summary>
	/// <param name="_c_qub_list"> - list for storing controling qubits</param>
	/// <param name="_angles"> - list for storing angles of rotation</param>
	/// <param name="U"> - initial matrix to be decomposed</param>
	/// <param name="out"> - stream to be put in</param>
	void Shannon_decomposition_helper(std::deque<long>& _c_qub_list, std::deque<double>& _angles_z, std::deque<double>& _angles_y,
		Eigen::MatrixXcd& U, std::ofstream& out) {
		if (U.cols() == 2) {
			one_qubit_arbitrary_decomposition_helper(U, _c_qub_list.front(), out);
		}
		else {
			long new_size, q1;
			q1 = _c_qub_list.front();
			_c_qub_list.pop_front();
			new_size = U.cols() >> 1;
			Eigen::MatrixXcd G1(new_size, new_size), G2(new_size, new_size),
				L(U.cols(), U.cols()), R(U.cols(), U.cols()), D(U.cols(), U.cols()),
				M_k(new_size, new_size), S(new_size, new_size), G3(new_size, new_size),
				G4(new_size, new_size), D2(U.cols(), U.cols());
			D2.setZero(U.cols(), U.cols());
			G3.setZero(new_size, new_size);
			G4.setZero(new_size, new_size);
			L.setZero(U.cols(), U.cols());
			R.setZero(U.cols(), U.cols());
			D.setZero(U.cols(), U.cols());
			S.setZero(new_size, new_size);
			G1.setZero(new_size, new_size);
			G2.setZero(new_size, new_size);
			M_k.setZero(new_size, new_size);

			/*count G1 G2*/
			cosine_sine_decomposition_helper(U, L.topLeftCorner(new_size,new_size), L.bottomRightCorner(new_size,new_size),
											 R.topLeftCorner(new_size, new_size), R.bottomRightCorner(new_size, new_size), S);
			UMulticontroled_arbitrary_decomposition_helper(L, G2, G1, D);
			UMulticontroled_arbitrary_decomposition_helper(R, G4, G3, D2);
			make_M_k_matrix(M_k, new_size);
			Eigen::VectorXd theta_z_l = make_real_angles_R_z(D.topLeftCorner(new_size, new_size), M_k),
							theta_z_r = make_real_angles_R_z(D2.topLeftCorner(new_size, new_size), M_k),
							theta_y = make_real_angles_R_y(S, M_k);


			Shannon_decomposition_helper(_c_qub_list, _angles_z, _angles_y, G1, out);
			_c_qub_list.push_front(q1);
			umultycontrol_rotation_decomposition_helper(_c_qub_list, theta_z_l, "R_z", q1, out);
			Shannon_decomposition_helper(_c_qub_list, _angles_z, _angles_y, G2, out);
			_c_qub_list.push_front(q1);
			umultycontrol_rotation_decomposition_helper(_c_qub_list, theta_y, "R_y", q1,  out);
			Shannon_decomposition_helper(_c_qub_list, _angles_z, _angles_y, G3, out);
			_c_qub_list.push_front(q1);
			umultycontrol_rotation_decomposition_helper(_c_qub_list, theta_z_r, "R_z", q1, out);
			Shannon_decomposition_helper(_c_qub_list, _angles_z, _angles_y, G4, out);
			_c_qub_list.push_front(q1);
		}
	}

	/// <summary>
	/// Checker of start state for automata representing grammar of simulator; 
	/// the analising subautomatas don't recognize every opp-ty as our input is strict enough
	/// except the order of the logic
	/// </summary>
	/// <param name="s"> - input string of operations</param>
	/// <param name="out"> - stream to be put in</param>
	/// <returns>Returns bool if it's correct input instruction or not</returns>
	bool is_start(std::string& s, std::ofstream& out) {
		bool answer = false;
		std::string instruction, parameteres;
		std::stringstream ss(s);
		std::getline(ss, instruction, '(');
		/*if no element .at throws an exception*/
		if (operators_list.at(instruction) == 100) {
			std::getline(ss, parameteres, ')');
			parsed_register_size = std::stol(parameteres);
			out << instruction << '(' << parameteres << ')' << '\n';
			answer = true;
		}
		else {
			std::cout << "SYNTAX ERROR: Initialisation of the register is false. Wrong operator \n ";
			printf("---------------------------------------------------------------------\n\n");
		}

		return answer;
	}

	/// <summary>
	/// Checks whether the current command is in operation list; 
	/// if it's so the state makes the decomposition of analysed instruction
	/// </summary>
	/// <param name="s"> - input instructions</param>
	/// <param name="out"> - stream to be put in</param>
	/// <returns>Returns bool if it's correct input instruction or not</returns>
	bool is_operation_list(std::string& s, std::ofstream& out) {
		bool answer = false;
		std::string instruction, parameteres, tmp, rotation_type;
		std::string tempor;
		double angle = 0., new_angle = 0.;
		std::list<long> c_qub_list;
		std::vector<double> angles;
		std::deque<double> deq_angles;
		std::deque<double> deq_angles_z;
		std::deque<double> deq_angles_y;
		std::deque<long> deq_qubits;
		std::vector<long> c_qub_list2;
		Eigen::MatrixXcd matrix(10,10);
		long rows = 0, cols = 0;
		std::list<long>::const_iterator it;
		long aux = 0, trgt = 0, q_1 = 0, q_2 = 0, q_3 = 0, k1 = 0, k2 = 0;
		std::stringstream ss(s), ss1("");
		std::getline(ss, instruction, '(');
		std::getline(ss, parameteres);
		int command_code = 0;
		command_code = operators_list.at(instruction);
		switch (command_code) {

		case _Init_reg:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _Cnot:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _R_x:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _R_x_conj:
			ss.clear();
			ss.str(parameteres);
			std::getline(ss, tmp, ',');
			new_angle = -std::stod(tmp);
			std::getline(ss, tmp);
			out << "R_x(" << new_angle << tmp << '\n';
			answer = true;
			break;
		case _R_y:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _R_y_conj:
			ss.clear();
			ss.str(parameteres);
			std::getline(ss, tmp, ',');
			new_angle = -std::stod(tmp);
			std::getline(ss, tmp);
			out << "R_y(" << new_angle << tmp << '\n';
			answer = true;
			break;
		case _R_z:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _R_z_conj:
			ss.clear();
			ss.str(parameteres);
			std::getline(ss, tmp, ',');
			new_angle = -std::stod(tmp);
			std::getline(ss, tmp);
			out << "R_z(" << new_angle << tmp << '\n';
			answer = true;
			break;
		case _T:
			out << "P(" << M_PI_4 << "," << parameteres << '\n';
			answer = true;
			break;
		case _T_conj:
			out << "P(" << -M_PI_4 << "," << parameteres << '\n';
			answer = true;
			break;
		case _S:
			out << "P(" << M_PI_2 << "," << parameteres << '\n';
			answer = true;
			break;
		case _S_conj:
			out << "P(" << -M_PI_2 << "," << parameteres << '\n';
			answer = true;
			break;
		case _Z:
			out << "P(" << M_PI << "," << parameteres << '\n';
			answer = true;
			break;
		case _Y:
			out << "R_y(" << M_PI << "," << parameteres << '\n';
			out << "Phase(" << M_PI_2 << "," << parameteres << '\n';
			answer = true;
			break;
		case _X:
			out << "R_x(" << M_PI << "," << parameteres << '\n';
			out << "Phase(" << M_PI_2 << "," << parameteres << '\n';
			answer = true;
			break;
		case _H:
			ss.clear();
			ss.str(parameteres);
			std::getline(ss, tmp, ')');
			q_3 = std::stol(tmp);
			h_decomposition_helper(q_3, out);
			answer = true;
			break;
		case _Toffoli:
			ss.clear();
			ss.str(parameteres);
			std::getline(ss, tmp, ',');
			q_1 = std::stol(tmp);
			std::getline(ss, tmp, ',');
			q_2 = std::stol(tmp);
			std::getline(ss, tmp, ')');
			q_3 = std::stol(tmp);
			toffoli_decomposition_helper(q_1, q_2, q_3, out);
			answer = true;
			break;
		case _Multy_X_aux:
			ss.clear();
			ss.str(parameteres);
			std::getline(ss, instruction, ';');
			ss1.clear();
			ss1.str(instruction + ',');
			while (std::getline(ss1, tmp, ',')) {
				c_qub_list.push_back(std::stol(tmp));
			}
			std::getline(ss, tmp, ';');
			aux = std::stol(tmp);
			std::getline(ss, tmp, ')');
			trgt = std::stol(tmp);
			multy_x_aux_decomposition_helper(c_qub_list, 0, c_qub_list.size() - 1, aux, trgt, out);
			c_qub_list.clear();
			answer = true;
			break;
		case _SWAP:
			ss.clear();
			ss.str(parameteres);
			std::getline(ss, tmp, ',');
			q_1 = std::stol(tmp);
			std::getline(ss, tmp, ')');
			q_2 = std::stol(tmp);
			out << "Cnot(" << q_1 << ',' << q_2 << ')' << '\n';
			out << "Cnot(" << q_2 << ',' << q_1 << ')' << '\n';
			out << "Cnot(" << q_1 << ',' << q_2 << ')' << '\n';
			answer = true;
			break;
		case _P:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _P_conj:
			ss.clear();
			ss.str(parameteres);
			std::getline(ss, tmp, ',');
			new_angle = -std::stod(tmp);
			std::getline(ss, tmp);
			out << "P(" << new_angle << tmp << '\n';
			answer = true;
			break;
		case _Phase:
			out << instruction << '(' << parameteres << '\n';
			answer = true;
			break;
		case _Phase_conj:
			ss.clear();
			ss.str(parameteres);
			std::getline(ss, tmp, ',');
			new_angle = -std::stod(tmp);
			std::getline(ss, tmp);
			out << "Phase(" << new_angle << tmp << '\n';
			answer = true;
			break;
		case _Multycontrol_rotation:
			ss.clear();
			ss.str(parameteres);
			std::getline(ss, instruction, ';');
			ss1.clear();
			ss1.str(instruction + ',');
			while (std::getline(ss1, tmp, ',')) {
				c_qub_list.push_back(std::stol(tmp));
			}
			std::getline(ss, tmp, ';');
			rotation_type = tmp;
			std::getline(ss, tmp, ';');
			angle = std::stod(tmp);
			std::getline(ss, tmp, ')');
			trgt = std::stol(tmp);
			if (!operators_list.count(rotation_type)) {
				std::cout << "SYNTAX ERROR: Can't find such operator \n";
				printf("--------------------------------------\n\n");
			}
			else if (!(rotation_type == "R_x" || rotation_type == "R_y" || rotation_type == "R_z")) {
				std::cout << "SYNTAX ERROR: Rotation operator needed \n";
				printf("--------------------------------------\n\n");
			}
			else {
				command_code = operators_list.at(rotation_type);
				k1 = c_qub_list.size() / 2;
				k2 = c_qub_list.size() - k1;
				switch (command_code) {
				case _R_x:
					it = c_qub_list.cbegin();
					std::advance(it, k1 + k2 - 1);
					h_decomposition_helper(trgt, out);
					multy_x_aux_decomposition_helper(c_qub_list, 0, k1 - 1, *it, trgt, out);
					out << "R_z(" << angle / 4 << ',' << trgt << ")\n";
					multy_x_aux_decomposition_helper(c_qub_list, k1, k1 + k2 - 1, *c_qub_list.cbegin(), trgt, out);
					out << "R_z(" << -angle / 4 << ',' << trgt << ")\n";
					multy_x_aux_decomposition_helper(c_qub_list, 0, k1 - 1, *it, trgt, out);
					out << "R_z(" << angle / 4 << ',' << trgt << ")\n";
					multy_x_aux_decomposition_helper(c_qub_list, k1, k1 + k2 - 1, *c_qub_list.cbegin(), trgt, out);
					out << "R_z(" << -angle / 4 << ',' << trgt << ")\n";
					h_decomposition_helper(trgt, out);
					break;
				case _R_y:
					it = c_qub_list.cbegin();
					std::advance(it, k1 + k2 - 1);
					multy_x_aux_decomposition_helper(c_qub_list, 0, k1 - 1, *it, trgt, out);
					out << "R_y(" << -angle / 4 << ',' << trgt << ")\n";
					multy_x_aux_decomposition_helper(c_qub_list, k1, k1 + k2 - 1, *c_qub_list.cbegin(), trgt, out);
					out << "R_y(" << angle / 4 << ',' << trgt << ")\n";
					multy_x_aux_decomposition_helper(c_qub_list, 0, k1 - 1, *it, trgt, out);
					out << "R_y(" << -angle / 4 << ',' << trgt << ")\n";
					multy_x_aux_decomposition_helper(c_qub_list, k1, k1 + k2 - 1, *c_qub_list.cbegin(), trgt, out);
					out << "R_y(" << angle / 4 << ',' << trgt << ")\n";
					break;
				case _R_z:
					it = c_qub_list.cbegin();
					std::advance(it, k1 + k2 - 1);
					multy_x_aux_decomposition_helper(c_qub_list, 0, k1 - 1, *it, trgt, out);
					out << "R_z(" << -angle / 4 << ',' << trgt << ")\n";
					multy_x_aux_decomposition_helper(c_qub_list, k1, k1 + k2 - 1, *c_qub_list.cbegin(), trgt, out);
					out << "R_z(" << angle / 4 << ',' << trgt << ")\n";
					multy_x_aux_decomposition_helper(c_qub_list, 0, k1 - 1, *it, trgt, out);
					out << "R_z(" << -angle / 4 << ',' << trgt << ")\n";
					multy_x_aux_decomposition_helper(c_qub_list, k1, k1 + k2 - 1, *c_qub_list.cbegin(), trgt, out);
					out << "R_z(" << angle / 4 << ',' << trgt << ")\n";
					break;
				}
				answer = true;
			}
			break;
		case _UMultycontrol_rotation:
			ss.clear();
			ss.str(parameteres);
			std::getline(ss, instruction, ';');
			ss1.clear();
			ss1.str(instruction + ',');
			while (std::getline(ss1, tmp, ',')) {
				deq_qubits.push_back(std::stol(tmp));
			}
			std::getline(ss, tmp, ';');
			rotation_type = tmp;
			std::getline(ss, instruction, ';');
			ss1.clear();
			ss1.str(instruction + ',');
			while (std::getline(ss1, tmp, ',')) {
				deq_angles.push_back(std::stod(tmp));
			}
			std::getline(ss, tmp, ')');
			trgt = std::stol(tmp);
			if (!operators_list.count(rotation_type)) {
				std::cout << "SYNTAX ERROR: Can't find such operator \n";
				printf("--------------------------------------\n\n");
			}
			else if (!(rotation_type == "R_x" || rotation_type == "R_y" || rotation_type == "R_z")) {
				std::cout << "SYNTAX ERROR: Rotation operator needed \n";
				printf("--------------------------------------\n\n");
			}
			else {
				umultycontrol_rotation_decomposition_helper(deq_qubits, deq_angles, rotation_type, trgt, out);
				answer = true;
			}
			break;
		case _Arbit_transform:
			ss.clear();
			ss.str(parameteres + ';'); 
			std::getline(ss, tmp, ';');// read matr size
			long space_size;
			long size;
			space_size = std::stol(tmp);
			matrix.resize(space_size, space_size);
			ss1.clear();
			
			while (std::getline(ss, instruction, ';')) {
				cols = 0;
				ss1.str(instruction + ' ');
				while (ss1 >> tempor) {
					matrix(rows, cols) = read_complex(tempor);
					cols++;
				}
				ss1.clear();
				rows++;
			}
			size = log2(space_size);
			for (long i = 1; i <= size; ++i) {
				deq_qubits.push_back(i);
			}
			Shannon_decomposition_helper(deq_qubits, deq_angles_z, deq_angles_y, matrix, out);
			answer = true;
			break;
		case _Measure:
			if (parsed_register_size > 0) {
				out << "Measure(" << parameteres  << '\n';
				parsed_register_size--;
				answer = true;
			}
			else {
				std::cout << "SYNTAX ERROR: Extra Measure operation. Register has already been measured \n";
				printf("-------------------------------------------------------------------------\n\n");
			}
			break;
		default:
			std::cout << "SYNTAX ERROR: Can't find such operator \n";
			printf("--------------------------------------\n\n");
			break;
		}
		c_qub_list.clear();
		return answer;
	}

	/// <summary>
	/// Checks whether the input is correct end_state
	/// </summary>
	/// <param name="s"> - input instruction</param>
	/// <param name="out"> - output stream</param>
	/// <param name="command_que_left_elems"> - number of elements in the instructions queue left</param>
	/// <returns>Returns bool whether the input is correct or not</returns>
	bool is_end(std::string& s, std::ofstream& out, long command_que_left_elems) {
		bool answer = false;
		std::string instruction, parameteres;
		std::stringstream ss(s);
		std::getline(ss, instruction, '(');
		std::getline(ss, parameteres);
		if (operators_list.at(instruction) == 126) {
			out << instruction << '(' << parameteres << '\n';
			answer = true;
		}
		else if (operators_list.at(instruction) == 106 && parsed_register_size == 1) {
			out << instruction << '(' << parameteres << '\n';
			answer = true;
		}
		else if (operators_list.at(instruction) == 106 && command_que_left_elems == 1) {
			out << instruction << '(' << parameteres << '\n';
			answer = true;
		}
		return answer;
	}

	/// <summary>
	/// Represents the higher abstract grammar layer;
	/// Checks whether the input instructions matches the main quntum operator structure; 
	/// </summary>
	/// <param name="in"> - queue of input instructions</param>
	/// <returns>Returns bool answer</returns>
	bool Analyser(std::queue< std::string>& in) {
		bool answer = false;
		std::string input_instruction, temp;
		int grammar_state = _INITIAL_GRAMMAR_STATE;
		int error_code = 0;
		std::ofstream out;
		out.open("Debugging_test_file.txt", std::ios::out | std::ios::app);
		while (!in.empty()) {
			switch (grammar_state)
			{
			case _INITIAL_GRAMMAR_STATE:
				input_instruction = in.front();
				if (is_start(input_instruction, out)) {
					grammar_state = _START_STATE;
					in.pop();
				}
				else {
					grammar_state = _ERROR_STATE;
					error_code = 1;
				}

				break;
			case _START_STATE:
				input_instruction = in.front();
				if (is_operation_list(input_instruction, out)) {
					grammar_state = _READ_OPERATION_LIST_STATE;
					in.pop();
				}
				else {
					grammar_state = _ERROR_STATE;
					error_code = 2;
				}
				break;
			case _READ_OPERATION_LIST_STATE:
				input_instruction = in.front();
				if (is_end(input_instruction, out, in.size())) {
					grammar_state = _END_STATE;
					in.pop();
				}
				else if (is_operation_list(input_instruction, out)) {
					grammar_state = _READ_OPERATION_LIST_STATE;
					in.pop();
				}
				else {
					grammar_state = _ERROR_STATE;
					error_code = 3;
				}
				break;
			case _ERROR_STATE:
				switch (error_code)
				{
				case 1:

					printf("SYNTAX_ERROR: Wrong initialisation of register: %s \n", in.front().c_str());
					printf("----------------------------------------------\n\n");
					break;
				case 2:
					printf("SYNTAX_ERROR: Error occured in reading the list of operations: %s \n", in.front().c_str());
					printf("-------------------------------------------------------------\n\n");
					break;
				case 3:
					printf("SYNTAX_ERROR: Unexpected end of quantum instruction: %s \n", in.front().c_str());
					printf("---------------------------------------------------\n\n");
					break;
				case 4:
					printf("SYNTAX_ERROR: Unexpected end of quantum instruction. Some instructions found after the measurement\n");
					printf("The unexpected end: \n");
					while (!in.empty()) {
						std::cout << in.front() << std::endl;
						in.pop();
					}
					printf("---------------------------------------------------\n\n");
					break;
				default:
					printf("SYNTAX_ERROR: Unexpected instruction\n");
					printf("---------------------------------------\n\n");
					break;
				}
				/*clear the queue*/
				while (!in.empty()) {
					in.pop();
				}
				break;
				/*can get here only if we've read the end of input instruction but the queue is still not empty*/
			case _END_STATE:
				grammar_state = _ERROR_STATE;
				error_code = 4;

			default:
				std::cout << "SYNTAX_ERROR: Unhandled input instruction." << std::endl;
				printf("----------------------------\n\n");
				break;
			}
		}
		if (grammar_state == _END_STATE) answer = true;
		else if (grammar_state != _END_STATE && grammar_state != _ERROR_STATE) {
			switch (grammar_state) {
			case _INITIAL_GRAMMAR_STATE:
				printf("SYNTAX_ERROR: Empty instruction\n");
				printf("----------------------------------------------\n\n");
				break;
			case _START_STATE:
				printf("SYNTAX_ERROR: Operation list is supposed to be after an initialization of register\n");
				printf("----------------------------------------------\n\n");
				break;
			case _READ_OPERATION_LIST_STATE:
				printf("SYNTAX_ERROR: Unexpected ending of quantum instruction\n");
				printf("---------------------------------------------------\n\n");
				break;

			}
		}
		out.close();
		return answer;
	}
}



