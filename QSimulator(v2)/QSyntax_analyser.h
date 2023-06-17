#pragma once
#include <fstream>
#include <string>
#include "macro.h"
#include <queue>
#include <unordered_map>
#include <math.h>
#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>

namespace Work_namespace {
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
	static long parsed_register_size = 0;
	void h_decomposition_helper(long qubit_num, std::ofstream& out) {
		out << "R_x(" << M_PI_2 << "," << qubit_num << ')' << '\n';
		out << "R_z(" << M_PI_2 << "," << qubit_num << ')' << '\n';
		out << "R_x(" << M_PI_2 << "," << qubit_num << ')' << '\n';
		out << "Phase(" << M_PI_2 << "," << qubit_num << ')' << '\n';
	}
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
	std::complex<double> read_complex(std::string& str) {
		std::stringstream ss(str);
		std::complex<double> complex;
		ss >> complex;
		return complex;
	}
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
	void umultycontrol_rotation_decomposition_helper(std::deque<long>& c_qub, std::deque<double>& ang,
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
	void make_M_k_matrix(Eigen::Ref<Eigen::MatrixXcd> M_k, long size) {
		long gray_0 = 0, gray_1 = 0;
		long j = 0;
		for (j = 0; j < size; ++j) {
			gray_1 = (j ^ (j >> 1));
			for (long i = 0; i < size; ++i) {
				M_k(i, j) = std::pow(-1, i ^ gray_1);
			}
		}
	}
	bool cosine_sine_decomposition_helper(const Eigen::Ref<const Eigen::MatrixXcd>& U, Eigen::Ref<Eigen::MatrixXcd> L0, Eigen::Ref<Eigen::MatrixXcd> L1, Eigen::Ref<
		Eigen::MatrixXcd> R0, Eigen::Ref<Eigen::MatrixXcd> R1, Eigen::Ref<Eigen::MatrixXcd> S) {

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
		Eigen::MatrixXcd tmp(size, size);
		tmp.topLeftCorner(new_size, new_size) = L0 * c * R0;
		tmp.bottomLeftCorner(new_size, new_size) = -L1 * S * R0;
		tmp.topRightCorner(new_size, new_size) = L0 * S * R1;
		tmp.bottomRightCorner(new_size, new_size) = L1 * c * R1;
		if (!tmp.isApprox(U, 10e-2))
		{
			std::cout << "COMPILE ERROR::Wrong CSD!\n";
			return false;
		}
		return true;
	}
	
	bool UMulticontroled_arbitrary_decomposition_helper(const Eigen::Ref<const Eigen::MatrixXcd>& G,
		Eigen::Ref<Eigen::MatrixXcd> V, Eigen::Ref<Eigen::MatrixXcd> W, Eigen::Ref<Eigen::MatrixXcd> R_z_matrix) {
		long new_size = G.cols() >> 1;
		Eigen::MatrixXcd G0_G1adj = G.topLeftCorner(new_size, new_size) * G.bottomRightCorner(new_size, new_size).adjoint();
		Eigen::MatrixXcd tmp;
		Eigen::MatrixXcd tmp_sqrt;

		if (G0_G1adj == G0_G1adj.adjoint()) {
			Eigen::SelfAdjointEigenSolver<Eigen::MatrixXcd> eigensolver(G0_G1adj);
			if (eigensolver.info() != Eigen::Success) {
				std::cout << "COMPILE ERROR:: Failed to make eigen decomposition\n";
				return false;
			}
			V = eigensolver.eigenvectors();
			tmp = eigensolver.eigenvalues().asDiagonal();
			tmp_sqrt = tmp.sqrt();
			R_z_matrix.topLeftCorner(new_size, new_size) = tmp_sqrt;
			R_z_matrix.bottomRightCorner(new_size, new_size) = tmp_sqrt.adjoint();
			W = tmp_sqrt * V.adjoint() * G.bottomRightCorner(new_size, new_size);
		}
		else {
			Eigen::ComplexEigenSolver<Eigen::MatrixXcd> cmplx_eigensolver(G0_G1adj);
			if (cmplx_eigensolver.info() != Eigen::Success) {
				std::cout << "COMPILE ERROR:: Failed to make eigen decomposition\n";
				return false;
			}
			V = cmplx_eigensolver.eigenvectors();
			tmp = cmplx_eigensolver.eigenvalues().asDiagonal();
			tmp_sqrt = tmp.sqrt();
			R_z_matrix.topLeftCorner(new_size, new_size) = tmp_sqrt;
			R_z_matrix.bottomRightCorner(new_size, new_size) = tmp_sqrt.adjoint();
			W = tmp_sqrt * V.adjoint() * G.bottomRightCorner(new_size, new_size);
		}
		return true;
	}

	Eigen::VectorXd make_real_angles_R_z(const Eigen::Ref<const Eigen::MatrixXcd>& D, const Eigen::Ref<const Eigen::MatrixXcd>& M_k) {
		std::complex<double> i(0, 1);
		Eigen::VectorXcd alpha = D.asDiagonal();
		alpha.array().log();
		alpha.array()* i* (-2);
		Eigen::VectorXcd theta = M_k.colPivHouseholderQr().solve(alpha);
		return theta.array().real();
	}

	Eigen::VectorXd make_real_angles_R_y(const Eigen::Ref<const Eigen::MatrixXcd>& S, const Eigen::Ref<const Eigen::MatrixXcd>& M_k) {
		Eigen::VectorXcd alpha = S.asDiagonal();
		alpha.array().asin();
		alpha.array()*2;
		Eigen::VectorXcd theta = M_k.colPivHouseholderQr().solve(alpha);
		return theta.array().real();
	}
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
	
	void Shannon_decomposition_helper(std::deque<long>& _c_qub_list, std::deque<double>& _angles,
		Eigen::MatrixXcd& U, std::ofstream& out) {
		if (U.cols() == 2) {
			one_qubit_arbitrary_decomposition_helper(U, _c_qub_list.front(), out);
		}
		else {
			Eigen::MatrixXcd G1, G2, L, R, D, M_k, S;
			long target, new_size, q1;
			target = 0;
			new_size = U.cols() >> 1;
			G1.resize(new_size, new_size);
			G2.resize(new_size, new_size);
			S.resize(new_size, new_size);
			L.resize(U.cols(), U.cols());
			R.resize(U.cols(), U.cols());
			D.resize(U.cols(), U.cols());
			M_k.resize(new_size, new_size);

			/*count G1 G2*/
			cosine_sine_decomposition_helper(U, L.topLeftCorner(new_size, new_size), L.bottomRightCorner(new_size, new_size),
				R.topLeftCorner(new_size, new_size), R.bottomRightCorner(new_size, new_size), S);
			UMulticontroled_arbitrary_decomposition_helper(L, G2, G1, D);
			q1 = _c_qub_list.front();
			_c_qub_list.pop_front();
			Shannon_decomposition_helper(_c_qub_list, _angles, G1, out);
			make_M_k_matrix(M_k, 1 << _c_qub_list.size());
			Eigen::VectorXd theta = make_real_angles_R_z(D, M_k);
			_angles.clear();
			for (long i = 0; i < theta.size(); ++i) {
				_angles.push_back(theta(i));
			}
			umultycontrol_rotation_decomposition_helper(_c_qub_list, _angles, "R_z", q1, out);
			Shannon_decomposition_helper(_c_qub_list, _angles, G2, out);
			theta = make_real_angles_R_y(S, M_k);
			_angles.clear();
			for (long i = 0; i < theta.size(); ++i) {
				_angles.push_back(theta(i));
			}
			umultycontrol_rotation_decomposition_helper(_c_qub_list, _angles, "R_y", target,  out);

			/*recount G1 G2*/
			UMulticontroled_arbitrary_decomposition_helper(R, G2, G1, D);
			Shannon_decomposition_helper(_c_qub_list, _angles, G1, out);
			theta = make_real_angles_R_z(D, M_k);
			_angles.clear();
			for (long i = 0; i < theta.size(); ++i) {
				_angles.push_back(theta(i));
			}
			umultycontrol_rotation_decomposition_helper(_c_qub_list, _angles, "R_z", target, out);
			Shannon_decomposition_helper(_c_qub_list, _angles, G2, out);
		}
	}
	/*the analising subautomats don't recognize every opp-ty as our input is strict enough
	  except the order of the logic*/
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
	bool is_operation_list(std::string& s, std::ofstream& out) {
		bool answer = false;
		std::string instruction, parameteres, tmp, rotation_type;
		double angle = 0., new_angle = 0.;
		std::list<long> c_qub_list;
		std::vector<double> angles;
		std::deque<double> deq_angles;
		std::deque<long> deq_qubits;
		std::vector<long> c_qub_list2;
		Eigen::MatrixXcd matrix;
		/*
		std::list<double>::const_iterator double_it;*/

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
			ss >> parameteres;
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
			ss >> parameteres;
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
			ss >> parameteres;
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
			space_size = std::stol(tmp);
			matrix.resize(space_size, space_size);
			ss1.clear();
			while (std::getline(ss, instruction, ';')) {
				ss1.str(instruction + ',');
				while (std::getline(ss1, tmp, ',')) {
					matrix << read_complex(tmp);
				}
			}
			Shannon_decomposition_helper(deq_qubits, deq_angles, matrix, out);
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



