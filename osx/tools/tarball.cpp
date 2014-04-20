//
//  tarball.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-04-19.
//
//

#include "tarball.hpp"
#include <boost/lexical_cast.hpp>

tarball::header_posix_ustar tarball::generateTarHeader(const std::string& fileName, unsigned long long fileSize, bool directory){
	static_assert(sizeof(header_posix_ustar) == 512, "Uh-oh! Padding in the tarball header!");
	if(fileSize > 077777777777LL)
		throw std::length_error("Specified file size >= 8 GB");
	if(fileName.length() >= 100)
		throw std::length_error("Specified file name longer than 99 characters.");
	header_posix_ustar header;
	char* init = (char*) &header;
	for(unsigned int i = 0; i < sizeof(header); i++) init[i] = 0;
	
	snprintf(header.name,100,"%s",fileName.c_str());
	snprintf(header.mode,8,"%07o",0600);
	// leave uid filled with NULs
	// leave gid filled with NULs
	snprintf(header.size,12,"%011llo",fileSize);
	snprintf(header.mtime,12,"%011lo",time(NULL));
	memset(header.checksum,' ',8);
	header.typeflag[0] = directory ? '5' : '0';
	// leave linkname filled with NULs
	snprintf(header.magic,6,"ustar  ");
	snprintf(header.version,2," ");
	// leave uname filled with NULs
	// leave gname filled with NULs
	// leave devmajor filled with NULs
	// leave devminor filled with NULs
	// leave prefix filled with NULs
	// leave pad filled with NULs
	
	unsigned int sum = 0;
	unsigned char* ptr = reinterpret_cast<unsigned char*>(&header);
	for(unsigned int i = 0; i < sizeof(header); i++){
		sum += ptr[i];
	}
	if(sum > 0777777)
		throw std::runtime_error("Checksum overflow");
	snprintf(header.checksum,8,"%o",sum);
	return header;
}

void tarball::writeTo(std::ostream& out) {
	static const char padding[512] = {0};
	for(tarfile& entry : files) {
		entry.contents.seekg(0, std::ios_base::end);
		unsigned long long size = entry.contents.tellg();
		unsigned long long padLength = 512 - size % 512;
		entry.contents.seekg(0);
		entry.header = generateTarHeader(entry.filename, size);
		out.write((char*)&entry.header, sizeof(header_posix_ustar));
		out << entry.contents.rdbuf();
		out.write(padding, padLength);
	}
}

void tarball::readFrom(std::istream& in) {
	static char buf[513];
	while(!in.eof()) {
		files.push_back(tarfile());
		header_posix_ustar& header = files.back().header;
		in.read((char*)&header, sizeof(header_posix_ustar));
		files.back().filename = header.name;
		unsigned long long size = boost::lexical_cast<unsigned long long>(header.size);
		unsigned long long padLength = 512 - size % 512;
		in.read(buf, size);
		files.back().contents.write(buf, size);
		in.seekg(padLength, std::ios_base::cur);
	}
}

std::ostream& tarball::newFile(std::string fname) {
	files.push_back(tarfile());
	files.back().filename = fname;
	return files.back().contents;
}

std::istream& tarball::getFile(std::string fname) {
	for(tarfile& entry : files) {
		if(entry.filename == fname) {
			entry.contents.seekg(0);
			return entry.contents;
		}
	}
	// If the file doesn't exist, return an empty stream
	static std::istringstream empty;
	empty.clear();
	empty.seekg(0);
	return empty;
}
