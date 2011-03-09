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

//��{����
char MyIP[32];		//�m�h�b�f�o�C�X��IP�A�h���X(������)
//�e��ݒ荀��
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

	//�ݒ�t�@�C��(setting.ini)����ǂݍ���
	LoadSetting();
	
	//Winsock�̏�����
	if(WSAStartup(MAKEWORD(2,1),&wsad)){
		printf("�������Ɏ��s���܂���");
		return -1;
	}
	//Ver�`�F�b�N
	if(wsad.wVersion != MAKEWORD(2,1)){
		printf("���Ή��̃��@�[�W�����ł�");
		return -1;
	}
	//�\�P�b�g�̎擾
	sock = socket(AF_INET,SOCK_RAW,IPPROTO_IP);
	if(sock == INVALID_SOCKET){
		printf("�\�P�b�g�̎擾�Ɏ��s���܂���");
		return -1;
	}

	//�\���̂̏�����
	memset(&saddr,0,sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = RawAddr();
	saddr.sin_port = htons(0);
	if(bind(sock,(struct sockaddr *)&saddr,sizeof(struct sockaddr))==SOCKET_ERROR){
		printf("�\�P�b�g�������G���[");
		return -1;
	}
	if(0!=ioctlsocket(sock,0x98000001,&arg)){
		printf("ioctlsocket�G���[");
		return 1;
	}

	sprintf(MyIP,"%s",inet_ntoa(saddr.sin_addr));
	ioctlsocket(sock, FIONBIO, &dwNonBlocking);

	printf("-------------------------------------------------------------------------------\n");
	printf("������Ď����J�n���܂�\n");
	printf("-------------------------------------------------------------------------------\n");

	//�R�}���h���͗p�X���b�h�̊J�n
	//�蔲���ɂ��F�X�܂������������Ă��邯��ǋC�ɂ��Ȃ�
	CreateThread(NULL , 0 , ThreadFunc , (LPVOID)sock , 0 , &dwID);

	//���C�����[�v
	while(TRUE){
		WSASetLastError(0);
		RecvSize = recv(sock,(char *)&temp,2048,0);
		if(RecvSize > 0){
			//��M���e���o��
			PacketOutput((char *)&temp,RecvSize);
		} else {
			DWORD errorCode = WSAGetLastError();
			if(errorCode == WSAEWOULDBLOCK){
				Sleep(1000);
			} else if(errorCode == 0){
				RecvSize=0;
			} else {
				printf("recv�G���[\n");
				return 1;
			}
		}
	}
}

//�ݒ�ǂݍ��ݏ���
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

//�g�p�m�h�b�f�o�C�X���o�p
u_long RawAddr(void){
	SOCKET sock;
	SOCKET_ADDRESS_LIST* addressList;
	DWORD d;
	char buffer[1024];
	int i;

	//�\�P�b�g������
	if (INVALID_SOCKET == (sock = socket(AF_INET, SOCK_RAW, IPPROTO_IP)))
	{
		printf("INVALID_SOCKET\n");
		return FALSE;
	}


	//�f�o�C�X�̗�
	if (WSAIoctl(sock, SIO_ADDRESS_LIST_QUERY, NULL, 0, buffer, 1024, &d, NULL, NULL) != 0)
	{
		printf("INVALID_SOCKET\n");
		return 0;
	}

	addressList = (SOCKET_ADDRESS_LIST*)buffer;

	switch(addressList->iAddressCount){						//�m�h�b�f�o�C�X���ɂ���ď����𕪊�
		case 0:									//�m�h�b�f�o�C�X���Ȃ��ꍇ�͋����I��
			printf("�m�h�b�f�o�C�X�����o�ł��Ȃ������̂�\n�v���O�������I�����܂��B\n");
			exit(0);
		case 1:									//�m�h�b�f�o�C�X����݂̂̏ꍇ�͂����I��
			printf("1�̂m�h�b�f�o�C�X�����o\n");
			i = 0;
			break;
		default:								//���������o�����ꍇ�̏���
			printf("%d�̂m�h�b�f�o�C�X�����o[0-%d]\n",addressList->iAddressCount,addressList->iAddressCount-1);
			for(i = 0 ; i < addressList->iAddressCount ; i++){
				printf("[%d]%s\n",i,inet_ntoa(((SOCKADDR_IN*)addressList->Address[i].lpSockaddr)->sin_addr));
			}
			if(AdapterNum < 0 || AdapterNum >= addressList->iAddressCount){	//�ݒ�t�@�C���ɖ��ݒ�̏ꍇ�͑I��������
				printf("�g�p����m�h�b�f�o�C�X��I�����Ă�������\n");
				while(!(i >= 0 && i < addressList->iAddressCount)){
					gets(buffer);
					i = atoi(buffer);
					if(i == 0 && buffer[0] != '0')i = -1;		//������������atoi��0���߂邽�߁A0����Ȃ��ꍇ��NULL��
				}
			}else{								//�ݒ�t�@�C���ɐݒ�ς݂̏ꍇ�͂����I��
				i = AdapterNum;	
			}	
			break;
	}
	printf("%s�̂m�h�b�f�o�C�X���g�p���܂�\n",inet_ntoa(((SOCKADDR_IN*)addressList->Address[i].lpSockaddr)->sin_addr));

	return ((SOCKADDR_IN*)addressList->Address[i].lpSockaddr)->sin_addr.s_addr;
}

//�p�P�b�g�o�͏���
void PacketOutput(unsigned char *str,int RecvSize){
	static time_t time1,time2;
	static struct tm *t_st;
	static FILE *fp = NULL,*fp2;
	static char FName[1024];
	char IP1[128],IP2[128];
	int i,ii,Port1,Port2;

	//�܂��t�@�C�����J���Ă��Ȃ��Ȃ猻�ݎ��������Ƃɍ쐬
	if(fp == NULL){
		time(&time1);
		time(&time2);
		t_st = localtime(&time1);
		//�o�̓^�C�v���Ƃɕ���
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

	//0�o�C�g�p�P�b�g�ȊO�Ȃ�Ώo�͏���
	if(RecvSize != 0){
		//�p�P�b�g�w�b�_���v�Z
		ii = 4*(str[0] & 0x0F);
		switch(str[9]){
			case 6://TCP
				i = ii+ 4*((str[ii+12] & 0xF0) >> 4);
				break;
			case 17://UDP
				i = ii + 8;
				break;
			default://����ȊO
				i = ii;
				break;
		}
		sprintf(IP1,"%d.%d.%d.%d",str[12],str[13],str[14],str[15]);
		sprintf(IP2,"%d.%d.%d.%d",str[16],str[17],str[18],str[19]);
		Port1 = (str[ii] << 8)+str[ii+1];
		Port2 = (str[ii+2] << 8)+str[ii+3];

		//��M�p�P�b�g�̏�������(������Â炢���Ƃ��̂����Ȃ��c�c)
		if(strcmp(IP2,MyIP) == 0 && (FilterSendIP[0] == '\0' || !strcmp(IP1,FilterSendIP)) && (FilterRecvIP[0] == '\0' || !strcmp(IP2,FilterRecvIP)) &&
			(FilterSendPort == -1 || Port1 == FilterSendPort) && (FilterRecvPort == -1 || Port2 == FilterRecvPort)){

			//�����p�P�b�g���f�p�f�B�X�v���C�o�͏���
/*			if((str[5]+str[4]*0x100) == PacketID)printf("%d==%d\n",PacketID,str[5]+str[4]*0x100);
			PacketID = str[5]+str[4]*0x100;
			if(str[6]&0x1F)printf("\a\a");
*/
			//��M�p�P�b�g�t�@�C���o�͏���(�w�b�_��͓��e)
			switch(SavePacketMode){
				case 1:
					fprintf(fp,"[%08.0f]R",difftime(time2,time1));
					break;
				case 2:
					fprintf(fp,"[%08.0f]R ",difftime(time2,time1));
					fprintf(fp,"HeadSize:%d ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d",i,IP1,Port1,Port2);
					break;
				case 3:
					fprintf(fp2,"[%08.0f]R",difftime(time2,time1));
					break;
				case 4:
					fprintf(fp2,"[%08.0f]R ",difftime(time2,time1));
					fprintf(fp2,"HeadSize:%d ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d\n",i,IP1,Port1,Port2);
					break;
				case 5:
					fprintf(fp,"[%08.0f]R",difftime(time2,time1));
					fprintf(fp2,"[%08.0f]R",difftime(time2,time1));
					break;
				case 6:
					fprintf(fp,"[%08.0f]R ",difftime(time2,time1));
					fprintf(fp,"HeadSize:%d ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d",i,IP1,Port1,Port2);
					fprintf(fp2,"[%08.0f]R ",difftime(time2,time1));
					fprintf(fp2,"HeadSize:%d ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d\n",i,IP1,Port1,Port2);
					break;
			}
			//�f�B�X�v���C�o��(�w�b�_��͓��e)
			switch(PrintPacket){
				case 1:
				case 2:
				case 3:
					printf("[%06.0f]R ",difftime(time2,time1));
					printf("HeadSize:%d ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d",i,IP1,Port1,Port2);
					break;
			}

			//�f�[�^���̏o��
			if(SaveHeadMode)i = 0;
			while(i < RecvSize){
				//�t�@�C���ւ̏o��
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
				//�f�B�X�v���C�ւ̏o��
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

		//���M�p�P�b�g�̏�������A�ꕔ���ȊO�ł͑��M�p�P�b�g����M�ł��Ȃ��̂łقږ��Ӗ�
		//2006�N12��15���ǋL�F�o�͓��e�ɕs�����A��������ł̌��؂��s�\�Ȃ̂ŕ��u
		//2007�N04��15���ǋL�FVista��Ńm�[�g���搶��UDP���M�p�P�b�g�����m�A�ꕔ�K�����ɘa����Ă���H
		}else if(strcmp(IP1,MyIP) == 0 && (FilterSendIP[0] == '\0' || !strcmp(IP1,FilterSendIP)) && (FilterRecvIP[0] == '\0' || !strcmp(IP2,FilterRecvIP)) &&
			(FilterSendPort == -1 || Port1 == FilterSendPort) && (FilterRecvPort == -1 || Port2 == FilterRecvPort)){

			switch(SavePacketMode){
				case 1:
					fprintf(fp,"[%08.0f]S",difftime(time2,time1));
					break;
				case 2:
					fprintf(fp,"[%08.0f]S ",difftime(time2,time1));
					fprintf(fp,"HeadSize:%d ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d",i,IP2,Port1,Port2);
					break;
				case 3:
					fprintf(fp2,"[%08.0f]S",difftime(time2,time1));
					break;
				case 4:
					fprintf(fp2,"[%08.0f]S ",difftime(time2,time1));
					fprintf(fp2,"HeadSize:%d ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d\n",i,IP2,Port1,Port2);
					break;
				case 5:
					fprintf(fp,"[%08.0f]S",difftime(time2,time1));
					fprintf(fp2,"[%08.0f]S",difftime(time2,time1));
					break;
				case 6:
					fprintf(fp,"[%08.0f]S ",difftime(time2,time1));
					fprintf(fp,"HeadSize:%d ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d",i,IP2,Port1,Port2);
					fprintf(fp2,"[%08.0f]S ",difftime(time2,time1));
					fprintf(fp2,"HeadSize:%d ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d\n",i,IP2,Port1,Port2);
					break;
			}
			switch(PrintPacket){
				case 1:
				case 2:
				case 3:
					printf("[%06.0f]S ",difftime(time2,time1));
					printf("HeadSize:%d ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d",i,IP2,Port1,Port2);
					break;
			}



			//�f�[�^���̏o��
			if(SaveHeadMode)i = 0;
			while(i < RecvSize){
				//�t�@�C���ւ̏o��
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
				//�f�B�X�v���C�ւ̏o��
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
		}else if(0){//�P�Ȃ�ʉ߃p�P�b�g(���g�����M�҂ł���M�҂ł��Ȃ��p�P�b�g)
			switch(SavePacketMode){
				case 1:
					fprintf(fp,"[%08.0f]?",difftime(time2,time1));
					break;
				case 2:
					fprintf(fp,"[%08.0f]? ",difftime(time2,time1));
					fprintf(fp,"HeadSize:%d ���M��IP:%15s ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d",i,IP1,IP2,Port1,Port2);
					break;
				case 3:
					fprintf(fp2,"[%08.0f]?",difftime(time2,time1));
					break;
				case 4:
					fprintf(fp2,"[%08.0f]? ",difftime(time2,time1));
					fprintf(fp2,"HeadSize:%d ���M��IP:%15s ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d\n",i,IP1,IP2,Port1,Port2);
					break;
				case 5:
					fprintf(fp,"[%08.0f]?",difftime(time2,time1));
					fprintf(fp2,"[%08.0f]?",difftime(time2,time1));
					break;
				case 6:
					fprintf(fp,"[%08.0f]? ",difftime(time2,time1));
					fprintf(fp,"HeadSize:%d ���M��IP:%15s ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d",i,IP1,IP2,Port1,Port2);
					fprintf(fp2,"[%08.0f]? ",difftime(time2,time1));
					fprintf(fp2,"HeadSize:%d ���M��IP:%15s ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d\n",i,IP1,IP2,Port1,Port2);
					break;
			}
			switch(PrintPacket){
				case 1:
				case 2:
				case 3:
					printf("[%06.0f]? ",difftime(time2,time1));
					printf("HeadSize:%d ���M��IP:%15s ���M��Port:%5d ���M��Port:%5d",i,IP1,IP2,Port1,Port2);
					break;
			}



			//�f�[�^���̏o��
			if(SaveHeadMode)i = 0;
			while(i < RecvSize){
				//�t�@�C���ւ̏o��
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
				//�f�B�X�v���C�ւ̏o��
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

//�ȈՕ�����u�������֐�
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

//�f�t�H���g�w��\��atoi
int atoi2(char *str,int def){
	int i;

	if(!isdigit(str[0]))return def;
	i = atoi(str);
	if(i == 0 && str[0] != '0')return def;

	return i;
} 

//�R�}���h�����p�X���b�h
DWORD WINAPI ThreadFunc(LPVOID vdParam){
	char s[128],i;

	//printf("�R�}���h�@�\�������K�v�Ȃ��help���A�v�����v�g�͏o�܂���B\n");
	printf("�I������Ƃ��� exit �Ɠ��͂��AEnter�������Ă�������.\n");

	//�R�}���h����
	while(TRUE){
		gets(s);
		for(i = 0 ; i < strlen(s) ; i++)s[i]=tolower(s[i]);

		if(!strncmp(s,"exit",4)){
			//�\�P�b�g�̊J��
			shutdown((SOCKET)vdParam, SD_BOTH);
			closesocket((SOCKET)vdParam);
			//Winsock�̊J��
			WSACleanup();

			exit(0);
		}else if(!strncmp(s,"send",4)){
			printf("Windows���̎d�l�ύX�ɂ��@�\�p�~\n");
		}else if(!strncmp(s,"read",4)){
			//�t�@�C���̍ăI�[�v�������Ă��Ȃ��̂�SavePacketMode�ς���Ƃ��Ԃ�~�܂�
			while(s[0] != 'y'&& s[0] != 'n'){
				printf("setting.ini���ēǂݍ��݂��܂����H(Y/N)");
				gets(s);
				s[0] = tolower(s[0]);
				if(s[0] == 'y')LoadSetting();
			}
		}else if(!strncmp(s,"help",4)){
			printf("\n");
			printf("exit\n");
			printf("    RawFilter���I�����܂��B\n");
			printf("send S_IP S_Port R_IP R_PORT Data�c\n");
			printf("    TCP/IP�v���g�R����IP��Port���U�������p�P�b�g�𑗐M����B\n");
			printf("    S_IP:���M��IP\n");
			printf("    S_Port:���M��Port\n");
			printf("    R_IP:���M��IP\n");
			printf("    R_Port:���M��Port\n");
			printf("    Data:���ۂ̑��M���e(�����w���)\n");
			printf("read\n");
			printf("    �ݒ�t�@�C�����ēǂݍ��݂��܂�\n");
			printf("help\n");
			printf("    �R�}���h�ɂ��Ă̏ڍׂ�\�����܂�\n");
		}
	}
}

