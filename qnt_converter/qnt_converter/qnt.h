
#ifndef __QNT_H__
#define __QNT_H__

typedef struct {
        unsigned char red[256];
        unsigned char green[256];
        unsigned char blue[256];
} Pallet256;

typedef enum {
	ALCG_UNKNOWN = 1,
	ALCG_VSP     = 2,
	ALCG_PMS8    = 3,
	ALCG_PMS16   = 4,
	ALCG_BMP8    = 5,
	ALCG_BMP24   = 6,
	ALCG_QNT     = 7
} CG_TYPE;

class CGData {
public:
	CG_TYPE type;   /* cg format type             */
	int x;          /* default display location x */
	int y;          /* default display location y */
	unsigned int width;
	unsigned int height;
	
	std::vector<unsigned char> pic;
	
	int vsp_bank;   /* pallet bank for vsp */
	int pms_bank;   /* pallet bank for pms */
	
	int spritecolor; /* sprite color for vsp and pms8 */
	int alphalevel;  /* alpha level of image */
	
	int data_offset; /* pic offset for clipping */
};



typedef struct {
	int hdr_size;     /* header size */
	int x0;           /* display location x */
	int y0;           /* display location y */
	int width;        /* image width        */
	int height;       /* image height       */
	int bpp;          /* image data depth   */
	int rsv;          /* reserved data      */
	int pixel_size;   /* compressed pixel size       */
	int alpha_size;   /* compressed alpha pixel size */
} qnt_header;

#endif /* __QNT_H__ */
