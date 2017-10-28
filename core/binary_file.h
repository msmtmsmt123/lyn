#ifndef BINARY_FILE_H
#define BINARY_FILE_H

#include <iostream>
#include <vector>
#include <cstdint>

namespace lyn {

class binary_file {
public:
	using byte_t = unsigned char;
	using size_t = std::size_t;

public:
	void load_from_stream(std::istream& input);
	void load_from_other(const binary_file& other, unsigned int start = 0, int size = -1);

	size_t size() const { return mData.size(); }

	bool is_cstr_at(int pos) const;
	const char* cstr_at(int pos) const;

	std::vector<byte_t>&       data()       { return mData; }
	const std::vector<byte_t>& data() const { return mData; }

	template<typename T, int byte_count = sizeof(T)>
	T read(int& pos) const;

	template<typename T, int byte_count = sizeof(T)>
	T at(int pos) const { return read<T, byte_count>(pos); }

	byte_t read_byte(int& pos) const;
	byte_t byte_at(int pos) const { return read_byte(pos); }

private:
	std::vector<byte_t> mData;
};

template<typename T, int byte_count = sizeof(T)>
T binary_file::read(int& pos) const {
	T result = 0;

	for (int i=0; i<byte_count; ++i)
		result |= (read_byte(pos) << (i*8));

	return result;
}

} // namespace lyn

#endif // BINARY_FILE_H
