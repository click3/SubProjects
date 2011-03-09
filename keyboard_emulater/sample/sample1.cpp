#include <stdio.h>
#include <windows.h>
#include "../keyboard_emulater.h"

/** F12を押すと[keyboard_emulator call]と入力するサンプル
 *
 * 一部仮想キーコードはアスキーコードと同一である事を利用してキー入力を行っている。
 * [_]の3字はアスキーコードとずれているため手動入力。
 * 特に_はshiftとの同時押しを再現するためにあえて入れてある。
 */
int main(){
	printf("F12キーを押すと[keyboard_emulator call]と入力します。\n");
	while(true){
		if(GetAsyncKeyState(VK_F12)<0){
			char *key[2] = {
				"KEYBOARD",
				"EMULATOR CALL"
			};
			int i;
			keyboard_emulater::vkEvent(VK_OEM_4);		//[
			i = 0;
			while(i < strlen(key[0])){
				keyboard_emulater::vkEvent(key[0][i]);	//keyboard
				i++;
			}
			keyboard_emulater::vkEventOn(VK_SHIFT);
			keyboard_emulater::vkEvent(VK_OEM_102);		//_
			keyboard_emulater::vkEventOff(VK_SHIFT);
			i = 0;
			while(i < strlen(key[1])){
				keyboard_emulater::vkEvent(key[1][i]);	//emulator call
				i++;
			}
			keyboard_emulater::vkEvent(VK_OEM_6);		//]
			break;
		}
		Sleep(16);
	}
	printf("終了するにはEnterを押してください\n");
	getchar();
	return 0;
}
