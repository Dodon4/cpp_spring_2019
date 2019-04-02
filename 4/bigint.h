#include <iostream>
#include<algorithm>

class BigInt {
private:
	uint8_t* data_;
	size_t length;
	bool sgnMinus;
public:
	BigInt() : data_(new uint8_t[1]),
	length(1),
	sgnMinus(false) {
		data_[0] = 0;
	}

	BigInt(int64_t x) {
		if (x < 0)
			sgnMinus = true;
		else 
			sgnMinus = false;
		size_t len = 1;
		int64_t tmp = x;
		while ((tmp /= 10) != 0) {
			++len;
		}
		length = len;
		data_ = new uint8_t[length];
		if (sgnMinus)
			x = -x;
		for (size_t i = 0; i < len; ++i) {
			data_[i] = x % 10;
			x /= 10;
		}
	}

	BigInt(const BigInt& copy) : data_(new uint8_t[copy.length]),
		length(copy.length),sgnMinus(copy.sgnMinus) {
		std::copy(copy.data_, copy.data_ + length, data_);
	}

	BigInt(BigInt&& moved) : data_(moved.data_),length(moved.length),sgnMinus(moved.sgnMinus) {
		moved.data_ = nullptr;
		moved.length = 0;
		moved.sgnMinus = false;
	}

	~BigInt() {
		delete[] data_;
	}

	BigInt& operator=(const BigInt& copied) {
		if (this == &copied) {
			return *this;
		}
		uint8_t* tmp = new uint8_t[copied.length];
		delete[] data_;
		data_ = tmp;
		length = copied.length;
		sgnMinus = copied.sgnMinus;
		std::copy(copied.data_, copied.data_ + length, data_);
		return *this;
	}

	BigInt& operator=(BigInt&& moved) {
		if (this == &moved) {
			return *this;
		}
		delete[] data_;
		data_ = moved.data_;
		length = moved.length;
		sgnMinus = moved.sgnMinus;
		moved.data_ = nullptr;
		moved.length = 0;
		moved.sgnMinus = false;
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& out, const BigInt& x);
	

	bool operator==(const BigInt& other)const {
		if (this == &other)
			return true;
		if (sgnMinus == other.sgnMinus) {
			if (length == other.length) {
				for (size_t j = 0; j < length; ++j) 
					if (data_[j] != other.data_[j]) 
						return false;
				return true;
			}
			else {
				const BigInt* len2, *len1;
				if (length >= other.length) {
					len2 = this;
					len1 = &other;
				}
				else {
					len2 = &other;;
					len1 = this;
				}
				for (size_t i = 0; i < len1->length; ++i)//проверка элементов для числа меньшей размерности массива
					if (data_[i] != other.data_[i]) 
						return false;
				
				for (size_t i = len1->length; i < len2->length; ++i)//остальные элементы должны быть 0,иначе не равны
					if (len2->data_[i] != 0)
						return false;
				return true;
			}
		}
		else {
			bool f = false;//с разными знаками равны,если оба 0
			for (size_t i = 0; i < length; ++i)
				if (data_[i] != 0)
					return false;
			for (size_t i = 0; i < other.length; ++i) 
				if (other.data_[i] != 0)
					return false;
			return true;
		}
	}

	bool operator!=(const BigInt& other)const {
		return !(*this == other);
	}
	BigInt operator+(const BigInt& other) const {
		if ((sgnMinus && other.sgnMinus) || ((!sgnMinus) && (!other.sgnMinus))) {
			BigInt tmp;
			tmp.length = std::max(length, other.length) + 1;//максимальная длина из 1 и 2 числа и +1 в случае переполнения
			tmp.sgnMinus = sgnMinus;
			delete[] tmp.data_;
			tmp.data_ = new uint8_t[tmp.length];
			size_t lim = std::min(length, other.length);
			uint8_t plus = 0;
			for (size_t i = 0; i < lim; ++i) {
				tmp.data_[i] = data_[i] + other.data_[i] + plus;
				plus = tmp.data_[i] / 10;//в случае переполнения 1 в след разряд
				tmp.data_[i] %= 10;
			}
			const BigInt* longest;
			if (length >= other.length) {
				longest = this;
			}
			else {
				longest = &other;;
			}
			for (size_t i = lim; i < longest->length; ++i) {
				tmp.data_[i] = longest->data_[i] + plus;
				plus = tmp.data_[i] / 10;
				tmp.data_[i] %= 10;
			}
			tmp.data_[longest->length] = plus;
			return tmp;
		}
		else
			return *this - -other;//для разных знаков
	}
	BigInt operator-() const {
		BigInt tmp(*this);
		tmp.sgnMinus = !sgnMinus;
		return tmp;
	}
	BigInt operator-(const BigInt& other) const {
		if ((sgnMinus && other.sgnMinus) || ((!sgnMinus) && (!other.sgnMinus))) 
			if (!sgnMinus) 
				if (*this >= other) {
					BigInt tmp(*this);
					uint8_t per = 0;//показывает,забрали ли 1 у более высокого разряда
					for (size_t i = 0; i < other.length; ++i) {
						if (tmp.data_[i] >= (other.data_[i] + per)) {
							tmp.data_[i] -= (other.data_[i] + per);
							per = 0;
						}
						else {
							tmp.data_[i] += (10 - other.data_[i] - per);
							per = 1;
						}
					}
					for (size_t i = other.length; i < length; ++i) {
						if (tmp.data_[i] >= per) {
							tmp.data_[i] -= (per);
							per = 0;
						}
						else {
							tmp.data_[i] += (10 - per);
							per = 1;
						}
					}
					return tmp;
				}
				else
					return -(other - *this);//если 1 число меньше
			else
				return -(-*this - (-other));//если 1 число отрицательное
		else
			return *this + -other;//если разные знаки
	}
	bool operator<(const BigInt& other)const {
		if (this == &other) 
			return false;
		if (sgnMinus && !other.sgnMinus)//проверка знаков
			return true;
		if (!sgnMinus && other.sgnMinus)
			return false;
		if (sgnMinus && other.sgnMinus)
			return (-(*this)) > (-other);
		size_t len1 = length - 1;
		size_t len2 = other.length - 1;
		for (; len1 > 0; --len1) //считаем длину без нулей для 1 значения
			if (data_[len1] != 0)
				break;
		for (; len2 > 0; --len2) //считаем длину без нулей для 2 значения
			if (other.data_[len2] != 0)
				break;
		
		if (len1 < len2)//если число длиннее,то оно больше
			return true;
		if (len1 > len2)
			return false;
		for (size_t i = len1; i >= 0; --i) {//поэлементная проверка
			if (data_[i] > other.data_[i])
				return false;
			if (data_[i] < other.data_[i])
				return true;
		}
		return false;
	}

	bool operator>(const BigInt& other)const {
		return ((*this != other) && !(*this<other));
	}

	bool operator<=(const BigInt& other)const {
		return!(*this > other);
	}

	bool operator>=(const BigInt& other)const {
		return!(*this < other);
	}
};

std::ostream& operator<<(std::ostream& out, const BigInt& x) {
	if (x == (-BigInt(0))) {
		out << 0;
		return out;
	}
	if (x.length > 0) {
		bool prov = false;//для вывода начиная с первого ненулевого элемента
		if (x.sgnMinus)
			out << "-"; 
		for (size_t i = x.length - 1; i > 0; --i) {
			if (prov) 
				out << x.data_[i] + 0;
			else 
				if (x.data_[i] != 0) {
					prov = true;
					out << x.data_[i] + 0;
				}
		}
	}
	out << x.data_[0] + 0;
	return out;
}
