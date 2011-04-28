#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <ctype.h>

#include "boost/assign.hpp"
#include "boost/crc.hpp"
#include "boost/spirit/home/support/detail/endian.hpp"

using namespace boost::assign;
using namespace boost::integer;

bool PngCheck(std::string &error_message, const char *filename) {
	std::ifstream in(filename, std::ios::binary);
	if(!in.is_open()) {
		error_message = "�t�@�C���I�[�v���Ɏ��s���܂����B";
		return false;
	}
	in.seekg(0, std::ios::end);
	const unsigned int file_size = static_cast<unsigned int>(in.tellg());
	in.seekg(0, std::ios::beg);
#define READ(buffer, size) in.read(buffer, size); if(!in.good()) { error_message = "�t�@�C������̓ǂݍ��݂Ɏ��s���܂����B"; return false; }
	char signature[8];
	const unsigned char png_signature[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1a, 0x0A};
	READ(signature, sizeof(signature));
	if(::memcmp(png_signature, signature, sizeof(signature)) != 0) {
		error_message = "PNG�t�H�[�}�b�g�ł͂���܂���B";
		return false;
	}

	std::map<std::string, unsigned int> chunk_size_list = map_list_of
		("IHDR", 13)("IEND", 0)("gAMA", 4)("cHRM", 32)("sRGB", 1)("pHYs", 9)("tIME", 7);
	bool first = true;
	bool is_plte = false;
	bool plte_found = false;
	bool data_found = false;
	while(true) {
		big32_t chunk_length;
		READ(reinterpret_cast<char *>(&chunk_length), sizeof(chunk_length));

		if(file_size - in.tellg() < chunk_length + 8) {
			error_message = "�`�����N�T�C�Y���s���ł��B";
			return false;
		}
		char chunk_name[4];
		READ(chunk_name, sizeof(chunk_name));
		for(unsigned int i = 0; i < sizeof(chunk_name); i++) {
			if(::isprint(chunk_name[i]) == 0) {
				error_message = "�`�����N�����s���ł��B";
				return false;
			}
		}
		std::map<std::string, unsigned int>::const_iterator it = chunk_size_list.find(std::string(chunk_name, sizeof(chunk_name)));
		if(it != chunk_size_list.end() && it->second != chunk_length) {
			error_message = "�`�����N�T�C�Y���d�l�ƈ�v���Ă��܂���B";
			return false;
		}
		std::vector<unsigned char> chunk_data(chunk_length);
		READ(reinterpret_cast<char *>(&chunk_data.front()), chunk_data.size());
		boost::crc_32_type crc;
		crc.process_bytes(chunk_name, sizeof(chunk_name));
		crc.process_bytes(&chunk_data.front(), chunk_data.size());
		const unsigned int crc_value = crc.checksum();
		big32_t chunk_crc;
		READ(reinterpret_cast<char *>(&chunk_crc), sizeof(chunk_crc));
		if(crc_value != chunk_crc) {
			error_message = "�`�����N��CRC���Ԉ���Ă��܂��B";
			return false;
		}
		if(first) {
			first = false;
			if(::strncmp("IHDR", chunk_name, sizeof(chunk_name)) != 0) {
				error_message = "�擪�̃`�����N��IHDR�ł͂���܂���B";
				return false;
			}
			const unsigned int color_type = chunk_data[9];
			std::vector<unsigned int> list;
			switch(color_type) {
				case 0:
					list += 1,2,4,8,16;
					break;
				case 2:
					list += 8,16;
					break;
				case 3:
					list += 1,2,4,8;
					is_plte = true;
					break;
				case 4:
					list += 8,16;
					break;
				case 6:
					list += 8,16;
					break;
				default :
					error_message = "�s���ȃJ���[�^�C�v�ł��B";
					return false;
			}
			const unsigned int bit_depth = chunk_data[8];
			if(std::find(list.begin(), list.end(), bit_depth) == list.end()) {
				error_message = "�s���ȃr�b�g�[�x�ł��B";
				return false;
			}
		}
		if(::strncmp("PLTE", chunk_name, sizeof(chunk_name)) == 0) {
			if(chunk_length % 3 != 0) {
				error_message = "PLTE�`�����N�̃T�C�Y���s���ł��B";
				return false;
			}
			plte_found = true;
		}
		if(::strncmp("IDAT", chunk_name, sizeof(chunk_name)) == 0) {
			if(is_plte && !plte_found) {
				error_message = "PLTE�`�����N���K�v�ł����AIDAT�`�����N�܂łɌ�����܂���ł����B";
				return false;
			}
			data_found = true;
		}
		if(static_cast<unsigned int>(in.tellg()) == file_size) {
			if(::strncmp("IEND", chunk_name, sizeof(chunk_name)) != 0) {
				error_message = "�I�[�`�����N��IEND�ł͂���܂���B";
				return false;
			}
			break;
		}
		if(::strncmp("IEND", chunk_name, sizeof(chunk_name)) == 0) {
			error_message = "IEND�`�����N���t�@�C���I�[�ȊO�ɑ��݂��Ă͂����܂���B";
			return false;
		}
	}
	if(!data_found) {
		error_message = "IDAT�`�����N�����݂��Ă��܂���B";
		return false;
	}
#undef READ
	return true;
}

int main(unsigned int argc, const char * const *argv) {
	if(argc <= 1) {
		return 1;
	}
	bool result = true;
	for(unsigned int i = 1; result && i < argc; i++) {
		std::string error_message;
		result = PngCheck(error_message, argv[i]);
		if(!result) {
			printf("%s\n", error_message.c_str());
		}
	}
	return result ? 0 : 1;
}


