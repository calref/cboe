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

#include <functional>
#include <vector>

template<typename Type, typename Alloc = std::allocator<Type>> class vector2d {
	friend class row_ref;
	friend class col_ref;
	friend class const_row_ref;
	friend class const_col_ref;
	std::vector<Type, Alloc> data;
	size_t w, h;
	std::function<Type()> get_default_value_function=nullptr;
public:
	using value_type = Type;
	class row_ref {
		friend class vector2d<Type, Alloc>;
		vector2d<Type, Alloc>& ref;
		size_t y;
		row_ref(vector2d<Type, Alloc>& ref, size_t row) : ref(ref), y(row) {}
	public:
		Type& operator[](size_t x) {
			if (x<ref.w)
				return ref.data[ref.w * y + x];
			return ref.get_bad_value();
		}
		const Type& operator[](size_t x) const {
			if (x<ref.w)
				return ref.data[ref.w * y + x];
			return ref.get_bad_value();
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
		// Seems like defining a move assignment operator deletes the copy constructor. Don't want that.
		row_ref(const row_ref&) = default;
	};
	class col_ref {
		friend class vector2d<Type, Alloc>;
		vector2d<Type, Alloc>& ref;
		size_t x;
		col_ref(vector2d<Type, Alloc>& ref, size_t col) : ref(ref), x(col) {}
	public:
		Type& operator[](size_t y) {
			if (y<ref.h)
				return ref.data[ref.w * y + x];
			return ref.get_bad_value();
		}
		const Type& operator[](size_t y) const {
			if (y<ref.h)
				return ref.data[ref.w * y + x];
			return ref.get_bad_value();
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
			if (x<ref.w)
				return ref.data[ref.w * y + x];
			return ref.get_bad_value();
		}
	};
	class const_col_ref {
		friend class vector2d<Type, Alloc>;
		const vector2d<Type, Alloc>& ref;
		size_t x;
		const_col_ref(const vector2d<Type, Alloc>& ref, size_t col) : ref(ref), x(col) {}
	public:
		const Type& operator[](size_t y) const {
			if (y<ref.h)
				return ref.data[ref.w * y + x];
			return ref.get_bad_value();
		}
	};
	col_ref operator[](size_t x) {
		if (x<w)
			return col_ref(*this, x);
		return get_bad_vector()[0];
	}
	const_col_ref operator[](size_t x) const {
		if (x<w)
			return const_col_ref(*this, x);
		return get_bad_const_vector()[0];
	}
	col_ref col(size_t x) {
		if (x<w)
			return col_ref(*this, x);
		return get_bad_vector()[0];
	}
	const_col_ref col(size_t x) const {
		if (x<w)
			return const_col_ref(*this, x);
		return get_bad_const_vector()[0];
	}
	row_ref row(size_t y) {
		if (y<h)
			return row_ref(*this, y);
		return get_bad_vector().row(0);
	}
	const_row_ref row(size_t y) const {
		if (y<h)
			return const_row_ref(*this, y);
		return get_bad_const_vector().row(0);
	}
	Type& operator()(size_t x, size_t y) {
		if (x<w && y<h)
			return (*this)[x][y];
		return get_bad_value();
	}
	const Type& operator()(size_t x, size_t y) const {
		if (x<w && y<h)
			return (*this)[x][y];
		return get_bad_value();
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
	void resize(size_t width, size_t height) {
		if(w > width) {
			size_t dx = w - width;
			for(int y = 1; y < h; y++) {
				std::move(data.begin() + w * y, data.begin() + w * (y + 1) - dx, data.begin() + width * y);
			}
		}
		size_t old_w = w, old_h = h;
		w = width; h = height;
		data.resize(w * h);
		// ASAN undefined behaviour if old_h==0, y=old_h-1 is ...
		if(old_w < width && old_h) {
			size_t dx = width - old_w;
			for(int y = old_h - 1; y > 0; y--) {
				std::move_backward(data.begin() + old_w * y, data.begin() + old_w * (y + 1), data.begin() + w * (y + 1) - dx);
				std::fill_n(data.begin() + old_w + w * (y - 1), dx, Type());
			}
		}
	}
	void swap(vector2d& other) {
		data.swap(other.data);
		std::swap(w, other.w);
		std::swap(h, other.h);
	}
	bool empty() const {
		return data.empty();
	}
	vector2d() : w(0), h(0) {}
	vector2d(size_t w, size_t h) : w(w), h(h) {
		resize(w,h);
	}
	
	Type &get_bad_value() {
		static Type badValue;
		badValue=get_default_value_function ? get_default_value_function() : Type(-1);
		return badValue;
	}
	const Type &get_bad_value() const {
		static Type badValue;
		badValue = get_default_value_function ? get_default_value_function() : Type(-1);
		return badValue;
	}
	vector2d &get_bad_vector() {
		static vector2d bad_vector(1,1);
		bad_vector.data[0]=get_bad_value();
		bad_vector.set_get_default_function(get_default_value_function);
		return bad_vector;
	}
	vector2d const &get_bad_const_vector() const {
		static vector2d bad_vector(1,1);
		bad_vector.data[0]=get_bad_value();
		bad_vector.set_get_default_function(get_default_value_function);
		return bad_vector;
	}
	void set_get_default_function(std::function<Type()> const &func) {
		get_default_value_function=func;
	}
};

#endif
