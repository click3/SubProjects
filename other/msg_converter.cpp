
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <vector>

#define NOMINMAX
#include <windows.h>

#include "boost/assert.hpp"
#include "boost/utility.hpp"
#include "boost/assign.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/optional.hpp"
#include "boost/foreach.hpp"
#include "boost/iostreams/stream.hpp"
#include "boost/enable_shared_from_this.hpp"

using namespace boost::assign;

void MyFClose(FILE *fp) {
	if(fp != NULL) {
		::fclose(fp);
	}
}

boost::shared_ptr<FILE> MyFOpen(const char *path, const char *type) {
	return boost::shared_ptr<FILE>(::fopen(path, type), MyFClose);
}

void print_tab(boost::shared_ptr<FILE> fp, unsigned int count) {
	for(unsigned int i = 0; i < count; i++) {
		fprintf(fp.get(), "\t");
	}
}

void SetAppDir(void){
	int i;
	char AppDir[256];

	GetModuleFileName(NULL,AppDir,256);
	i = strlen(AppDir)-1;
	while(AppDir[i] != '\\' && AppDir[i] != '/' && i > 0)i--;
	AppDir[i] = '\0';
	SetCurrentDirectory(AppDir);
}

void del_sp(char *s){
	while(*s != '\0'){
		if(*s == '"'){
			s++;
			while(*s != '"' && *s != '\0') {
				s++;
			}
		} else if(*s == ' ' || *s == '\t' || *s == '\n' || *s == ';'){
			memmove(s, &s[1], strlen(&s[1])+1);
		} else {
			*s = tolower(*s);
			s++;
		}
	}
}

bool th11text_convert(unsigned char *data, unsigned int size){
	BOOST_ASSERT(data);
	unsigned char a = 0x77;
	unsigned char b = 0x07;
	for(unsigned int i = 0; i < size; i++){
		data[i] ^= a;
		a += b;
		b += 0x10;
	}
	return true;
}

void th125mission_convert(unsigned char *in, unsigned int size, const char *filename) {
	BOOST_ASSERT(in);
	BOOST_ASSERT(filename);
	BOOST_ASSERT(size >= 4);
	const unsigned int msg_count = *reinterpret_cast<unsigned int *>(&in[0]);
	unsigned char out_list[13][2048];
	BOOST_ASSERT(size >= msg_count * 4 + 4);
	for(unsigned int i = 0; i < msg_count; i++) {
		const unsigned int msg_addr = *reinterpret_cast<unsigned int *>(&in[4 + i * 4]);
		BOOST_ASSERT(size >= msg_addr + 44);
		unsigned short out_id = *reinterpret_cast<unsigned short *>(&in[msg_addr]);
		unsigned short pos = *reinterpret_cast<unsigned short *>(&in[msg_addr + 2]);
		const unsigned int unknown3 = *reinterpret_cast<unsigned int *>(&in[msg_addr + 40]);
		if(out_id < 14) {
			unsigned char *out = out_list[out_id];
			const unsigned short unknown4 = *reinterpret_cast<unsigned short *>(&in[msg_addr + 4]);
			out[pos * 29 + unknown4 + 14] = unknown3;
			unsigned int unknown5 = in[msg_addr + 6];
			if(unknown4 != 0) {
				unknown5 *= 0x64;
			}

		} else {
			// TODO
		}
	}
}

class Proc;

class CodeModel : public boost::enable_shared_from_this<CodeModel> {
public:
	enum VAR_TYPE {
		VAR_TYPE_STRING,
		VAR_TYPE_NUMBER,
		VAR_TYPE_SYMBOL,
		VAR_TYPE_BOOL,
		VAR_TYPE_FLOAT,
	};
	CodeModel(const char *proc_name, unsigned char code) :
		proc_name(proc_name), code(code), symbol(NULL)
	{
		BOOST_ASSERT(proc_name);
	}
	CodeModel &operator()(VAR_TYPE var_type) {
		BOOST_ASSERT(var_type != VAR_TYPE_SYMBOL);
		param_type_list.push_back(var_type);
		return *this;
	}
	CodeModel &operator()(VAR_TYPE var_type, const char *symbol) {
		BOOST_ASSERT(var_type == VAR_TYPE_SYMBOL);
		param_type_list.push_back(var_type);
		this->symbol = symbol;
		return *this;
	}
	const char *GetProcName(void) const {
		return proc_name;
	}
	unsigned char GetCode(void) const {
		return code;
	}
	const std::vector<VAR_TYPE> &GetParamTypeList(void) const {
		return param_type_list;
	}
	const char *GetSymbol(void) const {
		return symbol;
	}
	boost::optional<Proc> TryParse(unsigned short *cur_time, const char *proc_name, const std::vector<const char *>param_list, const char *param_end) const;
protected:
	const char *proc_name;
	unsigned char code;
	std::vector<VAR_TYPE> param_type_list;
	const char *symbol;
};

class ModelList : public std::vector<boost::shared_ptr<const CodeModel> > {
private:
	ModelList(void) {
		std::vector<CodeModel> source;
		source +=
			CodeModel("sleep",			0xFF)(CodeModel::VAR_TYPE_NUMBER),//本来codeは存在しない
			CodeModel("",				0x00),
			CodeModel("enable_img",		0x01)(CodeModel::VAR_TYPE_SYMBOL, "player")(CodeModel::VAR_TYPE_NUMBER),
			CodeModel("enable_img",		0x02)(CodeModel::VAR_TYPE_SYMBOL, "enemy")(CodeModel::VAR_TYPE_NUMBER),
			CodeModel("enable_textarea",	0x03),
			CodeModel("disable_img",		0x04)(CodeModel::VAR_TYPE_SYMBOL, "player"),
			CodeModel("disable_img",		0x05)(CodeModel::VAR_TYPE_SYMBOL, "enemy"),
			CodeModel("disable_textarea",	0x06),
			CodeModel("mode",			0x07)(CodeModel::VAR_TYPE_SYMBOL, "player"),
			CodeModel("mode",			0x08)(CodeModel::VAR_TYPE_SYMBOL, "enemy"),
			CodeModel("mode",			0x09)(CodeModel::VAR_TYPE_SYMBOL, "other"),
			CodeModel("skip",			0x0A)(CodeModel::VAR_TYPE_BOOL),
			CodeModel("enter_sleep",		0x0B)(CodeModel::VAR_TYPE_NUMBER),
			CodeModel("call_boss",		0x0C),
			CodeModel("image_change",		0x0D)(CodeModel::VAR_TYPE_SYMBOL, "player")(CodeModel::VAR_TYPE_NUMBER),
			CodeModel("image_change",		0x0E)(CodeModel::VAR_TYPE_SYMBOL, "enemy")(CodeModel::VAR_TYPE_NUMBER),
			CodeModel("print",			0x11)(CodeModel::VAR_TYPE_STRING),
			CodeModel("play_bgm",		0x13),
			CodeModel("name_img",		0x14),
			CodeModel("next_stage",		0x15),
			CodeModel("end_bgm",			0x16),
			CodeModel("unknown23",		0x17),
			CodeModel("unknown24",		0x18),
			CodeModel("print_position",		0x19)(CodeModel::VAR_TYPE_NUMBER),
			CodeModel("bgm_fadeout",		0x1b)(CodeModel::VAR_TYPE_FLOAT),
			CodeModel("set_ballon_position",	0x1c)(CodeModel::VAR_TYPE_FLOAT)(CodeModel::VAR_TYPE_FLOAT),
			CodeModel("set_baloon_type",	0x1d)(CodeModel::VAR_TYPE_NUMBER),
			CodeModel("unknown30",		0x1e);
		BOOST_FOREACH(CodeModel model, source) {
			push_back(boost::shared_ptr<CodeModel>(new CodeModel(model)));
		}
	}
	static ModelList instance;

	struct FindModelCode {
		FindModelCode(unsigned char code) : code(code) { }
		const unsigned char code;
		bool operator ()(boost::shared_ptr<const CodeModel> model) const {
			return model->GetCode() == code;
		}
	};
	struct FindModelProcName {
		FindModelProcName(const char *proc_name) : proc_name(proc_name) { }
		const char * const proc_name;
		bool operator ()(boost::shared_ptr<const CodeModel> model) const {
			return (::strcmp(proc_name, model->GetProcName()) == 0);
		}
	};
public:
	static ModelList &GetInstance(void) {
		return instance;
	}
	static boost::shared_ptr<const CodeModel> FindCode(unsigned char code) {
		std::vector<boost::shared_ptr<const CodeModel> >::const_iterator it = std::find_if(GetInstance().begin(), GetInstance().end(), FindModelCode(code));
		if(it == GetInstance().end()) {
			return boost::shared_ptr<const CodeModel>();
		}
		return *it;
	}
	static boost::shared_ptr<const CodeModel> FindProcName(const char *proc_name) {
		std::vector<boost::shared_ptr<const CodeModel> >::const_iterator it = std::find_if(GetInstance().begin(), GetInstance().end(), FindModelProcName(proc_name));
		if(it == GetInstance().end()) {
			return boost::shared_ptr<const CodeModel>();
		}
		return *it;
	}
};
ModelList ModelList::instance;

class Proc {
public:
	Proc(unsigned short time, boost::shared_ptr<const CodeModel> model, const std::vector<unsigned char> &param = std::vector<unsigned char>()) :
		time(time), model(model), param(param)
	{
	}
	Proc(const Proc &obj) :
		time(obj.time), model(obj.model), param(obj.param)
	{
	}
	Proc() :
		time(0)
	{
	}
	~Proc() {
	}
	static boost::optional<Proc> Parse(const char *text, unsigned short *cur_time) {
		const char * const proc_name_start = text;
		const char *cur = proc_name_start;
		while(*cur != '(') {
			cur++;
		}
		char proc_name[256];
		strncpy_s(proc_name, sizeof(proc_name), proc_name_start, cur - proc_name_start);
		cur++;
		const char * const param_start = cur;
		std::vector<const char *> param_list;
		if(param_start[0] != ')') {
			param_list.push_back(param_start);
			while(*cur != ')') {
				if(*cur == ',') {
					param_list.push_back(&cur[1]);
				}
				if(*cur == '"') {
					do {
						cur++;
					} while(*cur != '"' && *cur != '\0');
					if(*cur == '\0') {
						printf("Error:二重引用符("")が閉じていません。\n");
						return boost::none;
					}
				}
				cur++;
			}
		}
		const char * const param_end = cur;

		BOOST_FOREACH(boost::shared_ptr<const CodeModel> model, ModelList::GetInstance()) {
			boost::optional<Proc> opt_proc = model->TryParse(cur_time, proc_name, param_list, param_end);
			if(opt_proc) {
				return opt_proc;
			}
		}
		return boost::none;
	}
	unsigned int Size(void) const {
		return 4 + param.size();
	}
	void fprint(boost::shared_ptr<FILE> fp, unsigned int tab) {
		if(model->GetCode() == 0x00) {
			return;
		}
		print_tab(fp, tab);
		fprintf(fp.get(), "%s(", model->GetProcName());
		bool first = true;
		unsigned int param_index = 0;
		BOOST_FOREACH(CodeModel::VAR_TYPE var_type, model->GetParamTypeList()) {
			if(!first) {
				fprintf(fp.get(), ",");
			}
			switch(var_type) {
				case CodeModel::VAR_TYPE_STRING: {
					BOOST_ASSERT(param_index == 0);
					BOOST_ASSERT(param.size() > 0);
					std::vector<unsigned char> text(param);
					const bool result = th11text_convert(&text.front(), text.size());
					BOOST_ASSERT(result);
					fprintf(fp.get(), "\"%s\"", reinterpret_cast<char *>(&text.front()));
					param_index = UINT_MAX;
					break;
				}
				case CodeModel::VAR_TYPE_NUMBER: {
					BOOST_ASSERT(param_index + 4 <= param.size());
					const int value = *reinterpret_cast<int *>(&param[param_index]);
					fprintf(fp.get(), "%d", value);
					param_index += 4;
					break;
				}
				case CodeModel::VAR_TYPE_SYMBOL: {
					fprintf(fp.get(), "%s", model->GetSymbol());
					break;
				}
				case CodeModel::VAR_TYPE_BOOL: {
					const char * const boolean[2] = {"FALSE", "TRUE"};
					BOOST_ASSERT(param_index + 4 <= param.size());
					const unsigned int bool_index = *reinterpret_cast<unsigned int *>(&param[param_index]);
					BOOST_ASSERT(bool_index < 2);
					fprintf(fp.get(), "%s", boolean[bool_index]);
					param_index += 4;
					break;
				}
				case CodeModel::VAR_TYPE_FLOAT: {
					BOOST_ASSERT(param_index + 4 <= param.size());
					const float value = *reinterpret_cast<float *>(&param[param_index]);
					fprintf(fp.get(), "%f", value);
					param_index += 4;
					break;
				}
			}
			first = false;
		}
		fprintf(fp.get(), ");\n");
	}

	unsigned short time;
	boost::shared_ptr<const CodeModel> model;
	std::vector<unsigned char> param;
};


boost::optional<Proc> CodeModel::TryParse(unsigned short *cur_time, const char *proc_name, const std::vector<const char *>param_list, const char *param_end) const {
	if(strcmp(this->proc_name, proc_name) != 0) {
		return boost::none;
	}
	BOOST_ASSERT(param_list.size() == param_type_list.size());
	std::vector<unsigned char> params;
	unsigned int index = 0;
	unsigned char code = this->code;
	BOOST_FOREACH(VAR_TYPE var_type, param_type_list) {
		char var[256];
		if(index < param_list.size()-1) {
			strncpy_s(var, sizeof(var), param_list[index], param_list[index+1] - param_list[index] -1);
		} else if(param_list.size()-1 == index) {
			strncpy_s(var, sizeof(var), param_list[index], param_end - param_list[index]);
		} else {
			BOOST_ASSERT(false);
			return boost::none;
		}
		if(var[0] == '"') {
			memmove(var, &var[1], strlen(var) - 2);
			var[strlen(var)-2] = '\0';
		}
		if(strcmp(proc_name, "sleep") == 0) { // sleepだけ例外処理
			*cur_time += atoi(var);
			return boost::none;
		}
		switch(var_type) {
			case VAR_TYPE_STRING: {
				const unsigned int len = static_cast<unsigned int>(::ceil((strlen(var) + 1.0) / 4.0)) * 4;
				memset(&var[strlen(var) + 1], '\0', len - strlen(var) - 1);
				params.resize(params.size() + len);
				bool result = th11text_convert(reinterpret_cast<unsigned char *>(var), len);
				memcpy_s(reinterpret_cast<char *>(&params[params.size() - len]), len, var, len);
				BOOST_ASSERT(result);
				break;
			}
			case VAR_TYPE_NUMBER: {
				unsigned int value = atoi(var);
				params.resize(params.size() + 4);
				memcpy_s(&params[params.size() - 4], 4, &value, 4);
				break;
			}
			case VAR_TYPE_SYMBOL: {
				if(strcmp(symbol, var) != 0) {
					return boost::none;
				}
				break;
			}
			case VAR_TYPE_BOOL: {
				unsigned int value = (strcmp(var, "true") == 0) ? 1 : 0;
				params.resize(params.size() + 4);
				memcpy_s(&params[params.size() - 4], 4, &value, 4);
				break;
			}
			case VAR_TYPE_FLOAT: {
				float value = atof(var);
				params.resize(params.size() + 4);
				memcpy_s(&params[params.size() - 4], 4, &value, 4);
				break;
			}
			default: {
				BOOST_ASSERT(false);
				return boost::none;
			}
		}
		index++;
	}
	return Proc(*cur_time, shared_from_this(), params);
}

std::ostream& operator<<(std::ostream &out, const Proc &proc) {
	unsigned char buf[4];
	*reinterpret_cast<unsigned short *>(buf) = proc.time;
	buf[2] = proc.model->GetCode();
	buf[3] = proc.param.size();
	out.write(reinterpret_cast<const char *>(buf), sizeof(buf));
	if(!proc.param.empty()) {
		out.write(reinterpret_cast<const char *>(&proc.param.front()), proc.param.size());
	}
	return out;
}

std::istream &operator>>(std::istream &in, Proc &proc) {
	unsigned char header[4];
	in.read(reinterpret_cast<char *>(header), sizeof(header));
	BOOST_ASSERT(in.gcount() == sizeof(header));
	proc.time = *reinterpret_cast<unsigned short *>(&header[0]);
	const unsigned char code = header[2];
	const boost::shared_ptr<const CodeModel> model = ModelList::FindCode(code);
	BOOST_ASSERT(model);
	proc.model = model;
	proc.param.clear();
	const unsigned char param_len = header[3];
	if(param_len == 0) {
		return in;
	}
	std::vector<unsigned char> param_buffer(param_len);
	in.read(reinterpret_cast<char *>(&param_buffer.front()), param_buffer.size());
	BOOST_ASSERT(in.gcount() == param_buffer.size());
	boost::iostreams::stream<boost::iostreams::basic_array_source<unsigned char> > param_stream(&param_buffer.front(), param_buffer.size());
	BOOST_FOREACH(CodeModel::VAR_TYPE var_type, proc.model->GetParamTypeList()) {
		switch(var_type) {
			case CodeModel::VAR_TYPE_STRING: {
				BOOST_ASSERT(proc.param.size() == 0);
				proc.param.resize(param_len);
				param_stream.read(&proc.param.front(), proc.param.size());
				BOOST_ASSERT(param_stream.gcount() == proc.param.size());
				break;
			}
			case CodeModel::VAR_TYPE_FLOAT:
			case CodeModel::VAR_TYPE_NUMBER: {
				proc.param.resize(proc.param.size() + 4);
				param_stream.read(&proc.param[proc.param.size() - 4], 4);
				BOOST_ASSERT(param_stream.gcount() == 4);
				break;
			}
			case CodeModel::VAR_TYPE_SYMBOL:
				break;
			case CodeModel::VAR_TYPE_BOOL: {
				unsigned int bool_value;
				param_stream.read(reinterpret_cast<unsigned char *>(&bool_value), 4);
				BOOST_ASSERT(param_stream.gcount() == 4);
				BOOST_ASSERT(bool_value < 2);
				proc.param.resize(proc.param.size() + 4);
				memcpy(&proc.param[proc.param.size() - 4], &bool_value, 4);
				break;
			}
			default: {
				BOOST_ASSERT(false);
			}
		}
	}
	return in;
}

class Script {
public:
	unsigned int Size(void) const {
		unsigned int result = 0;
		BOOST_FOREACH(Proc proc, proc_list) {
			result += proc.Size();
		}
		result += 4;
		return result;
	}
	void fprint(boost::shared_ptr<FILE> fp, unsigned int proc_id, unsigned int tab = 0) {
		print_tab(fp, tab);
		fprintf(fp.get(), "function proc%d{\n", proc_id);
		unsigned int time = 0;
		BOOST_FOREACH(Proc proc, proc_list) {
			if(proc.time != time) {
				print_tab(fp, tab+1);
				fprintf(fp.get(), "sleep(%d);\n\n", proc.time - time);
			}
			proc.fprint(fp, tab + 1);
			time = proc.time;
		}
		print_tab(fp, tab);
		fprintf(fp.get(), "}\n");
	}


	std::vector<Proc> proc_list;
};

std::ostream& operator<<(std::ostream &out, const Script &script) {
	BOOST_FOREACH(Proc proc, script.proc_list) {
		out << proc;
	}
	const unsigned int end_signature = 0x00000000;
	out.write(reinterpret_cast<const char *>(&end_signature), sizeof(end_signature));
	return out;
}

std::istream &operator>>(std::istream &in, Script &script) {
	while(true) {
		Proc proc;
		in >> proc;
		script.proc_list.push_back(proc);
		if(proc.model->GetCode() == 0x00) {
			break;
		}
	}
	unsigned int end_signature;
	in.read(reinterpret_cast<char *>(&end_signature), sizeof(end_signature));
	BOOST_ASSERT(in.gcount() == sizeof(end_signature));
	BOOST_ASSERT(end_signature == 0x00000000);
	return in;
}

std::ostream &operator<<(std::ostream &out, const std::vector<Script> &script_list) {
	char script_count[4];
	*reinterpret_cast<unsigned int *>(script_count) = script_list.size();
	out.write(script_count, sizeof(script_count));

	unsigned int script_address = 4 + script_list.size() * 8;
	BOOST_FOREACH(Script script, script_list) {
		char data[8];
		*reinterpret_cast<unsigned int *>(&data[0]) = script_address;
		*reinterpret_cast<unsigned int *>(&data[4]) = 0x100; // よくわからないので固定値
		out.write(data, sizeof(data));
		script_address += script.Size();
	}
	
	BOOST_FOREACH(Script script, script_list) {
		out << script;
	}
	return out;	
}

std::istream &operator>>(std::istream &in, std::vector<Script> &script_list) {
	unsigned int script_count;
	in.read(reinterpret_cast<char *>(&script_count), 4);
	BOOST_ASSERT(in.gcount() == 4);

	std::vector<unsigned int> addr_list(script_count);
	for(unsigned int i = 0; i < script_count; i++) {
		in.read(reinterpret_cast<char *>(&addr_list[i]), 4);
		BOOST_ASSERT(in.gcount() == 4);
		unsigned int signature;
		in.read(reinterpret_cast<char *>(&signature), 4);
		BOOST_ASSERT(in.gcount() == 4);
		BOOST_ASSERT(signature == 0x100);
	}

	BOOST_FOREACH(unsigned int addr, addr_list) {
		in.seekg(addr, std::istream::beg);
		Script script;
		in >> script;
		script_list.push_back(script);
	}
	return in;
}

void th11msg_print(boost::shared_ptr<FILE> fp, const std::vector<Script> &script_list, unsigned int tab = 0) {
	unsigned int proc_id = 0;
	BOOST_FOREACH(Script script, script_list) {
		script.fprint(fp, proc_id, tab);
		proc_id++;
	}
}


void th11msg_convert_new(unsigned char *in, unsigned int size, const char *filename) {
	boost::iostreams::stream<boost::iostreams::array_source> in_stream(reinterpret_cast<char *>(in), size);
	std::vector<Script> script_list;
	in_stream >> script_list;
	char put_filename[256];
	if(false) {
		strcpy(put_filename, filename);
		put_filename[strlen(put_filename) - 4] = '\0';
		strcat(put_filename, ".txt");
	} else {
		sprintf(put_filename, "%s.txt", filename);
	}
	boost::shared_ptr<FILE> fp = MyFOpen(put_filename, "w");
	th11msg_print(fp, script_list);
	if(true) {
		sprintf(put_filename, "%s.msg", filename);
		std::ofstream out_stream(put_filename, std::ofstream::binary);
		out_stream << script_list;
	}
}

void th11msg_convert(unsigned char *in, unsigned int size, const char *filename) {
	int l;
	unsigned char data[256];
	const char *boolean[2] = {"FALSE", "TRUE"};
	BOOST_ASSERT(in != NULL);
	BOOST_ASSERT(filename != NULL);

	BOOST_ASSERT(size >= 4);
	const unsigned int msg_count = *(int*)&in[0];//msg個数の取得
	char put_filename[256];
	strcpy(put_filename, filename);
	strcpy(&put_filename[strlen(put_filename)-4], ".txt");
	FILE * const wfp = fopen(put_filename, "w");
	BOOST_ASSERT(wfp != NULL);

	bool ed_flag;
	if(put_filename[strlen(put_filename)-7] == 'e' || strncmp(&put_filename[strlen(put_filename)-9],"staff",5) == 0){
		ed_flag = true;
	} else {
		ed_flag = false;
	}

	for(unsigned int i = 0; i < msg_count; i++) {
		const unsigned int func_addr_ptr = 4 + 8 * i;
		BOOST_ASSERT(size >= func_addr_ptr + 4);
		const unsigned int func_addr = *(unsigned int*)&in[func_addr_ptr];
		fprintf(wfp,"function proc%d{\n", i);
		unsigned int time_count = 0;
		unsigned char *func_ptr = &in[func_addr];
		while(true) {
			BOOST_ASSERT(size >= func_ptr - in + 4);
			const unsigned int current_time = *(unsigned short*)func_ptr;
			if(current_time > time_count) {
				fprintf(wfp,"\tsleep(%d);\n\n", current_time - time_count);
				time_count = current_time;
			}
			const unsigned char proc = func_ptr[2];
			const unsigned char len = func_ptr[3];
			BOOST_ASSERT(size >= func_ptr - in + 4 + len);
			memcpy(data, &func_ptr[4], len);
			func_ptr += len + 4;
			fprintf(wfp,"\t");
			bool result;
			if(ed_flag){	//ED
				switch(proc){
					case 0x00:
						break;
					case 0x03:
						result = th11text_convert(data,len);
						BOOST_ASSERT(result);
						fprintf(wfp,"print(\"%s\")\n",data);
						break;
					case 0x05:
						BOOST_ASSERT(len == 4);
						fprintf(wfp,"enter_sleep(%d);\n", *(int *)data);
						break;
					case 0x06:
						BOOST_ASSERT(len == 4);
						fprintf(wfp,"enter_sleep_next_page(%d);\n", *(int *)data);
						break;
					case 0x07:
						BOOST_ASSERT(len >= 9);
						fprintf(wfp,"load_anm(%d,\"%s\");\n",*(int *)data,&data[4]);
						break;
					case 0x08:
						BOOST_ASSERT(len == 12);
						fprintf(wfp,"set_background_image(%d, %d, %d);\n", *(int *)&data[0], *(int *)&data[4], *(int *)&data[8]);
						break;
					case 0x09:
						BOOST_ASSERT(len == 4);
						fprintf(wfp,"set_text_color(%d, %d, %d);\n", data[0], data[1], data[2]);
						break;
					case 0x0A:
						BOOST_ASSERT(len >= 1);
						fprintf(wfp,"play_bgm(\"%s\");\n",data);
						break;
					case 0x0B:
						BOOST_ASSERT(len == 0);
						fprintf(wfp, "unknown11();\n");
						break;
					case 0x0C:
						fprintf(wfp, "call_staffroll(%s);\n", data);
						break;
					case 0x0E:
						BOOST_ASSERT(len == 4);
						fprintf(wfp, "unknown14(%d);\n", *(int *)&data[0]);
						break;
					case 0x0F:
						BOOST_ASSERT(len == 12);
						fprintf(wfp, "unknown15(%d, %d, %d);\n", *(int *)&data[0], *(int *)&data[4], *(int *)&data[8]);
						break;
					case 0x10:
						BOOST_ASSERT(len == 12);
						fprintf(wfp, "unknown16(%d, %d, %d);\n", *(int *)&data[0], *(int *)&data[4], *(int *)&data[8]);
						break;
					case 0x11:
						BOOST_ASSERT(len == 12);
						fprintf(wfp, "unknown17(%d, %d, %d);\n", *(int *)&data[0], *(int *)&data[4], *(int *)&data[8]);
						break;
					default:
						fprintf(wfp,"%d\t%d\t",proc,len);
						printf("%d\t%d\t",proc,len);
						l = 0;
						while(l < len){
							fprintf(wfp,"%02x ",data[l]);
							printf("%02x ",data[l]);
							l++;
						}
						fprintf(wfp,"\n");
						printf("\n");
						break;
				}
			} else {	//STG
				switch(proc){
					case 0x00:
						break;
					case 0x01:
						BOOST_ASSERT(len == 4);
						fprintf(wfp,"enable_img(player,%d);\n",*(int *)data);
						break;
					case 0x02:
						BOOST_ASSERT(len == 4);
						fprintf(wfp,"enable_img(enemy,%d);\n",*(int *)data);
						break;
					case 0x03:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"enable_textarea();\n");
						break;
					case 0x04:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"disable_img(player);\n");
						break;
					case 0x05:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"disable_img(enemy);\n");
						break;
					case 0x06:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"disable_textarea();\n");
						break;
					case 0x07:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"mode(player);\n");
						break;
					case 0x08:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"mode(enemy);\n");
						break;
					case 0x09:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"mode(other);\n");
						break;
					case 0x0A:
						BOOST_ASSERT(len == 4);
						BOOST_ASSERT(*(unsigned int *)data <= 1);
						fprintf(wfp,"skip(%s);\n", boolean[*(unsigned int *)data]);
						break;
					case 0x0B:
						BOOST_ASSERT(len == 4);
						fprintf(wfp,"enter_sleep(%d);\n",*(int *)data);
						break;
					case 0x0C:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"call_boss();\n");
						break;
					case 0x0D:
						BOOST_ASSERT(len == 4);
						fprintf(wfp,"image_change(player,%d);\n",*(int *)data);
						break;
					case 0x0E:
						BOOST_ASSERT(len == 4);
						fprintf(wfp,"image_change(enemy,%d);\n",*(int *)data);
						break;
					case 0x11:
						result = th11text_convert(data,len);
						BOOST_ASSERT(result);
						fprintf(wfp,"print(\"%s\");\n",data);
						break;
					case 0x12:
						BOOST_ASSERT(len == 0);
						fprintf(wfp, "unknown18();\n");
						break;
					case 0x13:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"play_bgm();\n");
						break;
					case 0x14:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"name_img();\n");
						break;
					case 0x15:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"next_stage();\n");
						break;
					case 0x16:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"end_bgm();\n");
						break;
					case 0x17:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"unknown23();\n");
						break;
					case 0x18:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"unknown24();\n");
						break;
					case 0x19:
						BOOST_ASSERT(len == 4);
						fprintf(wfp,"print_position(%d);\n", *(int *)data);
						break;
					case 0x1b:
						BOOST_ASSERT(len == 4);
						fprintf(wfp,"bgm_fadeout(%f);\n", *(float *)data);
						break;
					case 0x1c:
						BOOST_ASSERT(len == 8);
						fprintf(wfp,"set_ballon_position(%f, %f);\n", *(float *)&data[0], *(float *)&data[4]);
						break;
					case 0x1d:
						BOOST_ASSERT(len == 4);
						fprintf(wfp,"set_baloon_type(%d);\n", *(int *)data);
						break;
					case 0x1e:
						BOOST_ASSERT(len == 0);
						fprintf(wfp,"unknown30();\n");
						break;
					default:
						fprintf(wfp, "%d\t%d\t", proc,len);
						printf("%d\t%d\t", proc,len);
						l = 0;
						while(l < len){
							fprintf(wfp,"%02x ",data[l]);
							printf("%02x ",data[l]);
							l++;
						}
						fprintf(wfp,"\n");
						printf("\n");
						break;
				}
			}
			if(proc == 0x00) {
				break;
			}
		}
		fseek(wfp, -1, SEEK_CUR);
		fprintf(wfp, "}\n");
	}
	fclose(wfp);
}

void compile(const char *filename){
	unsigned short cur_time;
	unsigned int function_id;
	std::vector<Script> script_list;
	boost::shared_ptr<FILE> fp = MyFOpen(filename, "r");
	bool is_function = false;
	unsigned int line_count = 0;
	char line[256];
	while(fgets(line, sizeof(line), fp.get()) != NULL){
		line_count++;
		del_sp(line);
		if(line[0] == '\0' || line[0] == '#' || strcmp(line, "//") == 0) {
			continue;
		}
		if(!is_function){
			if(strncmp(line, "function", 8) != 0){
				printf("Error:予期せぬ構文です(行:%d)\n", line_count);
				return;
			}
			char *function_start = line;
			while(function_start[0] != '{' && function_start[0] != '\0') {
				function_start++;
			}
			if(function_start[0] == '{'){
				function_start[0] = '\0';
			} else {
				printf("Error:改行の前に{が必要です。(行:%d)\n", line_count);
				return;
			}
			if(strncmp(&line[8],"proc",4) != 0){
				printf("Error:関数名が定義外です(行:%d)\n", line_count);
				return;
			}
			is_function = true;
			cur_time = 0;
			function_id = atoi(&line[12]);
			script_list.push_back(Script());
		} else {
			if(line[0] == '}') {
				if(line[1] != '\0') {
					printf("Error:}の後に改行が必要です。(行:%d)\n", line_count);
					return;
				}
				//終端イベントの挿入
				const boost::shared_ptr<const CodeModel> model = ModelList::FindCode(0x00);
				script_list.back().proc_list.push_back(Proc(cur_time, model));
				is_function = false;
				continue;
			}
			boost::optional<Proc> opt_proc = Proc::Parse(line, &cur_time);
			if(opt_proc) {
				script_list.back().proc_list.push_back(opt_proc.get());
			}
		}
	}
	fp.reset();
	
	char put_filename[256];
	if(false) {
		strcpy(put_filename, filename);
		put_filename[strlen(put_filename) - 4] = '\0';
		strcat(put_filename, ".msg");
	} else {
		sprintf(put_filename, "%s.msg", filename);
	}
	std::ofstream out_stream(put_filename, std::ofstream::binary);
	out_stream << script_list;
	if(true) {
		sprintf(put_filename, "%s.txt", filename);
		boost::shared_ptr<FILE> fp = MyFOpen(put_filename, "w");
		th11msg_print(fp, script_list);
	}
}


void msg2txt(const char *filename){
	boost::shared_ptr<FILE> fp = MyFOpen(filename, "rb");
	fseek(fp.get(), 0, SEEK_END);
	const unsigned int size = ftell(fp.get());
	fseek(fp.get(), 0, SEEK_SET);
	std::vector<unsigned char> data(size);
	if(!data.empty()) {
		fread(&data.front(), 1, size, fp.get());
	}
	fp.reset();
	th11msg_convert(&data.front(), size, filename);
}

void txt2msg(const char *filename) {
	compile(filename);
}

int main(int argc,char *argv[]){
	int i;

	if(argc < 2)return 0;
	SetAppDir();

	i = 1;
	while(i < argc){
		if(strcmp(&argv[i][strlen(argv[i]) - 4], ".msg") == 0) {
			printf("%s msg to txt\n", argv[i]); 
			msg2txt(argv[i]);
		} else if(strcmp(&argv[i][strlen(argv[i]) - 4], ".txt") == 0) {
			printf("%s txt to msg\n", argv[i]);
			txt2msg(argv[i]);
		} else {
			printf("Unknown Extention File(%s)\n", argv[i]);
		}
		i++;
	}
	printf("Press Enter To Exit...");
	getchar();
	return 0;
}
