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
	size_t extract(size_t i, T& to) const {
		if(i >= values.size()) return 1;
		// This bizarre construct simply allows us to not include <sstream> here
		using stream = typename std::conditional<std::is_same<T, T>::value, std::istringstream, void>::type;
		stream file(values[i]);
		file >> to;
		return 1;
	}
	
	template<typename T>
	size_t encode(size_t i, const T& from) {
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

	const std::string key;
	cTagFile_Tag(const std::string& key);
	void readFrom(std::istream& file);
	void writeTo(std::ostream& file);
	template<typename T>
	bool operator==(const T& other) {
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
	operator bool() const {
		return tag;
	}
	template<typename T>
	cTagFile_TagExtractProxy operator>>(T& value) {
		if(tag == nullptr) return *this;
		size_t j = i;
		j += tag->extract(i, value);
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
	cTagFile_Page* parent;
	std::deque<cTagFile_Tag> tags;
	mutable size_t i = 0;
	cTagFile_TagList(const cTagFile_TagList&) = delete;
	void internal_add_tag();
public:
	const std::string key;
	cTagFile_TagList(cTagFile_Page& parent, const std::string& key);
	cTagFile_Tag& add();
	cTagFile_Tag& operator[](size_t n);
	const cTagFile_Tag& operator[](size_t n) const;
	bool empty() const;
	template<typename T>
	cTagFile_TagEncodeProxy operator<<(const T& value) {
		internal_add_tag();
		return cTagFile_TagEncodeProxy(tags.back()) << value;
	}
	template<typename T>
	cTagFile_TagExtractProxy operator>>(T& value) const {
		if(i >= tags.size()) {
			i = 0;
			return cTagFile_TagExtractProxy() >> value;
		} else {
			return cTagFile_TagExtractProxy(tags[i++]) >> value;
		}
	}
	template<typename Container>
	void extract(Container& values) const {
		using T = typename Container::value_type;
		values.clear();
		for(size_t n = 0; n < tags.size(); n++) {
			T value;
			*this >> value;
			values.push_back(value);
			if(i == 0) break;
		}
	}
	template<typename Container>
	void encode(const Container& values) {
		for(const auto& value : values) {
			*this << value;
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
protected:
	std::string getFirstKey() const;
public:
	cTagFile_Page() = default;
	void readFrom(std::istream& file);
	void writeTo(std::ostream& file) const;
	cTagFile_Tag& add(const std::string& key);
	bool contains(const std::string& key) const;
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
	cTagFile_Page& add();
	cTagFile_Page& operator[](size_t n);
	const cTagFile_Page& operator[](size_t n) const;
	iterator begin() { return pages.begin(); }
	iterator end() { return pages.end(); }
	const_iterator begin() const { return pages.begin(); }
	const_iterator end() const { return pages.end(); }
	
};

#endif
