#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <ctype.h>
#include <windows.h>

bool CharToWChar(std::vector<wchar_t> &result, const char *buffer, unsigned int buffer_size, unsigned int code_page) {
	if(buffer == NULL) {
		return false;
	}
	const int size = ::MultiByteToWideChar(code_page, MB_ERR_INVALID_CHARS, buffer, static_cast<int>(buffer_size), NULL, 0);
	if(size <= 0) {
		return false;
	}
	result.resize(static_cast<unsigned int>(size));
	const int convert_size = ::MultiByteToWideChar(code_page, MB_ERR_INVALID_CHARS, buffer, static_cast<int>(buffer_size), &result.front(), static_cast<int>(result.size()));
	if(convert_size <= 0) {
		return false;
	}
	return true;
}

#define MULTI_TO_WIDE_PROCS(name, code_page)													\
bool name##ToWChar(std::vector<wchar_t> &result, const char *str) {										\
	return CharToWChar(result, str, strlen(str), code_page);										\
}																			\
bool name##ToWChar(std::vector<wchar_t> &result, const std::vector<char> &str) {								\
	return CharToWChar(result, &str.front(), str.size(), code_page);									\
}

MULTI_TO_WIDE_PROCS(SJIS, CP_ACP)
MULTI_TO_WIDE_PROCS(UTF8, CP_UTF8)


struct CheckNoPrintString {
	bool operator()(bool value, wchar_t a) const {
		return value && (::iswcntrl(a) == 0 || ::iswspace(a) != 0);
	}
};


bool TextCheck(std::string &error_message, const char *filename) {
	std::ifstream in(filename, std::ios::binary);
	if(!in.is_open()) {
		error_message = "ファイルオープンに失敗しました。";
		return false;
	}
	in.seekg(0, std::ios::end);
	const unsigned int file_size = static_cast<unsigned int>(in.tellg());
	in.seekg(0, std::ios::beg);
#define READ(buffer, size) in.read(buffer, size); if(!in.good()) { error_message = "ファイルからの読み込みに失敗しました。"; return false; }

	if(file_size == 0) {
		return true;
	}
	std::vector<char> buffer(file_size);
	READ(&buffer.front(), file_size);
	std::vector<wchar_t> result;
	if(!SJISToWChar(result, buffer) && !UTF8ToWChar(result, buffer)) {
		error_message = "不明な文字エンコードです。";
		return false;
	}
	const bool is_no_print_char = std::accumulate(result.begin(), result.end(), true, CheckNoPrintString());
	if(!is_no_print_char) {
		error_message = "印字不可能文字が記述されています。";
		return false;
	}

	return true;
}

int main(unsigned int argc, const char * const *argv) {
	if(argc <= 1) {
		return 1;
	}
	bool result = true;
	for(unsigned int i = 1; result && i < argc; i++) {
		std::string error_message;
		result = TextCheck(error_message, argv[i]);
		if(!result) {
			printf("%s\n", error_message.c_str());
		}
	}
	return result ? 0 : 1;
}