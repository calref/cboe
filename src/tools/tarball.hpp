//
//  tarball.hpp
//  BoE
//
//  Created by Celtic Minstrel on 14-04-19.
//
//

#ifndef BoE_tarball_hpp
#define BoE_tarball_hpp

#include <sstream>
#include <deque>

class tarball {
	struct header_posix_ustar {
		char name[100];
		char mode[8];
		char uid[8];
		char gid[8];
		char size[12];
		char mtime[12];
		char checksum[8];
		char typeflag[1];
		char linkname[100];
		char magic[6];
		char version[2];
		char uname[32];
		char gname[32];
		char devmajor[8];
		char devminor[8];
		char prefix[155];
		char pad[12];
	};
	struct tarfile {
		header_posix_ustar header;
		std::string filename;
		std::stringstream contents;
		// This seems to be necessary to compile in Visual Studio
		// Seemingly, stringstream is non-copyable.
		tarfile() {}
		tarfile(const tarfile& other) {
			contents.str("");
			contents << other.contents.str();
		}
	};
	std::deque<tarfile> files;
	static header_posix_ustar generateTarHeader(const std::string& fileName, unsigned long long fileSize, bool directory=false);
public:
	void writeTo(std::ostream& out);
	void readFrom(std::istream& in);
	std::ostream& newFile(std::string fname);
	void newDirectory(std::string dname);
	std::istream& getFile(std::string fname);
	bool hasFile(std::string fname);
	std::deque<tarfile>::iterator begin() {return files.begin();}
	std::deque<tarfile>::iterator end() {return files.end();}
};


#endif
