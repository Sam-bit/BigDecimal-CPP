/*
MIT License
Copyright (c) 2017 Sambit Samal
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef BIGDECIMAL_H
#define BIGDECIMAL_H

#include<string>
#include<iostream>
#include<sstream>
#include <algorithm>
#include <limits.h>
template <typename T>
std::string to_string(const T& t) { 
   std::stringstream ss; 
   ss<<t; 
   return ss.str(); 
} 
class BigDecimal {

public:
    BigDecimal() { }
    BigDecimal(const BigDecimal &num) : value(num.value) { }
    BigDecimal(const char *num) : value(num) {  }
    BigDecimal(std::string num) : value(num) {  }
    BigDecimal(int num) : value() { std::stringstream ss; ss << num; value = ss.str(); }
    BigDecimal(long long num) : value() { std::stringstream ss; ss << num; value = ss.str(); }
    BigDecimal(unsigned int num) : value() { std::stringstream ss; ss << num; value = ss.str(); }
    BigDecimal(unsigned long long num) : value() { std::stringstream ss; ss << num; value = ss.str(); }
    BigDecimal(float num) : value() { std::stringstream ss; ss << num; value = ss.str(); }
    BigDecimal(double num) : value() { std::stringstream ss; ss << num; value = ss.str(); }
    BigDecimal(long double num) { std::stringstream ss; ss << num; value = ss.str(); }
	
    BigDecimal operator+(const BigDecimal& num) {
        return BigDecimal::add(value, num.value);
    }

    BigDecimal operator-(const BigDecimal& num) {
        return BigDecimal::subtract(value, num.value);
    }

    BigDecimal operator*(const BigDecimal& num) {
        return BigDecimal::multiply(value, num.value);
    }

    BigDecimal operator/(const BigDecimal& num) {
        return BigDecimal::divide(value, num.value);
    }

    BigDecimal operator%(const BigDecimal& num) {
        return BigDecimal::modulus(value, num.value);
    }

    BigDecimal operator^(const BigDecimal& num) {
        return BigDecimal::pow(value, num.value);
    }

    void operator+=(const BigDecimal& num) {
        value = BigDecimal::add(value, num.value);
    }
    void operator-=(const BigDecimal& num) {
        value = BigDecimal::subtract(value, num.value);
    }
    void operator*=(const BigDecimal& num) {
        value = BigDecimal::multiply(value, num.value);
    }
    void operator/=(const BigDecimal& num) {
        value = BigDecimal::divide(value, num.value);
    }
    void operator^=(const BigDecimal& num) {
        value = BigDecimal::pow(value, num.value);
    }

    bool operator > (const BigDecimal& num) {
        return BigDecimal::compareTo(value, num.value)>0;
    }
    bool operator >= (const BigDecimal& num) {
        return BigDecimal::compareTo(value, num.value)>=0;
    }
    bool operator == (const BigDecimal& num) {
        return BigDecimal::compareTo(value, num.value)==0;
    }
    bool operator < (const BigDecimal& num) {
        return BigDecimal::compareTo(value, num.value)<0;
    }
    bool operator <= (const BigDecimal& num) {
        return BigDecimal::compareTo(value, num.value)<=0;
    }

    int toInt() {
        std::istringstream buffer(value);
        int ret;
        buffer >> ret;
        return ret;
    }

    unsigned int toUInt() {
        std::istringstream buffer(value);
        unsigned int ret;
        buffer >> ret;
        return ret;
    }

    long long toLongLong() {
        std::istringstream buffer(value);
        long long ret;
        buffer >> ret;
        return ret;
    }

    unsigned long long toULongLong() {
        std::istringstream buffer(value);
        unsigned long long ret;
        buffer >> ret;
        return ret;
    }

    long double toLongDouble() {
        std::istringstream buffer(value);
        long double ret;
        buffer >> ret;
        return ret;
    }

    double toDouble() {
        std::istringstream buffer(value);
        double ret;
        buffer >> ret;
        return ret;
    }

    float toFloat() {
        std::istringstream buffer(value);
        float ret;
        buffer >> ret;
        return ret;
    }

    std::string toString() {
        return value;
    }

    void round(int scale) {
        if(scale>=1)
            value = BigDecimal::round(value, scale);
    }

    std::string getIntPart() {
        std::size_t dot = value.find('.');
        if(dot != std::string::npos) {
            if(dot == 0)
                return std::string("0");
            if(dot == 1 && value[0] == '-')
                return std::string("-0");
            return value.substr(0, dot);
        }else{
            return value;
        }
    }

    std::string getDecPart() {
        std::size_t dot = value.find('.');
        if(dot != std::string::npos)
            return value.length()>dot+1?value.substr(dot+1):std::string("0");
        else
            return std::string("0");
    }

private:
    std::string value;

public:

    static void setscale (int scale);

    static std::string divide (const std::string &lhs, const std::string &rhs,int scale = INT_MIN);

    static std::string modulus (const std::string &lhs, const std::string &rhs,int scale = INT_MIN);

    static std::string pow (const std::string &lhs, const std::string &rhs,int scale = INT_MIN);

    static std::string add (const std::string &lhs, const std::string &rhs,int scale = INT_MIN);

    static std::string subtract (const std::string &lhs, const std::string &rhs,int scale = INT_MIN);

    static std::string multiply (const std::string &lhs, const std::string &rhs,int scale = INT_MIN);

    static std::string round (const std::string &lhs,int scale = INT_MIN);

    static int compareTo (const std::string &lhs, const std::string &rhs,int scale = INT_MIN);
	
	static std::string log2(const std::string &lhs,int scale = INT_MIN);
	
	static std::string ln(const std::string &lhs,int scale = INT_MIN);
	
	static std::string log(const std::string &lhs,int scale = INT_MIN);
	
	static std::string sin(const std::string &lhs,int scale = INT_MIN);
	
	static std::string stringToHex(std::string &lhs,int caps = 0);
};


#endif // BIGDECIMAL_H
