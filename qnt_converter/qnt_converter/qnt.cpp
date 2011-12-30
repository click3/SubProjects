
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>
#include <algorithm>

#include <zlib.h>

#include "boost/assert.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/foreach.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/gil/gil_all.hpp"
#include "boost/gil/extension/io/png_io.hpp"

#include "qnt.h"

bool extract_header(boost::shared_ptr<qnt_header> &result, const unsigned char *b) {
	if(b == NULL) {
		return false;
	}
	result.reset(new qnt_header());
	if (*reinterpret_cast<const unsigned int *>(&b[4]) == 0) {
		result->hdr_size = 48;
		::memcpy(&result->x0, &b[8], 32);
	} else {
		::memcpy(&result->hdr_size, &b[8], 36);
	}
	return true;
}

bool extract_pixel(std::vector<unsigned char> &pic, boost::shared_ptr<const qnt_header> qnt, const unsigned char *b) {
	if(!qnt || b == NULL || qnt->width <= 0 || qnt->height <= 0) {
		return false;
	}

	const unsigned int w = qnt->width;
	const unsigned int h = qnt->height;

	pic.resize(w * h * 4); // pic.resize((qnt->width+10) * (qnt->height+10) * 3);

	const unsigned int buf_size = w * h * 3;
	std::vector<unsigned char> raw(buf_size);

	uLongf orig_size = buf_size;
	const int uncomp_result = ::uncompress(&raw.front(), &orig_size, b, qnt->pixel_size);
	if((Z_BUF_ERROR != uncomp_result && Z_OK != uncomp_result) || buf_size != orig_size) {
		return false;
	}

	unsigned int j = 0;
	for(unsigned int i = 2; i <= 2; i--) {
		for(unsigned int y = 0; y < (h - 1); y+=2) {
			for(unsigned int x = 0; x < (w - 1); x+=2) {
				pic[( y   *w+x)  *4 +i] = raw[j];
				pic[((y+1)*w+x)  *4 +i] = raw[j+1];
				pic[( y   *w+x+1)*4 +i] = raw[j+2];
				pic[((y+1)*w+x+1)*4 +i] = raw[j+3];
				j+=4;
			}
			if((w%1) == 1) {
				pic[( y   *w+w-1)*4 +i] = raw[j];
				pic[((y+1)*w+w-1)*4 +i] = raw[j+1];
				j+=4;
			}
		}
		if((h%1) == 1) {
			const unsigned int y = h - 1;
			for(unsigned int x = 0; x < (w - 1); x+=2) {
				pic[(y*w+x  )*4+i] = raw[j];
				pic[(y*w+x+1)*4+i] = raw[j+2];
				j+=4;
			}
		}
	}

	if(w > 1) {
		for(unsigned int x = 1; x < w; x++) {
			pic[x*4  ] = pic[(x-1)*4  ] - pic[x*4  ];
			pic[x*4+1] = pic[(x-1)*4+1] - pic[x*4+1];
			pic[x*4+2] = pic[(x-1)*4+2] - pic[x*4+2];
		}
	}

	if(h > 1) {
		for(unsigned int y = 1; y < h; y++) {
			pic[(y*w)*4  ] = pic[((y-1)*w)*4  ] - pic[(y*w)*4  ];
			pic[(y*w)*4+1] = pic[((y-1)*w)*4+1] - pic[(y*w)*4+1];
			pic[(y*w)*4+2] = pic[((y-1)*w)*4+2] - pic[(y*w)*4+2];

			for(unsigned int x = 1; x < w; x++) {
				unsigned int px, py;
				py = pic[((y-1)*w+x  )*4];
				px = pic[( y   *w+x-1)*4];
				pic[(y*w+x)*4] = ((py+px)>>1) - pic[(y*w+x)*4];
				py = pic[((y-1)*w+x  )*4+1];
				px = pic[( y   *w+x-1)*4+1];
				pic[(y*w+x)*4+1] = ((py+px)>>1) - pic[(y*w+x)*4+1];
				py = pic[((y-1)*w+x  )*4+2];
				px = pic[( y   *w+x-1)*4+2];
				pic[(y*w+x)*4+2] = ((py+px)>>1) - pic[(y*w+x)*4+2];
			}
		}
	}
	return true;
}

bool extract_alpha(std::vector<unsigned char> &pic, boost::shared_ptr<const qnt_header> qnt, const unsigned char *b) {
	if(!qnt || b == NULL || qnt->width <= 0 || qnt->height <= 0) {
		return false;
	}

	const unsigned int w = qnt->width;
	const unsigned int h = qnt->height;

	if(qnt->alpha_size == 0) {
		for(unsigned int y = 0; y < h; y++) {
			for(unsigned int x = 0; x < w; x++) {
				pic[(y*w+x)*4+3] = 0xFF;
			}
		}
		return true;
	}

	const unsigned int buf_size = w * h;
	std::vector<unsigned char> raw(buf_size);
	
	uLongf orig_size = buf_size;
	const int uncomp_result = ::uncompress(&raw.front(), &orig_size, b, qnt->alpha_size);
	if((Z_BUF_ERROR != uncomp_result && Z_OK != uncomp_result) || buf_size != orig_size) {
		return false;
	}

	unsigned int i = 1;
	if(w > 1) {
		pic[3] = raw[0];
		for(unsigned int x = 1; x < w; x++) {
			pic[x*4+3] = pic[(x-1)*4+3] - raw[i];
			i++;
		}
	}

	if(h > 1) {
		for(unsigned int y = 1; y < h; y++) {
			pic[y*w*4] = pic[(y-1)*w*4] - raw[i];
			i++;
			for(unsigned int x = 1; x < w; x++) {
				unsigned int pax, pay;
				pax = pic[( y   *w+x-1)*4+3];
				pay = pic[((y-1)*w+x  )*4+3];
				pic[(y*w+x)*4+3] = ((pax+pay) >> 1) - raw[i];
				i++;
			}
		}
	}
	return true;
}

bool qnt_extract(boost::shared_ptr<CGData> &result, const unsigned char *data) {
	if(data == NULL) {
		return false;
	}
	boost::shared_ptr<qnt_header> qnt;
	if(!extract_header(qnt, data)) {
		return false;
	}

	result.reset(new CGData());
	if(!extract_pixel(result->pic, qnt, data + qnt->hdr_size)) {
		return false;
	}
	if(!extract_alpha(result->pic, qnt, data + qnt->hdr_size + qnt->pixel_size)) {
		return false;
	}

	result->type   = ALCG_QNT;
	result->x      = qnt->x0;
	result->y      = qnt->y0;
	result->width  = qnt->width;
	result->height = qnt->height;

	return true;
}

bool ReadQNT(boost::shared_ptr<CGData> &cg, boost::gil::rgba8_view_t &view, const boost::filesystem::path &filepath) {
	if(!boost::filesystem::is_regular_file(filepath)) {
		return false;
	}
	const unsigned int file_size = boost::filesystem::file_size(filepath);
	if(file_size == 0) {
		return false;
	}
	std::vector<unsigned char> buf(file_size);
	boost::filesystem::ifstream ifs(filepath, std::ios::binary);
	if(!ifs.is_open()) {
		return false;
	}
	ifs.read(reinterpret_cast<char *>(&buf.front()), buf.size());
	if(!ifs.good()) {
		return false;
	}
	ifs.close();

	if(!qnt_extract(cg, &buf.front())) {
		return false;
	}
	view = boost::gil::interleaved_view(cg->width, cg->height, reinterpret_cast<boost::gil::rgba8_pixel_t*>(&cg->pic.front()), cg->width*4);
	return true;
}

bool QNT2PNG(const boost::filesystem::path &filepath) {
	boost::shared_ptr<CGData> cg;
	boost::gil::rgba8_view_t view;
	if(!ReadQNT(cg, view, filepath)) {
		return false;
	}
	const boost::filesystem::path out_path = boost::filesystem::path(filepath).replace_extension(".png");
	boost::gil::png_write_view(out_path.string(), view);
	return true;
}

int main(unsigned int argc, const char * const *argv) {
	if(argc == 1) {
		BOOST_FOREACH(const boost::filesystem::path &path, std::make_pair(boost::filesystem::directory_iterator("./"), boost::filesystem::directory_iterator())) {
			std::wstring ext = path.extension().wstring();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
			if(ext == L".qnt") {
				if(!QNT2PNG(path)) {
					return 1;
				}
			}
		}
	} else {
		for(unsigned int i = 1; i < argc; i++) {
			if(!QNT2PNG(argv[i])) {
				return 1;
			}
		}
	}
	return 0;
}
