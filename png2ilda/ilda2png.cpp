#include <stdio.h>
#include <vector>
#include "libpng.h"

#define ASSERT(a) if(!(a)){printf("ASSERT %s:%d\n%s\n",__FILE__ ,__LINE__,#a);getchar();}

void put32bit(char *data, int n){
	data[0] = ((char*)&n)[3];
	data[1] = ((char*)&n)[2];
	data[2] = ((char*)&n)[1];
	data[3] = ((char*)&n)[0];
}
void put32bit(char *data, unsigned int n){
	put32bit(data, *(int*)&n);
}
void put16bit(char *data, short n){
	data[0] = ((char*)&n)[1];
	data[1] = ((char*)&n)[0];
}
void put16bit(char *data, unsigned short n){
	put16bit(data, *(short*)&n);
}
void put16bit(char *data, int n){
	put16bit(data, *(short*)&n);
}
void put16bit(char *data, unsigned int n){
	put16bit(data, *(short*)&n);
}
void put8bit(char *data, char n){
	*data = n;
}
void put8bit(char *data, unsigned char n){
	put8bit(data, *(char*)&n);
}
void put8bit(char *data, int n){
	put8bit(data, *(char*)&n);
}
void put8bit(char *data, unsigned int n){
	put8bit(data, *(char*)&n);
}
int read32bit(char *data){
	int ret;
	((char*)&ret)[0] = data[3];
	((char*)&ret)[1] = data[2];
	((char*)&ret)[2] = data[1];
	((char*)&ret)[3] = data[0];
	return ret;
}
short read16bit(char *data){
	short ret;
	((char*)&ret)[0] = data[1];
	((char*)&ret)[1] = data[0];
	return ret;
}
char read8bit(char *data){
	return *data;
}

class ilda {
public:
	class palette {
	public:
		int FormatID;
		char Name[9];
		char Company[9];
		int PaletteID;
		int ScanHead;
		class color {
		public:
			unsigned char r;
			unsigned char g;
			unsigned char b;
			bool puts(char *data){
				((unsigned char*)data)[0] = r;
				((unsigned char*)data)[1] = g;
				((unsigned char*)data)[2] = b;
				return true;
			}
			bool puts(FILE *fp){
				char data[3];
				return (puts(data) && (fwrite(data,1,3,fp) == 3));
			}
			int size(){
				return 3;
			}
			bool read(char *data, int *length) {
				r = read8bit(data);
				data += 1;
				g = read8bit(data);
				data += 1;
				b = read8bit(data);
				data += 1;
				*length = size();
				return true;
			}
		};
		std::vector<color> colors;
		bool putHeader(char *data){
			strcpy(data, "ILDA");//ファイル識別子
			data += 4;
			put32bit(data, FormatID);//フォーマットコード(カラーパレッド)
			data += 4;
			strncpy(data, Name, 8);//フレーム名
			data += 8;
			strncpy(data, Company, 8);//会社名
			data += 8;
			put16bit(data, colors.size());//Total colors
			data += 2;
			put16bit(data, PaletteID);//Palette number
			data += 2;
			put16bit(data, 0);//予備1
			data += 2;
			put16bit(data, ScanHead);//スキャナーヘッド
			data += 1;
			put8bit(data, 0);//予備2
			return true;
		}
		bool puts(char *data){
			if(!putHeader(data)){
				return false;
			}
			data += 32;
			std::vector<color>::iterator it = colors.begin();
			while(it != colors.end()){
				if(!it->puts(data)){
					return false;
				}
				data += it->size();
				++it;
			}
			return true;
		}
		bool puts(FILE *fp){
printf("palette puts\n");
			char data[32];
			if(!putHeader(data) || fwrite(data,1,sizeof(data),fp) != sizeof(data)){
				return false;
			}
			std::vector<color>::iterator it = colors.begin();
			while(it != colors.end()){
				if(!it->puts(fp)){
					return false;
				}
				++it;
			}
printf("palette end\n");
			return true;
		}
		int size() {
			std::vector<color>::iterator it = colors.begin();
			int ret = 32;
			while(it != colors.end()){
				ret += it->size();
				++it;
			}
			return ret;
		}
		bool read(char *data, int *length){
			int colorNum;
			int yobi;
			if(strncmp(data, "ILDA", 4) != 0) {
				char Identity[5];
				strncpy(Identity, data, 4);
				Identity[4] = '\0';
				printf("Not Identity:%s\n", Identity);
				return false;
			}
			data += 4;
			FormatID = read32bit(data);
			if(FormatID != 2 && FormatID != 3){
				printf("Undefind Format ID:%d\n",FormatID);
				return false;
			}
			data += 4;
			memcpy(Name, data, 8);
			Name[8] = '\0';
			data += 8;
			memcpy(Company, data, 8);
			Company[8] = '\0';
			data += 8;
			colorNum = read16bit(data);
			data += 2;
			PaletteID = read16bit(data);
			data += 2;
			yobi = read16bit(data);
			if(yobi != 0){
				printf("yobi error:%d\n",yobi);
				//return false;
			}
			data += 2;
			ScanHead = read8bit(data);
			data += 1;
			yobi = read8bit(data);
			if(yobi != 0){
				return false;
			}
			data += 1;
			*length = 32;
			while(colorNum > 0){
				color c;
				int len;
				if(!c.read(data, &len)){
					return false;
				}
				data += len;
				*length += len;
				colorNum--;
				colors.push_back(c);
			}
			return true;
		}
	};
	class frame {
	public:
		char Name[9];
		char Company[9];
		int FormatID;
		int Num;
		int AllNum;
		int ScanHead;
		class point {
		public:
			bool IsXYOnly;
			short x;
			short y;
			short z;
			unsigned char status;
			unsigned char colorID;
			bool puts(char *data){
				put16bit(data, x);
				data += 2;
				put16bit(data, y);
				data += 2;
				if(!IsXYOnly){
					put16bit(data, z);
					data += 2;
				}
				put8bit(data, status);
				data += 1;
				put8bit(data, colorID);
				return true;
			}
			bool puts(FILE *fp){
				char data[8];
				if(!puts(data) || fwrite(data,1,size(),fp) != size()){
					return false;
				}
				return true;
			}
			int size(){
				return 6 + (IsXYOnly ? 0 : 2);
			}
			bool read(char *data, int *length, bool _IsXYOnly = false) {
				IsXYOnly = _IsXYOnly;
				x = read16bit(data);
				data += 2;
				y = read16bit(data);
				data += 2;
				if(!IsXYOnly){
					z = read16bit(data);
					data += 2;
				}
				status = read8bit(data);
				data += 1;
				colorID = read8bit(data);
				*length = size();
				return true;
			}
		};
		std::vector<point> points;
		bool putHeader(char *data){
			strcpy(data, "ILDA");
			data += 4;
			put32bit(data, FormatID);
			data += 4;
			strncpy(data, Name, 8);//フレーム名
			data += 8;
			strncpy(data, Company, 8);//会社名
			data += 8;
			put16bit(data, points.size());//総ポイント数
			data += 2;
			put16bit(data, Num);//フレーム番号
			data += 2;
			put16bit(data, AllNum);//総フレーム数
			data += 2;
			put8bit(data, ScanHead);//スキャナヘッド
			data += 1;
			put8bit(data, 0);//予備
			return true;
		}
		bool puts(char *data){
			if(!putHeader(data)){
				return false;
			}
			data += 32;
			std::vector<point>::iterator it = points.begin();
			while(it != points.end()){
				if(!it->puts(data)){
					return false;
				}
				data += it->size();
				++it;
			}
			return true;
		}
		bool puts(FILE *fp){
			char data[32];
			if(!putHeader(data) || fwrite(data,1,sizeof(data),fp) != sizeof(data)){
				return false;
			}
			std::vector<point>::iterator it = points.begin();
			while(it != points.end()){
				if(!it->puts(fp)){
					return false;
				}
				++it;
			}
			return true;
		}
		int size(){
			std::vector<point>::iterator it = points.begin();
			int ret = 32;
			while(it != points.end()){
				ret += it->size();
				++it;
			}
			return ret;
		}
		bool read(char *data, int *length){
			int pointNum;
			int yobi;
			bool IsXYOnly;
			if(strncmp(data, "ILDA", 4) != 0) {
				return false;
			}
			data += 4;
			FormatID = read32bit(data);
			if(FormatID != 0 && FormatID != 1){
				return false;
			}
			data += 4;
			memcpy(Name, data, 8);
			Name[8] = '\0';
			data += 8;
			memcpy(Company, data, 8);
			Company[8] = '\0';
			data += 8;
			pointNum = read16bit(data);
			data += 2;
			Num = read16bit(data);
			data += 2;
			AllNum = read16bit(data);
			data += 2;
			ScanHead = read8bit(data);
			data += 1;
			yobi = read8bit(data);
			data += 1;
			if(yobi != 0){
				return false;
			}
			IsXYOnly = (FormatID == 1);
			*length = 32;
			while(pointNum > 0){
				point p;
				int len;
				if(!p.read(data, &len, IsXYOnly)){
					return false;
				}
				data += len;
				*length += len;
				pointNum--;
				points.push_back(p);
			}
			return true;
		}
	};
	std::vector<palette> palettes;
	std::vector<frame> frames;
	bool puts(char *data){
		std::vector<frame>::iterator fi = frames.begin();
		std::vector<palette>::iterator pi = palettes.begin();
		int count = 0;
		while(fi != frames.end() || pi != palettes.end()){
			if(fi != frames.end()){
				if(!fi->puts(data)){
					return false;
				}
				count++;
				data += fi->size();
				++fi;
			}
			if(pi != palettes.end()){
				if(!pi->puts(data)){
					return false;
				}
				data += pi->size();
				++pi;
			}
		}
		return true;
	}
	bool puts(FILE *fp){
		std::vector<frame>::iterator fi = frames.begin();
		std::vector<palette>::iterator pi = palettes.begin();
		while(fi != frames.end() || pi != palettes.end()){
			if(fi != frames.end()){
				if(!fi->puts(fp)){
					return false;
				}
				++fi;
			}
			if(pi != palettes.end()){
				if(!pi->puts(fp)){
					return false;
				}
				++pi;
			}
		}
		return true;
	}
	int size(){
		int ret = 0;
		{
			std::vector<palette>::iterator it = palettes.begin();
			while(it != palettes.end()){
				ret += it->size();
				++it;
			}
		}
		{
			std::vector<frame>::iterator it = frames.begin();
			while(it != frames.end()){
				ret += it->size();
				++it;
			}
		}
		return ret;
	}
	bool read(char *data){
		while(strcmp(data,"ILDA") == 0){
			int format;
			format = read32bit(data+4);
			switch(format){
				case 0:
				case 1:{
					frame fr;
					int length;
					if(!fr.read(data, &length)){
						return false;
					}
					data += length;
					frames.push_back(fr);
					break;
				}
				case 2:{
					palette pl;
					int length;
					if(!pl.read(data, &length)){
						return false;
					}
					data += length;
					palettes.push_back(pl);
					break;
				}
				case 3:{
					int i = 8;
					bool flag = true;
					while(i < 24){
						flag = flag && 0x20 <= data[i] && data[i] <= 0x79;
						i++;
					}
					if(flag){//先頭16字がASCIIだったら、独自拡張版フォーマットID3だと認識する。
						palette pl;
						int length;
						if(!pl.read(data, &length)){
							return false;
						}
						data += length;
						palettes.push_back(pl);
						break;
					}
				}
				default:{
					int length = read32bit(data+8);
					data += 12 + length;
					break;
				}
			}
		}
		return true;
	}
};

bool pngout(unsigned int width, unsigned int height, png_bytepp data, char *filename, unsigned int color_num) {
	FILE		*fp;
	png_structp	png_ptr;
	png_infop	info_ptr;

	fp = fopen(filename, "wb");
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, fp);
	png_set_filter(png_ptr, 0, PNG_ALL_FILTERS);
	png_set_compression_level(png_ptr, 9);
	png_set_IHDR(png_ptr, info_ptr, width, height,
	                color_num, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
	                PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_set_gAMA(png_ptr, info_ptr, 1.0);
	png_write_info(png_ptr, info_ptr);
	png_write_image(png_ptr, data);
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
	return true;

}

void ilda2png(ilda &image, char *fileName) {
	const static unsigned char ilda_palette[256][3]={
		{   0,   0,   0 },	{ 255, 255, 255 },	{ 255,   0,   0 },	{ 255, 255,   0 },	{   0, 255,   0 },
		{   0, 255, 255 },	{   0,   0, 255 },	{ 255,   0, 255 },	{ 255, 128, 128 },	{ 255, 140, 128 },
		{ 255, 151, 128 },	{ 255, 163, 128 },	{ 255, 174, 128 },	{ 255, 186, 128 },	{ 255, 197, 128 },
		{ 255, 209, 128 },	{ 255, 220, 128 },	{ 255, 232, 128 },	{ 255, 243, 128 },	{ 255, 255, 128 },
		{ 243, 255, 128 },	{ 232, 255, 128 },	{ 220, 255, 128 },	{ 209, 255, 128 },	{ 197, 255, 128 },
		{ 186, 255, 128 },	{ 174, 255, 128 },	{ 163, 255, 128 },	{ 151, 255, 128 },	{ 140, 255, 128 },
		{ 128, 255, 128 },	{ 128, 255, 140 },	{ 128, 255, 151 },	{ 128, 255, 163 },	{ 128, 255, 174 },
		{ 128, 255, 186 },	{ 128, 255, 197 },	{ 128, 255, 209 },	{ 128, 255, 220 },	{ 128, 255, 232 },
		{ 128, 255, 243 },	{ 128, 255, 255 },	{ 128, 243, 255 },	{ 128, 232, 255 },	{ 128, 220, 255 },
		{ 128, 209, 255 },	{ 128, 197, 255 },	{ 128, 186, 255 },	{ 128, 174, 255 },	{ 128, 163, 255 },
		{ 128, 151, 255 },	{ 128, 140, 255 },	{ 128, 128, 255 },	{ 140, 128, 255 },	{ 151, 128, 255 },
		{ 163, 128, 255 },	{ 174, 128, 255 },	{ 186, 128, 255 },	{ 197, 128, 255 },	{ 209, 128, 255 },
		{ 220, 128, 255 },	{ 232, 128, 255 },	{ 243, 128, 255 },	{ 255, 128, 255 },	{ 255, 128, 243 },
		{ 255, 128, 232 },	{ 255, 128, 220 },	{ 255, 128, 209 },	{ 255, 128, 197 },	{ 255, 128, 186 },
		{ 255, 128, 174 },	{ 255, 128, 163 },	{ 255, 128, 151 },	{ 255, 128, 140 },	{ 255,   0,   0 },
		{ 255,  23,   0 },	{ 255,  46,   0 },	{ 255,  70,   0 },	{ 255,  93,   0 },	{ 255, 116,   0 },
		{ 255, 139,   0 },	{ 255, 162,   0 },	{ 255, 185,   0 },	{ 255, 209,   0 },	{ 255, 232,   0 },
		{ 255, 255,   0 },	{ 232, 255,   0 },	{ 209, 255,   0 },	{ 185, 255,   0 },	{ 162, 255,   0 },
		{ 139, 255,   0 },	{ 116, 255,   0 },	{  93, 255,   0 },	{  70, 255,   0 },	{  46, 255,   0 },
		{  23, 255,   0 },	{   0, 255,   0 },	{   0, 255,  23 },	{   0, 255,  46 },	{   0, 255,  70 },
		{   0, 255,  93 },	{   0, 255, 116 },	{   0, 255, 139 },	{   0, 255, 162 },	{   0, 255, 185 },
		{   0, 255, 209 },	{   0, 255, 232 },	{   0, 255, 255 },	{   0, 232, 255 },	{   0, 209, 255 },
		{   0, 185, 255 },	{   0, 162, 255 },	{   0, 139, 255 },	{   0, 116, 255 },	{   0,  93, 255 },
		{   0,  70, 255 },	{   0,  46, 255 },	{   0,  23, 255 },	{   0,   0, 255 },	{  23,   0, 255 },
		{  46,   0, 255 },	{  70,   0, 255 },	{  93,   0, 255 },	{ 116,   0, 255 },	{ 139,   0, 255 },
		{ 162,   0, 255 },	{ 185,   0, 255 },	{ 209,   0, 255 },	{ 232,   0, 255 },	{ 255,   0, 255 },
		{ 255,   0, 232 },	{ 255,   0, 209 },	{ 255,   0, 185 },	{ 255,   0, 162 },	{ 255,   0, 139 },
		{ 255,   0, 116 },	{ 255,   0,  93 },	{ 255,   0,  70 },	{ 255,   0,  46 },	{ 255,   0,  23 },
		{ 128,   0,   0 },	{ 128,  12,   0 },	{ 128,  23,   0 },	{ 128,  35,   0 },	{ 128,  47,   0 },
		{ 128,  58,   0 },	{ 128,  70,   0 },	{ 128,  81,   0 },	{ 128,  93,   0 },	{ 128, 105,   0 },
		{ 128, 116,   0 },	{ 128, 128,   0 },	{ 116, 128,   0 },	{ 105, 128,   0 },	{  93, 128,   0 },
		{  81, 128,   0 },	{  70, 128,   0 },	{  58, 128,   0 },	{  47, 128,   0 },	{  35, 128,   0 },
		{  23, 128,   0 },	{  12, 128,   0 },	{   0, 128,   0 },	{   0, 128,  12 },	{   0, 128,  23 },
		{   0, 128,  35 },	{   0, 128,  47 },	{   0, 128,  58 },	{   0, 128,  70 },	{   0, 128,  81 },
		{   0, 128,  93 },	{   0, 128, 105 },	{   0, 128, 116 },	{   0, 128, 128 },	{   0, 116, 128 },
		{   0, 105, 128 },	{   0,  93, 128 },	{   0,  81, 128 },	{   0,  70, 128 },	{   0,  58, 128 },
		{   0,  47, 128 },	{   0,  35, 128 },	{   0,  23, 128 },	{   0,  12, 128 },	{   0,   0, 128 },
		{  12,   0, 128 },	{  23,   0, 128 },	{  35,   0, 128 },	{  47,   0, 128 },	{  58,   0, 128 },
		{  70,   0, 128 },	{  81,   0, 128 },	{  93,   0, 128 },	{ 105,   0, 128 },	{ 116,   0, 128 },
		{ 128,   0, 128 },	{ 128,   0, 116 },	{ 128,   0, 105 },	{ 128,   0,  93 },	{ 128,   0,  81 },
		{ 128,   0,  70 },	{ 128,   0,  58 },	{ 128,   0,  47 },	{ 128,   0,  35 },	{ 128,   0,  23 },
		{ 128,   0,  12 },	{ 255, 192, 192 },	{ 255,  64,  64 },	{ 192,   0,   0 },	{  64,   0,   0 },
		{ 255, 255, 192 },	{ 255, 255,  64 },	{ 192, 192,   0 },	{  64,  64,   0 },	{ 192, 255, 192 },
		{  64, 255,  64 },	{   0, 192,   0 },	{   0,  64,   0 },	{ 192, 255, 255 },	{  64, 255, 255 },
		{   0, 192, 192 },	{   0,  64,  64 },	{ 192, 192, 255 },	{  64,  64, 255 },	{   0,   0, 192 },
		{   0,   0,  64 },	{ 255, 192, 255 },	{ 255,  64, 255 },	{ 192,   0, 192 },	{  64,   0,  64 },
		{ 255,  96,  96 },	{ 255, 255, 255 },	{ 245, 245, 245 },	{ 235, 235, 235 },	{ 224, 224, 224 },
		{ 213, 213, 213 },	{ 203, 203, 203 },	{ 192, 192, 192 },	{ 181, 181, 181 },	{ 171, 171, 171 },
		{ 160, 160, 160 },	{ 149, 149, 149 },	{ 139, 139, 139 },	{ 128, 128, 128 },	{ 117, 117, 117 },
		{ 107, 107, 107 },	{  96,  96,  96 },	{  85,  85,  85 },	{  75,  75,  75 },	{  64,  64,  64 },
		{  53,  53,  53 },	{  43,  43,  43 },	{  32,  32,  32 },	{  21,  21,  21 },	{  11,  11,  11 },
		{   0,   0,   0 }
	};
	std::vector<ilda::frame>::iterator it = image.frames.begin();
	int frame_count = 0;
	while(it != image.frames.end()){
		frame_count++;
		unsigned int height = 0;
		unsigned int width = 0;
		if(it->points.size() == 0){
			++it;
			continue;
		}
		std::vector<ilda::frame::point>::iterator p = it->points.begin();
		while(p != it->points.end()){
			p->x = ((int)p->x + 32767) / 128;
			p->y = ((int)p->y + 32767) / 128 - 64;
			if(p->y == 383) {
				p->y = 382;
			}
			ASSERT(p->x >= 0);
			ASSERT(p->y >= 0);
			if(p->x >= width){
				width = p->x + 1;
			}
			if(p->y >= height){
				height = p->y + 1;
			}
			++p;
		}
		ASSERT(width < 512);
		ASSERT(height < 384);
		width = 512;
		height = 384;
		unsigned int size = width * height;
		unsigned char *data = (unsigned char*)malloc(size);
		ASSERT(data != NULL);
		memset(data, 0, size);
		p = it->points.begin();
		unsigned int prev[2] = {0, 0};
		while(p != it->points.end()){
			ASSERT(p->x >= 0);
			ASSERT(p->y >= 0);
			const bool status = ((p->status&0x40) == 0);
			ASSERT((p->status&0x3F) == 0);
			if(status){
				int xDiff = prev[0] - p->x;
				int yDiff = prev[1] - p->y;
				int count = (abs(xDiff) > abs(yDiff) ? abs(xDiff) : abs(yDiff));
				for(int i = 0; i < count; i++){
					const unsigned int x = (p->x + i * xDiff / count);
					const unsigned int y = (p->y + i * yDiff / count);
					ASSERT(x >= 0);
					ASSERT(x < width);
					ASSERT(y >= 0);
					ASSERT(y < height);
					//printf("%d\t%3d:%3d => %3d:%3d(%d:%d) => %3d:%3d(%d:%d)\n",i,prev[0],prev[1],p->x,p->y,xDiff,yDiff,x,y,width,height);
					const unsigned int pos = x + (height - 1 - y) * width;
					ASSERT(pos >= 0);
					ASSERT(pos < width * height);
					data[pos] = 0xFF;
				}
			}
			prev[0] = p->x;
			prev[1] = p->y;
			++p;
		}
		char fileName2[256];
		sprintf(fileName2, "%04d_%s",frame_count, fileName);
		unsigned char **list = (unsigned char**)malloc(sizeof(unsigned char*) * height);
		ASSERT(list != NULL);
		for(unsigned int i = 0; i < height; i++) {
			list[i] = &data[width * i];
		}
		pngout(width, height, list, fileName2, 8);
		free(list);
		free(data);
		++it;
	}
	
}

void proc(char *inFileName,char *outFileName){
	FILE *fp = fopen(inFileName,"rb");
	fseek(fp,0,SEEK_END);
	int size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	char *data = (char*)malloc(size);
	fread(data,1,size,fp);
	fclose(fp);

	ilda image;
	if(!image.read(data)){
		printf("ilda parse error\n");
	} else {
	/*
		fp = fopen("testput.ild","wb");
		printf("%d\n",image.puts(fp));
		fclose(fp);
	*/

		ilda2png(image,outFileName);
	}
}

int main(int argc, char **argv){
	if(argc == 1){
		return 0;
	}
	while(argc > 1){
		char out[256];
		strcpy(out,argv[argc-1]);
		if(strcmp(&out[strlen(out)-4],".ild") != 0){
			printf("file is not ilda\n");
		} else {
			strcpy(&out[strlen(out)-4],".png");
			proc(argv[argc-1],out);
		}
		argc--;
	}
	return 0;
}

