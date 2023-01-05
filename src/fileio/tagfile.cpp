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
	for(auto& p : pages) p.get().reset();
	std::istringstream page_in;
	std::string cur_page;
	size_t i = 0;
	while(getline(file, cur_page, '\f')) {
		page_in.clear();
		page_in.str(cur_page);
		while(!pages[i].get().canReadFrom(page_in)) {
			i++;
		}
		if(i >= pages.size()) break;
		pages[i].get().readFrom(page_in);
	}
}

void cTagFile::writeTo(std::ostream& file) {
	bool first = true;
	for(const auto& page : pages) {
		if(!first) file << '\f';
		first = false;
		page.get().writeTo(file);
	}
}

cTagFile_Page::cTagFile_Page(cTagFile& owner) {
	owner.pages.push_back(*this);
}

void cTagFile_Page::readFrom(std::istream& file) {
	hasBeenRead = true;
	std::istringstream tag_in;
	for(std::string key; file >> key; ) {
		if(tag_map.count(key) == 0) continue;
		std::string line;
		getline(file, line);
		tag_in.clear();
		tag_in.str(line);
		tag_map.at(key).get().readFrom(key, tag_in);
		tag_in.clear();
	}
}

void cTagFile_Page::writeTo(std::ostream& file) {
	for(const auto& tag : tag_list) {
		tag.get().writeTo(tag.get().key, file);
	}
}

bool cTagFile_Page::canReadFrom(std::istream&) const {
	return !hasBeenRead;
}

void cTagFile_Page::reset() {
	hasBeenRead = false;
}

std::string cTagFile_Page::getFirstKey() const {
	if(tag_list.empty()) return std::string();
	return tag_list.front().get().key;
}

cTagFile_Tag::cTagFile_Tag(cTagFile_Page& owner, const std::string& key) : key(key) {
	owner.tag_map.emplace(key, *this);
	owner.tag_list.push_back(*this);
}

void cTagFile_Tag::readValueFrom(std::istream& file, std::string& to) {
	to = read_maybe_quoted_string(file);
}

void cTagFile_Tag::writeValueTo(std::ostream& file, const std::string& from) {
	file << maybe_quote_string(from);
}

void cTagFile_Tag::readValueFrom(std::istream& file, bool& to) {
	auto f = file.flags();
	file >> std::boolalpha >> to;
	file.flags(f);
}

void cTagFile_Tag::writeValueTo(std::ostream& file, const bool& from) {
	auto f = file.flags();
	file << std::boolalpha << from;
	file.flags(f);
}

void cTagFile_Tag::readValueFrom(std::istream& file, char& to) {
	int temp;
	file >> temp;
	to = char(temp);
}

void cTagFile_Tag::writeValueTo(std::ostream& file, const char& from) {
	file << int(from);
}

void cTagFile_Tag::readValueFrom(std::istream& file, signed char& to) {
	int temp;
	file >> temp;
	to = char(temp);
}

void cTagFile_Tag::writeValueTo(std::ostream& file, const signed char& from) {
	file << int(from);
}

void cTagFile_Tag::readValueFrom(std::istream& file, unsigned char& to) {
	int temp;
	file >> temp;
	to = char(temp);
}

void cTagFile_Tag::writeValueTo(std::ostream& file, const unsigned char& from) {
	file << int(from);
}
