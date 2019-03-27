class ProxyM {
private:
	size_t columns;
	int* data_;
public:
	ProxyM(const size_t col) :columns(col) {
		data_ = new int[columns];
	}
	~ProxyM() {
			delete[] data_;
	}
	const int& operator[](size_t i) const {
				if (i >= columns) 
					throw std::out_of_range("");
				return data_[i];
	}
	int& operator[](size_t i) {
		if (i >= columns)
			throw std::out_of_range("");
		return data_[i];
	}
	ProxyM& operator*=(const int& other) {
		for (size_t i = 0; i < columns; ++i)
			data_[i] *= other;
		return *this;
	}

	bool operator==(const ProxyM& other) const {
		if (this == &other)
			return true;
		if (columns != other.columns) 
			return false;
		for (size_t i = 0; i < columns; ++i)
			if (data_[i] != other.data_[i])
				return false;
		return true;
	}
	bool operator!=(const ProxyM& other) const {
		return !(*this == other);
	}
};
class Matrix {
private:
	size_t rows, columns;
	ProxyM* data_;
public:
	Matrix(const size_t row, const size_t col) : rows(row), columns(col) {
		data_ = static_cast<ProxyM*>(operator new[](sizeof(ProxyM)*rows));
		for (size_t i = 0; i < rows; ++i) {
			new (data_ + i) ProxyM(col);
		}
	}
	~Matrix() {
		for (size_t i = 0; i < rows; ++i) {
			data_[i].~ProxyM();
		}
		operator delete[](data_);
	}
	const ProxyM& operator[](size_t i) const {
		if (i >= rows) 
			throw std::out_of_range("");
		return data_[i];
	}
	ProxyM& operator[](size_t i) {
		if (i >= rows) 
			throw std::out_of_range("");
		return data_[i];
	}

	Matrix& operator *=(const int other) {
		for (size_t i = 0; i < rows; ++i) 
			data_[i] *= other;
		return *this;
	}
	bool operator==(const Matrix& other) const {
		if (this == &other)
			return true;
		if (rows != other.rows || columns != other.columns) {
			return false;
		}
		for (size_t i = 0; i < rows; ++i)
			if (data_[i] != other.data_[i])
				return false;
		return true;
		}


		bool operator!=(const Matrix& other) const {
			return !(*this == other);
		}
	size_t getRows() const {
		return rows;
	}
	size_t getColumns() const {
		return columns;
	}
};
