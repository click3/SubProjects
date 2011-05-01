#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <iostream>
#include <vector>
#include <string>

#include "boost/foreach.hpp"
#include "boost/tuple/tuple.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/spirit/home/support/detail/endian.hpp"

bool ParseFirstData(unsigned int &func_count, unsigned int &file_count, std::vector<boost::tuple<unsigned int, std::string> > &file_list, std::vector<boost::tuple<unsigned int, std::string> > &func_list, const std::vector<unsigned char> &data) {
	if(data.size() < 4) {
		std::cout << __LINE__ << std::endl;
		return false;
	}
	boost::integer::ubig32_t big_func_count;
	::memcpy(&big_func_count, &data.front(), 4);
	func_count = big_func_count;

	if(data.size() < 4 + 4 * func_count + 1 * func_count) {
		return false;
	}
	func_list.resize(func_count);
	file_count = 0;
	file_list.clear();
	if(func_count == 0) {
		std::cout << __LINE__ << std::endl;
		return true;
	}
	unsigned int data_index = 4;
	std::vector<unsigned int> func_file_addr_list;
	for(unsigned int i = 0; i < func_count; i++) {
		boost::integer::ubig32_t obj_addr;
		::memcpy(&obj_addr, &data[data_index], 4);
		data_index += 4;
		func_file_addr_list.push_back(obj_addr);
	}
	std::vector<unsigned int> file_addr_list(func_file_addr_list);
	std::sort(file_addr_list.begin(), file_addr_list.end());
	std::vector<unsigned int>::iterator new_end = std::unique(file_addr_list.begin(), file_addr_list.end());
	file_addr_list.erase(new_end, file_addr_list.end());
	file_count = file_addr_list.size();
	BOOST_FOREACH(unsigned int file_addr, file_addr_list) {
		file_list.push_back(boost::make_tuple(file_addr, std::string()));
	}
	for(unsigned int i = 0; i < func_count; i++) {
		const unsigned char * const func_name_end = reinterpret_cast<const unsigned char *>(::strchr(reinterpret_cast<const char *>(&data[data_index]), '\0'));
		if(func_name_end == NULL || std::distance(&data.front(), func_name_end) >= data.size()) {
			std::cout << __LINE__ << std::endl;
			return false;
		}
		const std::vector<unsigned int>::iterator it = std::find(file_addr_list.begin(), file_addr_list.end(), func_file_addr_list[i]);
		if(it == file_addr_list.end()) {
			std::cout << __LINE__ << std::endl;
			return false;
		}
		const unsigned int file_index = std::distance(file_addr_list.begin(), it);
		func_list[i] = boost::make_tuple(file_index, std::string(&data[data_index], func_name_end));
		data_index += std::distance(&data[data_index], func_name_end) + 1;
	}
	if(data_index != data.size()) {
		std::cout << __LINE__ << std::endl;
		return false;
	}
	return true;
}

struct GetFileListAddr {
	unsigned int operator()(const boost::tuple<unsigned int, std::string> &file_data) const {
		return file_data.get<0>();
	}
};

struct FindFuncName {
	FindFuncName(const std::string &funcname) : funcname(funcname) { }
	const std::string funcname;
	bool operator()(const boost::tuple<unsigned int, std::string> &func_data) const {
		return (func_data.get<1>() == funcname);
	}
};

bool ParseSecondData(unsigned int &func_count, unsigned int &file_count, std::vector<boost::tuple<unsigned int, std::string> > &file_list, std::vector<boost::tuple<unsigned int, std::string> > &func_list, const std::vector<unsigned char> &data) {
	if(data.size() < 4) {
		std::cout << __LINE__ << std::endl;
		return false;
	}
	boost::integer::ulittle32_t little_file_count;
	::memcpy(&little_file_count, &data.front(), 4);
	if(file_count > little_file_count) {
		std::cout << __LINE__ << std::endl;
		return false;
	}
	file_count = little_file_count;

	if(data.size() < 4 + file_count * 4) {
		std::cout << __LINE__ << std::endl;
		return false;
	}
	unsigned int data_index = 4;
	std::vector<unsigned int> file_addr_list(file_count);
	bool is_file_list_equal = true;
	if(file_count > 0) {
		for(unsigned int i = 0; i < file_count; i++) {
			boost::integer::ulittle32_t little_file_addr;
			::memcpy(&little_file_addr, &data[data_index], 4);
			file_addr_list[i] = little_file_addr;
			data_index += 4;
		}
		std::vector<unsigned int> sorted_file_addr_list(file_addr_list);
		std::sort(sorted_file_addr_list.begin(), sorted_file_addr_list.end());
		std::vector<boost::tuple<unsigned int, std::string> >::const_iterator it = file_list.begin();
		BOOST_FOREACH(unsigned int file_addr, sorted_file_addr_list) {
			if(it->get<0>() < file_addr) {
				std::cout << __LINE__ << std::endl;
				return false;
			}
			if(it->get<0>() > file_addr) {
				is_file_list_equal = false;
				continue;
			}
			++it;
		}
	}

	boost::integer::ulittle32_t little_func_count;
	::memcpy(&little_func_count, &data[data_index], 4);
	data_index += 4;
	if(func_count != little_func_count || data.size() <= data_index + func_count * (2 + 1)) {
		std::cout << __LINE__ << std::endl;
		return false;
	}

	std::vector<unsigned int> file_index_list(func_count);
	for(unsigned int i = 0; i < func_count; i++) {
		boost::integer::ulittle16_t little_file_index;
		::memcpy(&little_file_index, &data[data_index], 2);
		file_index_list[i] = little_file_index - 1;
		if(file_addr_list.size() <= file_index_list[i]) {
			std::cout << __LINE__ << std::endl;
			return false;
		}
		data_index += 2;
	}
	
	std::vector<unsigned int> old_file_addr_list(file_list.size());
	std::transform(file_list.begin(), file_list.end(), old_file_addr_list.begin(), GetFileListAddr());
	for(unsigned int i = 0; i < func_count; i++) {
		const unsigned char * const func_name_end = reinterpret_cast<const unsigned char *>(::strchr(reinterpret_cast<const char *>(&data[data_index]), '\0'));
		if(func_name_end == NULL || std::distance(&data.front(), func_name_end) >= data.size()) {
			std::cout << __LINE__ << std::endl;
			return false;
		}
		std::vector<boost::tuple<unsigned int, std::string> >::iterator it = std::find_if(func_list.begin(), func_list.end(), FindFuncName(std::string(&data[data_index], func_name_end)));
		if(it == func_list.end()) {
			std::cout << __LINE__ << std::endl;
			return false;
		}
		if(is_file_list_equal) {
			if(file_index_list[i] != it->get<0>()) {
				std::cout << it->get<1>() << " == " << reinterpret_cast<const char *>(&data[data_index]) << std::endl;
				std::cout << file_index_list[i] << " == " << it->get<0>() << std::endl;
				std::cout << __LINE__ << std::endl;
				return false;
			}
		} else {
			if(old_file_addr_list.size() <= it->get<0>()) {
				std::cout << __LINE__ << std::endl;
				return false;
			}
			if(file_addr_list[file_index_list[i]] != old_file_addr_list[it->get<0>()]) {
				std::cout << __LINE__ << std::endl;
				return false;
			}
			*it = boost::make_tuple(file_index_list[i], it->get<1>());
		}
		data_index += std::distance(&data[data_index], func_name_end) + 1;
	}
	if(!is_file_list_equal) {
		file_list.resize(file_count);
		for(unsigned int i = 0; i < file_count; i++) {
			file_list[i] = boost::make_tuple(file_addr_list[i], std::string());
		}
	}
	if(data_index != data.size()) {
		std::cout << __LINE__ << std::endl;
		return false;
	}
	return true;
}

bool ParseThirdData(std::vector<boost::tuple<unsigned int, std::string> > &file_list, const std::vector<unsigned char> &data) {
	unsigned int data_index = 0;
	if(data.size() < file_list.size() * 1) {
		std::cout << __LINE__ << std::endl;
		return false;
	}
	for(unsigned int i = 0; i < file_list.size(); i++) {
		if(!file_list[i].get<1>().empty()) {
			std::cout << __LINE__ << std::endl;
			return false;
		}
		const unsigned char * const file_name_end = reinterpret_cast<const unsigned char *>(::strchr(reinterpret_cast<const char *>(&data[data_index]), '\0'));
		if(file_name_end == NULL || std::distance(&data.front(), file_name_end) >= data.size()) {
			std::cout << __LINE__ << std::endl;
			return false;
		}
		file_list[i] = boost::make_tuple(file_list[i].get<0>(), std::string(&data[data_index], file_name_end));
		data_index += std::distance(&data[data_index], file_name_end) + 1;
	}
	if(data_index != data.size()) {
		std::cout << __LINE__ << std::endl;
		return false;
	}
	return true;
}

bool ParseLib(const char *lib_filename) {
	const boost::filesystem::path lif_path(lib_filename);
	std::cout << "parse " << boost::filesystem::system_complete(lif_path) << std::endl;
	boost::filesystem::ifstream in(lif_path, std::ios::binary);
	if(!in.is_open()) {
		std::cout << __LINE__ << std::endl;
		return false;
	}
	in.seekg(0, std::ios::end);
	const unsigned int file_size = static_cast<unsigned int>(in.tellg());
	in.seekg(0, std::ios::beg);
	if(file_size < 7) {
		std::cout << __LINE__ << std::endl;
		return false;
	}
	char signature[7];
	in.read(signature, sizeof(signature));
	if(::memcmp(signature, "!<arch>", sizeof(signature)) != 0) {
		std::cout << __LINE__ << std::endl;
		return false;
	}

	unsigned int func_count;
	unsigned int file_count;
	std::vector<boost::tuple<unsigned int, std::string> > file_list;
	std::vector<boost::tuple<unsigned int, std::string> > func_list;
	std::vector<char> filename_buffer;
	unsigned int set_filename_index = 0;
	const boost::filesystem::path base_dir = boost::filesystem::path(lib_filename).replace_extension();
	for(unsigned int index = 0; file_size > static_cast<unsigned int>(in.tellg()); index++) {
		char text_header[61];
		bool is_filename_found = false;
		while(true) {
			if(file_list.size() > set_filename_index && file_list[set_filename_index].get<1>().empty() && file_list[set_filename_index].get<0>() == in.tellg()) {
				is_filename_found = true;
			}
			in.read(text_header, 1);
			if(!in.good()) {
				std::cout << __LINE__ << std::endl;
				return false;
			}
			if(text_header[0] != '\n') {
				break;
			}
		}
		in.read(&text_header[1], sizeof(text_header) - 2);
		if(!in.good() || ::isdigit(text_header[16]) == 0 || ::isdigit(text_header[40]) == 0 || ::isdigit(text_header[48]) == 0 || ::strncmp(&text_header[58], "`\n", 2) != 0) {
			std::cout << __LINE__ << std::endl;
			return false;
		}
		text_header[60] = '\0';
		const char * const filename_end = ::strrchr(text_header, '/');
		if(filename_end == NULL) {
			std::cout << __LINE__ << std::endl;
			return false;
		}
		const unsigned int filename_length = std::distance(const_cast<const char *>(text_header), filename_end);
		if(filename_length > 15) {
			std::cout << __LINE__ << std::endl;
			return false;
		}
		char filename[256];
		if(filename_length > 0 || ::isdigit(text_header[1]) == 0) {
			::strncpy(filename, text_header, filename_length);
			filename[filename_length] = '\0';
		} else {
			const unsigned int filename_index = static_cast<unsigned int>(::atoi(&text_header[1]));
			if(filename_index >= filename_buffer.size()) {
				std::cout << __LINE__ << std::endl;
				return false;
			}
			::strcpy_s(filename, sizeof(filename), &filename_buffer[filename_index]);
		}
		if(is_filename_found) {
			file_list[set_filename_index] = boost::make_tuple(file_list[set_filename_index].get<0>(), std::string(filename));
			set_filename_index++;
		}
		const unsigned int time_stamp = static_cast<unsigned int>(::atoi(&text_header[16]));
		const unsigned int unknown = static_cast<unsigned int>(::atoi(&text_header[40]));
		const unsigned int data_size = static_cast<unsigned int>(::atoi(&text_header[48]));
		std::cout << "filename: \"" << filename << "\"" << std::endl;
		std::cout << "time_stamp: " << time_stamp << std::endl;
		std::cout << "unknown: " << unknown << std::endl;
		std::cout << "data_size: " << data_size << std::endl;
		if(data_size == 0) {
			continue;
		}
		std::vector<unsigned char> data(data_size);
		in.read(reinterpret_cast<char *>(&data.front()), data_size);
		if(!in.good()) {
			std::cout << __LINE__ << std::endl;
			return false;
		}
		switch(index) {
			case 0:
				if(!ParseFirstData(func_count, file_count, file_list, func_list, data)) {
					std::cout << __LINE__ << std::endl;
					return false;
				}
				break;
			case 1:
				if(!ParseSecondData(func_count, file_count, file_list, func_list, data)) {
					std::cout << __LINE__ << std::endl;
					return false;
				}
				break;
			case 2:
				if(::strcmp(filename, "/") == 0) {
					filename_buffer.swap(std::vector<char>(reinterpret_cast<const char *>(&data.front()), reinterpret_cast<const char *>(&data.back()) + 1));
					break;
				}
				//filenameÇ™/èoÇ»Ç¢èÍçáÇÕÇªÇÃå„Ç∆ìØÇ∂èàóùÇÇ∑ÇÈ
			default: {
				const boost::filesystem::path file_path = base_dir / (filename == boost::filesystem::system_complete(filename) ? boost::filesystem::path(filename).filename() : boost::filesystem::path(filename));

				const boost::filesystem::path dir_path = boost::filesystem::path(file_path).remove_filename();
				boost::filesystem::create_directories(dir_path);
				boost::filesystem::ofstream out(file_path, std::ios::binary);
				if(!out.is_open()) {
					std::cout << __LINE__ << std::endl;
					return false;
				}
				out.write(reinterpret_cast<char *>(&data.front()), data.size());
				out.close();
				break;
			}
		}
	}

	const boost::filesystem::path file_path = base_dir/"func_list.txt";
	boost::filesystem::ofstream out(file_path);
	if(!out.is_open()) {
		std::cout << __LINE__ << std::endl;
		return false;
	}
	out << "func_count: " << func_count << std::endl;
	for(unsigned int i = 0; i < func_count; i++) {
		const boost::filesystem::path obj_path(file_list[func_list[i].get<0>()].get<1>());
		out << func_list[i].get<1>() << "(" << obj_path.filename() << ")" << std::endl;
	}
	out.close();
	return true;
}

int main(unsigned int argc, const char * const argv[]) {
	for(unsigned int i = 1; i < argc; i++) {
		ParseLib(argv[i]);
	}
	return 0;
}

