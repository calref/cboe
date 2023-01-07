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
#include <memory>
#include <boost/optional.hpp>

class cTagFile;
class cTagFile_Page;

// Helper functions for inheriting constructors in tagfile classes
// This is needed because Xcode 4 does not support inheriting constructors.
// Note that both these macros set the access level to public!
#define DEFINE_TAGFILE_PAGE(Class, Base) protected: Class() = default; public: Class(cTagFile& owner) : Base(owner) {}
#define DEFINE_TAGFILE_MULTIPAGE(Class, Base) friend class pMultiPage<Class>; DEFINE_TAGFILE_PAGE(Class, Base)
#define DEFINE_TAGFILE_TAG(Class, Base) public: Class(cTagFile_Page& owner, const std::string& key) : Base(owner, key) {}

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
		file >> std::boolalpha >> to;
	}
	
	template<typename T>
	static void writeValueTo(std::ostream& file, const T& from) {
		file << std::boolalpha << from;
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
class pMultiPage : public cTagFile_Page {
	std::shared_ptr<Self> next;
	std::shared_ptr<Self> make_page() {
		return std::shared_ptr<Self>(new Self);
	}
protected:
	pMultiPage() : cTagFile_Page() {}
public:
	pMultiPage(cTagFile& owner) : cTagFile_Page(owner) {};
	void readFrom(std::istream& file) override {
		if(!hasBeenRead) {
			cTagFile_Page::readFrom(file);
		} else {
			if(!next) next = make_page();
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
		if(!next) next = make_page();
		return next->operator[](i - 1);
	}
	Self& add() {
		if(next) return next->add();
		next = make_page();
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

template<typename T> class tArrayTag;

template<typename T>
class tBasicTag : public cTagFile_Tag {
	T value;
public:
	DEFINE_TAGFILE_TAG(tBasicTag, cTagFile_Tag);
	void readFrom(const std::string&, std::istream& file) override {
		readValueFrom(file, value);
	}
	void writeTo(const std::string& key, std::ostream& file) override {
		file << key << ' ';
		writeValueTo(file, value);
		file << '\n';
	}
	operator T() const { return value; }
	tBasicTag operator=(const T& new_value) { value = new_value; return *this; }
	T& operator*() { return value; }
	const T& operator*() const { return value; }
	T* operator->() { return &value; }
	const T* operator->() const { return &value; }
	bool operator==(const T& other) { return value == other; }
	bool operator!=(const T& other) { return value != other; }
};

template<typename T>
class tHexTag : public cTagFile_Tag {
	T value;
public:
	DEFINE_TAGFILE_TAG(tHexTag, cTagFile_Tag);
	void readFrom(const std::string&, std::istream& file) override {
		readValueFrom(file, value);
	}
	void writeTo(const std::string& key, std::ostream& file) override {
		file << key << ' ' << std::hex;
		writeValueTo(file, value);
		file << std::dec << '\n';
	}
	operator T() const { return value; }
	tHexTag operator=(const T& new_value) { value = new_value; return *this; }
	T& operator*() { return value; }
	const T& operator*() const { return value; }
	T* operator->() { return &value; }
	const T* operator->() const { return &value; }
	bool operator==(const T& other) { return value == other; }
	bool operator!=(const T& other) { return value != other; }
};

template<typename T>
class tOptionalTag : public cTagFile_Tag {
	boost::optional<T> value;
public:
	DEFINE_TAGFILE_TAG(tOptionalTag, cTagFile_Tag);
	void readFrom(const std::string&, std::istream& file) override {
		value.emplace();
		readValueFrom(file, *value);
	}
	void writeTo(const std::string& key, std::ostream& file) override {
		if(value) {
			file << key << ' ';
			writeValueTo(file, *value);
			file << '\n';
		}
	}
	const T& get() const { return *value; }
	tOptionalTag operator=(const T& new_value) { value = new_value; return *this; }
	tOptionalTag operator=(boost::none_t) { value.reset(); return *this; }
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
class tOptionalTag<bool> : public cTagFile_Tag {
	bool value = false;
public:
	DEFINE_TAGFILE_TAG(tOptionalTag, cTagFile_Tag);
	void readFrom(const std::string&, std::istream&) override {
		value = true;
	}
	void writeTo(const std::string& key, std::ostream& file) override {
		if(value) {
			file << key << '\n';
		}
	}
	const bool& get() const { return value; }
	tOptionalTag operator=(const bool& new_value) { value = new_value; return *this; }
	bool& operator*() { return value; }
	const bool& operator*() const { return value; }
	bool operator==(const bool& other) { return value == other; }
	bool operator!=(const bool& other) { return value != other; }
	bool operator==(boost::none_t) { return false; }
	bool operator!=(boost::none_t) { return false; }
};

template<typename T>
class tArrayTag : public cTagFile_Tag {
	std::vector<T> values;
public:
	DEFINE_TAGFILE_TAG(tArrayTag, cTagFile_Tag)
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
