#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include "boost/assert.hpp"
#include "boost/utility.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/foreach.hpp"

bool th125mission_convert_impl(unsigned char *data, unsigned char level, unsigned char number, unsigned char player, unsigned char line){
	BOOST_ASSERT(data);
	const unsigned char key[4] = {0, 1, 1};
	unsigned char a, b;
	a = 13 * player + 11 * number + 7 * level + 58;
	b = 23 * line;
	for(unsigned int i = 0; i < 64; i++){
		data[i] += a;
		a += b + 23 + i;
	}
	return true;
}

class Player : boost::noncopyable {
public:
	enum PLAYER_NAME{
		PLAYER_NAME_AYA = 0,
		PLAYER_NAME_HATATE,
	};
	Player(PLAYER_NAME player, unsigned char img_no, const char *clear_message) :
		player(player), img_no(img_no), clear_message(clear_message, &clear_message[strlen(clear_message) + 1])
	{
	}
	const char *Name(void) const {
		const char * const name_list[2] = {"aya", "hatate"};
		return name_list[static_cast<unsigned int>(player)];
	}
	unsigned char ImageNumber(void) const { // ImageNumero‚É‚·‚é‚©–À‚Á‚½‚¯‚ÇA‚ ‚Æ‚Åâ‘Î¬—‚·‚é‚Ì‚Å‚±‚Á‚¿‚Å
		return img_no;
	}
	const char *ClearMessage(void) const {
		BOOST_ASSERT(clear_message.size() > 0);
		return &clear_message.front();
	}
protected:
	const PLAYER_NAME player;
	const unsigned char img_no;
	const std::vector<char> clear_message;
};

std::ostream &operator<<(std::ostream &out, const Player &player) {
	char out_str_head[256];
	sprintf(out_str_head,
"      - name: %s\n"
"        img_no: %d\n"
"        clear_message: |-\n", player.Name(), player.ImageNumber());
	out << out_str_head;

	char message_lines[6][64] = {{""}, {""}, {""}, {""}, {""}, {""}};
	const char *start, *end;
	start = player.ClearMessage();
	for(unsigned int i = 0; i < 6 && *start != '\0'; i++) {
		end = ::strchr(start, '\n');
		if(end == NULL) {
			end = &start[strlen(start)];
		}
		strncpy(message_lines[i], start, end - start);
		start = end;
		if(*start == '\n') {
			start++;
		}
	}
	for(unsigned int i = 0; i < 6 && message_lines[i][0] != '\0'; i++) {
		char out_str[74];
		sprintf(out_str, "          %s\n", message_lines[i]);
		out << out_str;
	}
	return out;
}

class Stage : boost::noncopyable {
public:
	Stage(unsigned char number, unsigned char photo_count, unsigned int norma, unsigned int unknown) :
		number(number), photo_count(photo_count), norma(norma), unknown(unknown), player_list(2)
	{
	}
	boost::shared_ptr<Player> &operator [](Player::PLAYER_NAME index) {
		return player_list[static_cast<unsigned int>(index)];
	}
	const boost::shared_ptr<Player> &operator [](Player::PLAYER_NAME index) const {
		return player_list[static_cast<unsigned int>(index)];
	}
	unsigned char Number(void) const {
		return number;
	}
	unsigned char PhotoCount(void) const {
		return photo_count;
	}
	unsigned int Norma(void) const {
		return norma;
	}
	unsigned int Unknown(void) const {
		return unknown;
	}
protected:
	const unsigned char number;
	const unsigned char photo_count;
	const unsigned int norma;
	const unsigned int unknown;
	std::vector<boost::shared_ptr<Player> > player_list;
};

std::ostream &operator<<(std::ostream &out, const Stage &stage) {
	char out_str[256];
	sprintf(out_str,
"  - number: %d\n"
"    photo_count: %d\n"
"    norma: %d\n"
"    unknown: %d\n"
"    player:\n", stage.Number(), stage.PhotoCount(), stage.Norma(), stage.Unknown());
	out << out_str;
	out << *stage[Player::PLAYER_NAME_AYA];
	out << *stage[Player::PLAYER_NAME_HATATE];
	return out;
}

class StageList : boost::noncopyable {
public:
	StageList(unsigned int level) :
		level(level)
	{
		BOOST_ASSERT(level == 98 || level < 14);
	}
	void Resize(unsigned int size) {
		list.resize(size);
	}
	unsigned int Size(void) const {
		return list.size();
	}
	unsigned int Level(void) const {
		return level;
	}
	boost::shared_ptr<Stage> &operator [](unsigned int index) {
		return list[index];
	}
	const boost::shared_ptr<Stage> &operator [](unsigned int index) const {
		return list[index];
	}
protected:
	const unsigned char level;
	std::vector<boost::shared_ptr<Stage> > list;
};

std::ostream &operator<<(std::ostream &out, const StageList &stage_list) {
	char out_str[256];
	sprintf(out_str,
"- level: %d\n"
"  stage:\n", stage_list.Level());
	out << out_str;
	for(unsigned int i = 0; i < stage_list.Size(); i++) {
		if(stage_list[i]) {
			out << *stage_list[i];
		}
	}
	return out;
}

std::ostream &operator<<(std::ostream &out, const std::vector<boost::shared_ptr<StageList> > level_list) {
	BOOST_FOREACH(boost::shared_ptr<StageList> stage_list, level_list) {
		if(stage_list) {
			out << *stage_list;
		}
	}
	return out;
}

std::vector<boost::shared_ptr<StageList> > th125mission_convert(const unsigned char *data, unsigned int size){
	BOOST_ASSERT(data);
	std::vector<boost::shared_ptr<StageList> > result(15);
	const unsigned int msg_count = *reinterpret_cast<const unsigned int *>(&data[0]);
	for(unsigned int i = 0; i < msg_count; i++) {
		const unsigned int base_addr = *reinterpret_cast<const unsigned int *>(&data[4 + i * 4]);
		const unsigned char *base_ptr = &data[base_addr];
		const unsigned short level = *reinterpret_cast<const unsigned short *>(&base_ptr[0]);
		const unsigned int stage_index = (level == 98 ? 14 : level);
		if(!result[stage_index]) {
			result[stage_index].reset(new StageList(level));
		}
		const unsigned short number = *reinterpret_cast<const unsigned short *>(&base_ptr[2]);
		const unsigned char photo_count = base_ptr[7];
		const unsigned int norma = *reinterpret_cast<const unsigned int *>(&base_ptr[8]);
		const unsigned int unknown = *reinterpret_cast<const unsigned int *>(&base_ptr[12]);
		if(result[stage_index]->Size() <= number) {
			result[stage_index]->Resize(number + 1);
		}
		if(!(*result[stage_index])[number]) {
			(*result[stage_index])[number].reset(new Stage(number, photo_count, norma, unknown));
		}
		const unsigned short player_id = *reinterpret_cast<const unsigned short *>(&base_ptr[4]);
		const Player::PLAYER_NAME player = static_cast<Player::PLAYER_NAME>(player_id);
		const unsigned char img_no = base_ptr[6];
		char clear_message[64 * 6 + 1] = "";
		for(unsigned int line = 0; line < 3; line++) {
			if(base_ptr[16 + line * 8] != 0xFF) {
				const unsigned int pos = *reinterpret_cast<const unsigned int *>(&base_ptr[16 + line * 8]);
				const unsigned int space = *reinterpret_cast<const unsigned int *>(&base_ptr[16 + line * 8 + 4]);
				unsigned char ruby_data[64];
				memcpy(ruby_data, &base_ptr[40 + (line + 3) * 64], 64);
				th125mission_convert_impl(ruby_data, level, number, player, line + 3);
				sprintf(&clear_message[strlen(clear_message)], "|%d,%d,%s\n", pos, space, ruby_data);

			}
			unsigned char line_data[64];
			memcpy(line_data, &base_ptr[40 + line * 64], 64);
			th125mission_convert_impl(line_data, level, number, player, line);
			strcat(clear_message, reinterpret_cast<char *>(line_data));
			if(line < 2) {
				strcat(clear_message, "\n");
			}
		}
		(*(*result[stage_index])[number])[player].reset(new Player(player, img_no, clear_message));
	}
	return result;
}

int main(int argc, char **argv) {
	for(unsigned int i = 1; i < argc; i++) {
		FILE * const fp = fopen(argv[i], "rb");
		if(fp == NULL) {
			continue;
		}
		fseek(fp, 0, SEEK_END);
		const unsigned int size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		std::vector<unsigned char> data(size);
		fread(&data.front(), 1, size, fp);
		fclose(fp);
		std::vector<boost::shared_ptr<StageList> > level_list = th125mission_convert(&data.front(), size);
		std::cout << level_list;
	}
	return 0;
}
