#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <windows.h>
#include <Shlwapi.h>
#include "boost/assign.hpp"

#pragma comment(lib, "Shlwapi.lib")

using namespace boost::assign;

#undef GetModuleFileName
bool GetModuleFileName(std::vector<char> &str) {
	::SetLastError(ERROR_SUCCESS);
	unsigned int size = 256;
	while(true) {
		str.resize(size);
		const unsigned int length = ::GetModuleFileNameA(NULL, &str.front(), str.size());
		const DWORD error = ::GetLastError();
		if(error == ERROR_SUCCESS && length <= size) {
			if(length < size) {
				str.resize(length);
			}
			break;
		} else if(error == ERROR_INSUFFICIENT_BUFFER) {
			size *= 2;
			continue;
		} else {
			str.clear();
			return false;
		}
	}
	return true;
}

void GetAppDir(std::vector<char> &str) {
	const bool get_result = GetModuleFileName(str);
	BOOST_ASSERT(get_result);
	BOOST_ASSERT(!str.empty());
	const BOOL path_remove_result = ::PathRemoveFileSpec(&str.front());
	BOOST_ASSERT(path_remove_result == TRUE);
	str.resize(::strlen(&str.front()) + 1);
}

bool CheckFile(const char *filename) {
	const std::map<std::string, const char *> ext_list = map_list_of
		(".txt", "text_checker")(".wav", "wav_checker")(".png", "png_checker");
	const char * const file_ext = ::PathFindExtensionA(filename);
	const std::map<std::string, const char *>::const_iterator it = ext_list.find(file_ext);
	if(it == ext_list.end()) {
		return true;
	}
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	::ZeroMemory(&si, sizeof(si));
	::ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	std::vector<char> dir;
	GetAppDir(dir);
	char command_line[256];
	::sprintf_s(command_line, sizeof(command_line), "%s\\%s \"%s\"", &dir.front(), it->second, filename);

	char cur_dir[256];
	::GetCurrentDirectory(sizeof(cur_dir), cur_dir);
	printf("%s\\%s ", cur_dir, filename);

	if(0 == ::CreateProcess(NULL, command_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		std::cout << command_line << "‚ÉŽ¸”s‚µ‚Ü‚µ‚½B" << std::endl;
		return false;
	}

	::WaitForSingleObject(pi.hProcess, INFINITE);
	DWORD exit_code;
	const BOOL result = ::GetExitCodeProcess(pi.hProcess, &exit_code);
	::CloseHandle(pi.hThread);
	::CloseHandle(pi.hProcess);
	if(result == 0 || exit_code != 0) {
		std::cout << it->second << "‚ªŽ¸”s‚µ‚Ü‚µ‚½B" << std::endl;
		return false;
	}

	printf(" OK\n");
	return true;
}

bool ProcFile(const char *filename) {
	if(::PathIsDirectory(filename) == 0) {
		return ::CheckFile(filename);
	}
	if(::PathIsDirectoryEmpty(filename) != 0) {
		return true;
	}

	char dir[256];
	::GetCurrentDirectory(sizeof(dir), dir);
	::SetCurrentDirectory(filename);
	WIN32_FIND_DATA fd;
	const HANDLE find_handle = ::FindFirstFile("*", &fd);
	if(find_handle == INVALID_HANDLE_VALUE) {
		::SetCurrentDirectory(dir);
		printf("FindFirstFile‚ÉŽ¸”s‚µ‚Ü‚µ‚½B\n");
		return false;
	}
	bool result = true;
	while(true) {
		if(fd.cFileName[0] != '.') {
			if(!::ProcFile(fd.cFileName)) {
				result = false;
				break;
			}
		}
		if(0 == ::FindNextFile(find_handle, &fd)) {
			if(ERROR_NO_MORE_FILES != ::GetLastError()) {
				printf("FindNextFile‚ÉŽ¸”s‚µ‚Ü‚µ‚½B\n");
				result = false;
			}
			break;
		}
	}
	::FindClose(find_handle);
	::SetCurrentDirectory(dir);
	return result;
}

int main(unsigned int argc, const char * const argv[]) {
	for(unsigned int i = 1; i < argc; i++) {
		if(!ProcFile(argv[i])) {
			return 1;
		}
	}
	return 0;
}
