#include <stdio.h>

#include <windows.h>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/thread.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/bind.hpp"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")

void __stdcall WaitThread(void) {
	MessageBox(NULL, "PC使用禁止時間のため\nシャットダウンします。", "警告", MB_OK);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	if(FALSE == ::OpenProcessToken(::GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		::MessageBoxA(NULL, "OpenProcessToken Error", "Error", MB_OK);
		return 1;
	}
	if(FALSE == ::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) {
		::MessageBoxA(NULL, "LookupPrivilegeValue Error", "Error", MB_OK);
		return 1;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if(FALSE == ::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,reinterpret_cast<TOKEN_PRIVILEGES *>(NULL), 0)) {
		::MessageBoxA(NULL, "AdjustTokenPrivileges Error", "Error", MB_OK);
		return 1;
	}
	boost::shared_ptr<boost::thread> th;
	while(true) {
		boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		tm time_struct = boost::posix_time::to_tm(now);
		if(time_struct.tm_hour < 7 || (time_struct.tm_hour >= 11 && time_struct.tm_hour < 13) || time_struct.tm_hour >= 22) {
			if(!th || th->timed_join(boost::posix_time::seconds(0))) {
				th.reset(new boost::thread(&WaitThread));
			} else {
				if(0 == ::ExitWindowsEx(EWX_POWEROFF, 0)) {
					::MessageBoxA(NULL, "ExitWindowsEx Error", "Error", MB_OK);
					return 1;
				}
				break;
			}
		}
		::Sleep(1000 * 60 * 5);
	}
	return 0;
}
