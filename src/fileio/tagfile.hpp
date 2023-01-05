//
//  tagfile.hpp
//  BoE
//
//  Created by Celtic Minstrel on 2022-07-12.
//

#ifndef BoE_tagfile_hpp
#define BoE_tagfile_hpp

#include <string>
#include <vector>
#include <map>
#include <functional> // for reference_wrapper
#include <istream>
#include <ostream>
#include <boost/optional.hpp>

class cTagFile;
class cTagFile_Page;

class cTagFile_Tag {
protected:
	void readValueFrom(std::istream& file, std::string& to);
	void writeValueTo(std::ostream& file, const std::string& from);
	void readValueFrom(std::istream& file, bool& to);
	void writeValueTo(std::ostream& file, const bool& from);
	void readValueFrom(std::istream& file, char& to);
	void writeValueTo(std::ostream& file, const char& from);
	void readValueFrom(std::istream& file, signed char& to);
	void writeValueTo(std::ostream& file, const signed char& from);
	void readValueFrom(std::istream& file, unsigned char& to);
	void writeValueTo(std::ostream& file, const unsigned char& from);

	template<typename T>
	static void readValueFrom(std::istream& file, T& to) {
		file >> to;
	}
	
	template<typename T>
	static void writeValueTo(std::ostream& file, const T& from) {
		file << from;
	}
	
	template<typename A, typename B>
	void readValueFrom(std::istream& file, std::pair<A, B>& to) {
		readValueFrom(file, to.first);
		readValueFrom(file, to.second);
	}
	
	template<typename A, typename B>
	void writeValueTo(std::ostream& file, const std::pair<A, B>& from) {
		writeValueTo(file, from.first); file << ' ';
		writeValueTo(file, from.second);
	}
	
	template<typename A, typename B, typename C>
	void readValueFrom(std::istream& file, std::tuple<A, B, C>& to) {
		readValueFrom(file, std::get<0>(to));
		readValueFrom(file, std::get<1>(to));
		readValueFrom(file, std::get<2>(to));
	}
	
	template<typename A, typename B, typename C>
	void writeValueTo(std::ostream& file, const std::tuple<A, B, C>& from) {
		writeValueTo(file, std::get<0>(from)); file << ' ';
		writeValueTo(file, std::get<1>(from)); file << ' ';
		writeValueTo(file, std::get<2>(from));
	}
	
	template<typename A, typename B, typename C, typename D>
	void readValueFrom(std::istream& file, std::tuple<A, B, C, D>& to) {
		readValueFrom(file, std::get<0>(to));
		readValueFrom(file, std::get<1>(to));
		readValueFrom(file, std::get<2>(to));
		readValueFrom(file, std::get<3>(to));
	}
	
	template<typename A, typename B, typename C, typename D>
	void writeValueTo(std::ostream& file, const std::tuple<A, B, C, D>& from) {
		writeValueTo(file, std::get<0>(from)); file << ' ';
		writeValueTo(file, std::get<1>(from)); file << ' ';
		writeValueTo(file, std::get<2>(from)); file << ' ';
		writeValueTo(file, std::get<3>(from));
	}

public:
	const std::string key;
	cTagFile_Tag(cTagFile_Page& owner, const std::string& key);
	virtual void readFrom(const std::string& key, std::istream& file) = 0;
	virtual void writeTo(const std::string& key, std::ostream& file) = 0;
	virtual ~cTagFile_Tag() = default;
};

class cTagFile_Page {
	friend class cTagFile_Tag;
	std::map<std::string, std::reference_wrapper<cTagFile_Tag>> tag_map;
	std::vector<std::reference_wrapper<cTagFile_Tag>> tag_list;
protected:
	mutable bool hasBeenRead = false;
	std::string getFirstKey() const;
	cTagFile_Page() = default;
public:
	cTagFile_Page(cTagFile& owner);
	virtual void readFrom(std::istream& file);
	virtual void writeTo(std::ostream& file);
	virtual bool canReadFrom(std::istream& file) const;
	virtual void reset();
	virtual ~cTagFile_Page() = default;
};

template<typename Self>
class cTagFile_MultiPage : public cTagFile_Page {
	std::shared_ptr<Self> next;
public:
	using cTagFile_Page::cTagFile_Page;
	void readFrom(std::istream& file) override {
		if(!hasBeenRead) {
			cTagFile_Page::readFrom(file);
		} else {
			if(!next) next = std::make_shared<Self>();
			next->readFrom(file);
		}
	}
	void writeTo(std::ostream& file) override {
		cTagFile_Page::writeTo(file);
		if(next) {
			file << '\f';
			next->writeTo(file);
		}
	}
	bool canReadFrom(std::istream& file) const override {
		auto at = file.tellg();
		std::string key;
		file >> key;
		file.seekg(at);
		return key == getFirstKey();
	}
	void reset() override {
		cTagFile_Page::reset();
		next.reset();
	}
	Self& operator[](size_t i) {
		if(i == 0) return static_cast<Self&>(*this);
		if(!next) next = std::make_shared<Self>();
		return next->operator[](i - 1);
	}
	Self& add() {
		if(next) return next->add();
		next = std::make_shared<Self>();
		return *next;
	}
	size_t size() {
		return 1 + (next ? next->size() : 0);
	}
};

class cTagFile {
	friend class cTagFile_Page;
	std::vector<std::reference_wrapper<cTagFile_Page>> pages;
public:
	void readFrom(std::istream& file);
	void writeTo(std::ostream& file);
};

template<typename T> class cTagFile_ArrayTag;

template<typename T>
class cTagFile_BasicTag : public cTagFile_Tag {
	T value;
public:
	using cTagFile_Tag::cTagFile_Tag;
	void readFrom(const std::string&, std::istream& file) override {
		readValueFrom(file, value);
	}
	void writeTo(const std::string& key, std::ostream& file) override {
		file << key << ' ';
		writeValueTo(file, value);
		file << '\n';
	}
	operator T() const { return value; }
	cTagFile_BasicTag operator=(const T& new_value) { value = new_value; return *this; }
	T& operator*() { return value; }
	const T& operator*() const { return value; }
	T* operator->() { return &value; }
	const T* operator->() const { return &value; }
	bool operator==(const T& other) { return value == other; }
	bool operator!=(const T& other) { return value != other; }
};

template<typename T>
class cTagFile_OptionalTag : public cTagFile_Tag {
	boost::optional<T> value;
public:
	using cTagFile_Tag::cTagFile_Tag;
	void readFrom(const std::string&, std::istream& file) override {
		value.emplace();
		readValueFrom(file, *value);
	}
	void writeTo(const std::string& key, std::ostream& file) override {
		if(value.has_value()) {
			file << key << ' ';
			writeValueTo(file, *value);
			file << '\n';
		}
	}
	const T& get() const { return *value; }
	cTagFile_OptionalTag operator=(const T& new_value) { value = new_value; return *this; }
	cTagFile_OptionalTag operator=(boost::none_t) { value.reset(); return *this; }
	T& operator*() { return value; }
	const T& operator*() const { return value; }
	T* operator->() { return &value; }
	const T* operator->() const { return &value; }
	bool operator==(const T& other) { return value == other; }
	bool operator!=(const T& other) { return value != other; }
	bool operator==(boost::none_t) { return value == boost::none; }
	bool operator!=(boost::none_t) { return value != boost::none; }
};

template<>
class cTagFile_OptionalTag<bool> : public cTagFile_Tag {
	bool value = false;
public:
	using cTagFile_Tag::cTagFile_Tag;
	void readFrom(const std::string&, std::istream&) override {
		value = true;
	}
	void writeTo(const std::string& key, std::ostream& file) override {
		if(value) {
			file << key << '\n';
		}
	}
	const bool& get() const { return value; }
	cTagFile_OptionalTag operator=(const bool& new_value) { value = new_value; return *this; }
	bool& operator*() { return value; }
	const bool& operator*() const { return value; }
	bool operator==(const bool& other) { return value == other; }
	bool operator!=(const bool& other) { return value != other; }
	bool operator==(boost::none_t) { return false; }
	bool operator!=(boost::none_t) { return false; }
};

template<typename T>
class cTagFile_ArrayTag : public cTagFile_Tag {
	std::vector<T> values;
public:
	using cTagFile_Tag::cTagFile_Tag;
	void readFrom(const std::string&, std::istream& file) override {
		values.emplace_back();
		readValueFrom(file, values.back());
	}
	void writeTo(const std::string& key, std::ostream& file) override {
		for(const auto& v : values) {
			file << key << ' ';
			writeValueTo(file, v);
			file << '\n';
		}
	}
	size_t size() const {
		return values.size();
	}
	void add(const T& v) {
		values.push_back(v);
	}
	typename std::vector<T>::const_iterator begin() const {
		return values.begin();
	}
	typename std::vector<T>::const_iterator end() const {
		return values.end();
	}
	const T& operator[](size_t i) const {
		return values.at(i);
	}
};

#endif
