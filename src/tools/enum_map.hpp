//
//  enum_map.hpp
//  BoE
//
//  Created by Celtic Minstrel on 19-11-23.
//
//

#ifndef BoE_ENUM_MAP_HPP
#define BoE_ENUM_MAP_HPP

#include <array>

// This is designed for contiguous enums, it'll waste space otherwise.
template<typename E, typename T, int n>
struct enum_map_impl : public std::array<T, n> {
	// Since the superclass is a dependent type we need to explicitly import these to use them in the class definition
	using typename array::reference;
	using typename array::const_reference;
	using typename array::size_type;
	enum_map_impl() = default;
	enum_map_impl(std::initializer_list<T> l) {
		std::copy(l.begin(), l.end(), this->begin());
	}
	reference at(size_type pos) = delete;
	const_reference at(size_type pos) const = delete;
	reference operator[](size_type pos) = delete;
	const_reference operator[](size_type pos) const = delete;
	reference at(E pos) {return this->array::at(static_cast<int>(pos));};
	const_reference at(E pos) const {return this->array::operator[](static_cast<int>(pos));};
	reference operator[](E pos) {return this->array::operator[](static_cast<int>(pos));};
	const_reference operator[](E pos) const = delete;
	static const std::array<E, n> keys() {
		static std::array<E, n> keys;
		static bool inited = false;
		if(!inited) {
			for(size_t i = 0; i < n; i++)
				keys[i] = static_cast<E>(static_cast<int>(E()) + i);
			inited = true;
		}
		return keys;
	}
};

#define enum_map_STR2(a, b) a ## b
#define enum_map_STR(a, b) enum_map_STR2(a, b)
#define enum_map(E, T) enum_map_impl<E, T, enum_map_STR(MAX_, E)>

#endif
