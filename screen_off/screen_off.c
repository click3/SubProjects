#include <windows.h>
#include <stdio.h>

#pragma comment (lib,"user32.lib")

main(){
	SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER,  2);
	return 0;
}
