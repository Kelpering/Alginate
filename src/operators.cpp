/**
*   File: operators.cpp
*   Project: Alginate
*   SPDX-License-Identifier: Unlicense
* 
*   Operators.cpp contains simple wrappers for the overloaded operators
*   to provide AlgInts with a more intuitive interface for simple arithmetic.
*/
#include "Alginate.hpp"

AlgInt& AlgInt::operator=(const AlgInt& other)
{
    // Resize instead of copy to allocate a new array
    resize(other.size);

    // Deep copy other.num
    for (size_t i = 0; i < other.size; i++)
        num[i] = other.num[i];

    // Remove leading zeroes
    trunc();

    // Copy other.sign
    sign = other.sign;

    return *this;
}

AlgInt& AlgInt::operator=(AlgInt&& other)
{
    AlgInt::swap(*this, other);
    return *this;
}

AlgInt AlgInt::operator+(const AlgInt& other) const {
    AlgInt temp;
    add(*this, other, temp);
    return temp;
}
AlgInt AlgInt::operator+(uint32_t other) const {
    AlgInt temp;
    add(*this, other, temp);
    return temp;
}

AlgInt AlgInt::operator-(const AlgInt& other) const {
    AlgInt temp;
    sub(*this, other, temp);
    return temp;
}
AlgInt AlgInt::operator-(uint32_t other) const {
    AlgInt temp;
    sub(*this, other, temp);
    return temp;
}

AlgInt AlgInt::operator*(const AlgInt& other) const {
    AlgInt temp;
    mul(*this, other, temp);
    return temp;
}
AlgInt AlgInt::operator*(uint32_t other) const {
    AlgInt temp;
    mul(*this, other, temp);
    return temp;
}

AlgInt AlgInt::operator/(const AlgInt& other) const {
    AlgInt temp;
    div(*this, other, temp);
    return temp;
}
AlgInt AlgInt::operator/(uint32_t other) const {
    AlgInt temp;
    div(*this, other, temp);
    return temp;
}

AlgInt AlgInt::operator%(const AlgInt& other) const {
    AlgInt temp;
    mod(*this, other, temp);
    return temp;
}
AlgInt AlgInt::operator%(uint32_t other) const {
    AlgInt temp;
    mod(*this, other, temp);
    return temp;
}

AlgInt AlgInt::operator&(const AlgInt& other) const {
    AlgInt temp;
    bw_and(*this, other, temp);
    return temp;
}
AlgInt AlgInt::operator^(const AlgInt& other) const {
    AlgInt temp;
    bw_xor(*this, other, temp);
    return temp;
}
AlgInt AlgInt::operator|(const AlgInt& other) const {
    AlgInt temp;
    bw_or(*this, other, temp);
    return temp;
}
AlgInt AlgInt::operator<<(size_t other) const {
    AlgInt temp;
    bw_shl(*this, other, temp);
    return temp;
}
AlgInt AlgInt::operator>>(size_t other) const {
    AlgInt temp;
    bw_shr(*this, other, temp);
    return temp;
}

AlgInt& AlgInt::operator+=(const AlgInt& other) {
    add(*this, other, *this);
    return *this;
}
AlgInt& AlgInt::operator+=(uint32_t other) {
    add(*this, other, *this);
    return *this;
}

AlgInt& AlgInt::operator-=(const AlgInt& other) {
    sub(*this, other, *this);
    return *this;
}
AlgInt& AlgInt::operator-=(uint32_t other) {
    sub(*this, other, *this);
    return *this;
}

AlgInt& AlgInt::operator*=(const AlgInt& other) {
    mul(*this, other, *this);
    return *this;
}
AlgInt& AlgInt::operator*=(uint32_t other) {
    mul(*this, other, *this);
    return *this;
}

AlgInt& AlgInt::operator/=(const AlgInt& other) {
    div(*this, other, *this);
    return *this;
}
AlgInt& AlgInt::operator/=(uint32_t other) {
    div(*this, other, *this);
    return *this;
}

AlgInt& AlgInt::operator%=(const AlgInt& other) {
    mod(*this, other, *this);
    return *this;
}
AlgInt& AlgInt::operator%=(uint32_t other) {
    mod(*this, other, *this);
    return *this;
}

AlgInt& AlgInt::operator&=(const AlgInt& other) {
    bw_and(*this, other, *this);
    return *this;
}
AlgInt& AlgInt::operator^=(const AlgInt& other) {
    bw_xor(*this, other, *this);
    return *this;
}
AlgInt& AlgInt::operator|=(const AlgInt& other) {
    bw_or(*this, other, *this);
    return *this;
}
AlgInt& AlgInt::operator<<=(size_t other) {
    bw_shl(*this, other, *this);
    return *this;
}
AlgInt& AlgInt::operator>>=(size_t other) {
    bw_shr(*this, other, *this);
    return *this;
}

bool AlgInt::operator<(const AlgInt& other) const {
    int cmp_ret = cmp(*this, other);
    return cmp_ret < 0;
}
bool AlgInt::operator<=(const AlgInt& other) const {
    int cmp_ret = cmp(*this, other);
    return cmp_ret <= 0;
}
bool AlgInt::operator!=(const AlgInt& other) const {
    int cmp_ret = cmp(*this, other);
    return cmp_ret != 0;
}
bool AlgInt::operator==(const AlgInt& other) const {
    int cmp_ret = cmp(*this, other);
    return cmp_ret == 0;
}
bool AlgInt::operator>(const AlgInt& other) const {
    int cmp_ret = cmp(*this, other);
    return cmp_ret > 0;
}
bool AlgInt::operator>=(const AlgInt& other) const {
    int cmp_ret = cmp(*this, other);
    return cmp_ret >= 0;
}