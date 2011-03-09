#include "keyboard_emulater.h"

#pragma comment (lib,"user32.lib")
#pragma comment (lib,"winmm.lib")

//コンストラクタ
void keyboard_emulater::initialize(){
	mCounter = &mNum;
	mNum = 0;
	mKeyFrame = 0;
	mList.clear();
}
keyboard_emulater::keyboard_emulater(){
	initialize();
}
keyboard_emulater::keyboard_emulater(int *counter){
	initialize();
	mCounter = counter;
}
keyboard_emulater::keyboard_emulater(int keyFrame){
	initialize();
	mKeyFrame = keyFrame;
}
keyboard_emulater::keyboard_emulater(int *counter, int keyFrame){
	initialize();
	mCounter = counter;
	mKeyFrame = keyFrame;
}
keyboard_emulater::keyboard_emulater(int keyFrame, int *counter){
	initialize();
	mCounter = counter;
	mKeyFrame = keyFrame;
}


//アクセサ
int keyboard_emulater::setKeyFrame(int keyFrame){
	int temp = mKeyFrame;
	mKeyFrame = keyFrame;
	return temp;
}
int *keyboard_emulater::setCounter(int *counter){
	int *temp = mCounter;
	mCounter = counter;
	return temp;
}
int keyboard_emulater::getKeyFrame(){
	return mKeyFrame;
}
int *keyboard_emulater::getCounter(){
	return mCounter;
}


//イベントループ
int keyboard_emulater::run(){
	std::vector<KEY_ACTION>::iterator p = mList.end();
	std::vector<KEY_ACTION>::iterator s = mList.begin();
	int i = 0;
	while(p != s){
		p--;
		if((*p).frame < *mCounter){
			event((*p).vk,(*p).dik,(*p).flag);
			p = mList.erase(p);
			i++;
		}
		p--;
	}
	mNum++;
	return i;
}


//インスタンスメソッド
void keyboard_emulater::addVKEvent(int vk, DWORD dwFlags){
	addEvent(vk, 0, dwFlags);
}
void keyboard_emulater::addDIKEvent(int dik, DWORD dwFlags){
	addEvent(0, dik, dwFlags);
}
void keyboard_emulater::addEvent(int vk, int dik, DWORD dwFlags){
	KEY_ACTION act;
	if(dik==0){
		dik = MapVirtualKey(vk,0);
	} else if(vk==0){
		vk = MapVirtualKey(dik,1);
	}

	act.vk = vk;
	act.dik = dik;
	act.flag = dwFlags;
	act.frame = *mCounter + mKeyFrame;
	mList.push_back(act);
}


//クラスメソッド
void keyboard_emulater::vkEvent(int vk, DWORD dwFlags){
	event(vk, 0, dwFlags);
}
void keyboard_emulater::vkEvent(int vk){
	vkEvent(vk, 0);
	Sleep(9);//9msほど待つ
	vkEvent(vk, KEYEVENTF_KEYUP);
}
void keyboard_emulater::vkEventOn(int vk){
	vkEvent(vk, 0);
}
void keyboard_emulater::vkEventOff(int vk){
	vkEvent(vk, KEYEVENTF_KEYUP);
}
void keyboard_emulater::dikEvent(int dik, DWORD dwFlags){
	event(0, dik, dwFlags);
}
void keyboard_emulater::dikEvent(int dik){
	dikEvent(dik, 0);
	Sleep(9);//9msほど待つ
	dikEvent(dik, KEYEVENTF_KEYUP);
}
void keyboard_emulater::dikEventOn(int dik){
	dikEvent(dik, 0);
}
void keyboard_emulater::dikEventOff(int dik){
	dikEvent(dik, KEYEVENTF_KEYUP);
}
void keyboard_emulater::event(int vk, int dik, DWORD dwFlags){
	if(dik==0){
		dik = MapVirtualKey(vk,0);
	} else if(vk==0){
		vk = MapVirtualKey(dik,1);
	}
	if((dwFlags&KEYEVENTF_KEYUP) == 0)dik += 768;
	keybd_event(static_cast<BYTE>(vk), static_cast<BYTE>(dik), dwFlags, 0);
}
void keyboard_emulater::event(int vk, int dik){
	event(vk, dik, 0);
	Sleep(9);//9msほど待つ
	event(vk, dik, KEYEVENTF_KEYUP);
}
void keyboard_emulater::eventOn(int vk, int dik){
	event(vk, dik, 0);
}
void keyboard_emulater::eventOff(int vk, int dik){
	event(vk, dik, KEYEVENTF_KEYUP);
}
