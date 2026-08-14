#ifndef RANDOMUNCATEGORIZEDBULLSHIT_H
#define RANDOMUNCATEGORIZEDBULLSHIT_H

void* SYS_AllocArena2MemLo(u32 size,u32 align);

#define nullcheck(x) if(x == NULL) { printf("%s is NULL!!", #x); while(1){} }

void bs_genMtxArrForTheTree(float* vertpos, uint8_t* mtxArr, int tricount) {//mtxarr is 1/3 the length of vertpos
	float centerrad = 1.0f;
	float middlecut = 5.5f;

	for(int i = 0; i < tricount * 3; i++) { //once per vertex
		mtxArr[i] = 0;
		//write this again kill yourself
		if (vertpos[(i*3)+1] >= middlecut) {
			if (vertpos[(i*3)] >= 0) {
				if (vertpos[(i*3)+2] >= 0) {
					//Top right front
					mtxArr[i] = 2;
				} else {
					//Top right back
					mtxArr[i] = 3;
				}
			} else {
				if (vertpos[(i*3)+2] >= 0) {
					//Top left front
					mtxArr[i] = 1;
				} else {
					//Top left back
					mtxArr[i] = 4;
				}
			}
		} else {
			if (vertpos[(i*3)] >= 0) {
				if (vertpos[(i*3)+2] >= 0) {
					//Bottom right front
					mtxArr[i] = 6;
				} else {
					//Bottom right back
					mtxArr[i] = 7;
				}
			} else {
				if (vertpos[(i*3)+2] >= 0) {
					//Bottom left front
					mtxArr[i] = 5;
				} else {
					//Bottom left back
					mtxArr[i] = 8;
				}
			}
		}
		if ( sqrt( fsq(0 - vertpos[(i*3)]) + fsq(0 - vertpos[(i*3)+2]) ) < centerrad ) { //what
			mtxArr[i] = 0;
		}	
	}	
}

void bs_loadTreeMatricies(Mtx view, float x, float y, float z, int frame) {
	Mtx treemtx[9];
	Mtx treemodelview[9];
	Mtx treemvi[9];
	for(int i = 0; i < 9; i++) {
		guMtxIdentity(treemtx[i]);
		guMtxTransApply(treemtx[i], treemtx[i], x, y, z);
	}
	float wavespeed = 0.01f;
	for(int i = 1; i < 9; i++) {
		guMtxTransApply(treemtx[i], treemtx[i], 0.0f, (cosf(   ( ((float)frame * wavespeed) + (float)i * 0.5 )    ) * 0.05), 0.0f);
		Mtx rotmtx;
		guMtxRotAxisDeg(rotmtx, &yAxis, (sinf(   ( ((float)frame * wavespeed) + (float)i * 0.5 )    ) * 2.0f));
		guMtxConcat(treemtx[i], rotmtx, treemtx[i]);
	}
	for(int i = 0; i < 9; i++) {
		guMtxConcat(view, treemtx[i], treemodelview[i]);
	}
	GX_LoadPosMtxImm(treemodelview[0], GX_PNMTX0);
	GX_LoadPosMtxImm(treemodelview[1], GX_PNMTX1);
	GX_LoadPosMtxImm(treemodelview[2], GX_PNMTX2);
	GX_LoadPosMtxImm(treemodelview[3], GX_PNMTX3);
	GX_LoadPosMtxImm(treemodelview[4], GX_PNMTX4);
	GX_LoadPosMtxImm(treemodelview[5], GX_PNMTX5);
	GX_LoadPosMtxImm(treemodelview[6], GX_PNMTX6);
	GX_LoadPosMtxImm(treemodelview[7], GX_PNMTX7);
	GX_LoadPosMtxImm(treemodelview[8], GX_PNMTX8);
	for(int i = 0; i < 9; i++) {
		guMtxInverse(treemodelview[i], treemvi[i]);
		guMtxTranspose(treemvi[i], treemodelview[i]);
	}
	GX_LoadNrmMtxImm(treemodelview[0], GX_PNMTX0);
	GX_LoadNrmMtxImm(treemodelview[1], GX_PNMTX1);
	GX_LoadNrmMtxImm(treemodelview[2], GX_PNMTX2);
	GX_LoadNrmMtxImm(treemodelview[3], GX_PNMTX3);
	GX_LoadNrmMtxImm(treemodelview[4], GX_PNMTX4);
	GX_LoadNrmMtxImm(treemodelview[5], GX_PNMTX5);
	GX_LoadNrmMtxImm(treemodelview[6], GX_PNMTX6);
	GX_LoadNrmMtxImm(treemodelview[7], GX_PNMTX7);
	GX_LoadNrmMtxImm(treemodelview[8], GX_PNMTX8);
}

f32 testcamDataAahAah[] = {
1.000000f, -0.000000f, 0.000000f, -0.020120f,
-0.000000f, -0.000000f, 1.000000f, -0.842734f,
0.000000f, -1.000000f, -0.000000f, -4.118052f
};

void setTestCam(Mtx view) {
	view[0][0] = testcamDataAahAah[0];
	view[0][1] = testcamDataAahAah[1];
	view[0][2] = testcamDataAahAah[2];
	view[0][3] = testcamDataAahAah[3];
	view[1][0] = testcamDataAahAah[4];
	view[1][1] = testcamDataAahAah[5];
	view[1][2] = testcamDataAahAah[6];
	view[1][3] = testcamDataAahAah[7];
	view[2][0] = testcamDataAahAah[8];
	view[2][1] = testcamDataAahAah[9];
	view[2][2] = testcamDataAahAah[10];
	view[2][3] = testcamDataAahAah[11];
	return;
}

#include "randomuncategorizeddata/load.h"

void drawLoad() {
	GX_SetCopyClear(LC_BLACK, 0x00ffffff);

	Mtx model, modelview;

	GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
	GX_LoadProjectionMtx(overlayPersp, GX_PERSPECTIVE);
	liy_VtxDescConfig(LV_VP | LV_VC, GX_VTXFMT0);
	GX_SetNumTevStages(1);	
	nolight();
	GX_SetChanAmbColor(GX_COLOR0A0,LC_WHITE);
	GX_SetTevOp(GX_TEVSTAGE0,GX_PASSCLR);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ONE, GX_CC_ONE, GX_CC_ONE, GX_CC_ONE); //too tired to do this in a civil way
	guMtxIdentity(model);
	guMtxTransApply(model, model, 0.0f, 0.0f, -5.0f);
	guMtxConcat(overlayView, model, modelview);
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);
	GX_SetCurrentMtx(GX_PNMTX0);


	drawArrDirectPC_UniformCol(loadTriData, LOADTRICOUNT, &LC_WHITE, GX_VTXFMT0);
		

	GX_SetCopyClear(background, 0x00ffffff);

	GX_CopyDisp(frameBuffer,GX_TRUE);

	GX_DrawDone();

	VIDEO_SetNextFramebuffer(frameBuffer);
	VIDEO_Flush();
	VIDEO_WaitVSync();

	fb ^= 1;
}

int liy_bufPos(int x, int y, int bufwidth) {
	return x+(y*bufwidth);
}

/*void alphabeltCalcRightbound() {
	for (int i = 0; i < NUM_LETTERS; i++) {
		for (int j = 0; j < alphabetTricounts[i] * 3; j++) {
			alphabetRightbounds[i] = liym_max(alphabetRightbounds[i], alphabetVertpos[i][(j*3)]);
		}
		printf("%f\r", alphabetRightbounds[i]);
	}
	alphabetRightbounds[29] = 0.25f; // space width
}*/

int alphTableIndexFromChar(char c) {
	switch(c) {
		case 'a':
			return 0;
		case 'b':
			return 1;
		case 'c':
			return 2;
		case 'd':
			return 3;
		case 'e':
			return 4;
		case 'f':
			return 5;
		case 'g':
			return 6;
		case 'h':
			return 7;
		case 'i':
			return 8;
		case 'j':
			return 9;
		case 'k':
			return 10;
		case 'l':
			return 11;
		case 'm':
			return 12;
		case 'n':
			return 13;
		case 'o':
			return 14;
		case 'p':
			return 15;
		case 'q':
			return 16;
		case 'r':
			return 17;
		case 's':
			return 18;
		case 't':
			return 19;
		case 'u':
			return 20;
		case 'v':
			return 21;
		case 'w':
			return 22;
		case 'x':
			return 23;
		case 'y':
			return 24;
		case 'z':
			return 25;
		case '(':
			return 26;
		case ')':
			return 27;
		case '?':
			return 28;
		case ' ':
			return 29;
		case '\'':
			return 30;
		case ',':
			return 31;
		case '!':
			return 32;
		case '.':
			return 33;
		default:
			return 28;
	
	}
}

int basicStringsFrame;

void basicRenderString(char *letters) {
	/*Mtx model, modelview;
	GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
	GX_SetCurrentMtx(GX_PNMTX0);

	GX_LoadProjectionMtx(overlayPersp, GX_PERSPECTIVE);

	liy_VtxDescConfig(LV_VP | LV_VC, GX_VTXFMT0);
		
	GX_SetNumTevStages(1);	
	nolight();

	GX_SetTevOp(GX_TEVSTAGE0,GX_PASSCLR);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);

	GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);	

	float overscanPadding = 0.0f;
	float lineSpacing = 0.7f;
	float charSpacing = 0.01f;

	float hozpoz = (widescreen) ? -8.0f + overscanPadding : -6.0f + overscanPadding;

	for(int i = 0; i < strlen(letters); i++) {
		int atablei = alphTableIndexFromChar(letters[i]);
		if(i != 0){hozpoz += alphabetRightbounds[alphTableIndexFromChar(letters[i - 1])] + charSpacing;}

		guMtxIdentity(model);
		guMtxTransApply(model, model, hozpoz, 4.0f - (((f32)basicStringsFrame * lineSpacing) + overscanPadding)  ,-10.0f);
		//guMtxTransApply(model, model, ((f32)i * charSpacing) - lorigin, 4.0f - ((f32)basicStringsFrame * lineSpacing)  ,-10.0f);
		guMtxConcat(overlayView,model,modelview);
		GX_LoadPosMtxImm(modelview, GX_PNMTX0);
		GX_LoadNrmMtxImm(modelview, GX_PNMTX0);

		drawArrDirectPC_UniformCol(alphabetVertpos[atablei], alphabetTricounts[atablei], &LC_BLACK, GX_VTXFMT0);
	}
	basicStringsFrame++;*/
}

//liy button
   #define LIYB_UP 0b10000000
#define LIYB_RIGHT 0b01000000
 #define LIYB_DOWN 0b00100000
 #define LIYB_LEFT 0b00010000
    #define LIYB_A 0b00001000
    #define LIYB_B 0b00000100
    #define LIYB_1 0b00000010
    #define LIYB_2 0b00000001

void dbgRefresh() {
	GX_SetCopyClear(background, 0x00ffffff);

	GX_CopyDisp(frameBuffer,GX_TRUE);

	GX_DrawDone();

	VIDEO_SetBlack(false);

	VIDEO_SetNextFramebuffer(frameBuffer);
	VIDEO_Flush();
	VIDEO_WaitVSync();

	fb ^= 1;
}

void liyrub_drawScreenquad(f32 scale) {
	liy_VtxDescConfig(LV_VP | LV_TC, 0);
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-1.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(0.0f, scale);
		GX_Position3f32(1.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(scale, scale);
		GX_Position3f32(1.0f, 1.0f, 0.0f);
		GX_TexCoord2f32(scale, 0.0f);
		GX_Position3f32(-1.0f, 1.0f, 0.0f);
		GX_TexCoord2f32(0.0f, 0.0f);
}

void liyrub_drawRotScreenquad(f32 scale) {
	liy_VtxDescConfig(LV_VP | LV_TC, 0);
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-1.0f, -1.0f, 0.0f);

		GX_TexCoord2f32(scale, 1.0f);

		GX_Position3f32(1.0f, -1.0f, 0.0f);

		GX_TexCoord2f32(scale, 1.0f - scale);

		GX_Position3f32(1.0f, 1.0f, 0.0f);

		GX_TexCoord2f32(0.0f, 1.0f - scale);

		GX_Position3f32(-1.0f, 1.0f, 0.0f);

		GX_TexCoord2f32(0.0f, 1.0f);
}

void seeds() {
	float tcscal = 0.25f;
	float scale = 0.5f;
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-scale, -scale, 0.25f);
		GX_TexCoord2f32(0.0f, tcscal);
		GX_Position3f32(scale, -scale, 0.25f);
		GX_TexCoord2f32(tcscal, tcscal);
		GX_Position3f32(scale, scale, 0.25f);
		GX_TexCoord2f32(tcscal, 0.0f);
		GX_Position3f32(-scale, scale, 0.25f);
		GX_TexCoord2f32(0.0f, 0.0f);
}

unsigned short dumbrandseed = 0;
u8 * dumbrandvals;
void initdumbrand() {
	dumbrandvals = malloc(65536);
	for(int i = 0; i < 65535; i++) {
		dumbrandvals[i] = rand();
	}
}
unsigned char dumbrand() {
	return dumbrandvals[dumbrandseed++];
}

void stylizei4(char * in, char * out, short width, short height) {
	width /= 2;
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			char rnd = dumbrand();
			int pos = (i*width)+j;
			out[pos] = in[pos];
			out[pos] &= rnd | 0x33;
		}
	}
}

void greyi4(char * in, char * out, short width, short height) {
	width /= 2;
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			int pos = (i*width)+j;
			out[pos] = in[pos] & 0xCC;
		}
	}
}

char singlequadralizer(char twobits) {
	twobits &= 3;
	switch(twobits) {
		case 0:
			return 0;
		case 1:
			return 0x0F;
		case 2:
			return 0xF0;
		case 3:
			return 0xFF;
		default:
			return 0;
	}
}

void quadralizer(char * in, char * out, int length) {
	int length4 = length / 4;
	for(int i = 0; i < length4; i++) {
		int i4 = i * 4;

		out[i4] = singlequadralizer(in[i] >> 6);
		out[i4+1] = singlequadralizer(in[i] >> 4);
		out[i4+2] = singlequadralizer(in[i] >> 2);
		out[i4+3] = singlequadralizer(in[i]);
	}
}

//Various decompression macro

#define modelIndexedToRaw(outpos, outnor, inpos, inposidx, innor, innoridx, tricount) \
for(int i = 0; i < tricount * 3; i++) { \
	int i3 = i * 3;	\
	outpos[i3  ] = inpos[ inposidx[i] * 3   ]; \
	outpos[i3+1] = inpos[(inposidx[i] * 3)+1]; \
	outpos[i3+2] = inpos[(inposidx[i] * 3)+2]; \
	outnor[i3  ] = innor[ innoridx[i] * 3   ]; \
	outnor[i3+1] = innor[(innoridx[i] * 3)+1]; \
	outnor[i3+2] = innor[(innoridx[i] * 3)+2]; \
}

#define modelIndexedToRawPos(outpos, inpos, inposidx, tricount) \
for(int i = 0; i < tricount * 3; i++) { \
	int i3 = i * 3;	\
	outpos[i3  ] = inpos[ inposidx[i] * 3   ]; \
	outpos[i3+1] = inpos[(inposidx[i] * 3)+1]; \
	outpos[i3+2] = inpos[(inposidx[i] * 3)+2]; \
}

#define liy_unpack_liyabc(title, destlen) \
u8 * title##Uncompressedbytes = malloc(destlen); \
nullcheck(title##Uncompressedbytes); \
unsigned long title##Destlen = destlen; \
unsigned long title##Sourcelen = (unsigned long)title##_liyabc_size; \
u16 title##Primcount, title##Framecount; \
int title##puffret = puff( \
	title##Uncompressedbytes, &title##Destlen, \
	title##_liyabc, &title##Sourcelen \
); \
if(title##puffret == 0) { \
	memcpy(&title##Primcount, &title##Uncompressedbytes[0], 2); \
	memcpy(&title##Framecount, &title##Uncompressedbytes[2], 2); \
} \
float * title##Data = (float *)&title##Uncompressedbytes[4]; \
nullcheck(title##Data); \
//printf("unpack animation: puff returned %d, %d prims, %d frames. %s\n", title##puffret, title##Primcount, title##Framecount, #title);


#define liy_unpackTPL(tex) \
TPL_OpenTPLFromMemory(&tplfileFromMem, (void *)tex##_tpl, tex##_tpl_size);\
TPL_GetTexture(&tplfileFromMem, tex, &tex##TexObj); \
TPL_CloseTPLFile(&tplfileFromMem);

#define liy_unpackQOI_RGBA8(tex) \
qoiParse = qoi_decode(tex##_qoi, tex##_qoi_size, &qoiParseDesc, 4); \
if(qoiParse == NULL) {printf("qoi error?\n"); exit(0);} \
void *tex##Data = memalign(32, qoiParseDesc.width*qoiParseDesc.height*4); \
nullcheck(tex##Data); \
liy_ConvertRGBA8(qoiParse, tex##Data, qoiParseDesc.width, qoiParseDesc.height); \
DCFlushRange(tex##Data, qoiParseDesc.width*qoiParseDesc.height*4); \
GX_InitTexObj(&tex##TexObj, tex##Data, qoiParseDesc.width, qoiParseDesc.height, GX_TF_RGBA8, GX_REPEAT, GX_REPEAT, GX_FALSE); \
free(qoiParse);

#define liy_unpackQOI_RGBA8_mem2(tex) \
qoiParse = qoi_decode(tex##_qoi, tex##_qoi_size, &qoiParseDesc, 4); \
if(qoiParse == NULL) {printf("qoi error?\n"); exit(0);} \
void *tex##Data = malloc(qoiParseDesc.width*qoiParseDesc.height*4); \
liy_ConvertRGBA8(qoiParse, tex##Data, qoiParseDesc.width, qoiParseDesc.height); \
memcpy(mem2texarena, tex##Data, qoiParseDesc.width*qoiParseDesc.height*4); \
uint tex##index = mem2texarenaindex; \
GX_InitTexObj(&tex##TexObj, mem2texarena + tex##index, qoiParseDesc.width, qoiParseDesc.height, GX_TF_RGBA8, GX_REPEAT, GX_REPEAT, GX_FALSE); \
mem2texarenaindex += qoiParseDesc.width*qoiParseDesc.height*4; \
free(qoiParse); \
free(tex##Data);

#define liy_unpackQOI_RGB565(tex) \
qoiParse = qoi_decode(tex##_qoi, tex##_qoi_size, &qoiParseDesc, 4); \
if(qoiParse == NULL) {printf("qoi error?\n"); exit(0);} \
void *tex##Data = memalign(32, qoiParseDesc.width*qoiParseDesc.height*2); \
liy_ConvertRGB565(qoiParse, tex##Data, qoiParseDesc.width, qoiParseDesc.height); \
GX_InitTexObj(&tex##TexObj, tex##Data, qoiParseDesc.width, qoiParseDesc.height, GX_TF_RGB565, GX_REPEAT, GX_REPEAT, GX_FALSE); \
free(qoiParse);

#define liy_unpackQOI_RGB5A3(tex) \
qoiParse = qoi_decode(tex##_qoi, tex##_qoi_size, &qoiParseDesc, 4); \
if(qoiParse == NULL) {printf("qoi error?\n"); exit(0);} \
void *tex##Data = memalign(32, qoiParseDesc.width*qoiParseDesc.height*2); \
liy_ConvertRGB5A3(qoiParse, tex##Data, qoiParseDesc.width, qoiParseDesc.height); \
GX_InitTexObj(&tex##TexObj, tex##Data, qoiParseDesc.width, qoiParseDesc.height, GX_TF_RGB5A3, GX_REPEAT, GX_REPEAT, GX_FALSE); \
free(qoiParse);


//uncompressed data is 8 pixels per byte
//and need 2 convert it to 2 pixels per byte

#define liy_unpackDoodle(tex, xres, yres) \
unsigned long tex##Destlen = (xres*yres)/8; \
unsigned long tex##Sourcelen = (unsigned long)tex##_doodle_size; \
u8 * tex##Uncompressedbytes = malloc(tex##Destlen); \
nullcheck(tex##Uncompressedbytes); \
s8 tex##puffret = puff( \
	tex##Uncompressedbytes, &tex##Destlen, \
	tex##_doodle, &tex##Sourcelen \
); \
u8 * tex##Quadruple; \
u8 * tex##QuadrupleConv; \
unsigned long tex##Quadlen = tex##Destlen * 4; \
if(tex##puffret == 0) { \
	tex##Quadruple = malloc(tex##Quadlen); \
	nullcheck(tex##Quadruple); \
	memset(tex##Quadruple, 0, tex##Quadlen); \
	quadralizer(tex##Uncompressedbytes, tex##Quadruple, tex##Quadlen); \
	free(tex##Uncompressedbytes); \
	tex##QuadrupleConv = memalign(32, tex##Quadlen); \
	nullcheck(tex##QuadrupleConv); \
	liy_ConvertI4(tex##Quadruple, tex##QuadrupleConv, xres, yres); \
	stylizei4(tex##QuadrupleConv, tex##Quadruple, xres, yres); \
	memcpy(tex##QuadrupleConv, tex##Quadruple, tex##Quadlen); \
	DCFlushRange(tex##QuadrupleConv, tex##Quadlen); \
	free(tex##Quadruple); \
	GX_InitTexObj(&tex##TexObj, tex##QuadrupleConv, xres, yres, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE); \
} \
//printf("unpackDoodle: puff returned %d. %s\n", tex##puffret, #tex);


#define liy_unpackDoodlegrey(tex, xres, yres) \
unsigned long tex##Destlen = (xres*yres)/8; \
unsigned long tex##Sourcelen = (unsigned long)tex##_doodle_size; \
u8 * tex##Uncompressedbytes = malloc(tex##Destlen); \
nullcheck(tex##Uncompressedbytes); \
s8 tex##puffret = puff( \
	tex##Uncompressedbytes, &tex##Destlen, \
	tex##_doodle, &tex##Sourcelen \
); \
u8 * tex##Quadruple; \
u8 * tex##QuadrupleConv; \
unsigned long tex##Quadlen = tex##Destlen * 4; \
if(tex##puffret == 0) { \
	tex##Quadruple = malloc(tex##Quadlen); \
	nullcheck(tex##Quadruple); \
	memset(tex##Quadruple, 0, tex##Quadlen); \
	quadralizer(tex##Uncompressedbytes, tex##Quadruple, tex##Quadlen); \
	free(tex##Uncompressedbytes); \
	tex##QuadrupleConv = memalign(32, tex##Quadlen); \
	nullcheck(tex##QuadrupleConv); \
	liy_ConvertI4(tex##Quadruple, tex##QuadrupleConv, xres, yres); \
	DCFlushRange(tex##QuadrupleConv, tex##Quadlen); \
	free(tex##Quadruple); \
	GX_InitTexObj(&tex##TexObj, tex##QuadrupleConv, xres, yres, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE); \
} \
//printf("unpackDoodle: puff returned %d. %s\n", tex##puffret, #tex);


#ifndef QOI_NO_STDIO
void dumpXfbToSD(u8 * xfb) {
	u8 * rgbbuf = malloc(640*480*3);
	for (int y = 0; y < 480; y++) {
		for (int x = 0; x < 320; x++) {
		int i = (y*320)+x;
		int i6 = i * 3;
		u8 *ycbcr = (u8*)xfb + (i*4);
		u8 Y1 = ycbcr[0];
		u8 Cb = ycbcr[1];
		u8 Y2 = ycbcr[2];
		u8 Cr = ycbcr[3]; 

		#define CLAMP(value, min_val, max_val) \
   			 ((value) < (min_val) ? (min_val) : ((value) > (max_val) ? (max_val) : (value)))

		u16 r1 = round( CLAMP((float)Y1 + 1.371f*((float)Cr - 128.0f), 0.1f, 254.9f) );
		u16 g1 = round( CLAMP((float)Y1 - 0.698f*((float)Cr - 128.0f) - 0.336f*((float)Cb - 128.0f), 0.1f, 254.9f) );
		u16 b1 = round( CLAMP((float)Y1 + 1.732f*((float)Cb - 128.0f), 0.1f, 254.9f) );

		u16 r2 = round( CLAMP((float)Y2 + 1.371f*((float)Cr - 128.0f), 0.1f, 254.9f) );
		u16 g2 = round( CLAMP((float)Y2 - 0.698f*((float)Cr - 128.0f) - 0.336f*((float)Cb - 128.0f), 0.1f, 254.9f) );
		u16 b2 = round( CLAMP((float)Y2 + 1.732f*((float)Cb - 128.0f), 0.1f, 254.9f) );
			
		u32 pos = ((y*320)+x);
		pos *= 6;

		rgbbuf[pos  ] = r1;
		rgbbuf[pos+1] = g1;
		rgbbuf[pos+2] = b1;

		rgbbuf[pos+3] = r2;
		rgbbuf[pos+4] = g2;
		rgbbuf[pos+5] = b2;
			
		}
    	}
	qoi_write("fb.qoi", rgbbuf, &(qoi_desc){
	.width = 640,
	.height = 480,
	.channels = 3,
	.colorspace = QOI_SRGB
	});
}
#endif

void genrimtex(unsigned char * buf, unsigned char strength) {
	if(!buf) return;
	for(int i = 0; i < 128; i++) {
		float yf = (float)i/128.0f;
		yf *= -2.0f;
		yf += 1.0f;
		for(int j = 0; j < 128; j++) {
			float xf = (float)j/128.0f;
			xf *= 2.0f;
			xf -= 1.0f;
			float dist = (xf*xf)+(yf*yf);
			dist = (dist > 1.0f) ? 1.0f : dist;
			for(int k = 0; k < strength; k++) {
				dist *= dist;
			}
			buf[(i*128)+j] = (char)(dist*255.0f);
		}
	}
}

int getcrazytime() {
	return rand();
}

#endif