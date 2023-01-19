//
//  tagfile.cpp
//  BoE
//
//  Created by Celtic Minstrel on 2022-07-12.
//

#include "tagfile.hpp"
#include <sstream>
#include "fileio.hpp"

void cTagFile::readFrom(std::istream& file) {
	std::istringstream page_in;
	std::string cur_page;
	pages.clear();
	while(getline(file, cur_page, '\f')) {
		page_in.clear();
		page_in.str(cur_page);
		add().readFrom(page_in);
	}
}

void cTagFile::writeTo(std::ostream& file) const {
	bool first = true;
	for(const auto& page : pages) {
		if(!first) file << '\f';
		first = false;
		page.writeTo(file);
	}
}

void cTagFile::clear() {
	pages.clear();
}

cTagFile_Page& cTagFile::add() {
	pages.emplace_back(pages.size());
	return pages.back();
}

cTagFile_Page& cTagFile::operator[](size_t i) {
	return pages.at(i);
}

const cTagFile_Page& cTagFile::operator[](size_t i) const {
	return pages.at(i);
}

cTagFile_Page::cTagFile_Page(size_t i) : i(i) {}

void cTagFile_Page::internal_add_page(const std::string &key) {
	tag_map.emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(std::ref(*this), key));
}

void cTagFile_Page::readFrom(std::istream& file) {
	std::istringstream tag_in;
	for(std::string key; file >> key; ) {
		std::string line;
		getline(file, line);
		tag_in.clear();
		tag_in.str(line);
		internal_add_page(key);
		auto& tag = tag_map.at(key).add();
		tag.readFrom(tag_in);
		tag_list.push_back(tag);
	}
}

void cTagFile_Page::writeTo(std::ostream& file) const {
	for(const auto& tag : tag_list) {
		tag.get().writeTo(file);
	}
}

std::string cTagFile_Page::getFirstKey() const {
	if(tag_list.empty()) return std::string();
	return tag_list.front().get().key;
}

size_t cTagFile_Page::index() const {
	return i;
}

void cTagFile_Page::clear() {
	tag_list.clear();
	tag_map.clear();
}

cTagFile_Tag& cTagFile_Page::add(const std::string& key) {
	internal_add_page(key);
	auto& tag = tag_map.at(key).add();
	tag_list.emplace_back(tag);
	return tag;
}

cTagFile_TagList& cTagFile_Page::operator[](const std::string& key) {
	internal_add_page(key);
	return tag_map.at(key);
}

const cTagFile_TagList& cTagFile_Page::operator[](const std::string& key) const {
	if(tag_map.count(key) > 0) return tag_map.at(key);
	static const cTagFile_TagList empty{};
	return empty;
}

cTagFile_Tag& cTagFile_TagList::add() {
	tags.emplace_back(key);
	return tags.back();
}

cTagFile_Tag& cTagFile_TagList::operator[](size_t i) {
	return tags.at(i);
}

const cTagFile_Tag& cTagFile_TagList::operator[](size_t i) const {
	return tags.at(i);
}

bool cTagFile_TagList::empty() const {
	return tags.empty();
}

size_t cTagFile_TagList::size() const {
	return tags.size();
}

void cTagFile_TagList::internal_add_tag() {
	tags.emplace_back(key);
	parent->tag_list.push_back(tags.back());
}

bool cTagFile_Page::contains(const std::string& key) const {
	return tag_map.count(key) > 0 && !tag_map.at(key).empty();
}

cTagFile_TagList::cTagFile_TagList(cTagFile_Page& parent, const std::string& key)
	: parent(&parent)
	, key(key)
{}

cTagFile_Tag::cTagFile_Tag(const std::string& key) : key(key) {}

void cTagFile_Tag::readFrom(std::istream& file) {
	while(!file.eof()) {
		values.push_back(read_maybe_quoted_string(file));
	}
}

void cTagFile_Tag::writeTo(std::ostream& file) {
	file << key;
	for(const std::string& val : values) {
		file << ' ';
		file << maybe_quote_string(val);
	}
	file << '\n';
}

size_t cTagFile_Tag::extract(size_t i, std::string& to) const {
	if(i < values.size()) to = values[i];
	return 1;
}

size_t cTagFile_Tag::encode(size_t i, const std::string& from) {
	if(i >= values.size()) values.resize(i + 1);
	values[i] = from;
	return 1;
}

size_t cTagFile_Tag::extract(size_t i, bool& to) const {
	if(i < values.size()) {
		to = false;
		if(values[i] == "true" || values[i] == "yes" || values[i] == "on" || values[i] == "1") {
			to = true;
		}
	}
	return 1;
}

size_t cTagFile_Tag::encode(size_t i, const bool& from) {
	if(i >= values.size()) values.resize(i + 1);
	if(from) values[i] = "true";
	else values[i] = "false";
	return 1;
}

size_t cTagFile_Tag::extract(size_t i, char& to) const {
	if(i < values.size()) {
		to = std::stoi(values[i]);
	}
	return 1;
}

size_t cTagFile_Tag::encode(size_t i, const char& from) {
	if(i >= values.size()) values.resize(i + 1);
	values[i] = std::to_string(from);
	return 1;
}

size_t cTagFile_Tag::extract(size_t i, signed char& to) const {
	if(i < values.size()) {
		to = std::stoi(values[i]);
	}
	return 1;
}

size_t cTagFile_Tag::encode(size_t i, const signed char& from) {
	if(i >= values.size()) values.resize(i + 1);
	values[i] = std::to_string(from);
	return 1;
}

size_t cTagFile_Tag::extract(size_t i, unsigned char& to) const {
	if(i < values.size()) {
		to = std::stoi(values[i]);
	}
	return 1;
}

size_t cTagFile_Tag::encode(size_t i, const unsigned char& from) {
	if(i >= values.size()) values.resize(i + 1);
	values[i] = std::to_string(from);
	return 1;
}

size_t cTagFile_Tag::extract(size_t, std::tuple<>&) const {
	return 0;
}

size_t cTagFile_Tag::encode(size_t, const std::tuple<>&) {
	return 0;
}
