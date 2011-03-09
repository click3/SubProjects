#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFSIZE 512

u_long RawAddr(void);
void PacketOutput(unsigned char *str,int RecvSize);
void LoadSetting(void);
int ChangeStr(char *str,char s1,char s2);
int atoi2(char *str,int def);
DWORD WINAPI ThreadFunc(LPVOID vdParam);

int PacketID = 0;

//基本項目
char MyIP[32];		//ＮＩＣデバイスのIPアドレス(文字列)
//各種設定項目
int PrintPacket;
int AdapterNum;
int FilterSendPort;
int FilterRecvPort;
char FilterSendIP[16];
char FilterRecvIP[16];
int SavePacketMode;
int SaveHeadMode;

int main(){
	WSADATA wsad;
	SOCKET sock;
	char temp[4096];
	SOCKET_ADDRESS_LIST *saddrL;
	DWORD d,dwID;
	struct sockaddr_in saddr;
	int RecvSize;
	u_long arg = 1;
	DWORD dwNonBlocking = 1;

	//設定ファイル(setting.ini)から読み込み
	LoadSetting();
	
	//Winsockの初期化
	if(WSAStartup(MAKEWORD(2,1),&wsad)){
		printf("初期化に失敗しました");
		return -1;
	}
	//Verチェック
	if(wsad.wVersion != MAKEWORD(2,1)){
		printf("未対応のヴァージョンです");
		return -1;
	}
	//ソケットの取得
	sock = socket(AF_INET,SOCK_RAW,IPPROTO_IP);
	if(sock == INVALID_SOCKET){
		printf("ソケットの取得に失敗しました");
		return -1;
	}

	//構造体の初期化
	memset(&saddr,0,sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = RawAddr();
	saddr.sin_port = htons(0);
	if(bind(sock,(struct sockaddr *)&saddr,sizeof(struct sockaddr))==SOCKET_ERROR){
		printf("ソケット初期化エラー");
		return -1;
	}
	if(0!=ioctlsocket(sock,0x98000001,&arg)){
		printf("ioctlsocketエラー");
		return 1;
	}

	sprintf(MyIP,"%s",inet_ntoa(saddr.sin_addr));
	ioctlsocket(sock, FIONBIO, &dwNonBlocking);

	printf("-------------------------------------------------------------------------------\n");
	printf("これより監視を開始します\n");
	printf("-------------------------------------------------------------------------------\n");

	//コマンド入力用スレッドの開始
	//手抜きにつき色々まずい処理をしているけれど気にしない
	CreateThread(NULL , 0 , ThreadFunc , (LPVOID)sock , 0 , &dwID);

	//メインループ
	while(TRUE){
		WSASetLastError(0);
		RecvSize = recv(sock,(char *)&temp,2048,0);
		if(RecvSize > 0){
			//受信内容を出力
			PacketOutput((char *)&temp,RecvSize);
		} else {
			DWORD errorCode = WSAGetLastError();
			if(errorCode == WSAEWOULDBLOCK){
				Sleep(1000);
			} else if(errorCode == 0){
				RecvSize=0;
			} else {
				printf("recvエラー\n");
				return 1;
			}
		}
	}
}

//設定読み込み処理
void LoadSetting(void){
	FILE *fp;
	char str[BUFSIZE];

	fp = fopen("setting.ini","r");

	while(fgets((char *)&str,BUFSIZE,fp) != NULL){
		if(!strncmp(str,"//",2) || !strncmp(str,"\n",1))continue;
		ChangeStr((char *)&str,'\n','\0');
		if(!strncmp(str,"PrintPacket=",12))PrintPacket = atoi2(&str[12],0);
		if(!strncmp(str,"AdapterNum=",11))AdapterNum = atoi2(&str[11],-1);
		if(!strncmp(str,"FilterSendPort=",15))FilterSendPort = atoi2(&str[15],-1);
		if(!strncmp(str,"FilterRecvPort=",15))FilterRecvPort = atoi2(&str[15],-1);
		if(!strncmp(str,"FilterSendIP=",13))strcpy(FilterSendIP,&str[13]);
		if(!strncmp(str,"FilterRecvIP=",13))strcpy(FilterRecvIP,&str[13]);
		if(!strncmp(str,"SavePacketMode=",15))SavePacketMode = atoi2(&str[15],5);
		if(!strncmp(str,"SaveHeadMode=",13))SaveHeadMode = atoi2(&str[13],1);
	}

	fclose(fp);

	return;
}

//使用ＮＩＣデバイス検出用
u_long RawAddr(void){
	SOCKET sock;
	SOCKET_ADDRESS_LIST* addressList;
	DWORD d;
	char buffer[1024];
	int i;

	//ソケット初期化
	if (INVALID_SOCKET == (sock = socket(AF_INET, SOCK_RAW, IPPROTO_IP)))
	{
		printf("INVALID_SOCKET\n");
		return FALSE;
	}


	//デバイスの列挙
	if (WSAIoctl(sock, SIO_ADDRESS_LIST_QUERY, NULL, 0, buffer, 1024, &d, NULL, NULL) != 0)
	{
		printf("INVALID_SOCKET\n");
		return 0;
	}

	addressList = (SOCKET_ADDRESS_LIST*)buffer;

	switch(addressList->iAddressCount){						//ＮＩＣデバイス数によって処理を分岐
		case 0:									//ＮＩＣデバイスがない場合は強制終了
			printf("ＮＩＣデバイスを検出できなかったので\nプログラムを終了します。\n");
			exit(0);
		case 1:									//ＮＩＣデバイスが一個のみの場合はそれを選択
			printf("1個のＮＩＣデバイスを検出\n");
			i = 0;
			break;
		default:								//複数個を検出した場合の処理
			printf("%d個のＮＩＣデバイスを検出[0-%d]\n",addressList->iAddressCount,addressList->iAddressCount-1);
			for(i = 0 ; i < addressList->iAddressCount ; i++){
				printf("[%d]%s\n",i,inet_ntoa(((SOCKADDR_IN*)addressList->Address[i].lpSockaddr)->sin_addr));
			}
			if(AdapterNum < 0 || AdapterNum >= addressList->iAddressCount){	//設定ファイルに無設定の場合は選択させる
				printf("使用するＮＩＣデバイスを選択してください\n");
				while(!(i >= 0 && i < addressList->iAddressCount)){
					gets(buffer);
					i = atoi(buffer);
					if(i == 0 && buffer[0] != '0')i = -1;		//文字列を入れるとatoiで0が戻るため、0じゃない場合はNULLに
				}
			}else{								//設定ファイルに設定済みの場合はそれを選択
				i = AdapterNum;	
			}	
			break;
	}
	printf("%sのＮＩＣデバイスを使用します\n",inet_ntoa(((SOCKADDR_IN*)addressList->Address[i].lpSockaddr)->sin_addr));

	return ((SOCKADDR_IN*)addressList->Address[i].lpSockaddr)->sin_addr.s_addr;
}

//パケット出力処理
void PacketOutput(unsigned char *str,int RecvSize){
	static time_t time1,time2;
	static struct tm *t_st;
	static FILE *fp = NULL,*fp2;
	static char FName[1024];
	char IP1[128],IP2[128];
	int i,ii,Port1,Port2;

	//まだファイルを開いていないなら現在時刻をもとに作成
	if(fp == NULL){
		time(&time1);
		time(&time2);
		t_st = localtime(&time1);
		//出力タイプごとに分岐
		switch(SavePacketMode){
			case 1:
			case 2:
				sprintf(FName,"Bibinary%04d%02d%02d.log",t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday);
				fp = fopen(FName,"a+");
				break;
			case 3:
			case 4:
				sprintf(FName,"Text%04d%02d%02d.log",t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday);
				fp2 = fopen(FName,"a+");
				fp = fp2;
				break;
			case 5:
			case 6:
				sprintf(FName,"Bibinary%04d%02d%02d.log",t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday);
				fp = fopen(FName,"a+");
				sprintf(FName,"Text%04d%02d%02d.log",t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday);
				fp2 = fopen(FName,"a+");
				break;
			default:
				fp = 0;
				break;
		}
	}else{
		time(&time2);
		t_st = localtime(&time2);
	}

	//0バイトパケット以外ならば出力処理
	if(RecvSize != 0){
		//パケットヘッダ長計算
		ii = 4*(str[0] & 0x0F);
		switch(str[9]){
			case 6://TCP
				i = ii+ 4*((str[ii+12] & 0xF0) >> 4);
				break;
			case 17://UDP
				i = ii + 8;
				break;
			default://それ以外
				i = ii;
				break;
		}
		sprintf(IP1,"%d.%d.%d.%d",str[12],str[13],str[14],str[15]);
		sprintf(IP2,"%d.%d.%d.%d",str[16],str[17],str[18],str[19]);
		Port1 = (str[ii] << 8)+str[ii+1];
		Port2 = (str[ii+2] << 8)+str[ii+3];

		//受信パケットの条件判定(分かりづらいことこのうえない……)
		if(strcmp(IP2,MyIP) == 0 && (FilterSendIP[0] == '\0' || !strcmp(IP1,FilterSendIP)) && (FilterRecvIP[0] == '\0' || !strcmp(IP2,FilterRecvIP)) &&
			(FilterSendPort == -1 || Port1 == FilterSendPort) && (FilterRecvPort == -1 || Port2 == FilterRecvPort)){

			//分割パケット判断用ディスプレイ出力処理
/*			if((str[5]+str[4]*0x100) == PacketID)printf("%d==%d\n",PacketID,str[5]+str[4]*0x100);
			PacketID = str[5]+str[4]*0x100;
			if(str[6]&0x1F)printf("\a\a");
*/
			//受信パケットファイル出力処理(ヘッダ解析内容)
			switch(SavePacketMode){
				case 1:
					fprintf(fp,"[%08.0f]R",difftime(time2,time1));
					break;
				case 2:
					fprintf(fp,"[%08.0f]R ",difftime(time2,time1));
					fprintf(fp,"HeadSize:%d 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d",i,IP1,Port1,Port2);
					break;
				case 3:
					fprintf(fp2,"[%08.0f]R",difftime(time2,time1));
					break;
				case 4:
					fprintf(fp2,"[%08.0f]R ",difftime(time2,time1));
					fprintf(fp2,"HeadSize:%d 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d\n",i,IP1,Port1,Port2);
					break;
				case 5:
					fprintf(fp,"[%08.0f]R",difftime(time2,time1));
					fprintf(fp2,"[%08.0f]R",difftime(time2,time1));
					break;
				case 6:
					fprintf(fp,"[%08.0f]R ",difftime(time2,time1));
					fprintf(fp,"HeadSize:%d 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d",i,IP1,Port1,Port2);
					fprintf(fp2,"[%08.0f]R ",difftime(time2,time1));
					fprintf(fp2,"HeadSize:%d 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d\n",i,IP1,Port1,Port2);
					break;
			}
			//ディスプレイ出力(ヘッダ解析内容)
			switch(PrintPacket){
				case 1:
				case 2:
				case 3:
					printf("[%06.0f]R ",difftime(time2,time1));
					printf("HeadSize:%d 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d",i,IP1,Port1,Port2);
					break;
			}

			//データ部の出力
			if(SaveHeadMode)i = 0;
			while(i < RecvSize){
				//ファイルへの出力
				switch(SavePacketMode){
					case 1:
					case 2:
						fprintf(fp," %02x",str[i]);
						break;
					case 3:
					case 4:
						fprintf(fp2,"%c",str[i]);
						break;
					case 5:
					case 6:
						fprintf(fp," %02x",str[i]);
						fprintf(fp2,"%c",str[i]);
						break;
				}
				//ディスプレイへの出力
				switch(PrintPacket){
					case 2:
						printf(" %02x",str[i]);
						break;
					case 3:
						printf("%c",str[i]);
						break;
				}
				i++;
			}
			if(PrintPacket)printf("\n");
			switch(SavePacketMode){
				case 1:
				case 2:
					fprintf(fp,"\n");
					break;
				case 3:
				case 4:
					fprintf(fp2,"\n");
					break;
				case 5:
				case 6:
					fprintf(fp,"\n");
					fprintf(fp2,"\n");
					break;
			}

		//送信パケットの条件判定、一部環境以外では送信パケットを受信できないのでほぼ無意味
		//2006年12月15日追記：出力内容に不具合あり、だが自宅での検証が不可能なので放置
		//2007年04月15日追記：Vista上でノートン先生のUDP送信パケットを感知、一部規制が緩和されている？
		}else if(strcmp(IP1,MyIP) == 0 && (FilterSendIP[0] == '\0' || !strcmp(IP1,FilterSendIP)) && (FilterRecvIP[0] == '\0' || !strcmp(IP2,FilterRecvIP)) &&
			(FilterSendPort == -1 || Port1 == FilterSendPort) && (FilterRecvPort == -1 || Port2 == FilterRecvPort)){

			switch(SavePacketMode){
				case 1:
					fprintf(fp,"[%08.0f]S",difftime(time2,time1));
					break;
				case 2:
					fprintf(fp,"[%08.0f]S ",difftime(time2,time1));
					fprintf(fp,"HeadSize:%d 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d",i,IP2,Port1,Port2);
					break;
				case 3:
					fprintf(fp2,"[%08.0f]S",difftime(time2,time1));
					break;
				case 4:
					fprintf(fp2,"[%08.0f]S ",difftime(time2,time1));
					fprintf(fp2,"HeadSize:%d 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d\n",i,IP2,Port1,Port2);
					break;
				case 5:
					fprintf(fp,"[%08.0f]S",difftime(time2,time1));
					fprintf(fp2,"[%08.0f]S",difftime(time2,time1));
					break;
				case 6:
					fprintf(fp,"[%08.0f]S ",difftime(time2,time1));
					fprintf(fp,"HeadSize:%d 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d",i,IP2,Port1,Port2);
					fprintf(fp2,"[%08.0f]S ",difftime(time2,time1));
					fprintf(fp2,"HeadSize:%d 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d\n",i,IP2,Port1,Port2);
					break;
			}
			switch(PrintPacket){
				case 1:
				case 2:
				case 3:
					printf("[%06.0f]S ",difftime(time2,time1));
					printf("HeadSize:%d 送信先IP:%15s 送信元Port:%5d 送信先Port:%5d",i,IP2,Port1,Port2);
					break;
			}



			//データ部の出力
			if(SaveHeadMode)i = 0;
			while(i < RecvSize){
				//ファイルへの出力
				switch(SavePacketMode){
					case 1:
					case 2:
						fprintf(fp," %02x",str[i]);
						break;
					case 3:
					case 4:
						fprintf(fp2,"%c",str[i]);
						break;
					case 5:
					case 6:
						fprintf(fp," %02x",str[i]);
						fprintf(fp2,"%c",str[i]);
						break;
				}
				//ディスプレイへの出力
				switch(PrintPacket){
					case 2:
						printf(" %02x",str[i]);
						break;
					case 3:
						printf("%c",str[i]);
						break;
				}
				i++;
			}
			if(PrintPacket)printf("\n");
			switch(SavePacketMode){
				case 1:
				case 2:
					fprintf(fp,"\n");
					break;
				case 3:
				case 4:
					fprintf(fp2,"\n");
					break;
				case 5:
				case 6:
					fprintf(fp,"\n");
					fprintf(fp2,"\n");
					break;
			}
		}else if(0){//単なる通過パケット(自身が送信者でも受信者でもないパケット)
			switch(SavePacketMode){
				case 1:
					fprintf(fp,"[%08.0f]?",difftime(time2,time1));
					break;
				case 2:
					fprintf(fp,"[%08.0f]? ",difftime(time2,time1));
					fprintf(fp,"HeadSize:%d 送信先IP:%15s 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d",i,IP1,IP2,Port1,Port2);
					break;
				case 3:
					fprintf(fp2,"[%08.0f]?",difftime(time2,time1));
					break;
				case 4:
					fprintf(fp2,"[%08.0f]? ",difftime(time2,time1));
					fprintf(fp2,"HeadSize:%d 送信先IP:%15s 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d\n",i,IP1,IP2,Port1,Port2);
					break;
				case 5:
					fprintf(fp,"[%08.0f]?",difftime(time2,time1));
					fprintf(fp2,"[%08.0f]?",difftime(time2,time1));
					break;
				case 6:
					fprintf(fp,"[%08.0f]? ",difftime(time2,time1));
					fprintf(fp,"HeadSize:%d 送信先IP:%15s 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d",i,IP1,IP2,Port1,Port2);
					fprintf(fp2,"[%08.0f]? ",difftime(time2,time1));
					fprintf(fp2,"HeadSize:%d 送信先IP:%15s 送信元IP:%15s 送信元Port:%5d 送信先Port:%5d\n",i,IP1,IP2,Port1,Port2);
					break;
			}
			switch(PrintPacket){
				case 1:
				case 2:
				case 3:
					printf("[%06.0f]? ",difftime(time2,time1));
					printf("HeadSize:%d 送信先IP:%15s 送信元Port:%5d 送信先Port:%5d",i,IP1,IP2,Port1,Port2);
					break;
			}



			//データ部の出力
			if(SaveHeadMode)i = 0;
			while(i < RecvSize){
				//ファイルへの出力
				switch(SavePacketMode){
					case 1:
					case 2:
						fprintf(fp," %02x",str[i]);
						break;
					case 3:
					case 4:
						fprintf(fp2,"%c",str[i]);
						break;
					case 5:
					case 6:
						fprintf(fp," %02x",str[i]);
						fprintf(fp2,"%c",str[i]);
						break;
				}
				//ディスプレイへの出力
				switch(PrintPacket){
					case 2:
						printf(" %02x",str[i]);
						break;
					case 3:
						printf("%c",str[i]);
						break;
				}
				i++;
			}
			if(PrintPacket)printf("\n");
			switch(SavePacketMode){
				case 1:
				case 2:
					fprintf(fp,"\n");
					break;
				case 3:
				case 4:
					fprintf(fp2,"\n");
					break;
				case 5:
				case 6:
					fprintf(fp,"\n");
					fprintf(fp2,"\n");
					break;
			}
		}
	}
	return;
}

//簡易文字列置き換え関数
int ChangeStr(char *str,char s1,char s2){
	int i,count = 0;

	for(i = 0 ; str[i] != '\0' ; i++){
		if(str[i] == s1){
			str[i] = s2;
			count++;
		}
	}

	return count;
}

//デフォルト指定可能版atoi
int atoi2(char *str,int def){
	int i;

	if(!isdigit(str[0]))return def;
	i = atoi(str);
	if(i == 0 && str[0] != '0')return def;

	return i;
} 

//コマンド処理用スレッド
DWORD WINAPI ThreadFunc(LPVOID vdParam){
	char s[128],i;

	//printf("コマンド機能説明が必要ならばhelpを、プロンプトは出ません。\n");
	printf("終了するときは exit と入力し、Enterを押してください.\n");

	//コマンド処理
	while(TRUE){
		gets(s);
		for(i = 0 ; i < strlen(s) ; i++)s[i]=tolower(s[i]);

		if(!strncmp(s,"exit",4)){
			//ソケットの開放
			shutdown((SOCKET)vdParam, SD_BOTH);
			closesocket((SOCKET)vdParam);
			//Winsockの開放
			WSACleanup();

			exit(0);
		}else if(!strncmp(s,"send",4)){
			printf("Windows側の仕様変更により機能廃止\n");
		}else if(!strncmp(s,"read",4)){
			//ファイルの再オープンをしていないのでSavePacketMode変えるとたぶん止まる
			while(s[0] != 'y'&& s[0] != 'n'){
				printf("setting.iniを再読み込みしますか？(Y/N)");
				gets(s);
				s[0] = tolower(s[0]);
				if(s[0] == 'y')LoadSetting();
			}
		}else if(!strncmp(s,"help",4)){
			printf("\n");
			printf("exit\n");
			printf("    RawFilterを終了します。\n");
			printf("send S_IP S_Port R_IP R_PORT Data…\n");
			printf("    TCP/IPプロトコルでIPやPortを偽装したパケットを送信する。\n");
			printf("    S_IP:送信者IP\n");
			printf("    S_Port:送信者Port\n");
			printf("    R_IP:送信先IP\n");
			printf("    R_Port:送信先Port\n");
			printf("    Data:実際の送信内容(複数指定可)\n");
			printf("read\n");
			printf("    設定ファイルを再読み込みします\n");
			printf("help\n");
			printf("    コマンドについての詳細を表示します\n");
		}
	}
}

