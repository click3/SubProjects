
#pragma once

#include <windows.h>
#include <vector>


/** キーボードエミュレート用簡易クラス
 *
 * 用語：
 * counter:フレーム計算に使われる、指定なしだとrun実行ごとに+1されるものとして動作する
 * keyFrame:遅延フレーム、指定フレームだけ遅れて実行される。
 * vk:仮想キーコード
 * dik:スキャンコード
 * dwFlags:MSDNのkeybd_eventのdwFlags参照
 *
 * .*?Event.*?系:
 * そのまま指定した入力でキーボードイベントを発生させる
 * クラスインスタンスを生成する必要なく使用できる。
 *
 * add.*?Event系:
 * runメソッド実行した際に.*?Eventを実行するよう登録する
 * 遅延フレーム数が設定済みの場合は、カウンター変数が登録時のカウンター変数+遅延フレーム数以上の時に実行される
 */
class keyboard_emulater{
private:
	typedef struct KeyAction{
		int vk;
		int dik;
		int flag;
		int frame;
	}KEY_ACTION;

	int mKeyFrame;
	int *mCounter;
	int mNum;
	std::vector<KEY_ACTION> mList;

public:
	/*
	 * 初期化メソッド、コンストラクタからも呼ばれる
	 */
	void initialize();
	/*
	 * コンストラクタ
	 */
	keyboard_emulater();
	/*
	 * コンストラクタ
	 * @param counter 初期化後setCounterを行う
	 */
	keyboard_emulater(int *counter);
	/*
	 * コンストラクタ
	 * @param keyFrame 初期化後setKeyFrameを行う
	 */
	keyboard_emulater(int keyFrame);
	/*
	 * コンストラクタ
	 * @param counter 初期化後setCounterを行う
	 * @param keyFrame 初期化後setKeyFrameを行う
	 */
	keyboard_emulater(int *counter, int keyFrame);
	/*
	 * コンストラクタ、keyboard_emulater(int *counter, int keyFrame)と完全に同じ動作をする
	 * @param keyFrame 初期化後setKeyFrameを行う
	 * @param counter 初期化後setCounterを行う
	 */
	keyboard_emulater(int keyFrame, int *counter);

	/*
	 * 遅延フレーム数を設定する
	 * @param keyFrame 設定する遅延フレーム数
	 */
	int setKeyFrame(int keyFrame);
	/*
	 * 外部でカウンター変数を持つ場合はそのポインター
	 * @param counter 設定するカウンター変数のポインタ
	 */
	int *setCounter(int *counter);
	/*
	 * 現在設定されている遅延フレーム数を取得する
	 */
	int getKeyFrame();
	/*
	 * 設定されているカウンター変数のポインターを取得する
	 */
	int *getCounter();

	/*
	 * add.*?Eventで設定されたキー入力を遅延フレーム数およびカウンター変数を元に実行する
	 * カウンター変数未設定の場合、実行ごとにカウンター変数を+1して動作する
	 */
	int run();

	/*
	 * 仮想キーコードを元にキーボードイベントを登録する
	 * @param vk 仮想キーコード
	 * @param dwFlags keybd_eventのdwFlagsと同様
	 */
	void addVKEvent(int vk, DWORD dwFlags);
	/*
	 * スキャンコードを元にキーボードイベントを登録する
	 * @param dik スキャンコード
	 * @param dwFlags keybd_eventのdwFlagsと同様
	 */
	void addDIKEvent(int dik, DWORD dwFlags);
	/*
	 * 仮想キーコードおよびスキャンコードを元にキーボードイベントを登録する
	 * @param vk 仮想キーコード
	 * @param dik スキャンコード
	 * @param dwFlags keybd_eventのdwFlagsと同様
	 */
	void addEvent(int vk, int dik, DWORD dwFlags);

	/*
	 * 仮想キーコードを元にキーボードイベントを実行する
	 * @param vk 仮想キーコード
	 * @param dwFlags keybd_eventのdwFlagsと同様
	 */
	static void vkEvent(int vk, DWORD dwFlags);
	/*
	 * 仮想キーコードを元にKEYDOWNとKEYUP相当のイベントを実行する
	 * @param vk 仮想キーコード
	 */
	static void vkEvent(int vk);
	/*
	 * 仮想キーコードを元にKEYDOWN相当のイベントを実行する
	 * @param vk 仮想キーコード
	 */
	static void vkEventOn(int vk);
	/*
	 * 仮想キーコードを元にKEYUP相当のイベントを実行する
	 * @param vk 仮想キーコード
	 */
	static void vkEventOff(int vk);
	/*
	 * スキャンコードを元にキーボードイベントを実行する
	 * @param dik スキャンコード
	 * @param dwFlags keybd_eventのdwFlagsと同様
	 */
	static void dikEvent(int dik, DWORD dwFlags);
	/*
	 * スキャンコードを元にKEYDOWNとKEYUP相当のイベントを実行する
	 * @param dik スキャンコード
	 */
	static void dikEvent(int dik);
	/*
	 * スキャンコードを元にKEYDOWN相当のイベントを実行する
	 * @param dik スキャンコード
	 */
	static void dikEventOn(int dik);
	/*
	 * スキャンコードを元にKEYUP相当のイベントを実行する
	 * @param dik スキャンコード
	 */
	static void dikEventOff(int dik);
	/*
	 * 仮想キーコードおよびスキャンコードを元にキーボードイベントを実行する
	 * @param vk 仮想キーコード
	 * @param dik スキャンコード
	 * @param dwFlags keybd_eventのdwFlagsと同様
	 */
	static void event(int vk, int dik, DWORD dwFlags);
	/*
	 * 仮想キーコードおよびスキャンコードを元にKEYDOWNとKEYUP相当のイベントを実行する
	 * @param vk 仮想キーコード
	 * @param dik スキャンコード
	 */
	static void event(int vk, int dik);
	/*
	 * 仮想キーコードおよびスキャンコードを元にKEYDOWN相当のイベントを実行する
	 * @param vk 仮想キーコード
	 * @param dik スキャンコード
	 */
	static void eventOn(int vk, int dik);
	/*
	 * 仮想キーコードおよびスキャンコードを元にKEYUP相当のイベントを実行する
	 * @param vk 仮想キーコード
	 * @param dik スキャンコード
	 */
	static void eventOff(int vk, int dik);
};
