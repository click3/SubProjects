#include <iostream>
#include <fstream>
#include <ctype.h>

bool WavCheck(std::string &error_message, const char *filename) {
	std::ifstream in(filename, std::ios::binary);
	if(!in.is_open()) {
		error_message = "�t�@�C���I�[�v���Ɏ��s���܂����B";
		return false;
	}
	in.seekg(0, std::ios::end);
	const unsigned int file_size = static_cast<unsigned int>(in.tellg());
	in.seekg(0, std::ios::beg);
#define READ(buffer, size) in.read(buffer, size); if(!in.good()) { error_message = "�t�@�C������̓ǂݍ��݂Ɏ��s���܂����B"; return false; }
	char signature[4];
	READ(signature, sizeof(signature));
	if(::strncmp("RIFF", signature, sizeof(signature)) != 0) {
		error_message = "RIFF�t�H�[�}�b�g�ł͂���܂���B";
		return false;
	}
	unsigned int wav_file_size;
	READ(reinterpret_cast<char *>(&wav_file_size), sizeof(wav_file_size));
	if(wav_file_size != file_size - 8) {
		error_message = "RIFF�`�����N�̃T�C�Y�Ǝ��T�C�Y���Ⴂ�܂��B";
		return false;
	}
	char riff_type[4];
	READ(riff_type, sizeof(riff_type));
	if(::strncmp("WAVE", riff_type, sizeof(riff_type)) != 0) {
		error_message = "RIFF�t�@�C���ł���WAVE�t�@�C���ł͂���܂���B";
		return false;
	}
	bool first = true;
	bool data_found = false;
	while(static_cast<unsigned int>(in.tellg()) != file_size) {
		char chunk_name[4];
		READ(chunk_name, sizeof(chunk_name));
		for(unsigned int i = 0; i < sizeof(chunk_name); i++) {
			if(::isprint(chunk_name[i]) == 0) {
				error_message = "�`�����N�����s���ł��B";
				return false;
			}
		}
		unsigned int chunk_length;
		READ(reinterpret_cast<char *>(&chunk_length), sizeof(chunk_length));
		if(file_size - in.tellg() < chunk_length) {
			error_message = "�`�����N�T�C�Y���s���ł��B";
			return false;
		}
		if(first) {
			if(::strncmp("fmt ", chunk_name, sizeof(chunk_name)) != 0) {
				error_message = "�擪�̃`�����N��fmt �ł͂���܂���B";
				return false;
			}
			first = false;
		}
		if(::strncmp("data", chunk_name, sizeof(chunk_name)) == 0) {
			data_found = true;
		}
		if(::strncmp("LIST", chunk_name, sizeof(chunk_name)) == 0) {
			char list_type[4];
			READ(list_type, sizeof(list_type));
			for(unsigned int i = 0; i < sizeof(list_type); i++) {
				if(::isprint(list_type[i]) == 0) {
					error_message = "LIST�`�����N�̃t�H�[���^�C�v�����s���ł��B";
					return false;
				}
			}
			in.seekg(chunk_length - 4, std::ios::cur);
		} else {
			in.seekg(chunk_length, std::ios::cur);
		}
		if(chunk_length%2 == 1) {
			in.seekg(1, std::ios::cur);
		}
	}
	if(!data_found) {
		error_message = "data�`�����N�����݂��Ă��܂���B";
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
		result = WavCheck(error_message, argv[i]);
		if(!result) {
			printf("%s\n", error_message.c_str());
		}
	}
	return result ? 0 : 1;
}


