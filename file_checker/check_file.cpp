#include <iostream>
#include <map>
#include <string>
#include <stdio.h>
#include <windows.h>
#include <Shlwapi.h>
#include "boost/assign.hpp"

#pragma comment(lib, "Shlwapi.lib")

using namespace boost::assign;

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

	char command_line[256];
	::sprintf_s(command_line, sizeof(command_line), "%s %s", it->second, filename);

	if(0 == ::CreateProcess(NULL, command_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		return false;
	}

	::WaitForSingleObject(pi.hProcess, INFINITE);
	DWORD exit_code;
	const BOOL result = ::GetExitCodeProcess(pi.hProcess, &exit_code);
	::CloseHandle(pi.hThread);
	::CloseHandle(pi.hProcess);
	if(result == 0 || exit_code != 0) {
		return false;
	}
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
		if(0 != ::FindNextFile(find_handle, &fd)) {
			if(ERROR_NO_MORE_FILES != ::GetLastError()) {
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
