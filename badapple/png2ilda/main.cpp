#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <numeric>

#include <libpng.h>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>

unsigned int g_points_max;
unsigned int g_distance_max;
unsigned int g_line_max;


#define ASSERT(a) if(!(a)){printf("ASSERT %s:%d\n%s\n",__FILE__ ,__LINE__,#a);getchar();}

class BinWriter {
protected:
	std::vector<unsigned char> data;
	unsigned int pos;
public:
	BinWriter(const unsigned int length) {
		data.resize(length);
		pos = 0;
	}
	void Put32(const int value) {
		data[pos + 0] = reinterpret_cast<const unsigned char*>(&value)[3];
		data[pos + 1] = reinterpret_cast<const unsigned char*>(&value)[2];
		data[pos + 2] = reinterpret_cast<const unsigned char*>(&value)[1];
		data[pos + 3] = reinterpret_cast<const unsigned char*>(&value)[0];
		pos += 4;
	}
	void Put32(const unsigned int value) {
		Put32(*reinterpret_cast<const int *>(&value));
	}
	void Put16(const short value) {
		data[pos + 0] = reinterpret_cast<const unsigned char*>(&value)[1];
		data[pos + 1] = reinterpret_cast<const unsigned char*>(&value)[0];
		pos += 2;
	}
	void Put16(const unsigned short value) {
		Put16(*reinterpret_cast<const short *>(&value));
	}
	void Put16(const int value) {
		Put16(*reinterpret_cast<const short *>(&value));
	}
	void Put16(const unsigned int value) {
		Put16(*reinterpret_cast<const short *>(&value));
	}
	void Put8(const char value) {
		data[pos] = value;
		pos++;
	}
	void Put8(const unsigned char value) {
		Put8(*reinterpret_cast<const char *>(&value));
	}
	void Put8(const int value) {
		Put8(*reinterpret_cast<const char *>(&value));
	}
	void Put8(const unsigned int value) {
		Put8(*reinterpret_cast<const char *>(&value));
	}
	void Put(const char value, const unsigned int length) {
		::memset(&data[pos], value, length);
		pos += length;
	}
	void Put(const char *value, const unsigned int length) {
		::memcpy(&data[pos], value, length);
		pos += length;
	}
	void Put(const unsigned char *value, const unsigned int length) {
		::memcpy(&data[pos], value, length);
		pos += length;
	}
	void Seek(const unsigned int pos) {
		this->pos = pos;
	}
	void FWrite(FILE * const fp) {
		::fwrite(&data[0], 1, data.size(), fp);
	}
};




const int zumi = 127;

struct line{
	int x1;
	int y1;
	int x2;
	int y2;
	struct line *next;
	struct line *prev;
};

class OutLiner {
public:
	unsigned int height;
	unsigned int width;
	unsigned char *data;
	unsigned int dir;
	unsigned int prev_dir;
	class Point {
	public:
		unsigned int x;
		unsigned int y;
		Point &operator=(const Point &other) {
			x = other.x;
			y = other.y;
			return *this;
		}
		bool operator==(const Point& other) const {
			return x == other.x && y == other.y;
		}
		bool operator!=(const Point& other) const {
			return !(*this == other);
		}
	};
	Point current;

	OutLiner(unsigned int _width, unsigned int _height, unsigned char *_data) {
		height = _height;
		width = _width;
		data = _data;
		current.x = 0;
		current.y = 0;
		dir = 0;
	}

	void seek(unsigned int x, unsigned int y) {
		ASSERT(x < 512);
		ASSERT(y < 384);
		current.x = x;
		current.y = y;
	}

	void setDirConfig(const int list[][2]) {
		memcpy(dirList, list, sizeof(dirList));
	}

	unsigned char *cur() const {
		return &data[(width * current.y + current.x) * 4];
	}

	unsigned char *adjacent(unsigned int d) const {
		return &data[(width * (current.y + dirList[d][0]) + current.x + dirList[d][1]) * 4];
	}
	unsigned char *adjacent() const {
		return adjacent(dir);
	}

	bool is_color(unsigned char color) const {
		return (*cur() == color);
	}

	bool equal_nextColor(unsigned int d) const {
		return (!is_wall(d) && is_color(*adjacent(d)));
	}

	bool equal_nextColor() const {
		return equal_nextColor(dir);
	}

	bool is_zumi() const {
		return (cur()[1] == zumi);
	}

	void set_zumi() {
		if(is_zumi()) {
			cur()[2]++;
			cur()[3] = cur()[3] * 8 + dir;
		} else {
			cur()[1] = zumi;
			cur()[2] = 0;
			cur()[3] = dir;
		}
	}

	bool is_wall(unsigned int d) const {
		return (current.y + dirList[d][0]) < 0 ||
			(current.y + dirList[d][0]) >= height ||
			(current.x + dirList[d][1]) < 0 ||
			(current.x + dirList[d][1]) >= width;
	}

	bool is_wall() const {
		return is_wall(dir);
	}


	bool is_line() const {
		for(unsigned int i = 0; i < 8; i++) {
			if(equal_nextColor(i) && adjacent(i)[1] != zumi) {
				return is_begin() || is_prev();
			}
		}
		return false;
	}

	bool is_begin() const {
		return current.x > 0 && current.y < height-1;
	}

	bool is_prev() const {
		return (!is_begin() || !line_end) && current.x < width-1 && current.y > 0;
	}

	void begin() {
		begin_impl(0);
	}

	void prev_begin() {
		int temp_prev = 8;
		for(unsigned int i = 0; i < 8; i++) {
			if(equal_nextColor((14 - i)%8)) {
				temp_prev = (10 - i) % 8;
				break;
			}
		}
		ASSERT(temp_prev != 8);
		begin_impl(3);
		prev_dir = temp_prev;
	}

	bool is_lineEnd() const {
		return (is_zumi() && (cur()[2] >= 3 || (cur()[3]&7) == dir || (cur()[3]/8&7) == dir));
	}

	unsigned int get_inDir(const unsigned int dir) const {
		if(prev_dir == 8) {
			return get_outDir(dir);
		}
		unsigned int result = dir;
		result = (result + 2 + (prev_dir < 8 ? prev_dir%2 : 1)) % 8;
		 for(int i = 0; i < 8; i++) {
			if(!equal_nextColor(result) && !is_wall(result)) {
				break;
			}
			result = (result + 7) % 8;
		}
		result = (result + 1) % 8;
		//ASSERT(is_wall(result) || equal_nextColor(result));
		if(!is_wall(result) && !equal_nextColor(result)) {
			return get_outDir(dir);
		}
		return result;
	}

	unsigned int get_outDir(const unsigned int dir) const {
		unsigned int result = dir;
		result = (result + 6) % 8;
		 for(int i = 0; i < 8; i++) {
			if(equal_nextColor(result) || is_wall(result)) {
				break;
			}
			result = (result + 1) % 8;
		}
		return result;
	}

	bool next() {
//printf("%d:%d\n",current.x,current.y);
		if(is_lineEnd()) {
			line_end = loop_point;
			set_zumi();
			return false;
		}
		set_zumi();

		unsigned int in_dir = get_inDir(dir);
		unsigned int out_dir = get_outDir(dir);

		if(is_wall(in_dir) && is_wall(out_dir)) {
			return false;
		}

		int temp_dir = dir;
		if(in_dir == out_dir) {
			dir = in_dir;
		} else if(is_wall(in_dir)) {
			dir = out_dir;
		} else if(is_wall(out_dir)){
			dir = in_dir;
		} else {
			while(out_dir != in_dir) {
//printf("%3d:%3d\tout:%d in:%d(%d)\n",current.x, current.y, out_dir,in_dir,dir);
				if(((out_dir - in_dir + 8) % 8) < 4) {
					in_dir = (in_dir + 1) % 8;
				} else {
					in_dir = (in_dir + 7) % 8;
				}
				if(!equal_nextColor(in_dir) && !is_wall(in_dir)) {
					memset(adjacent(in_dir), *cur(), 4);
				}
			}
			in_dir = get_inDir(dir);
			out_dir = get_outDir(dir);
//printf("%3d:%3d\tout:%d in:%d(%d)\n",current.x, current.y, out_dir,in_dir,dir);
			if(in_dir == out_dir){
				dir = out_dir;
			} else {
//printf("%3d:%3d\tback(%d)\n", current.x, current.y, dir);
				dir = (dir + 4) % 8;
				move();
				cur()[1] = cur()[2] = cur()[3] = *cur();
				if(prev_dir != 8) {
					dir = prev_dir;
				} else if(dirList[0][0] == +1){
					begin();
				} else {
					prev_begin();
				}
				prev_dir = 8;
				return true;
			}
		}
		prev_dir = temp_dir;
		move();
		return true;
	}
protected:
	unsigned int dirList[8][2];
	bool line_end;
	Point begin_point;
	bool loop_point;

	void move() {
		ASSERT(current.y != 0 || dirList[dir][0] >= 0);
		ASSERT(current.x != 0 || dirList[dir][1] >= 0);
		current.y += dirList[dir][0];
		current.x += dirList[dir][1];
		if(current == begin_point) {
			loop_point = true;
		}
		ASSERT(current.x < 512);
		ASSERT(current.y < 384);
	}

	void begin_impl(unsigned int start) {
		line_end = false;
		begin_point = current;
		loop_point = false;
		prev_dir = 8;
		for(unsigned int i = 0; i < 8; i++) {
			if(equal_nextColor((start + i)%8)) {
				dir = (start + i) % 8;
				ASSERT(!is_wall());
				move();
				return;
			}
		}
		ASSERT(is_line());
	}
};

bool OutLineStart(unsigned int height,unsigned int width,unsigned char *data, unsigned int h, unsigned int w,struct line **_lineData,struct line **_first,struct line **_last) {
	ASSERT(h < height);
	ASSERT(w < width);
	ASSERT(data != NULL);
	ASSERT(_lineData != NULL);
	ASSERT(_first != NULL);
	ASSERT(_last != NULL);
	const int dirList[2][8][2] = {
		{
			{+1,-1},	{+1, 0},	{+1,+1},
			{ 0,+1},	{-1,+1},
			{-1, 0},	{-1,-1},	{ 0,-1},
		},{
			{-1,-1},	{-1, 0},	{-1,+1},
			{ 0,+1},	{+1,+1},
			{+1, 0},	{+1,-1},	{ 0,-1},
		}
	};
	*_lineData = *_first = *_last = NULL;
//printf("OnLine\t%d\t%d\n",w,h);
	OutLiner img(width, height, data);
	img.setDirConfig(dirList[0]);
	img.seek(w, h);
//	ASSERT(!img.equal_nextColor(4));
//	ASSERT(!img.equal_nextColor(5));
//	ASSERT(!img.equal_nextColor(6));
//	ASSERT(!img.equal_nextColor(7));

	if(!img.is_line()) {
		img.set_zumi();
		return false;
	}

	struct line *lineData = (struct line*)malloc(sizeof(struct line)*4096);
	struct line *top = lineData;
	struct line *first = NULL;
	struct line *last = NULL;
	unsigned int prev_dir = 8;//絶対ありえない数値

	bool begin_call = false;
	if(img.is_begin()) {
		begin_call = true;
		img.begin();
		first = last = top;
		top++;
		last->prev = NULL;
		last->next = NULL;
		last->x1 = w;
		last->y1 = h;
		last->x2 = img.current.x;
		last->y2 = img.current.y;

		while(img.next()) {
//printf("begin %3d:%3d(%d)\n",img.current.x,img.current.y,img.dir);
			if(prev_dir != img.dir) {
				ASSERT(last->next == NULL);
				last->next = top;
				top++;
				last->next->prev = last;
				last->next->x1 = last->x2;
				last->next->y1 = last->y2;
				last = last->next;
				last->next = NULL;
			}
			last->x2 = img.current.x;
			last->y2 = img.current.y;
			prev_dir = img.dir;
			ASSERT(last->x1 < 512);
			ASSERT(last->y1 < 384);
			ASSERT(last->x2 < 512);
			ASSERT(last->y2 < 384);
		}
	}

	img.setDirConfig(dirList[1]);
	img.seek(w, h);
	if(img.is_prev()) {
		img.prev_begin();
		if(begin_call) {
			ASSERT(first->prev == NULL);
			first->prev = top;
			top++;
			first->prev->next = first;
			first = first->prev;
			first->prev = NULL;
		} else {
			first = last = top;
			top++;
			first->prev = NULL;
			first->next = NULL;
		}
		first->x1 = img.current.x;
		first->y1 = img.current.y;
		first->x2 = w;
		first->y2 = h;

		prev_dir = 8;//絶対にありえない数値
		while(img.next()) {
//printf("prev %3d:%3d(%d)\n",img.current.x,img.current.y,img.dir);
			if(prev_dir != img.dir) {
				ASSERT(first->prev == NULL);
				first->prev = top;
				top++;
				first->prev->next = first;
				first->prev->x2 = first->x1;
				first->prev->y2 = first->y1;
				first = first->prev;
				first->prev = NULL;
			}
			first->x1 = img.current.x;
			first->y1 = img.current.y;
			prev_dir = img.dir;
			ASSERT(first->x1 < 512);
			ASSERT(first->y1 < 384);
			ASSERT(first->x2 < 512);
			ASSERT(first->y2 < 384);
		}
	}
	ASSERT(first != NULL || last != NULL);

//getchar();
	*_lineData = lineData;
if(true || h == 67){
	*_first = first;
	*_last = last;
}
	return true;
}


#define LIGHT_ON	(true)
#define LIGHT_OFF	(false)

#define ILDA_X(x)	((x)*128-32768)
#define ILDA_Y(y)	((((y)+64)*128-32768)*-1)

class point {
public:
	int x;
	int y;
	bool light;
	unsigned char getColorID() const {
		const int result = (this->y + 32768 - 8192) / 8193 + 2;
		//const int result = (this->x + 32768) / 10923 + 2;
		ASSERT(result > 0 && result <= 7);
		switch(result){
		case 0:
			ASSERT(false);
			break;
		case 1:
			return 0x38;
		case 2:
			return 0x00;
		case 3:
			return 0x10;
		case 4:
			return 0x18;
		case 5:
			return 0x1F;
		case 6:
			return 0x28;
		case 7:
			return 0x30;
		default:
			ASSERT(false);
			break;
		}
		return result;
	}
};

bool SplitPoint(std::list<point> &points, unsigned int n) {
	std::list<point>::iterator p = points.begin();
	std::list<point>::iterator prev = p;
	++p;
	while(p != points.end()){
		const unsigned int x_dis = abs(prev->x - p->x);
		const unsigned int y_dis = abs(prev->y - p->y);
		if(x_dis > n || y_dis > n) {
			const unsigned int max = (x_dis > y_dis ? x_dis : y_dis);
			const double mag = 1.0 / ceil((double)max / n);
			point pos;
			pos.x = (int)(prev->x + (p->x - prev->x) * mag);
			pos.y = (int)(prev->y + (p->y - prev->y) * mag);
			pos.light = p->light;
			prev = p = points.insert(p, pos);
			++p;
		} else {
			prev = p;
			++p;
		}
	}
	return true;
}

bool DecPoint(std::list<point> &points, unsigned int n){
	n = n*128;
	std::list<point>::iterator p = points.begin();
	std::list<point>::iterator prev = p;
	++p;
	while(p != points.end()){
		if(prev->light == p->light && n >= abs(prev->x - p->x) + abs(prev->y - p->y)){
			prev->x = p->x;
			prev->y = p->y;
			prev = p = points.erase(p);
			--prev;
		} else {
			prev = p;
			++p;
		}
	}
	return true;
}

bool Line2Points(std::list<point> &points, int count, struct line **first) {
	ASSERT(count >= 0);
	ASSERT(first != NULL);
	unsigned int point_num = 2; // 始点 + 終点の分
	for(int i = 0; i < count; i++) {
		struct line *p = first[i];
		if(p != NULL) {
			point_num++;	// 線開始の分
		}
		while(p != NULL) {
			point_num++;
			p = p->next;
		}
	}

	points = std::list<point>(point_num);
	std::list<point>::iterator pos = points.begin();
	pos->x = ILDA_X(256);
	pos->y = ILDA_Y(192);
	pos->light = LIGHT_OFF;
	++pos;
	for(int i = 0; i < count; i++) {
		struct line *p = first[i];
		if(p != NULL) {
			ASSERT(p->x1 < 512);
			ASSERT(p->y1 < 384);
			pos->x = ILDA_X(p->x1);
			pos->y = ILDA_Y(p->y1);
			pos->light = LIGHT_OFF;
			++pos;
		}
		while(p != NULL) {
			ASSERT(p->x2 < 512);
			ASSERT(p->y2 < 384);
			pos->x = ILDA_X(p->x2);
			pos->y = ILDA_Y(p->y2);
			pos->light = LIGHT_ON;
			p = p->next;
			++pos;
		}
	}
	pos->x = ILDA_X(256);
	pos->y = ILDA_Y(192);
	pos->light = LIGHT_OFF;
	++pos;
	ASSERT(pos == points.end());
	return true;
}

bool WriteFrame(std::list<point> &points, const unsigned short frameNum, const unsigned short totalFrameNum, FILE *fp) {
	if(points.size() > g_points_max) {
		return false;
	}
	BinWriter head(32);
	head.Put("ILDA", 4);
	head.Put32(1);
	head.Put(' ',  16); // => ' ' * 8 * 2
	head.Put16(points.size());
	head.Put16(frameNum);
	head.Put16(totalFrameNum);
	head.Put8(0);
	head.Put8(0);
	head.FWrite(fp);

	std::list<point>::iterator p = points.begin();
	std::list<point>::iterator prev = p;
	while(p != points.end()){
		BinWriter data(6);
		data.Put16(p->x);
		data.Put16(p->y);
		data.Put8(p->light ? 0 : 0x40);
		data.Put8(p->getColorID());
		data.FWrite(fp);
		if(abs(p->x - prev->x) > g_distance_max) {
			printf("\n%d\n",abs(p->x - prev->x));
			printf("%d:%d => %d:%d\n",prev->x,prev->y,p->x,p->y);
			ASSERT(abs(p->x - prev->x) <= g_distance_max);
		}
		if(abs(p->y - prev->y) > g_distance_max) {
			printf("\n%d\n",abs(p->y - prev->y));
			printf("%d:%d => %d:%d\n",prev->x,prev->y,p->x,p->y);
			ASSERT(abs(p->y - prev->y) <= g_distance_max);
		}
		prev = p;
		++p;
	}
	return true;
}

bool SortLines(unsigned int count, struct line **first, struct line **last) {
	point cur;
	cur.x = 256;
	cur.y = 192;
	for(unsigned int i = 0; i < count; i++) {
		if(first[i] == NULL) {
			for(unsigned int l = i+1; l < count; l++) {
				if(first[l] != NULL) {
					first[i] = first[l];
					last[i] = last[l];
					first[l] = last[l] = NULL;
					break;
				}
			}
			continue;
		}
		unsigned int min = 0xFFFFFFFF;
		unsigned int index = i;
		for(unsigned int l = i; l < count; l++) {
			if(first[l] == NULL) {
				continue;
			}
			unsigned int dis = (unsigned int)sqrt((double)((first[l]->x1 - cur.x) * (first[l]->x1 - cur.x) + (first[l]->y1 - cur.y) * (first[l]->y1 - cur.y)));
			if(dis < min) {
				min = dis;
				index = l;
			}
		}
		if(index != i) {
			struct line *t;
			t = first[i];
			first[i] = first[index];
			first[index] = t;
			t = last[i];
			last[i] = last[index];
			last[index] = t;
		}
		cur.x = last[i]->x2;
		cur.y = last[i]->y2;
	}
	return true;
}

bool DecLine(unsigned int count, struct line **first, struct line **last) {
	unsigned int min = 0xFFFFFFFF;
	unsigned int index = 0;
	for(unsigned int i = 0; i < count; i++) {
		struct line *p = first[i];
		if(p != NULL) {
			unsigned int pointNum = 0;
			while(p != NULL) {
				unsigned int dis = (unsigned int)sqrt((double)((p->x1 - p->x2) * (p->x1 - p->x2) + (p->y1 - p->y2) * (p->y1 - p->y2)));
				pointNum += (int)ceil((double)dis / g_distance_max);
				p = p->next;
			}
			if(pointNum < min) {
				min = pointNum;
				index = i;
			}
		}
	}
	if(first[index] == NULL){
		return false;
	}
	first[index] = last[index] = NULL;
	return true;
}

bool isIgnoreColor(const unsigned char index) {
	const static unsigned char colorList[64][3] = {
		{255,	0,	0},
		{255,	16,	0},
		{255,	32,	0},
		{255,	48,	0},
		{255,	64,	0},
		{255,	80,	0},
		{255,	96,	0},
		{255,	112,	0},
		{255,	128,	0},
		{255,	144,	0},
		{255,	160,	0},
		{255,	176,	0},
		{255,	192,	0},
		{255,	208,	0},
		{255,	224,	0},
		{255,	240,	0},
		{255,	255,	0},
		{224,	255,	0},
		{192,	255,	0},
		{160,	255,	0},
		{128,	255,	0},
		{96,	255,	0},
		{64,	255,	0},
		{32,	255,	0},
		{0,	255,	0},
		{0,	255,	32},
		{0,	255,	64},
		{0,	255,	96},
		{0,	255,	128},
		{0,	255,	160},
		{0,	255,	192},
		{0,	255,	224},
		{0,	130,	255},
		{0,	114,	255},
		{0,	104,	255},
		{10,	96,	255},
		{0,	82,	255},
		{0,	74,	255},
		{0,	64,	255},
		{0,	32,	255},
		{0,	0,	255},
		{32,	0,	255},
		{64,	0,	255},
		{96,	0,	255},
		{128,	0,	255},
		{160,	0,	255},
		{192,	0,	255},
		{224,	0,	255},
		{255,	0,	255},
		{255,	32,	255},
		{255,	64,	255},
		{255,	96,	255},
		{255,	128,	255},
		{255,	160,	255},
		{255,	192,	255},
		{255,	224,	255},
		{255,	255,	255},
		{255,	224,	224},
		{255,	255,	255},
		{255,	160,	160},
		{255,	128,	128},
		{255,	96,	96},
		{255,	64,	64},
		{255,	32,	32}
	};
	ASSERT(index < 64);
	const unsigned char * const first = &colorList[index][0];
	const unsigned char * const end = &colorList[index][3];
	if(std::accumulate(first, end, 0) < 255) {
		return true;
	}
	return false;
}

void Line2Ilda(FILE *fp, int count, struct line **first, struct line **last) {
	static int frameNum = 0;
	const int totalFrameNum = 6572;

	std::list<point> points;
	do {
		ASSERT(SortLines(count, first, last));

		ASSERT(Line2Points(points, count, first));
//printf("points.size() == %d\n",points.size());

		for(unsigned int n = 0; n == 0 || (points.size() > g_points_max && n < g_distance_max / 128); n++){
			ASSERT(DecPoint(points, n));
			ASSERT(SplitPoint(points, g_distance_max));
		}
	} while(!WriteFrame(points, frameNum, totalFrameNum, fp) && DecLine(count, first, last));
	ASSERT(points.size() <= g_points_max);
	frameNum++;

	if(frameNum+2 == totalFrameNum){
		BinWriter head(38);
		head.Put("ILDA", 4);
		head.Put32(1);
		head.Put(' ', 16);
		head.Put16(1);
		head.Put16(frameNum);
		frameNum++;
		head.Put16(totalFrameNum);
		head.Put8(0);
		head.Put8(0);
		head.Put16(0);
		head.Put16(0);
		head.Put8(0xC0);
		head.Put8(0);
		head.FWrite(fp);

		head.Seek(24);
		head.Put16(0);
		head.Put16(frameNum);
		head.FWrite(fp);
	}
}

void OutLine(FILE *fp, unsigned int height,unsigned int width, unsigned char *data) {
#define p(h,w)	(&data[((h) * width + (w)) * 4 + 1])
	unsigned int color;
	unsigned int count = 0;
	struct line **lineData = static_cast<struct line **>(malloc(sizeof(struct line *) * g_line_max));
	struct line **first = static_cast<struct line **>(malloc(sizeof(struct line *) * g_line_max));
	struct line **last = static_cast<struct line **>(malloc(sizeof(struct line *) * g_line_max));
	unsigned int lineCount = 0;

	unsigned int h = 0;
	while(h < height){
		unsigned int w;
		if(h == 0){
			color = *p(h,0);
			w = 1;
		} else {
			w = 0;
			color = *p(h-1,w);
			if(color == zumi) {
				color = p(h,w)[-1];
			}
		}
		ASSERT(color != zumi);
		while(w < width){
			if(*p(h,w) != color){
				if(*p(h,w) == zumi){
					if(w+1 < width && *p(h,w+1) != zumi) {
						color = *p(h,w+1);
					}
				} else {
					ASSERT(lineCount < g_line_max);
					if(OutLineStart(height,width,data,h,w,&lineData[lineCount],&first[lineCount],&last[lineCount])){
						lineCount++;
						if(w+1 < width && *p(h,w+1) != zumi) {
							color = *p(h,w+1);
						}
					} else if(w+1 < width && *p(h,w+1) != zumi) {
						color = *p(h,w+1);
					}
				}
			}
			w++;
		}
		h++;
	}
	Line2Ilda(fp, lineCount, first, last);
	while(lineCount){
		lineCount--;
		free(lineData[lineCount]);
	}

#undef p
}

int procImage(FILE *ilda, const char *dir_name, const char *fileName) {
	char inName[256];
	sprintf(inName,"%s/%s", dir_name, fileName);
	FILE *fp = fopen(inName, "rb");
	if(fp == NULL){
		return 1;
	}
	png_structp png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, 0,0,0);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	png_infop end_info = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, fp);
	png_read_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	png_byte** row_pointers;
	row_pointers = png_get_rows(png_ptr, info_ptr);

	const unsigned int height = info_ptr->height;
	const unsigned int width = info_ptr->width;
	unsigned int size = 4 * width * height + width * 2;
	unsigned char *data = (unsigned char*)malloc(size);
	memset(data,0x80,size);

	data += width;
	{
		unsigned int h = 0;
		while(h < height){
			if(info_ptr->pixel_depth == 32){
				memcpy(&data[h * width * 4], row_pointers[h], width * 4);
			} else {
				unsigned int w = 0;
				while(w < width){
					if(info_ptr->pixel_depth == 24){
						memcpy(&data[(h * width + w) * 4],&row_pointers[h][w*3],3);
					} else if(info_ptr->pixel_depth == 8) {
						memset(&data[(h * width + w) * 4],row_pointers[h][w],3);
					} else if(info_ptr->pixel_depth == 1) {
						memset(&data[(h * width + w) * 4],(row_pointers[h][w/8]&(0x80>>(w%8))) ? 0xFF : 0,3);
					}
					data[(h * width + w) * 4 + 3] = 0xFF;
					w++;
				}
			}
			h++;
		}
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	fclose(fp);

	OutLine(ilda, height, width, data);
	free(data - width);


	return 0;
}

int main(int argc, char *argv[]){
	boost::program_options::variables_map opt_list;
	try {
		using boost::program_options::value;
		boost::program_options::options_description opt("オプション");
		opt.add_options()
			("help,h",										"ヘルプを表示")
			("out,o",		value<std::string>()->default_value("badapple.ild"),	"出力ファイル")
			("point,p",		value<unsigned int>()->default_value(1500),		"最大ポイント数")
			("distance,d",	value<unsigned int>()->default_value(1152),		"最大ライン長")
			("line,l",		value<unsigned int>()->default_value(500),		"最大ライン数");
		boost::program_options::options_description opt_hidden;
		opt.add_options()
			("input-dir",	value<std::string>(),						"対象ディレクトリ");
		boost::program_options::options_description opt_all;
		opt_all.add(opt).add(opt_hidden);

		boost::program_options::positional_options_description pod;
		pod.add("input-dir", 1);

		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(opt_all).positional(pod).run(), opt_list);
		boost::program_options::notify(opt_list);

		if(opt_list.count("help") || opt_list.count("input-dir") == 0) {
			std::cout << opt << std::endl;
			return 1;
		}
	} catch(std::exception& ex) {
		printf("コマンドライン引数の指定に誤りがあります: %d\n", ex.what());
		return 1;
	}

	g_points_max = opt_list["point"].as<unsigned int>();
	g_distance_max = opt_list["distance"].as<unsigned int>();
	g_line_max = opt_list["line"].as<unsigned int>();

	const std::string ilda_filename = opt_list["out"].as<std::string>();
	const std::string dir_name = opt_list["input-dir"].as<std::string>();
	ASSERT(!ilda_filename.empty());
	FILE *fp = fopen(ilda_filename.c_str(),"wb");

	int i = 1;
	char fileName[256];
	time_t s,e,d;
	time(&s);
	do{
		if(true || i%100 == 0){
			time(&e);
			d = e - s;
			printf("\b\b\b\b\b\b\b\b\b\b\b\b %4d %02d:%02d", i, (int)d/60, (int)d%60);
		}
		sprintf(fileName,"%d.png",i);
		i++;
	}while(procImage(fp, dir_name.c_str(), fileName) == 0);
	fclose(fp);
	return 0;
}

