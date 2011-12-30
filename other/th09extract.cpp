#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>


void SetAppDir(void){
	int i;
	char AppDir[256];

	GetModuleFileName(NULL,AppDir,256);
	i = strlen(AppDir)-1;
	while(AppDir[i] != '\\' && AppDir[i] != '/' && i > 0)i--;
	AppDir[i] = '\0';
	SetCurrentDirectory(AppDir);
}
class bitLoader {
public:
	unsigned char *p;
	unsigned char *start;
	int data;
	int point;

	bitLoader(void){
		point = 0;
	}
	bitLoader(unsigned char *p){
		point = 0;
		setPointer(p);
	}

	void setPointer(unsigned char *_p){
		start = p;
		p = _p;
	}

	int read(void){
		if(point == 0){
			data = *p;
			p++;
			point = 0x80;
		}
		int ret = ((data & point) != 0);
		point >>= 1;
		return ret;
	}
	int readNumber(void){
		int count = 1;
		if(read() != 0){
			count += 2;
		}
		if(read() != 0){
			count += 1;
		}
		return readNumberSize(count*8);
	}
	int readString(unsigned char *str, int size){
		int i = 0;
		while(i < size){
			str[i] = readByte();
			if(str[i] == '\0'){
				break;
			}
			i++;
		}
		return i;
	}
	int readByte(void){
		return readNumberSize(8);
	}
	int readNumberSize(int size){
		unsigned int pos = 1 << (size-1);
		unsigned int ret = 0;
		while(pos > 0){
			ret += read() ? pos : 0;
			pos >>= 1;
		}
		return *(int*)&ret;
	}

	void seek(int pos, int flag = SEEK_SET){
		point = 0;
		if(flag == SEEK_SET){
			p = start + pos;
		} else if(SEEK_CUR){
			p += pos;
		} else {
			printf("error");
			getchar();
			exit(1);
		}
	}
};

class dic {
private:
	dic(){
		map = NULL;
		size = 0;
		addr = 0;
	}
public:
	unsigned char *map;
	int size;
	int addr;
	dic(int _size){
		size = _size;
		map = (unsigned char*)malloc(size);
		memset(map,0,size);
		addr = 0;
	}
	~dic(){
		if(map != NULL){
			free(map);
		}
	}
	void next(int a = 1){
		addr += a;
		if(addr > size){
			addr -= size;
		}
	}
	void back(int a = 1){
		addr -= a;
		if(addr < 0){
			addr += size;
		}
	}
	void set(unsigned char a){
		map[addr%size] = a;
		next();
	}
	unsigned char get(int pos = 0){
		return map[pos%size];
	}

	void print(int start,int len){
		int i = start;
		while(i < start+len){
			printf("%02x ",map[i]);
			i++;
			if(i%16 == 0){
				printf("\b\n");
			}
		}
		if(i%16 != 0){
			printf("\b\n");
		}
	}
};

class th09dat {
private:
	FILE *fp;
	struct key{
		int id;
		int a;
		int b;
		int block;
		int len;
	};
	struct key keyList[8];

	bool XORCrypt(int a, int b, int len, unsigned char *data){
		if(len < 0 || data == NULL || a > 0xFF || a <= 0 || b > 0xFF || b <= 0){
			return false;
		}
		while(len){
			*data ^= a;
			a += b;
			len--;
			data++;
		}
		return true;
	}

	bool UnCrypt(int a,int b,int len,int block,unsigned char *data){
		if(len < 0 || data == NULL || a > 0xFF || a <= 0 || b > 0xFF || b <= 0 || block <= 2){
			return false;
		}
		{
			int t = len%block;
			if(t && t < block/4){
				if(len%2==1){
					len--;
				}
				len -= t;
			} else if(len%2==1){
				len--;
			}
		}
		if(!XORCrypt(a,b,len,data)){
			return false;
		}
		if(len == 0){
			return true;
		}
		unsigned char *ret = static_cast<unsigned char*>(malloc(len));
		int size,pos = 0;
		unsigned char *i =data, *o;
		while(pos < len){
			size = block > len-pos ? len-pos : block;
			o = &ret[pos+size-1];
			int half = size/2;
			while(o >= &ret[pos]){
				*o = *i;
				i++;
				o -= 2;
			}
			o = &ret[pos+size-2];
			while(i < &data[pos+size]){
				*o = *i;
				i++;
				o -= 2;
			}
			pos += size;
		}
		memcpy(data,ret,len);
		free(ret);
		return true;
	}
	bool UnCrypt(unsigned char *data, int size,int i){
		return UnCrypt(keyList[i].a,keyList[i].b,keyList[i].len > size ? size : keyList[i].len,keyList[i].block,data);
	}
	bool UnLZSS(unsigned char *in,int in_size,unsigned char *out, int out_size){
		unsigned char *o = out;
		dic map(8192);
		bitLoader bit(in);
		while(o < out+out_size && bit.p < in+in_size){
			if(bit.read() == 1){
				*o = bit.readByte();
				map.set(*o);
				o++;
			} else {
				int pos = bit.readNumberSize(13);
				if(pos == 0){
					break;
				}
				int len = bit.readNumberSize(4) + 3;
				int i = 0;
				while(i < len){
					*o = map.get(pos+i-1);
					if((o-out)>>8 == 0xffffffff){
						printf("%02x\n",*o);
					}
					map.set(*o);
					o++;
					i++;
				}
			}
		}
		return true;
	}

	bool LoadHead(FILE *fp){
		if(fp == NULL){
			return false;
		}
		int head[3];
		fread(head,1,12,fp);
		if(!UnCrypt(0x1b,0x37,0x0c,0x0c,(unsigned char*)head)){
			return false;
		}
		count = head[0] + 0xFFFE1DC1;
		flistAddr = head[1] + 0xFFFAB9B2;
		listSize = head[2] + 0xFFF755AD;
		return true;
	}
	bool LoadFList(FILE *fp){
		int size;
		fseek(fp,0,SEEK_END);
		size = ftell(fp) - flistAddr;
		fseek(fp,flistAddr,SEEK_SET);
		unsigned char *data = static_cast<unsigned char*>(malloc(size));
		fread(data,1,size,fp);
		UnCrypt(0x3E,0x9B,0x400 > size ? size : 0x400,0x80,data);
		unsigned char *temp = data;
		data = static_cast<unsigned char*>(malloc(listSize));
		UnLZSS(temp,size,data,listSize);
		free(temp);

		list = static_cast<struct list*>(malloc(sizeof(struct list) * count));
		int i = 0;
		char *p = (char*)data;
		while(i < count){
			int len = strlen(p)+1;
			list[i].name = static_cast<char *>(malloc(len));
			strcpy(list[i].name,p);
			p += len;
			list[i].addr = *(int*)p;
			p+=4;
			list[i].size = *(int*)p;
			p+=4;
			list[i].fumei = *(int*)p;
			p+=4;
			i++;
		}
		free(data);
		return true;
	}
	int GetKey(int id){
		int i = 0;
		while(i < sizeof(keyList) / sizeof(struct key)){
			if(keyList[i].id == id){
				return i;
			}
			i++;
		}
		return i-1;
	}
public:
	unsigned int id;
	unsigned int count;
	unsigned int flistAddr;
	unsigned int listSize;
struct list{
	char *name;
	int addr;
	int size;
	int fumei;
};
	struct list *list;

	void init(){
		struct key keyList[8] = {
			{0x4D,	0x1B,	0x37,	0x00000040,	0x00002800},//
			{0x54,	0x51,	0xE9,	0x00000040,	0x00003000},//
			{0x41,	0xC1,	0x51,	0x00000400,	0x00000400},//
			{0x4A,	0x03,	0x19,	0x00000400,	0x00000400},//
			{0x45,	0xAB,	0xCD,	0x00000200,	0x00001000},//
			{0x57,	0x12,	0x34,	0x00000400,	0x00000400},//
			{0x2D,	0x35,	0x97,	0x00000080,	0x00002800},//
			{0x85,	0x99,	0x37,	0x00000400,	0x00001000}
		};
		memcpy(this->keyList, keyList, sizeof(keyList));
		fp = NULL;
	}
	th09dat(){
		init();
	}
	th09dat(char *fn){
		init();
		Load(fn);
	}
	~th09dat(){
		if(fp != NULL){
			fclose(fp);
		}
	}
	bool Load(char *fn){
		fp = fopen(fn, "rb");
		if(fp == NULL){
			return false;
		}
		fread(&id,4,1,fp);
		if(id != 0x5a474250){
			return false;
		}
		if(!LoadHead(fp)){
			return false;
		}
		if(!LoadFList(fp)){
			return false;
		}
		return true;
	}
	bool extract(int i){
		int size;
		fseek(fp,list[i].addr,SEEK_SET);
		if(i == count-1){
			size = flistAddr - list[i].addr;
		} else {
			size = list[i+1].addr - list[i].addr;
		}
		//printf("%08x\t%d\n",ftell(fp),size);
		unsigned char *data = static_cast<unsigned char *>(malloc(size));
		unsigned char *out = static_cast<unsigned char *>(malloc(list[i].size));
		fread(data,1,size,fp);
		UnLZSS(data,size,out,list[i].size);
		free(data);
		if(out[0]==0x65 && out[1]==0x64 && out[2]==0x7A){/*
			int size = keyList[GetKey(out[3])].len > list[i].size-4 ? list[i].size-4 : keyList[GetKey(out[3])].len;
			int block = keyList[GetKey(out[3])].block;
			int a = size%block;
			if(a && a < block/4){
				printf("%s\t%d\t%d\t%d\n",list[i].name,a,size,block);
			}*/

			FILE *wfp = fopen(list[i].name,"wb");
			UnCrypt(out+4,list[i].size-4,GetKey(out[3]));
			fwrite(out+4,1,list[i].size-4,wfp);
			fclose(wfp);
		}
		free(out);
		return true;
	}
};



int main(int argc,char **argv){
	char *fn;
	if(argc != 2){
		fn = "th09.dat";
	} else {
		fn = argv[1];
	}
	th09dat dat(fn);
	SetAppDir();
	CreateDirectory("data",NULL);
	SetCurrentDirectory("data");
	int i = 0;
	while(i < dat.count){
		//if(strcmp(dat.list[i].name,"end07.end")==0){
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		printf("(%3d/%3d)%s               ",i+1,dat.count,dat.list[i].name);
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		dat.extract(i);
//}
		i++;
	}
	printf("\n“WŠJ‚ªŠ®—¹‚µ‚Ü‚µ‚½\nI—¹‚·‚éê‡‚ÍEnter‚ð‰Ÿ‚µ‚Ä‚­‚¾‚³‚¢");
	getchar();
	return 0;
}

