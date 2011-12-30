
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>
#include <algorithm>

#include <zlib.h>

#include "boost/assert.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/foreach.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"

bool Deflate(const boost::filesystem::path &filepath) {
	const unsigned int file_size = boost::filesystem::file_size(filepath);
	std::vector<unsigned char> raw(file_size);
	boost::filesystem::ifstream ifs(filepath, std::ios::binary);
	if(!ifs.is_open()) {
		return false;
	}
	ifs.read(reinterpret_cast<char *>(&raw.front()), file_size);
	if(!ifs.good()) {
		return false;
	}
	ifs.close();

	std::vector<unsigned char> out_buf(file_size * 10);
	uLongf deflate_size = file_size;
	const int uncomp_result = ::uncompress(&out_buf.front(), &deflate_size, &raw.front(), out_buf.size());
	if(Z_BUF_ERROR != uncomp_result && Z_OK != uncomp_result) {
		return false;
	}

	const boost::filesystem::path out_path = boost::filesystem::path(filepath).replace_extension(".out");
	boost::filesystem::ofstream ofs(out_path, std::ios::binary);
	if(!ofs.is_open()) {
		return false;
	}
	ofs.write(reinterpret_cast<char *>(&out_buf.front()), deflate_size);
	if(!ofs.good()) {
		return false;
	}
	ofs.close();

	return true;
}

int main(unsigned int argc, const char * const *argv) {
	if(argc == 2) {
		for(unsigned int i = 1; i < argc; i++) {
			if(!Deflate(argv[i])) {
				return 1;
			}
		}
	}
	return 0;
}
