#include <stdio.h>
#include <windows.h>
#include "../keyboard_emulater.h"

/** F12��������[keyboard_emulator call]�Ɠ��͂���T���v��
 *
 * �ꕔ���z�L�[�R�[�h�̓A�X�L�[�R�[�h�Ɠ���ł��鎖�𗘗p���ăL�[���͂��s���Ă���B
 * [_]��3���̓A�X�L�[�R�[�h�Ƃ���Ă��邽�ߎ蓮���́B
 * ����_��shift�Ƃ̓����������Č����邽�߂ɂ����ē���Ă���B
 */
int main(){
	printf("F12�L�[��������[keyboard_emulator call]�Ɠ��͂��܂��B\n");
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
	printf("�I������ɂ�Enter�������Ă�������\n");
	getchar();
	return 0;
}
