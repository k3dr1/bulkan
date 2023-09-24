#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <cassert>

template<int n>
struct vec {
	double data[n] = {0};
	double& operator[](const int i) {
		assert(i>=0 && i<n);
		return data[i];
	}
	double operator[](const int i) const {
		assert(i>=0 && i<n);
		return data[i];
	}
	double norm2() const {
		return (*this)*(*this);
	}
	double norm() const{
		return std::sqrt(norm2());
	}
};

// Vector * Vector
template<int n>
double operator*(const vec<n>& lhs, const vec<n>& rhs){
	double ret = 0;
	for (int i = n; i--; ret += lhs[i] * rhs[i]);
	return ret;
}

template<int n>
vec<n> operator+(const vec<n>& lhs, const vec<n>& rhs){
	vec<n> ret = lhs;
	for (int i = n; i--; ret[i] += rhs[i]);
	return ret;
}

template<int n>
vec<n> operator-(const vec<n>& lhs, const vec<n>& rhs){
	vec<n> ret = lhs;
	for (int i = n; i--; ret[i] -= rhs[i]);
	return ret;
}

// Scalar * Vector
template<int n>
vec<n> operator*(const double& scale, const vec<n>& rhs){
	vec<n> ret = rhs;
	for (int i = n; i--; ret[i] *= scale);
	return ret;
}

// Vector * Scalar
template<int n>
vec<n> operator*(const vec<n>& rhs, const double& scale){
	vec<n> ret = rhs;
	for (int i = n; i--; ret[i] *= scale);
	return ret;
}


template<int n>
vec<n> operator/(const vec<n>& lhs, const double& scale){
	vec<n> ret = lhs;
	for (int i = n; i--; ret[i] /= scale);
	return ret;
}

// Emplaces the vector into a larger vector
// fills new entries with 1 by default
template<int n1, int n2>
vec<n1> embed(const vec<n2>& v, double fill=1){
	vec<n1> ret;
	for (int i = 0; i < n1; i++){
		ret[i] = i < n2 ? v[i] : fill;
	}
	return ret;
}

// Squishes the vector into 
// a smaller vector, discards entries
template<int n1, int n2>
vec<n1> proj(const vec<n2> &v){
	assert(n2 > n1);
	vec<n1> ret;
	for (int i = 0; i < n1; i++){
		ret[i] = v[i];
	}
	return ret;
}

template <int n>
std::ostream& operator<<(std::ostream& out, const vec<n>& v){
	for (int i = 0; i < n; i++){
		out << v[i] << " ";
	}
	return out;
}

template<>
struct vec<2> {
	double x = 0;
	double y = 0;
	double& operator[](const int i){
		assert(i>=0 && i<2);
		return i ? y : x;
	}
	double operator[](const int i) const {
		assert(i>=0 && i<2);
		return i ? y : x;
	}
	double norm2() const {
		return (*this) * (*this);
	}
	double norm() const {
		return std::sqrt(norm2());
	}
	vec<2> normalized() {
		return (*this)/norm();
	}
};

template<>
struct vec<3> {
	double x = 0;
	double y = 0;
	double z = 0;
	double& operator[](const int i){
		assert(i>=0 && i<3);
		return i ? (1 == i ? y : z) : x;
	}
	double operator[](const int i) const {
		assert(i>=0 && i<3);
		return i ? (1 == i ? y : z) : x;
	}
	double norm2() const {
		return (*this) * (*this);
	}
	double norm() const {
		return std::sqrt(norm2());
	}
	vec<3> normalized() {
		return (*this)/norm();
	}
};

template<>
struct vec<4> {
	double x = 0;
	double y = 0;
	double z = 0;
	double w = 0;
	double& operator[](const int i){
		assert(i>=0 && i<4);
		return i ? (1 == i ? y : (2 == i ? z : w)) : x;
	}
	double operator[](const int i) const {
		assert(i>=0 && i<4);
		return i ? (1 == i ? y : (2 == i ? z : w)) : x;
	}
	double norm2() const {
		return (*this) * (*this);
	}
	double norm() const {
		return std::sqrt(norm2());
	}
	vec<4> normalized() {
		return (*this)/norm();
	}
	// Equates the w component to 1
	// and scales others accordingly
	vec<4> w_normalized() {
		return (*this)/w;
	}
};

typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;

// Transforms to homogeneous coordinates
template<int N>
vec<N+1> to_homogeneous(vec<N>& v, double scale){
	static_assert(N > 0);
	vec<N+1> ret = {};
	for (int i = 0; i < N; i++){
		ret[i] = v[i]*scale;
	}
	ret[N] = scale;
	return ret;
}

// Transforms back to regular coordinates
template<int N>
vec<N-1> to_regular(vec<N>& v){
	static_assert(N > 2);
	vec<N-1> ret = {};
	for (int i = 0; i < (N-1); i++){
		ret[i] = v[i]/v[N-1];
	}
	return ret;
}

vec3 cross(const vec3& p, const vec3& v);

struct vec2i{
	int x;
	int y;
};

struct vec3i{
	int x;
	int y;
	int z;
};

struct vec2ui{
	unsigned int x;
	unsigned int y;
};

struct vec3ui{
	unsigned int x;
	unsigned int y;
	unsigned int z;
};

template<int n> struct dt;

template<int nrows, int ncols>
struct mat {
	// nrows amount of vectors of length ncols
	// basically an array
	vec<ncols> rows[nrows] = {{}};

	vec<ncols>& operator[] (const int idx){
		assert(idx>=0 && idx<nrows);
		return rows[idx];
	}
	const vec<ncols>& operator[] (const int idx) const {
		assert(idx>=0 && idx<nrows);
		return rows[idx];
	}

	// Returns a newly constructed copy of a column
	vec<nrows> col(const int idx) const {
		assert(idx>=0 && idx<ncols);
		vec<nrows> ret;
		for (int i=nrows; i--; ret[i]=rows[i][idx]);
		return ret;
	}

	// Modifies the certain column of *this*
	void set_col(const int idx, const vec<nrows> &v){
		assert(idx>=0 && idx<ncols);
		for (int i=nrows; i--; rows[i][idx]=v[i]);
	}

	// Returns a newly created object
	static mat<nrows,ncols> identity() {
		mat<nrows, ncols> ret;
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				ret[i][j] = (i==j);
			}
		}
		return ret;
	}

	double det() const {
		return dt<ncols>::det(*this);
	}

	// Returns a matrix with the given row and column removed
	mat<nrows-1, ncols-1> get_minor(const int row, const int col) const {
		mat<nrows-1, ncols-1> ret;
		for (int i = 0; i < nrows-1; i++) {
			for (int j = 0; j < ncols-1; j++){
				ret[i][j] = rows[i<row?i:i+1][j<col?j:j+1];
			}
		}
		return ret;
	}

	double cofactor(const int row, const int col) const {
		return get_minor(row,col).det()*((row+col)%2 ? -1 : 1);
	}

	mat<nrows,ncols> adjugate() const {
		mat<nrows,ncols> ret;
		for (int i = 0; i < nrows; i++){
			for (int j = 0; j < nrows; j++){
				ret[i][j] = cofactor(i,j);
			}
		}
		return ret;
	}

	mat<nrows,ncols> invert_transpose() const {
		mat<nrows,ncols> ret = adjugate();
		return ret/(ret[0]*rows[0]);
	}

	mat<nrows,ncols> invert() const {
		return invert_transpose().transpose();
	}

	mat<ncols,nrows> transpose() const {
		mat<ncols,nrows> ret;
		for (int i = 0; i < ncols; i++){
			ret[i] = this->col(i);
		}
		return ret;
	}
};


template<int nrows,int ncols> 
vec<nrows> operator*(const mat<nrows,ncols>& lhs, const vec<ncols>& rhs) {
	vec<nrows> ret;
	for (int i = 0; i < nrows; i++){
		ret[i]=lhs[i]*rhs;
	}
	return ret;
}

// Yea this thing is inefficient
// because it copies columns
// I could probably my C linear algebra library
// from the ml framework but I gotta learn some more c++
template<int R1, int C1, int C2>
mat<R1,C2> operator*(const mat<R1,C1>& lhs, const mat<C1,C2>& rhs){
	mat<R1,C2> result;
	for (int i = 0; i < R1; i++){
		for (int j = 0; j < C2; j++){
			// this is a vector multiplication
			// adds up to a double
			result[i][j]=lhs[i]*rhs.col(j);
		}
	}
	return result;
}

// Matrix * Scalar
// basically multiplies all of the entries by val
template<int nrows,int ncols>
mat<nrows,ncols> operator*(const mat<nrows,ncols>& lhs, const double& val) {
    mat<nrows,ncols> result;
    for (int i = 0; i < nrows; i++){
	    result[i] = lhs[i]*val;
    }
    return result;
}

// Matrix / Scalar
// divides all entries by val
template<int nrows,int ncols>
mat<nrows,ncols> operator/(const mat<nrows,ncols>& lhs, const double& val) {
    mat<nrows,ncols> result;
    for (int i=nrows; i--; result[i] = lhs[i]/val);
    return result;
}

// Matrix + Matrix
template<int nrows,int ncols>
mat<nrows,ncols> operator+(const mat<nrows,ncols>& lhs, const mat<nrows,ncols>& rhs) {
    mat<nrows,ncols> result;
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
		result[i][j] = lhs[i][j] + rhs[i][j];
	}
    }

    return result;
}

// Matrix - Matrix
template<int nrows,int ncols>
mat<nrows,ncols> operator-(const mat<nrows,ncols>& lhs, const mat<nrows,ncols>& rhs) {
    mat<nrows,ncols> result;
    for (int i=nrows; i--; )
        for (int j=ncols; j--; result[i][j]=lhs[i][j]-rhs[i][j]);
    return result;
}

// Printing out a matrix
template<int nrows,int ncols>
std::ostream& operator<<(std::ostream& out, const mat<nrows,ncols>& m) {
    for (int i=0; i<nrows; i++) 
	    out << m[i] << '\n';
    return out;
}

// Determinant of an n-by-n matrix
template<int n> 
struct dt {
    static double det(const mat<n,n>& src) {
        double ret = 0;
        for (int i = 0; i < n; i++){
		ret += src[0][i]*src.cofactor(0, i);
	}
        return ret;
    }
};

template<> 
struct dt<1> {
    static double det(const mat<1,1>& src) {
        return src[0][0];
    }
};
