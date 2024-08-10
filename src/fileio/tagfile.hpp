//
//  tagfile.hpp
//  BoE
//
//  Created by Celtic Minstrel on 2022-07-12.
//

#ifndef BoE_tagfile_hpp
#define BoE_tagfile_hpp

#include <string>
#include <deque>
#include <map>
#include <functional> // for reference_wrapper
#include <istream>
#include <ostream>
#include <memory>
#include <boost/optional.hpp>
#include "tools/vector2d.hpp"

namespace detail {
	// To allow ADL with custom begin/end
	using std::begin;
	using std::end;

	template <typename T>
	auto is_iterable_impl(int)
	-> decltype (
		begin(std::declval<T&>()) != end(std::declval<T&>()), // begin/end and operator !=
		void(), // Handle evil operator ,
		++std::declval<decltype(begin(std::declval<T&>()))&>(), // operator ++
		void(*begin(std::declval<T&>())), // operator*
		std::true_type{}
	);

	template <typename T>
	std::false_type is_iterable_impl(...);
	
	template<typename T>
	struct is_container : decltype(detail::is_iterable_impl<T>(0)) {};
	
	template<typename T, size_t n>
	struct is_container<T[n]> : std::false_type {};
	
	template<>
	struct is_container<std::string> : std::false_type {};
	
	template<typename T>
	struct is_pair : public std::false_type {};
	
	template<typename A, typename B>
	struct is_pair<std::pair<A, B>> : public std::true_type {};
}

class cTagFile;
class cTagFile_Page;

class cTagFile_Tag {
	std::deque<std::string> values;
public:
	// All extract/encode functions take a starting index and
	// return the number of values extracted/encoded (usually 1).
	size_t extract(size_t i, cTagFile_Tag&) const = delete;
	size_t encode(size_t i, const cTagFile_Tag&) = delete;
	size_t extract(size_t i, std::string& to) const;
	size_t encode(size_t i, const std::string& from);
	size_t extract(size_t i, bool& to) const;
	size_t encode(size_t i, const bool& from);
	size_t extract(size_t i, char& to) const;
	size_t encode(size_t i, const char& from);
	size_t extract(size_t i, signed char& to) const;
	size_t encode(size_t i, const signed char& from);
	size_t extract(size_t i, unsigned char& to) const;
	size_t encode(size_t i, const unsigned char& from);
	size_t extract(size_t i, std::tuple<>& to) const;
	size_t encode(size_t i, const std::tuple<>& from);

	template<typename T>
	typename std::enable_if<!detail::is_container<T>::value, size_t>::type
	extract(size_t i, T& to) const {
		if(i >= values.size()) return 0;
		// This bizarre construct simply allows us to not include <sstream> here
		using stream = typename std::conditional<std::is_same<T, T>::value, std::istringstream, void>::type;
		stream file(values[i]);
		file >> to;
		return 1;
	}
	
	template<typename T>
	typename std::enable_if<!detail::is_container<T>::value, size_t>::type
	encode(size_t i, const T& from) {
		if(i >= values.size()) values.resize(i + 1);
		// This bizarre construct simply allows us to not include <sstream> here
		using stream = typename std::conditional<std::is_same<T, T>::value, std::ostringstream, void>::type;
		stream file;
		file << from;
		values[i] = file.str();
		return 1;
	}
	
	template<typename A, typename B>
	size_t extract(size_t i, std::pair<A, B>& to) const {
		return extract(i, to.first) + extract(i + 1, to.second);
	}
	
	template<typename A, typename B>
	size_t encode(size_t i, const std::pair<A, B>& from) {
		return encode(i, from.first) + encode(i + 1, from.second);
	}
	
private:
	template<typename... T>
	struct handle_tuple {
		template<size_t j = 0>
		typename std::enable_if<j < sizeof...(T), size_t>::type
		extract(const cTagFile_Tag& self, size_t i, std::tuple<T...>& to) {
			if(j >= sizeof...(T)) return 0;
			return self.extract(i, std::get<j>(to)) + extract<j + 1>(self, i + 1, to);
		}
		template<size_t j = 0>
		typename std::enable_if<j >= sizeof...(T), size_t>::type
		extract(const cTagFile_Tag&, size_t, std::tuple<T...>&) {
			return 0;
		}
		template<size_t j = 0>
		typename std::enable_if<j < sizeof...(T), size_t>::type
		encode(cTagFile_Tag& self, size_t i, const std::tuple<T...>& from) {
			if(j >= sizeof...(T)) return 0;
			return self.encode(i, std::get<j>(from)) + encode<j + 1>(self, i + 1, from);
		}
		template<size_t j = 0>
		typename std::enable_if<j >= sizeof...(T), size_t>::type
		encode(cTagFile_Tag&, size_t, const std::tuple<T...>&) {
			return 0;
		}
	};
	
public:
	template<typename... T>
	size_t extract(size_t i, std::tuple<T...>& to) const {
		handle_tuple<T...> handle;
		return handle.extract(*this, i, to);
	}
	
	template<typename... T>
	size_t encode(size_t i, const std::tuple<T...>& from) {
		handle_tuple<T...> handle;
		return handle.encode(*this, i, from);
	}
	
	template<typename T, size_t n>
	size_t extract(size_t i, std::array<T, n>& to) const {
		for(size_t j = 0; j < n; j++) {
			extract(i + j, to[j]);
		}
		return n;
	}
	
	template<typename T, size_t n>
	size_t encode(size_t i, const std::array<T, n>& from) {
		for(size_t j = 0; j < n; j++) {
			encode(i + j, from[j]);
		}
		return n;
	}
	
	template<typename T>
	size_t extract(size_t i, boost::optional<T>& to) const {
		if(i < values.size()) {
			return extract(i, *to);
		} else {
			to = boost::none;
			return 0;
		}
	}
	
	template<typename T>
	size_t encode(size_t i, const boost::optional<T>& from) {
		return from ? encode(i, *from) : 0;
	}
	
	template<typename Container>
	typename std::enable_if<detail::is_container<Container>::value, size_t>::type
	extract(size_t i, Container& to) const {
		using T = typename Container::value_type;
		to.clear();
		for(size_t n = i; n < values.size(); n++) {
			T value;
			extract(n, value);
			to.push_back(value);
		}
		return to.size();
	}
	
	template<typename Container>
	typename std::enable_if<detail::is_container<Container>::value, size_t>::type
	encode(size_t i, const Container& from) {
		for(const auto& value : from) {
			encode(i++, value);
		}
		return from.size();
	}
	
	const std::string key;
	cTagFile_Tag(const std::string& key);
	void readFrom(std::istream& file);
	void writeTo(std::ostream& file);
	template<typename T>
	bool operator==(const T& other) const {
		T self;
		if(extract(0, self) == 0) return false;
		return self == other;
	}
	
};

class cTagFile_TagExtractProxy {
	const cTagFile_Tag* tag;
	size_t i = 0;
	cTagFile_TagExtractProxy(const cTagFile_Tag& tag, size_t i) : tag(&tag), i(i) {}
public:
	cTagFile_TagExtractProxy() : tag(nullptr) {}
	cTagFile_TagExtractProxy(const cTagFile_Tag& tag) : tag(&tag) {}
	explicit operator bool() const {
		return tag;
	}
	template<typename T>
	cTagFile_TagExtractProxy operator>>(T& value) {
		if(tag == nullptr) return *this;
		size_t j = i;
		j += tag->extract(i, value);
		if(i == j) return cTagFile_TagExtractProxy();
		return cTagFile_TagExtractProxy(*tag, j);
	}
	template<typename T>
	cTagFile_TagExtractProxy operator>>(const T& value) {
		if(tag == nullptr) return *this;
		size_t j = i;
		T check = value;
		j += tag->extract(i, check);
		if(check != value) {
			return cTagFile_TagExtractProxy();
		}
		return cTagFile_TagExtractProxy(*tag, j);
	}
};

class cTagFile_TagEncodeProxy {
	cTagFile_Tag* tag;
	size_t i = 0;
	cTagFile_TagEncodeProxy(cTagFile_Tag& tag, size_t i) : tag(&tag), i(i) {}
public:
	cTagFile_TagEncodeProxy(cTagFile_Tag& tag) : tag(&tag) {}
	template<typename T>
	cTagFile_TagEncodeProxy operator<<(const T& value) {
		if(tag == nullptr) return *this;
		size_t j = i;
		j += tag->encode(i, value);
		return cTagFile_TagEncodeProxy(*tag, j);
	}
};

class cTagFile_TagList {
	friend class cTagFile_Page;
	cTagFile_Page* parent = nullptr;
	std::deque<cTagFile_Tag> tags;
	mutable size_t i = 0;
	cTagFile_TagList(const cTagFile_TagList&) = delete;
	void internal_add_tag();
	cTagFile_TagList() = default;
public:
	const std::string key;
	cTagFile_TagList(cTagFile_Page& parent, const std::string& key);
	cTagFile_Tag& add();
	cTagFile_Tag& operator[](size_t n);
	const cTagFile_Tag& operator[](size_t n) const;
	bool empty() const;
	size_t size() const;
	template<typename T>
	cTagFile_TagEncodeProxy operator<<(const T& value) {
		internal_add_tag();
		return cTagFile_TagEncodeProxy(tags.back()) << value;
	}
	template<typename T>
	cTagFile_TagExtractProxy operator>>(T&& value) const {
		if(i >= tags.size()) {
			i = 0;
			return cTagFile_TagExtractProxy() >> std::forward<T>(value);
		} else {
			return cTagFile_TagExtractProxy(tags[i++]) >> std::forward<T>(value);
		}
	}
	template<typename Container>
	void extract(Container& values) const {
		using T = typename Container::value_type;
		values.clear();
		i = 0;
		for(size_t n = 0; n < tags.size(); n++) {
			T value;
			*this >> value;
			values.push_back(value);
		}
	}
	template<typename Container>
	void encode(const Container& values) {
		for(const auto& value : values) {
			*this << value;
		}
	}
	template<typename Container>
	typename std::enable_if<!detail::is_pair<typename Container::value_type>::value>::type
	extractSparse(Container& values, typename Container::value_type def = typename Container::value_type()) const {
		using T = typename Container::value_type;
		values.clear();
		i = 0;
		for(size_t n = 0; n < tags.size(); n++) {
			size_t i = 0;
			T value;
			if(*this >> i >> value) {
				if(i >= values.size()) values.resize(i + 1, def);
				values[i] = value;
			}
		}
	}
	template<typename Container>
	typename std::enable_if<!detail::is_pair<typename Container::value_type>::value>::type
	encodeSparse(const Container& values, typename Container::value_type def = typename Container::value_type()) {
		size_t i = 0;
		for(const auto& value : values) {
			if(value != def) {
				*this << i << value;
			}
			i++;
		}
	}
	template<typename Container>
	typename std::enable_if<detail::is_pair<typename Container::value_type>::value>::type
	extractSparse(Container& values) const {
		using T = std::pair<
			typename std::remove_cv<typename Container::value_type::first_type>::type,
			typename Container::value_type::second_type
		>;
		values.clear();
		i = 0;
		for(size_t n = 0; n < tags.size(); n++) {
			T value;
			*this >> value.first >> value.second;
			values.insert(value);
		}
	}
	template<typename Container>
	typename std::enable_if<detail::is_pair<typename Container::value_type>::value>::type
	encodeSparse(const Container& values, typename Container::value_type::second_type def = typename Container::value_type::second_type()) {
		for(const auto& value : values) {
			if(value.second != def) {
				*this << value.first << value.second;
			}
		}
	}
	template<typename T>
	void extract(vector2d<T>& values) const {
		values.resize(0, tags.size());
		i = 0;
		for(size_t n = 0; n < tags.size(); n++) {
			std::vector<T> row;
			tags[n].extract(0, row);
			if(row.size() >= values.width()) {
				values.resize(row.size(), tags.size());
			}
			values.row(n) = row;
		}
	}
	template<typename T>
	void encode(const vector2d<T>& values) {
		for(size_t row = 0; row < values.height(); row++) {
			std::vector<T> row_contents;
			row_contents.reserve(values.width());
			for(size_t col = 0; col < values.width(); col++) {
				row_contents.push_back(values.row(row)[col]);
			}
			internal_add_tag();
			tags.back().encode(0, row_contents);
		}
	}
	template<typename T>
	void extractSparse(vector2d<T>& values, T def = T()) const {
		values.resize(0, 0);
		i = 0;
		for(size_t n = 0; n < tags.size(); n++) {
			size_t x = 0, y = 0;
			T value;
			if(*this >> x >> y >> value) {
				if(x >= values.width() || y >= values.height()) {
					values.resize(std::max(x + 1, values.width()), std::max(y + 1, values.height()), def);
				}
				values[x][y] = value;
			}
		}
	}
	template<typename T>
	void encodeSparse(const vector2d<T>& values, T def = T()) {
		for(size_t x = 0; x < values.width(); x++) {
			for(size_t y = 0; y < values.height(); y++) {
				const auto& value = values[x][y];
				if(value != def) {
					*this << x << y << value;
				}
			}
		}
	}
};

class cTagFile_Page {
	friend class cTagFile_TagList;
	// Tags are kept in a map by key, and there can be more than one of each tag.
	std::map<std::string, cTagFile_TagList> tag_map;
	// However, we also record the real order the tags were encountered in.
	std::deque<std::reference_wrapper<cTagFile_Tag>> tag_list;
	cTagFile_Page(const cTagFile_Page&) = delete;
	void internal_add_page(const std::string& key);
	const size_t i;
public:
	std::string getFirstKey() const;
	cTagFile_Page(size_t index);
	void readFrom(std::istream& file);
	void writeTo(std::ostream& file) const;
	void clear();
	cTagFile_Tag& add(const std::string& key);
	bool contains(const std::string& key) const;
	size_t index() const;
	cTagFile_TagList& operator[](const std::string& key);
	const cTagFile_TagList& operator[](const std::string& key) const;
};

class cTagFile {
	std::deque<cTagFile_Page> pages;
	using iterator = typename std::deque<cTagFile_Page>::iterator;
	using const_iterator = typename std::deque<cTagFile_Page>::const_iterator;
public:
	void readFrom(std::istream& file);
	void writeTo(std::ostream& file) const;
	void clear();
	cTagFile_Page& add();
	cTagFile_Page& operator[](size_t n);
	const cTagFile_Page& operator[](size_t n) const;
	iterator begin() { return pages.begin(); }
	iterator end() { return pages.end(); }
	const_iterator begin() const { return pages.begin(); }
	const_iterator end() const { return pages.end(); }
	
};

template<typename Int, typename = typename std::enable_if<std::is_integral<Int>::value>::type>
struct as_hex {
	Int value;
	as_hex(Int value = Int()) : value(value) {}
	as_hex& operator= (Int newval) { value = newval; return *this; }
	operator Int() { return value; }
};

template<typename Int>
inline std::ostream& operator<<(std::ostream& os, const as_hex<Int>& value) {
	auto f = os.flags();
	os << std::hex << value.value;
	os.flags(f);
	return os;
}

template<typename Int>
inline std::istream& operator>>(std::istream& is, as_hex<Int>& ref) {
	auto f = is.flags();
	is >> std::hex >> ref.value;
	is.flags(f);
	return is;
}

#endif
