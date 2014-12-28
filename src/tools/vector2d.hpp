//
//  vector_2d.hpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-21.
//
//

#ifndef BoE_VECTOR_2D_HPP
#define BoE_VECTOR_2D_HPP

// Tried using boost::multi_array, but it kept causing weird issues, so I decided to make my own.
// TODO: Fill out missing members (should have equivalents for most of the stuff in std::vector)
// TODO: Add row-wise access using the included row_ref, and support for row/column assignment
// (These would be very useful for implementing a resize outdoors function in the scenario editor.)

#include <vector>

template<typename Type, typename Alloc = std::allocator<Type>> class vector2d {
	friend class row_ref;
	friend class col_ref;
	friend class const_row_ref;
	friend class const_col_ref;
	std::vector<Type, Alloc> data;
	size_t w, h;
public:
	class row_ref {
		friend class vector2d<Type, Alloc>;
		vector2d<Type, Alloc>& ref;
		size_t y;
		row_ref(vector2d<Type, Alloc>& ref, size_t row) : ref(ref), y(row) {}
	public:
		Type& operator[](size_t x) {
			return ref.data[ref.w * y + x];
		}
	};
	class col_ref {
		friend class vector2d<Type, Alloc>;
		vector2d<Type, Alloc>& ref;
		size_t x;
		col_ref(vector2d<Type, Alloc>& ref, size_t col) : ref(ref), x(col) {}
	public:
		Type& operator[](size_t y) {
			return ref.data[ref.w * y + x];
		}
	};
	class const_row_ref {
		friend class vector2d<Type, Alloc>;
		const vector2d<Type, Alloc>& ref;
		size_t y;
		const_row_ref(const vector2d<Type, Alloc>& ref, size_t row) : ref(ref), y(row) {}
	public:
		const Type& operator[](size_t x) const {
			return ref.data[ref.w * y + x];
		}
	};
	class const_col_ref {
		friend class vector2d<Type, Alloc>;
		const vector2d<Type, Alloc>& ref;
		size_t x;
		const_col_ref(const vector2d<Type, Alloc>& ref, size_t col) : ref(ref), x(col) {}
	public:
		const Type& operator[](size_t y) const {
			return ref.data[ref.w * y + x];
		}
	};
	col_ref operator[](size_t x) {
		return col_ref(*this, x);
	}
	const_col_ref operator[](size_t x) const {
		return const_col_ref(*this, x);
	}
	size_t width() {
		return w;
	}
	size_t height() {
		return h;
	}
	size_t size() {
		return data.size();
	}
	void resize(size_t width, size_t height) {
		w = width; h = height;
		data.resize(w * h);
	}
	bool empty() {
		return data.empty();
	}
	vector2d() : w(0), h(0) {}
};

#endif
