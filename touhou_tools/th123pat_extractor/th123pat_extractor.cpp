
//�O�����C�u�����[�̌x���͖��������Ă���
#pragma warning(push)
#pragma warning(disable: 4061) // case��`����Ă��Ȃ��񋓎q������܂�
#pragma warning(disable: 4100) // �錾���ꂽ�������g�p���Ă��܂���
#pragma warning(disable: 4127) // ���������萔�ł�
#pragma warning(disable: 4180) // �֐��|�C���^�[�ɑ΂��Ė����ȑ����q���g�p���Ă��܂�
#pragma warning(disable: 4189) // �ϐ��ɑ����Q�Ƃ���Ă��܂���
#pragma warning(disable: 4191) // �֐��|�C���^�[�̃L���X�g�͊댯�ł�
#pragma warning(disable: 4201) // ��W���̊g���@�\�A�����̍\����/���p�̂��g�p���Ă��܂�
#pragma warning(disable: 4242) // �T�C�Y�������Ȍ^�ւƌ^�ϊ����s���܂���
#pragma warning(disable: 4244) // ���������_�^���琮���^�֌^�ϊ����s���܂���
#pragma warning(disable: 4245) // const�t���ϐ�����signed/unsigned���Ⴄ�^�ϊ����s���܂���
#pragma warning(disable: 4263) // ���z�֐����I�[�o�[���C�h���Ă��܂����A�����̌^���Ⴂ�܂�
#pragma warning(disable: 4264) // �����̌^�݈̂Ⴄ�����֐�����`���ꂽ���߁A���z�֐��ɂ̓A�N�Z�X�ł��Ȃ��Ȃ�܂�
#pragma warning(disable: 4265) // ���z�֐����܂ރN���X�̃f�X�g���N�^���񉼑z�֐��ł�
#pragma warning(disable: 4266) // ���z�֐��̂��ׂẴI�[�o�[���[�h���I�[�o�[���C�h���Ă��܂���
#pragma warning(disable: 4302) // �L���X�g��̌^�̕������������ߏ�񂪌����Ă���\��������܂�
#pragma warning(disable: 4350) // const�Ȃ��̎Q�Ɠn���̓��e�������󂯎�邱�Ƃ��ł��Ȃ����߁A�ʂ̃R���X�g���N�^���Ă΂�܂�
#pragma warning(disable: 4365) // signed/unsigned���Ⴄ�^�ϊ����s���܂���
#pragma warning(disable: 4505) // �Q�Ƃ���Ă��Ȃ����[�J���֐��͍폜����܂���
#pragma warning(disable: 4512) // ������Z�q�𐶐����邽�߂ɕK�v�ȃR���X�g���N�^���A�N�Z�X�ł��܂���A������Z�q�����܂���ł���
#pragma warning(disable: 4548) // �����ȃJ���}��
#pragma warning(disable: 4555) // ����p�̂Ȃ���
#pragma warning(disable: 4571) // catch(...)�ō\������O(SEH)�̓L���b�`����܂���
#pragma warning(disable: 4611) // ���ɂ�蓮�삪�قȂ�\���̂���@�\���g�p����Ă��܂�
#pragma warning(disable: 4619) // �����������݂��x���ԍ��͑��݂��܂���
#pragma warning(disable: 4625) // ��{�N���X�̃R�s�[�R���X�g���N�^���A�N�Z�X�s�\�Ȃ̂ŃR�s�[�R���X�g���N�^�����܂���ł���
#pragma warning(disable: 4626) // ��{�N���X�̑�����Z�q���A�N�Z�X�s�\�Ȃ̂ő�����Z�q�����܂���ł���
#pragma warning(disable: 4640) // static�ȃ��[�J���ϐ��̏��������X���b�h�Z�[�t�ł͂���܂���
#pragma warning(disable: 4668) // ��`����Ă��Ȃ��V���{����#if/#elif�Ŏg�p����܂���
#pragma warning(disable: 4738) // ���������_�̌v�Z���ʂ�32bit�Ɋi�[���Ă��邽�߃p�t�H�[�}���X���ቺ���Ă��܂�
#pragma warning(disable: 4820) // �\���̂̃p�b�e�B���O���������܂���
#pragma warning(disable: 4836) // ���[�J���^���e���v���[�g�����Ƃ��Ďg�p���Ă��܂��A����͔�W���̊g���@�\�Ȃ̂Ő�������܂���
#pragma warning(disable: 4928) // �����̃��[�U�[��`�̈Öق̌^�ϊ����s���Ă��܂�
#pragma warning(disable: 4986) // �ڍוs��
#pragma warning(disable: 4987) // ��W���̊g���@�\���g�p����Ă��܂�
#pragma warning(disable: 4996) // �Z�L�����e�B�[�z�[���ɂȂ肤��W��C�֐����g�p����Ă��܂�

// RELEASE�r���h���̂ݔ�������x���̖�����
#ifndef _DEBUG
#pragma warning(disable: 4710) // __inline�錾����Ă���֐�/���\�b�h��inline�W�J���܂���ł���
#pragma warning(disable: 4711) // __inline�錾����Ă��Ȃ��֐�/���\�b�h��inline�W�J���܂���
#pragma warning(disable: 4714) // __forceinline�錾����Ă���֐�/���\�b�h��inline�W�J���܂���ł���
#endif

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <vector>
#include <string>

#include <windows.h>

#include <boost/version.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#pragma warning(pop)
//�O�����C�u�����[�̌x�����������܂�

#pragma warning(disable: 4127) // ���������萔�ł�
#pragma warning(disable: 4191) // �֐��|�C���^�[�̃L���X�g�͊댯�ł�
#pragma warning(disable: 4350) // const�Ȃ��̎Q�Ɠn���̓��e�������󂯎�邱�Ƃ��ł��Ȃ����߁A�ʂ̃R���X�g���N�^���Ă΂�܂�
#pragma warning(disable: 4503) // ��������4096�����𒴂������ߐ؂�̂Ă��܂�
#pragma warning(disable: 4505) // �Q�Ƃ���Ă��Ȃ����[�J���֐��͍폜����܂���
#pragma warning(disable: 4514) // �g�p����Ă��Ȃ��֐�/���\�b�h���폜����܂���
#pragma warning(disable: 4640) // static�ȃ��[�J���ϐ��̏��������X���b�h�Z�[�t�ł͂���܂���
#pragma warning(disable: 4710) // �C�����C���֐��Ƃ��đI������܂������C�����C���W�J�ł��܂���ł���
#pragma warning(disable: 4836) // ���[�J���^���e���v���[�g�����Ƃ��Ďg�p���Ă��܂��A����͔�W���̊g���@�\�Ȃ̂Ő�������܂���

// RELEASE�r���h���̂ݔ�������x���̖�����
#ifndef _DEBUG
#pragma warning(disable: 4100) // �錾���ꂽ�������g�p���Ă��܂���
#pragma warning(disable: 4189) // �ϐ��ɑ����Q�Ƃ���Ă��܂���
#pragma warning(disable: 4710) // inline�錾����Ă���֐�/���\�b�h��inline�W�J���܂���ł���
#pragma warning(disable: 4711) // inline�錾����Ă��Ȃ��֐�/���\�b�h��inline�W�J���܂���
#pragma warning(disable: 4714) // __forceinline�錾����Ă���֐�/���\�b�h��inline�W�J���܂���ł���
#endif

#include <org/click3/utility.h>

#define IMG_NAME_LENGTH	(128)
#define MOTION_HEADER_SIZE	(13)

typedef char FileName[128];

struct Box {
	int left;
	int top;
	int right;
	int bottom;
};
#pragma pack(push, 1)
struct AttackAreaBox {
	int left;
	int top;
	int right;
	int bottom;
	char unknown;
};
#pragma pack(pop)

class Frame {
public:
	bool Load(const char *&in, unsigned int in_size, unsigned int version) {
		if(in_size < 19) {
			return false;
		}
		const char *p = in;
		::memcpy(&img_id, p, 19);
		p += 19;
/*
		img_id = *reinterpret_cast<const unsigned int *>(p);
		p += 4;
		unknown = *reinterpret_cast<const int*>(p);
		p += 4;
		width = *reinterpret_cast<const short*>(p);
		p += 2;
		height = *reinterpret_cast<const short*>(p);
		p += 2;
		x = *reinterpret_cast<const short*>(p);
		p += 2;
		y = *reinterpret_cast<const short*>(p);
		p += 2;
		frame = *reinterpret_cast<const short*>(p);
		p += 2;
		opt = *p;
		p++;
*/
		if(opt == 2) {
			const unsigned int opt_size = (version == 3 ? sizeof(opt_union.color_effect_old) : sizeof(opt_union.color_effect));
			if(in_size - (p - in) < opt_size) {
				return false;
			}
			memcpy(&opt_union, p, opt_size);
			p += opt_size;
		} else if(opt != 0 && opt != 1) {
			return false;
		}
		if(in_size - (p - in) < 49) {
			return false;
		}
		::memcpy(&damage, p, 49);
		p += 49;
/*
		damage = *reinterpret_cast<const short*>(p);
		p += 2;
		rate = *reinterpret_cast<const short*>(p);
		p += 2;
		cut_dam = *reinterpret_cast<const short*>(p);
		p += 2;
		rei_cut = *reinterpret_cast<const short*>(p);
		p += 2;
		ukemi = *reinterpret_cast<const short*>(p);
		p += 2;
		hit = *reinterpret_cast<const short*>(p);
		p += 2;
		limit = *reinterpret_cast<const short*>(p);
		p += 2;
		hitstop = *reinterpret_cast<const short*>(p);
		p += 2;
		enemy_hitstop = *reinterpret_cast<const short*>(p);
		p += 2;
		guardstop = *reinterpret_cast<const short*>(p);
		p += 2;
		enemy_guardstop = *reinterpret_cast<const short*>(p);
		p += 2;
		card = *reinterpret_cast<const short*>(p);
		p += 2;
		guard_card = *reinterpret_cast<const short*>(p);
		p += 2;
		airhit_motion = *reinterpret_cast<const short*>(p);
		p += 2;
		hit_motion = *reinterpret_cast<const short*>(p);
		p += 2;
		x_blowing = *reinterpret_cast<const short*>(p);
		p += 2;
		y_blowing = *reinterpret_cast<const short*>(p);
		p += 2;
		hit_se = *reinterpret_cast<const short*>(p);
		p += 2;
		hit_effect = *reinterpret_cast<const short*>(p);
		p += 2;
		flag = *reinterpret_cast<const short*>(p);
		p += 2;
		on = *p;
		p++;
		fflags = *reinterpret_cast<const unsigned int*>(p);
		p += 4;
		aflags = *reinterpret_cast<const unsigned int*>(p);
		p += 4;
*/

		if(!LoadBox(contact, p, in_size - (p - in))) {
			return false;
		}
		if(!LoadBox(hitarea, p, in_size - (p - in))) {
			return false;
		}
		if(!LoadBox(attackarea, p, in_size - (p - in))) {
			return false;
		}

		if(version == 5) {
			if(in_size - (p - in) < sizeof(fumei2)) {
				return false;
			}
			::memcpy(fumei2, p, sizeof(fumei2));
			p += sizeof(fumei2);
		}

		in = p;
		return true;
	}
	unsigned int img_id;
	int unknown;
	short width;
	short height;
	short x;
	short y;
	short frame;
	char opt; // 1�s���A2:color_effect
	char padding1[1]; // unused
	union {
		struct {
			short mode;
			unsigned char alpha;
			unsigned char red;
			unsigned char green;
			unsigned char blue;
			short scale;
			short y_angle;
			short z_angle;
			short x_angle;
		} color_effect_old;
		struct {
			short mode;
			unsigned char alpha;
			unsigned char red;
			unsigned char green;
			unsigned char blue;
			short scale;
			short y_angle;
			short z_angle;
			short x_angle;
			short fumei;
		} color_effect;
	} opt_union;
	short damage;
	short rate;
	short cut_dam;
	short rei_cut;
	short ukemi;
	short hit;//0x64�Ńq�b�g���ɔ�e���[�V���������A0�Ŏ��Ȃ�
	short limit;
	short hitstop;
	short enemy_hitstop;
	short guardstop;
	short enemy_guardstop;
	short card;
	short guard_card;
	short airhit_motion;
	short hit_motion;
	short x_blowing;//����̔�e���[�V�����ɉ��ړ�����������ꍇ�̑��x
	short y_blowing;
	short hit_se;//�q�b�g���ɍĐ�������ʉ���ID
	short hit_effect;//�q�b�g���̃G�t�F�N�g
	short flag;
	char on;//1�̎��A���̃��[�V�����͎��s����Ȃ�
	char padding2[3]; // unused
	unsigned int fflags;
	unsigned int aflags;
	std::vector<Box> contact;
	std::vector<Box> hitarea;
	std::vector<AttackAreaBox> attackarea;
	char fumei2[30];
	char padding3[2]; // unused
protected:
	template<class T>
	static bool LoadBox(std::vector<T> &box_list, const char *&in, unsigned int in_size) {
		if(in_size < 1) {
			return false;
		}
		box_list.clear();
		const char *p = in;
		const unsigned char box_count = *reinterpret_cast<const unsigned char *>(p);
		p++;
		if(box_count == 0) {
			in = p;
			return true;
		}
		const unsigned int all_box_size = box_count * sizeof(T);
		if(in_size < 1 + all_box_size) {
			return false;
		}
		box_list.resize(box_count);
		::memcpy(&box_list.front(), p, all_box_size);
		p += all_box_size;
		BOOST_FOREACH(const T &box, box_list) {
			// TODO left > right��top > bottom�̎��́H
			if(box.left == box.right || box.top == box.bottom) {
				//return false;
			}
		}
		in = p;
		return true;
	}
};

class Motion{
public:
	bool ToPTree(boost::property_tree::ptree &out, const std::vector<std::string> &img_name_list, unsigned int version) const {
		if(id == -1){
			out.add("reference_id", ref_id);
			return true;
		}
		out.add("fumei", *reinterpret_cast<const int *>(fumei));
		out.add("loop_flag", is_loop ? 1 : 0);
		out.add("frame_num", frame_list.size());

		unsigned int frame_id = 0;
		BOOST_FOREACH(const boost::shared_ptr<Frame> frame, frame_list) {
			boost::property_tree::ptree ptree;
			ptree.add("img_id",	img_name_list[frame->img_id]);
			ptree.add("unknown",	frame->unknown);
			ptree.add("width",	frame->width);
			ptree.add("height",	frame->height);
			ptree.add("x",	frame->x);
			ptree.add("y",	frame->y);
			ptree.add("frame",	frame->frame);
			if(frame->opt==2){
				boost::property_tree::ptree effect;
				if(version == 3) {
					effect.add("mode",	frame->opt_union.color_effect_old.mode);
					effect.add("alpha",	frame->opt_union.color_effect_old.alpha);
					effect.add("red",	frame->opt_union.color_effect_old.red);
					effect.add("green",	frame->opt_union.color_effect_old.green);
					effect.add("blue",	frame->opt_union.color_effect_old.blue);
					effect.add("scale",	frame->opt_union.color_effect_old.scale);
					effect.add("y_angle",frame->opt_union.color_effect_old.y_angle);
					effect.add("z_angle",frame->opt_union.color_effect_old.z_angle);
					effect.add("x_angle",frame->opt_union.color_effect_old.x_angle);
				} else if(version == 5) {
					effect.add("mode",	frame->opt_union.color_effect.mode);
					effect.add("alpha",	frame->opt_union.color_effect.alpha);
					effect.add("red",	frame->opt_union.color_effect.red);
					effect.add("green",	frame->opt_union.color_effect.green);
					effect.add("blue",	frame->opt_union.color_effect.blue);
					effect.add("scale",	frame->opt_union.color_effect.scale);
					effect.add("y_angle",frame->opt_union.color_effect.y_angle);
					effect.add("z_angle",frame->opt_union.color_effect.z_angle);
					effect.add("x_angle",frame->opt_union.color_effect.x_angle);
					effect.add("fumei",	frame->opt_union.color_effect.fumei);
				}
				ptree.add_child("effect", effect);
			}
			if(frame->attackarea.size() > 0) {
				ptree.add("damage",		frame->damage);
				ptree.add("rate",		frame->rate);
				ptree.add("guard_damage",	frame->cut_dam);
				ptree.add("numen_damage",	frame->rei_cut);
				ptree.add("passive_time",	frame->ukemi);
				ptree.add("unknown2",	frame->hit);
				ptree.add("limit",		frame->limit);
				ptree.add("hitstop",		frame->hitstop);
				ptree.add("enemy_hitstop",	frame->enemy_hitstop);
				ptree.add("guardstop",	frame->guardstop);
				ptree.add("enemy_guardstop",frame->enemy_guardstop);
				ptree.add("cardup",		frame->card);
				ptree.add("guard_cardup",	frame->guard_card);
				ptree.add("airhit_motion",	frame->airhit_motion);
				ptree.add("hito_motion",	frame->hit_motion);
				ptree.add("y_hitspeed",	frame->y_blowing);
				ptree.add("x_hitspeed",	frame->x_blowing);
				ptree.add("sound",		frame->hit_se);
				ptree.add("hit_effect",	frame->hit_effect);
				ptree.add("flag",		frame->flag);
				ptree.add("unknown3",	frame->on);
			}
			ptree.add("fflags",	frame->fflags);
			ptree.add("aflags",	frame->aflags);
			if(frame->contact.size() > 0){
				if(!AddBoxPTree(ptree, "contact", frame->contact)) {
					return false;
				}
			}
			if(frame->hitarea.size() > 0){
				if(!AddBoxPTree(ptree, "hitarea", frame->hitarea)) {
					return false;
				}
			}
			if(frame->attackarea.size() > 0){
				if(!AddBoxPTree(ptree, "attackarea", frame->attackarea)) {
					return false;
				}
			}
			for(unsigned int i = 0; i < sizeof(frame->fumei2); i++){
				if(frame->fumei2[i] > 0){
					char temp[10];
					SPRINTF(temp, "unknown%u", 4 + i);
					ptree.add(temp, frame->fumei2[i]);
				}
			}
			char key[12];
			SPRINTF(key, "frame[%d]", frame_id);
			out.add_child(key, ptree);
			frame_id++;
		}
		return true;
	}

	bool Load(const char *&in, unsigned int in_size, unsigned int version = 5) {
		if(in_size < 12) {
			return false;
		}
		const char *p = in;
		id = *reinterpret_cast<const int*>(p);
		p += 4;
		if(id == -1) {
			id2 = *reinterpret_cast<const unsigned int *>(p);
			p += 4;
			ref_id = *reinterpret_cast<const unsigned int *>(p);
			p += 4;
			in = p;
			return true;
		}
		if(in_size < MOTION_HEADER_SIZE) {
			return false;
		}
		::memcpy(fumei, p, sizeof(fumei));
		p += sizeof(fumei);
		const char loop_flag = *p;
		p++;
		BOOST_ASSERT(0 <= loop_flag && loop_flag <= 1);
		is_loop = (loop_flag == 1);
		const unsigned int frame_count = *reinterpret_cast<const unsigned int *>(p);
		p += 4;
		frame_list.resize(frame_count);
		BOOST_FOREACH(boost::shared_ptr<Frame> &frame, frame_list) {
			frame.reset(new Frame());
			if(!frame->Load(p, in_size - (p - in), version)) {
				return false;
			}
		}
		in = p;
		return true;
	}

	int id;
	unsigned int id2;	// id==-1�̎���id���i�[�����
	unsigned int ref_id;	// id==-1�̎��A�Q�Ɛ��ID���i�[�����B
	char fumei[4];
	bool is_loop;		// true�̎��͂��̃��[�V���������[�v����
	char padding[3];	// unused
	std::vector<boost::shared_ptr<Frame> > frame_list;
protected:
	static bool AddBoxPTree(boost::property_tree::ptree &out, const char *key, const std::vector<Box> &box_list) {
		boost::property_tree::ptree list;
		BOOST_FOREACH(const Box &box, box_list) {
			boost::property_tree::ptree box_ptree;
			box_ptree.add("left",	box.left);
			box_ptree.add("top",		box.top);
			box_ptree.add("right",	box.right);
			box_ptree.add("bottom",	box.bottom);
			list.insert(list.end(), std::make_pair("", box_ptree));
		}
		out.add_child(key, list);
		return true;
	}
	static bool AddBoxPTree(boost::property_tree::ptree &out, const char *key, const std::vector<AttackAreaBox> &box_list) {
		boost::property_tree::ptree list;
		BOOST_FOREACH(const AttackAreaBox &box, box_list) {
			boost::property_tree::ptree box_ptree;
			box_ptree.add("left",	box.left);
			box_ptree.add("top",		box.top);
			box_ptree.add("right",	box.right);
			box_ptree.add("bottom",	box.bottom);
			box_ptree.add("unknown",	box.unknown);
			list.insert(list.end(), std::make_pair("", box_ptree));
		}
		out.add_child(key, list);
		return true;
	}
};

class Pat {
public:
	bool Normalization(void) {
		struct iamge_use_info {
			unsigned int use_count;
			unsigned int new_id;
		};
		std::vector<iamge_use_info> img_info_list;
		std::vector<char> motion_count;

		img_info_list.resize(img_name_list.size());
		motion_count.resize(2048);

		BOOST_FOREACH(boost::shared_ptr<Motion> motion, motion_list) {
			if(motion->id < -2 || motion->id >= static_cast<int>(motion_count.size())) {
				return false;
			}
			if(motion->id == -1){
				if(motion_count.size() <= motion->id2) {
					return false;
				}
				motion_count[motion->id2] += 1;
			} else {
				if(motion->id >= 0) {
					motion_count[static_cast<unsigned int>(motion->id)] += 1;
				}
				BOOST_FOREACH(boost::shared_ptr<Frame> frame, motion->frame_list) {
					if(frame->img_id >= img_name_list.size()) {
						return false;
					}
					img_info_list[frame->img_id].use_count++;
				}
			}
		}
		unsigned int non_use_img_count = 0;
		for(unsigned int i = 0; i < img_info_list.size(); i++) {
			if(img_info_list[i].use_count == 0) {
				non_use_img_count++;
				continue;
			}
			img_info_list[i].new_id = i - non_use_img_count;
		}
		printf("NonUseImg\t%d\n", non_use_img_count);
	
		unsigned int non_use_motion_count = 0;
		BOOST_FOREACH(int count, motion_count) {
			if(count > 1) {
				non_use_motion_count += count-1;
			}
		}
		printf("NonUseMotion\t%d\n", non_use_motion_count);

		for(unsigned int i = 0; i < motion_list.size(); i++) {
			if(motion_list[i]->id == -1) {
				motion_count[motion_list[i]->id2]--;
				if(motion_count[motion_list[i]->id2] > 0) {
					motion_list.erase(motion_list.begin() + static_cast<int>(i));
					i--;
				}
			} else {
				BOOST_FOREACH(boost::shared_ptr<Frame> frame, motion_list[i]->frame_list) {
					BOOST_ASSERT(img_info_list[frame->img_id].use_count > 0);
					frame->img_id = img_info_list[frame->img_id].new_id;
				}
				if(motion_list[i]->id != -2) {
					motion_count[static_cast<unsigned int>(motion_list[i]->id)]--;
					if(motion_count[static_cast<unsigned int>(motion_list[i]->id)] > 0) {
						const std::vector<boost::shared_ptr<Motion> >::iterator erase_first = motion_list.begin() + static_cast<int>(i);
						std::vector<boost::shared_ptr<Motion> >::iterator erase_end = erase_first+1;
						while((*erase_end)->id == -2) {
							erase_end++;
						}
						motion_list.erase(erase_first, erase_end);
						i--;
					}
				}
			}
		}
		for(unsigned int i = 0; i < img_name_list.size(); i++) {
			if(img_info_list[i].use_count == 0) {
				img_info_list.erase(img_info_list.begin() + static_cast<int>(i));
				img_name_list.erase(img_name_list.begin() + static_cast<int>(i));
				i--;
			}
		}
		return true;
	}

	bool ToPTree(boost::property_tree::ptree &out) const {
		out.add("version", version);
		unsigned int motion_id = 0;
		unsigned int motion_sub_id = 0;
		for(unsigned int i = 0; i < motion_list.size(); i++) {
			BOOST_ASSERT(motion_list[i]->id > -3);
			char key[64];
			if(motion_list[i]->id == -1) {
				SPRINTF(key, "motion[%d]", motion_list[i]->id2);
			} else if(motion_list[i]->id == -2) {
				SPRINTF(key, "motion[%d][%d]", motion_id, motion_sub_id);
				motion_sub_id++;
			} else if(i + 1 < motion_list.size() && motion_list[i+1]->id == -2) {
				SPRINTF(key, "motion[%d][0]", motion_list[i]->id);
				motion_sub_id = 1;
				motion_id = static_cast<unsigned int>(motion_list[i]->id);
			} else {
				SPRINTF(key, "motion[%d]", motion_list[i]->id);
			}
			boost::property_tree::ptree ptree;
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%d/%d          ", i+1, motion_list.size());
			if(!motion_list[i]->ToPTree(ptree, img_name_list, version)) {
				return false;
			}
			out.add_child(key, ptree);
		}
		return true;
	}

	bool Load(std::istream &in) {
		in.seekg(0, std::ios::end);
		const unsigned int in_size = static_cast<unsigned int>(in.tellg());
		if(in_size < 7) {
			return false;
		}
		in.seekg(0, std::ios::beg);
		std::vector<char> in_data(in_size);
		in.read(&in_data.front(), in_size);
		const char *p = &in_data.front();
		if(!in.good()) {
			return false;
		}

		version = *reinterpret_cast<const unsigned char *>(p);
		p++;
		if(version != 3 && version != 5) {
			return false;
		}
		const unsigned int img_count = *reinterpret_cast<const unsigned short *>(p);
		p += 2;
		if(in_size < 7 + img_count * IMG_NAME_LENGTH) {
			return false;
		}
		img_name_list.resize(img_count);
		BOOST_FOREACH(std::string &img_name, img_name_list) {
			BOOST_ASSERT(STRLEN(p) < IMG_NAME_LENGTH);
			img_name = p;
			p += IMG_NAME_LENGTH;
		}

		const unsigned int motion_count = *reinterpret_cast<const unsigned int *>(p);
		p += 4;
		motion_list.resize(motion_count);

		BOOST_FOREACH(boost::shared_ptr<Motion> &motion, motion_list) {
			const unsigned int load_size = in_data.size() - (p - &in_data.front());
			if(load_size == 0) {
				break;
			}
			motion.reset(new Motion());
			if(!motion->Load(p, load_size, version)) {
				return false;
			}
		}
		while(!motion_list.back()) {
			motion_list.pop_back();
		}
		if(!Normalization()) {
			return false;
		}
		return true;
	}

	bool Load(const boost::filesystem::path &path) {
		boost::filesystem::ifstream in(path, std::ios::binary);
		if(!in.is_open()) {
			return false;
		}
		return Load(in);
	}

	void Clear(void) {
		version = 0;
		img_name_list.clear();
		motion_list.clear();
	}
protected:
	unsigned char version;
	char padding[3]; // unused
	std::vector<std::string> img_name_list;
	std::vector<boost::shared_ptr<Motion> > motion_list;
};

bool PatExtract(const boost::filesystem::path &path) {
	Pat pat;
	if(!pat.Load(path)) {
		return false;
	}
	boost::property_tree::ptree ptree;
	if(!pat.ToPTree(ptree)) {
		return false;
	}
	const boost::filesystem::path out_path = boost::filesystem::change_extension(path, ".json");
	boost::filesystem::ofstream out(out_path);
	if(!out.is_open()) {
		return false;
	}
	boost::property_tree::write_json(out, ptree);
	return true;
}

int main(unsigned int argc, const char * const argv[]){
	if(argc < 2) {
		return 0;
	}

	org::click3::Utility::SetAppDir();

	int ret = 0;
	for(unsigned int i = 1; i < argc; i++) {
		if(!PatExtract(argv[i])) {
			return 1;
		}
	}
	return ret;
}
