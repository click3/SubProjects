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
	FILE *fp;
	int data;
	int point;

	bitLoader(void){
		point = 0;
	}
	bitLoader(FILE *_fp){
		point = 0;
		setFilePointer(_fp);
	}

	void setFilePointer(FILE *_fp){
		fp = _fp;
	}

	int read(void){
		if(point == 0){
			fread(&data,1,1,fp);
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
		fseek(fp,pos,flag);
	}
};


class bitWriter {
public:
	FILE *fp;
	int data;
	int point;

	void init(void){
		point = 0x80;
		data = 0;
	}

	bitWriter(void){
		init();
	}
	bitWriter(FILE *_fp){
		init();
		setFilePointer(_fp);
	}

	void setFilePointer(FILE *_fp){
		fp = _fp;
	}
	void flash(void){
		if(point != 0x80){
			fwrite(&data,1,1,fp);
		}
		init();
	}
	void write(bool flag){
		data += flag ? point : 0;
		point >>= 1;
		if(point == 0){
			flash();
		}
	}
	void writeNumberSize(unsigned int a, int size){
		unsigned int pos = 1<<(size-1);
		while(pos > 0){
			write((a&pos) != 0);
			pos >>= 1;
		}
	}
	void writeNumber(unsigned int a){
		if(a > 0x00FFFFFF){
			writeNumberSize(3, 2);
			writeNumberSize(a, 32);
		} else if(a > 0x0000FFFF){
			writeNumberSize(2, 2);
			writeNumberSize(a, 24);
		} else if(a > 0x000000FF){
			writeNumberSize(1, 2);
			writeNumberSize(a, 16);
		} else {
			writeNumberSize(0, 2);
			writeNumberSize(a, 8);
		}
	}
	void writeString(unsigned char *str){
		while(true){
			writeByte(*str);
			if(*str == '\0'){
				break;
			}
			str++;
		}
	}
	void writeByte(unsigned char a){
		writeNumberSize(a,8);
	}
	void seek(int pos, int flag = SEEK_SET){
		if(point != 0x80){
			flash();
		}
		fseek(fp,pos,flag);
	}
	int getAddr(void){
		return ftell(fp);
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


class th06_dat {
private:
	FILE *fp;
	bitLoader bit;
	void init(void){
		fp = NULL;
		count = 0;
		isLoaded = false;
		list = NULL;
	}
public:

	struct file_list {
		int aiueo;
		int akstn;
		int size;
		int addr;
		int hash;
		unsigned char name[256];
	};

	char fn[256];
	int count;
	int addr;
	bool isLoaded;
	struct file_list *list;

	th06_dat(void){
		init();
	}
	th06_dat(char *_fn){
		init();
		open(_fn);
	}
	~th06_dat(){
		if(fp != NULL){
			fclose(fp);
			fp = NULL;
		}
		if(list != NULL){
			free(list);
			list = NULL;
		}
		count = 0;
		isLoaded = false;
	}

	bool checkIdentifier(void){
		if(fp == NULL){
			return false;
		}
		int id;
		fread(&id,1,4,fp);
		return (id == '3GBP');
	}
	void open(char *_fn){
		if(_fn == NULL || strlen(_fn) == 0){
			printf("fn NG\n");
			return;
		}
		strcpy(fn,_fn);
		fp = fopen(fn,"rb");
		if(fp == NULL){
			printf("ファイルオープンNG(%s)\n",fn);
			return;
		}
		if(checkIdentifier() == false){
			printf("識別子NG\n");
			return;
		}

		bit.setFilePointer(fp);
		count = bit.readNumber();
		addr = bit.readNumber();
		bit.seek(addr);
		list = (struct file_list*)malloc(sizeof(struct file_list)*count);

		int i = 0;
		while(i < count){
			list[i].aiueo = bit.readNumber();
			list[i].akstn = bit.readNumber();
			list[i].hash = bit.readNumber();
			list[i].addr = bit.readNumber();
			list[i].size = bit.readNumber();
			bit.readString(list[i].name, sizeof(list[i].name));
			//printf("%08x\t%08x\t%08x\t%08x\t%08x\t%s\n",list[i].aiueo,list[i].akstn,list[i].size,list[i].addr,list[i].wawon,list[i].name);
			i++;
		}

		isLoaded = true;
	}
	void extract(int id,unsigned char *out,int out_size){
		unsigned char *o = out;
		dic map(8192);
		bit.seek(list[id].addr);

		while(o < out+out_size){
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
	}
	void save(int id){
		unsigned char *data = (unsigned char *)malloc(list[id].size);
		extract(id, data, list[id].size);

		FILE *file = fopen((char*)list[id].name,"wb");
		fwrite(data, 1, list[id].size,file);
		fclose(file);
		free(data);
	}
	void allSave(void){
		printf("東方紅魔郷用datファイルの展開を開始します\n");
		int i = 0;
		while(i < count){
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
			printf("(%3d/%3d)%s          ",i+1,count,list[i].name);
			save(i);
			i++;
		}
		printf("\n展開を完了しました\n");
	}
	int hash(int id){
		bit.seek(list[id].addr);
		int max;
		if(id+1 < count){
			max = list[id+1].addr - list[id].addr;
		} else {
			max = addr - list[id].addr;
		}
		int i = 0;
		int a = 0;
		while(i < max){
			a += bit.readByte();
			i++;
		}
		return a;
	}
	bool hashCheck(void){
		int i = 0;
		while(i < count){
			if(list[i].hash != hash(i)){
				return false;
			}
			i++;
		}
		return true;
	}
};

class th06_dat_out {
private:
	struct file_list{
		int size;
		int addr;
		int hash;
		unsigned char path[256];
		unsigned char name[256];
	};
	struct file_list *list;
	int listSize;
	struct file_list *tail;
	bitWriter bit;
	int addr;
public:

	th06_dat_out(){
		list = NULL;
		listSize = 0;
		tail = NULL;
	}

	bool GetFileNameList(const char *dir, char **buf, int *num){
		HANDLE hSearch;
		WIN32_FIND_DATA fd;
		int count,t;
		char temp[256];
		bool ret;

		strncpy(temp,dir,sizeof(temp));
		strncat(temp,"/*",sizeof(temp));
		hSearch = FindFirstFile(temp, &fd );
		if(hSearch == INVALID_HANDLE_VALUE){
			return false;
		}
		count = 0;
		while(1){
			if(fd.cFileName[0] != '.'){
				sprintf(temp,"%s/%s",dir,fd.cFileName);
				t = *num-count;
				if(t < 0){
					t = 0;
				}
				ret = GetFileNameList(temp, buf, &t);
				if(ret && t > 0){
					count += t;
					if(buf!=NULL){
						buf += t;
					}
				} else {
					if(*num - count > 0){
						*buf = static_cast<char*>(malloc(sizeof(char)*strlen(temp)+1));
						strcpy(*buf,temp);
						buf++;
					}
					count++;
				}
			}
			if(!FindNextFile( hSearch, &fd )){
				if( GetLastError() == ERROR_NO_MORE_FILES ){
					break;
				}
			}
		}
		FindClose( hSearch );

		*num = count;
		return true;
	}
	void openDir(char *path){
		char **buf = NULL;
		int count = 0;
		if(GetFileNameList(path, buf, &count) == false){
			printf("ファイル一覧取得失敗\n");
			return;
		}
		printf("ファイル数:%d\n",count);
		buf = static_cast<char**>(malloc(sizeof(char*) * count));
		GetFileNameList(path, buf, &count);
		mallocList(count);
		int i = 0;
		while(i < count){
			addList(buf[i]);
			i++;
		}
		{
			int i = 0;
			while(i < count){
				free(buf[i]);
				i++;
			}
			free(buf);
		}
	}
	void mallocList(int count){
		if(list != NULL){
			free(list);
		}
		list = NULL;
		while(list == NULL){
			list = static_cast<struct file_list*>(malloc(sizeof(struct file_list)*count));
		}
		listSize = count;
		tail = list;
	}
	void addList(char *path){
		FILE *fp = fopen(path,"rb");
		if(fp == NULL){
			return;
		}
		fseek(fp, 0, SEEK_END);
		tail->size = ftell(fp);
		fclose(fp);

		strcpy((char*)tail->path,path);
		char *s = strrchr(path,'/');
		char *t = strrchr(path,'\\');
		if(t > s){
			s = t;
		}
		if(s == NULL){
			s = path;
		} else {
			s += 1;
		}
		strcpy((char*)tail->name, s);
		tail++;
	}

	void print(void){
		struct file_list *p = list;
		while(p < tail){
			printf("%08x\t%08x\t%08x\t%s\t%s\n",p->size,p->addr,p->hash, p->name, p->path);
			p++;
		}
	}
	void saveFile(int id){
		struct file_list *p = &list[id];
		p->addr = bit.getAddr();
		int i = 0;
		int data = 0x100;
		FILE *fp = fopen((char*)p->path, "rb");
		while(i < p->size){
			fread(&data, 1, 1, fp);
			bit.writeNumberSize(data, 9);
			i++;
		}
	}
	void saveFileListData(int id){
		struct file_list *p = &list[id];
		bit.writeNumber(0);
		bit.writeNumber(0);
		bit.writeNumber(p->hash);
		bit.writeNumber(p->addr);
		bit.writeNumber(p->size);
		bit.writeString(p->name);
	}
	void saveFileList(void){
		int i = 0;
		while(i < listSize){
			saveFileListData(i);
			i++;
		}
	}
	void save(char *path){
		printf("紅魔郷用datファイルの作成を開始します\n");
		FILE *fp = fopen(path,"w+b");
		if(fp == NULL){
			printf("%s ファイル作成失敗\n",path);
			return;
		}
		bit.setFilePointer(fp);
		bit.writeNumberSize('PBG3',32);
		//最後に書き込めるよう余分に空けておく
		bit.writeNumberSize(0,32);
		bit.writeNumberSize(0,32);
		bit.writeNumberSize(0,8);
		{
			int i = 0;
			while(i < listSize){
				printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
				printf("(%3d/%3d)%s          ",i+1,listSize,list[i].name);
				saveFile(i);
				bit.flash();
				i++;
			}
		}
		printf("\n格納ファイル情報を書き込み中\n");
		addr = bit.getAddr();
		{
			int i = 0;
			while(i < listSize){
				list[i].hash = hash(i);
				i++;
			}
		}
		bit.seek(addr);
		saveFileList();
		bit.seek(4);
		bit.writeNumber(listSize);
		bit.writeNumber(addr);
		bit.flash();
		fclose(fp);
		printf("datファイルを作成しました\n");
	}
	int hash(int id){
		bitLoader bit(this->bit.fp);
		bit.seek(list[id].addr);
		int max;
		if(id+1 < listSize){
			max = list[id+1].addr - list[id].addr;
		} else {
			max = addr - list[id].addr;
		}
		int i = 0;
		int a = 0;
		while(i < max){
			a += bit.readByte();
			i++;
		}
		return a;
	}
};


char *getDatName(char *dirPath){
	static char fn[256];
	char *s = strrchr(dirPath,'/');
	char *t = strrchr(dirPath,'\\');
	if(t > s){
		s = t;
	}
	if(s == NULL){
		s = dirPath;
	} else {
		s += 1;
	}
	sprintf(fn,"%s.dat",s);
	return fn;
}
char *getDirName(char *datPath){
	static char fn[256];
	strcpy(fn,datPath);
	char *s = strrchr(fn,'.');
	if(s != NULL){
		*s = '\0';
	}
	return fn;
}
int main(int argc, char **argv){
	unsigned char *data;
	char fn[256];
	int i;
	SetAppDir();

	//dat作成
	i = 1;
	while(i < argc){
		th06_dat_out dat;
		dat.openDir(argv[i]);
		dat.save(getDatName(argv[i]));
		i++;
	}

	printf("全ての処理が終了しました\nEnterを押すと終了します\n");
	getchar();
	return 0;
}

