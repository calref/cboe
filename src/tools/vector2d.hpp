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

#include <vector>
#include <cstddef>

// TODO: insert_row, insert_col
template<typename Type, typename Alloc = std::allocator<Type>> class vector2d {
	friend class row_ref;
	friend class col_ref;
	friend class const_row_ref;
	friend class const_col_ref;
	std::vector<Type, Alloc> data;
	size_t w, h;

	// Range-checking vector2d access is important because an out-of-bounds (x,y) combination can refer to an in-bounds index of
	// the underlying array
	Type& at(size_t x, size_t y) {
		if(x >= w || y >= h){
			throw "Tried to access out-of-range element at (" + std::to_string(x) + ", " + std::to_string(y) + ") in vector2d size "
					+ std::to_string(w) + "x" + std::to_string(h);
		}
		size_t idx = w * y + x;
		return data[idx];
	}
	const Type& at(size_t x, size_t y) const {
		if(x >= w || y >= h){
			throw "Tried to access out-of-range element at (" + std::to_string(x) + ", " + std::to_string(y) + ") in vector2d size "
					+ std::to_string(w) + "x" + std::to_string(h);
		}
		size_t idx = w * y + x;
		return data[idx];
	}
public:
	using value_type = Type;
	class row_ref {
		friend class vector2d<Type, Alloc>;
		vector2d<Type, Alloc>& ref;
		size_t y;
		row_ref(vector2d<Type, Alloc>& ref, size_t row) : ref(ref), y(row) {}
		template<typename Iterable>
		void assign_from_container(const Iterable& list) {
			row_ref& me = *this;
			int i = 0;
			for(Type n : list) {
				if(i >= ref.width()) break;
				me[i++] = n;
			}
		}
	public:
		Type& operator[](size_t x) {
			return ref.at(x, y);
		}
		const Type& operator[](size_t x) const {
			return ref.at(x, y);
		}
		row_ref operator=(row_ref&& other) {
			row_ref& me = *this;
			for(int i = 0; i < ref.width(); i++) {
				me[i] = std::move(other[i]);
				other[i] = Type();
			}
			return me;
		}
		row_ref operator=(const row_ref& other) {
			row_ref& me = *this;
			for(int i = 0; i < ref.width(); i++) {
				me[i] = other[i];
			}
			return me;
		}
		row_ref operator=(std::initializer_list<Type> list) {
			assign_from_container(list);
			return *this;
		}
		template<typename Iterable>
		row_ref operator=(const Iterable& list) {
			assign_from_container(list);
			return *this;
		}
		void fill(Type val) {
			row_ref& me = *this;
			for(int i = 0; i < ref.width(); i++) {
				me[i] = val;
			}
		}
		// Seems like defining a move assignment operator deletes the copy constructor. Don't want that.
		row_ref(const row_ref&) = default;
	};
	class col_ref {
		friend class vector2d<Type, Alloc>;
		vector2d<Type, Alloc>& ref;
		size_t x;
		col_ref(vector2d<Type, Alloc>& ref, size_t col) : ref(ref), x(col) {}
		template<typename Iterable>
		void assign_from_container(const Iterable& list) {
			col_ref& me = *this;
			int i = 0;
			for(Type n : list) {
				if(i >= ref.height()) break;
				me[i++] = n;
			}
		}
	public:
		Type& operator[](size_t y) {
			return ref.at(x, y);
		}
		const Type& operator[](size_t y) const {
			return ref.at(x, y);
		}
		col_ref operator=(col_ref&& other) {
			col_ref& me = *this;
			for(int i = 0; i < ref.height(); i++) {
				me[i] = std::move(other[i]);
				other[i] = Type();
			}
			return me;
		}
		col_ref operator=(const col_ref& other) {
			col_ref& me = *this;
			for(int i = 0; i < ref.height(); i++) {
				me[i] = other[i];
			}
			return me;
		}
		col_ref operator=(std::initializer_list<Type> list) {
			assign_from_container(list);
			return *this;
		}
		template<typename Iterable>
		col_ref operator=(const Iterable& list) {
			assign_from_container(list);
			return *this;
		}
		void fill(Type val) {
			col_ref& me = *this;
			for(int i = 0; i < ref.width(); i++) {
				me[i] = val;
			}
		}
		// Seems like defining a move assignment operator deletes the copy constructor. Don't want that.
		col_ref(const col_ref&) = default;
	};
	class const_row_ref {
		friend class vector2d<Type, Alloc>;
		const vector2d<Type, Alloc>& ref;
		size_t y;
		const_row_ref(const vector2d<Type, Alloc>& ref, size_t row) : ref(ref), y(row) {}
	public:
		const Type& operator[](size_t x) const {
			return ref.at(x, y);
		}
	};
	class const_col_ref {
		friend class vector2d<Type, Alloc>;
		const vector2d<Type, Alloc>& ref;
		size_t x;
		const_col_ref(const vector2d<Type, Alloc>& ref, size_t col) : ref(ref), x(col) {}
	public:
		const Type& operator[](size_t y) const {
			return ref.at(x, y);
		}
	};
	col_ref operator[](size_t x) {
		return col_ref(*this, x);
	}
	const_col_ref operator[](size_t x) const {
		return const_col_ref(*this, x);
	}
	col_ref col(size_t x) {
		return col_ref(*this, x);
	}
	const_col_ref col(size_t x) const {
		return const_col_ref(*this, x);
	}
	row_ref row(size_t x) {
		return row_ref(*this, x);
	}
	const_row_ref row(size_t x) const {
		return const_row_ref(*this, x);
	}
	Type& operator()(size_t x, size_t y) {
		return (*this)[x][y];
	}
	const Type& operator()(size_t x, size_t y) const {
		return (*this)[x][y];
	}
	Type& top_left() {
		return (*this)[0][0];
	}
	const Type& top_left() const {
		return (*this)[0][0];
	}
	Type& top_right() {
		return (*this)[0][h - 1];
	}
	const Type& top_right() const {
		return (*this)[0][h - 1];
	}
	Type& bottom_left() {
		return (*this)[w - 1][0];
	}
	const Type& bottom_left() const {
		return (*this)[h - 1][0];
	}
	Type& bottom_right() {
		return (*this)[w - 1][w - 1];
	}
	const Type& bottom_right() const {
		return (*this)[h - 1][w - 1];
	}
	Type& centre() {
		return (*this)[w / 2][h / 2];
	}
	const Type& centre() const {
		return (*this)[w / 2][h / 2];
	}
	size_t width() const {
		return w;
	}
	size_t height() const {
		return h;
	}
	typename std::vector<Type, Alloc>::iterator begin() {
		return data.begin();
	}
	typename std::vector<Type, Alloc>::iterator end() {
		return data.end();
	}
	size_t size() const {
		return data.size();
	}
	void resize(size_t width, size_t height, Type def = Type()) {
		if(w > width) {
			size_t dx = w - width;
			for(int y = 1; y < h; y++) {
				size_t old_begin = w * y, old_end = w * (y + 1) - dx, new_begin = width * y;
				std::move(data.begin() + old_begin, data.begin() + old_end, data.begin() + new_begin);
			}
		}
		size_t old_w = w, old_h = h;
		size_t old_sz = old_w * old_h, new_sz = width * height;
		w = width; h = height;
		if(new_sz > old_sz) {
			data.resize(new_sz, def);
		}
		if(old_w < width) {
			size_t dx = width - old_w;
			for(size_t y = old_h; y > 1; y--) {
				size_t old_begin = old_w * (y - 1), old_end = old_w * y, new_end = w * y - dx;
				if(new_end > new_sz) continue;
				std::move_backward(data.begin() + old_begin, data.begin() + old_end, data.begin() + new_end);
				std::fill_n(data.begin() + old_begin, y - 1, Type());
			}
		}
		if(old_sz > new_sz) {
			data.resize(new_sz, def);
		}
	}
	friend void swap(vector2d& lhs, vector2d& rhs) {
		using std::swap;
		swap(lhs.data, rhs.data);
		swap(lhs.w, rhs.w);
		swap(lhs.h, rhs.h);
	}
	bool empty() const {
		return data.empty();
	}
	void fill(Type val) {
		for(size_t x = 0; x < w; x++) {
			for(size_t y = 0; y < h; y++) {
				(*this)[x][y] = val;
			}
		}
	}
	vector2d() : w(0), h(0) {}
	vector2d(size_t w, size_t h) : vector2d() {
		resize(w,h);
	}
};

#endif
