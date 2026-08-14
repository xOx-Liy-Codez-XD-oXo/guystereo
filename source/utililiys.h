#ifndef UTILILIYS_H
#define UTILILIYS_H

void * LilyCoolMalloc(uint size) {
	int lala = (int)malloc(size + 31);
	//lala += lala % 32;
	lala += 31;
	lala &= ~31;
	//printf("Lily cool malloc to the rescue!!! %d bytes at 0x%x\n", size, lala); 
	return (void *)lala;
}

#undef memalign
#define memalign(x, y) LilyCoolMalloc(y)

#include "colors.h"

#define pi 3.141592653589793

float fsq(float x) {
	return x * x;
}

#define printMtx(mtx) \
	printf("Mtx\r");\
	printf("%f, %f, %f\r", mtx[0][0], mtx[1][0], mtx[2][0]);\
	printf("%f, %f, %f\r", mtx[0][1], mtx[1][1], mtx[2][1]);\
	printf("%f, %f, %f\r", mtx[0][2], mtx[1][2], mtx[2][2]);\
	printf("%f, %f, %f\r", mtx[0][3], mtx[1][3], mtx[2][3]);\


#define printMtx44(mtx) \
	printf("Mtx44\r");\
	printf("%f, %f, %f, %f\r", mtx[0][0], mtx[1][0], mtx[2][0], mtx[3][0]);\
	printf("%f, %f, %f, %f\r", mtx[0][1], mtx[1][1], mtx[2][1], mtx[3][1]);\
	printf("%f, %f, %f, %f\r", mtx[0][2], mtx[1][2], mtx[2][2], mtx[3][2]);\
	printf("%f, %f, %f, %f\r", mtx[0][2], mtx[1][3], mtx[2][3], mtx[3][3]);\

static guVector xAxis = {1,0,0};
static guVector yAxis = {0,1,0};
static guVector zAxis = {0,0,1};

//liy vert
#define LV_VP 1		 //0000 0000  0000 0001
#define LV_VN 2		 //0000 0000  0000 0010
#define LV_TC 4		 //0000 0000  0000 0100
#define LV_VC 8		 //0000 0000  0000 1000
#define LV_MX 16		//0000 0000  0001 0000
#define LV_VPIDX8 32	//0000 0000  0010 0000
#define LV_VPIDX16 64   //0000 0000  0100 0000
#define LV_VNIDX8 128   //0000 0000  1000 0000
#define LV_VNIDX16 256  //0000 0001  0000 0000
#define LV_TCIDX8  512  //0000 0010  0000 0000
#define LV_TCIDX16 1024 //0000 0100  0000 0000
#define LV_TC1	 2048 //0000 1000  0000 0000

void liy_VtxDescConfig(uint16_t config, int vtxfmt) {
	GX_ClearVtxDesc();
	if(config & LV_MX) { GX_SetVtxDesc(GX_VA_PTNMTXIDX, GX_DIRECT); }
	if(config & LV_VP) { GX_SetVtxDesc(GX_VA_POS, GX_DIRECT); }
	if(config & LV_VPIDX8) { GX_SetVtxDesc(GX_VA_POS, GX_INDEX8); }
	if(config & LV_VPIDX16) { GX_SetVtxDesc(GX_VA_POS, GX_INDEX16); }
	if(config & LV_VN) { GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT); }
	if(config & LV_VNIDX8) { GX_SetVtxDesc(GX_VA_NRM, GX_INDEX8); }
	if(config & LV_VNIDX16) { GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16); }
	if(config & LV_VC) { GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT); }
	if(config & LV_TC) { GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT); }
	if(config & LV_TC1){ GX_SetVtxDesc(GX_VA_TEX1, GX_DIRECT); }
	if(config & LV_TCIDX8) { GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8); }
	if(config & LV_TCIDX16) { GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16); }
	//attrfmts
	if(config & LV_MX) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_PTNMTXIDX, GX_INDEX8, GX_U8, 0); }
	if(config & LV_VP) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_POS, GX_POS_XYZ, GX_F32, 0); }
	if(config & LV_VPIDX8) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_POS, GX_POS_XYZ, GX_F32, 0); }
	if(config & LV_VPIDX16) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_POS, GX_POS_XYZ, GX_F32, 0); }
	if(config & LV_VN) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0); }
	if(config & LV_VNIDX8) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0); }
	if(config & LV_VNIDX16) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0); }
	if(config & LV_VC) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0); }
	if(config & LV_TC) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TC1){ GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX8) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX16) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0); }
}

void liy_VtxDescConfigMultitex(uint16_t config, uint8_t texcount, int vtxfmt) {
	GX_ClearVtxDesc();
	if(config & LV_MX) { GX_SetVtxDesc(GX_VA_PTNMTXIDX, GX_DIRECT); }
	if(config & LV_VP) { GX_SetVtxDesc(GX_VA_POS, GX_DIRECT); }
	if(config & LV_VPIDX8) { GX_SetVtxDesc(GX_VA_POS, GX_INDEX8); }
	if(config & LV_VPIDX16) { GX_SetVtxDesc(GX_VA_POS, GX_INDEX16); }
	if(config & LV_VN) { GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT); }
	if(config & LV_VNIDX8) { GX_SetVtxDesc(GX_VA_NRM, GX_INDEX8); }
	if(config & LV_VNIDX16) { GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16); }
	if(config & LV_VC) { GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT); }

	if(config & LV_TC) { GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT); }
	if(config & LV_TC && texcount >= 2) { GX_SetVtxDesc(GX_VA_TEX1, GX_DIRECT); }
	if(config & LV_TC && texcount >= 3) { GX_SetVtxDesc(GX_VA_TEX2, GX_DIRECT); }
	if(config & LV_TC && texcount >= 4) { GX_SetVtxDesc(GX_VA_TEX3, GX_DIRECT); }
	if(config & LV_TC && texcount >= 5) { GX_SetVtxDesc(GX_VA_TEX4, GX_DIRECT); }
	if(config & LV_TC && texcount >= 6) { GX_SetVtxDesc(GX_VA_TEX5, GX_DIRECT); }
	if(config & LV_TC && texcount >= 7) { GX_SetVtxDesc(GX_VA_TEX6, GX_DIRECT); }
	if(config & LV_TC && texcount >= 8) { GX_SetVtxDesc(GX_VA_TEX7, GX_DIRECT); }

	if(config & LV_TCIDX8) { GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8); }
	if(config & LV_TCIDX8 && texcount >= 2) { GX_SetVtxDesc(GX_VA_TEX1, GX_INDEX8); }
	if(config & LV_TCIDX8 && texcount >= 3) { GX_SetVtxDesc(GX_VA_TEX2, GX_INDEX8); }
	if(config & LV_TCIDX8 && texcount >= 4) { GX_SetVtxDesc(GX_VA_TEX3, GX_INDEX8); }
	if(config & LV_TCIDX8 && texcount >= 5) { GX_SetVtxDesc(GX_VA_TEX4, GX_INDEX8); }
	if(config & LV_TCIDX8 && texcount >= 6) { GX_SetVtxDesc(GX_VA_TEX5, GX_INDEX8); }
	if(config & LV_TCIDX8 && texcount >= 7) { GX_SetVtxDesc(GX_VA_TEX6, GX_INDEX8); }
	if(config & LV_TCIDX8 && texcount >= 8) { GX_SetVtxDesc(GX_VA_TEX7, GX_INDEX8); }
	
	if(config & LV_TCIDX16) { GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16); }
	if(config & LV_TCIDX16 && texcount >= 2) { GX_SetVtxDesc(GX_VA_TEX1, GX_INDEX16); }
	if(config & LV_TCIDX16 && texcount >= 3) { GX_SetVtxDesc(GX_VA_TEX2, GX_INDEX16); }
	if(config & LV_TCIDX16 && texcount >= 4) { GX_SetVtxDesc(GX_VA_TEX3, GX_INDEX16); }
	if(config & LV_TCIDX16 && texcount >= 5) { GX_SetVtxDesc(GX_VA_TEX4, GX_INDEX16); }
	if(config & LV_TCIDX16 && texcount >= 6) { GX_SetVtxDesc(GX_VA_TEX5, GX_INDEX16); }
	if(config & LV_TCIDX16 && texcount >= 7) { GX_SetVtxDesc(GX_VA_TEX6, GX_INDEX16); }
	if(config & LV_TCIDX16 && texcount >= 8) { GX_SetVtxDesc(GX_VA_TEX7, GX_INDEX16); }
	
	//attrfmts
	if(config & LV_MX) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_PTNMTXIDX, GX_INDEX8, GX_U8, 0); }
	if(config & LV_VP) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_POS, GX_POS_XYZ, GX_F32, 0); }
	if(config & LV_VPIDX8) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_POS, GX_POS_XYZ, GX_F32, 0); }
	if(config & LV_VPIDX16) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_POS, GX_POS_XYZ, GX_F32, 0); }
	if(config & LV_VN) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0); }
	if(config & LV_VNIDX8) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0); }
	if(config & LV_VNIDX16) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0); }
	if(config & LV_VC) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0); }

	if(config & LV_TC) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TC && texcount >= 2) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TC && texcount >= 3) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX2, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TC && texcount >= 4) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX3, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TC && texcount >= 5) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX4, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TC && texcount >= 6) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX5, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TC && texcount >= 7) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX6, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TC && texcount >= 8) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX7, GX_TEX_ST, GX_F32, 0); }
	
	if(config & LV_TCIDX8) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX8 && texcount >= 2) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX8 && texcount >= 3) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX2, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX8 && texcount >= 4) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX3, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX8 && texcount >= 5) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX4, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX8 && texcount >= 6) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX5, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX8 && texcount >= 7) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX6, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX8 && texcount >= 8) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX7, GX_TEX_ST, GX_F32, 0); }
	
	if(config & LV_TCIDX16) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX16 && texcount >= 2) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX16 && texcount >= 3) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX2, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX16 && texcount >= 4) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX3, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX16 && texcount >= 5) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX4, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX16 && texcount >= 6) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX5, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX16 && texcount >= 7) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX6, GX_TEX_ST, GX_F32, 0); }
	if(config & LV_TCIDX16 && texcount >= 8) { GX_SetVtxAttrFmt(vtxfmt, GX_VA_TEX7, GX_TEX_ST, GX_F32, 0); }
}

void calcTableDelta(float *sources, float *differents, float *outs, int itemcount) {
	for(int i = 0; i < itemcount; i++) {
		outs[i] = differents[i] - sources[i];
	}
}

#define LIY_HASVERTPOS 0b00000001
#define LIY_HASFACENOR 0b00000010
#define LIY_HASVERTNOR 0b00000100
#define LIY_HASTEXCORD 0b00001000
#define LIY_HASVCINDEX 0b00010000
#define LIY_HASVERTCOL 0b00100000
#define LIY_HASMTXIDX 0b01000000

int calcSizeDisplaylistLegacy(uint8_t arrfeatures, int facecost, int tricount) {
	int cost = 3; //cost is the cost in bytes. GX_Begin() is 3 bytes

	if (arrfeatures & LIY_HASVERTPOS) { cost += facecost * 4 * tricount * 3; } //f32 is 4 big
	//we do not support sending face normal to gx
	if (arrfeatures & LIY_HASVERTNOR) { cost += facecost * 4 * tricount * 3; }
	if (arrfeatures & LIY_HASTEXCORD) { cost += facecost * 4 * tricount * 2; } //we send texcords in 2f32 instead
	if (arrfeatures & LIY_HASVCINDEX) { cost += facecost * 1 * tricount * 1; } //just 1 byte per vertex
	if (arrfeatures & LIY_HASVERTCOL) { cost += facecost * 1 * tricount * 3; } //3 bytes per vert
	if (arrfeatures & LIY_HASMTXIDX) { cost += facecost * 1 * tricount * 1; }	

	cost += 32; //throw in an extra 32. this gets shaven off later but things fail w/o this

	cost += 32 - (cost % 32); //round size up to nearest 32nd

	return cost;
}

int calcSizeDisplaylist(uint16_t arrfeatures, int facecost, int tricount) {
	int cost = 3; //cost is the cost in bytes. GX_Begin() is 3 bytes

	if (arrfeatures & LV_VP) { cost += facecost * 4 * tricount * 3; } //f32 is 4 big
	if (arrfeatures & LV_VN) { cost += facecost * 4 * tricount * 3; }
	if (arrfeatures & LV_TC) { cost += facecost * 4 * tricount * 2; } //we send texcords in 2f32 instead
	if (arrfeatures & LV_VPIDX8) { cost += facecost * 1 * tricount * 1; } //just 1 byte per vertex
	if (arrfeatures & LV_VNIDX8) { cost += facecost * 1 * tricount * 1; }
	if (arrfeatures & LV_TCIDX8) { cost += facecost * 1 * tricount * 1; }
	if (arrfeatures & LV_VPIDX16) { cost += facecost * 2 * tricount * 1; }
	if (arrfeatures & LV_VNIDX16) { cost += facecost * 2 * tricount * 1; }
	if (arrfeatures & LV_TCIDX16) { cost += facecost * 2 * tricount * 1; }
	if (arrfeatures & LV_VC) { cost += facecost * 1 * tricount * 3; } //uhh
	if (arrfeatures & LV_MX) { cost += facecost * 1 * tricount * 1; }	

	cost += 32; //extraaa

	cost += 32 - (cost % 32); //round size up to nearest 32nd

	return cost;
}

void liy_batchLoadMtx(Mtx *mtxarr, int *indexArr, uint16_t whichones) { //1023 for all of them
	int i = 0;
	if(whichones & 1)  { GX_LoadPosMtxImm(mtxarr[indexArr[i]], GX_PNMTX0); 
							 GX_LoadNrmMtxImm(mtxarr[indexArr[i]], GX_PNMTX0); i++;}
	if(whichones & 2)  { GX_LoadPosMtxImm(mtxarr[indexArr[i]], GX_PNMTX1); 
							 GX_LoadNrmMtxImm(mtxarr[indexArr[i]], GX_PNMTX1); i++;}
	if(whichones & 4)  { GX_LoadPosMtxImm(mtxarr[indexArr[i]], GX_PNMTX2); 
							 GX_LoadNrmMtxImm(mtxarr[indexArr[i]], GX_PNMTX2); i++;}
	if(whichones & 8)  { GX_LoadPosMtxImm(mtxarr[indexArr[i]], GX_PNMTX3); 
							 GX_LoadNrmMtxImm(mtxarr[indexArr[i]], GX_PNMTX3); i++;}
	if(whichones & 16) { GX_LoadPosMtxImm(mtxarr[indexArr[i]], GX_PNMTX4); 
							 GX_LoadNrmMtxImm(mtxarr[indexArr[i]], GX_PNMTX4); i++;}
	if(whichones & 32) { GX_LoadPosMtxImm(mtxarr[indexArr[i]], GX_PNMTX5); 
							 GX_LoadNrmMtxImm(mtxarr[indexArr[i]], GX_PNMTX5); i++;}
	if(whichones & 64) { GX_LoadPosMtxImm(mtxarr[indexArr[i]], GX_PNMTX6); 
							 GX_LoadNrmMtxImm(mtxarr[indexArr[i]], GX_PNMTX6); i++;}
	if(whichones & 128){ GX_LoadPosMtxImm(mtxarr[indexArr[i]], GX_PNMTX7); 
							 GX_LoadNrmMtxImm(mtxarr[indexArr[i]], GX_PNMTX7); i++;}
	if(whichones & 256){ GX_LoadPosMtxImm(mtxarr[indexArr[i]], GX_PNMTX8); 
							 GX_LoadNrmMtxImm(mtxarr[indexArr[i]], GX_PNMTX8); i++;}
	if(whichones & 512){ GX_LoadPosMtxImm(mtxarr[indexArr[i]], GX_PNMTX9); 
							 GX_LoadNrmMtxImm(mtxarr[indexArr[i]], GX_PNMTX9);}
}

void liy_batchLoadTexMtx(Mtx *mtxarr, int *indexArr, uint16_t whichones) { //just assuming 3x4
	int i = 0;
	if(whichones & 1)  GX_LoadTexMtxImm(mtxarr[indexArr[i]], GX_TEXMTX0, GX_MTX3x4); 
	if(whichones & 2)  GX_LoadTexMtxImm(mtxarr[indexArr[i]], GX_TEXMTX1, GX_MTX3x4); 
	if(whichones & 4)  GX_LoadTexMtxImm(mtxarr[indexArr[i]], GX_TEXMTX2, GX_MTX3x4); 
	if(whichones & 8)  GX_LoadTexMtxImm(mtxarr[indexArr[i]], GX_TEXMTX3, GX_MTX3x4); 
	if(whichones & 16) GX_LoadTexMtxImm(mtxarr[indexArr[i]], GX_TEXMTX4, GX_MTX3x4); 
	if(whichones & 32) GX_LoadTexMtxImm(mtxarr[indexArr[i]], GX_TEXMTX5, GX_MTX3x4); 
	if(whichones & 64) GX_LoadTexMtxImm(mtxarr[indexArr[i]], GX_TEXMTX6, GX_MTX3x4); 
	if(whichones & 128)GX_LoadTexMtxImm(mtxarr[indexArr[i]], GX_TEXMTX7, GX_MTX3x4); 
	if(whichones & 256)GX_LoadTexMtxImm(mtxarr[indexArr[i]], GX_TEXMTX8, GX_MTX3x4); 
	if(whichones & 512)GX_LoadTexMtxImm(mtxarr[indexArr[i]], GX_TEXMTX9, GX_MTX3x4); 
}

void LX_LoadMtxImm(Mtx mtx, int whichone) {
	GX_LoadPosMtxImm(mtx, whichone);
	GX_LoadNrmMtxImm(mtx, whichone);
}

void drawDial(uint8_t r, uint8_t g, uint8_t b) {
	GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
		GX_Position3f32(-0.1f, 0.0f, 0.0f);
		GX_Color3u8(r, g, b);
		GX_Position3f32( 0.1f, 0.0f, 0.0f);
		GX_Color3u8(r, g, b);
		GX_Position3f32( 0.0f, 1.0f, 0.0f);
		GX_Color3u8(r, g, b);
}

#define LM_0 1 //liy mtx
#define LM_1 2
#define LM_2 4
#define LM_3 8
#define LM_4 16
#define LM_5 32
#define LM_6 64
#define LM_7 128
#define LM_8 256
#define LM_9 512
#define LM_ALL 1023

//Texture conversion functions by Lameguy64 for mtek-gdl
//https://github.com/Lameguy64/mtek-gdl/blob/master/source/wii/mgdl-texture.cpp

void liy_ConvertRGBA8(void *inBuff, void *outBuff, short width, short height) {
	short stride = width;

	// Converts the raw image data into a RGBA8 texture (or RGBA32)

	uint8_t *src = (uint8_t*)inBuff;
	uint8_t *dst = (uint8_t*)outBuff;

	for(uint16_t block = 0; block < height; block += 4) {
		for(uint16_t i = 0; i < width; i += 4) {

			uint8_t *doffs=dst+(64*( (i/4)+((stride/4)*(block/4) )));

			for (uint32_t c = 0; c < 4; c++) {

				uint32_t blockWid = (((block+c)*width)+i)<<2;

				*doffs++ = src[blockWid + 3];  // ar = 0
				*doffs++ = src[blockWid + 0];
				*doffs++ = src[blockWid + 7];  // ar = 1
				*doffs++ = src[blockWid + 4];
				*doffs++ = src[blockWid + 11]; // ar = 2
				*doffs++ = src[blockWid + 8];
				*doffs++ = src[blockWid + 15]; // ar = 3
				*doffs++ = src[blockWid + 12];

			}

			for (uint32_t c = 0; c < 4; c++) {

				uint32_t blockWid = (((block+c)*width)+i)<<2;

				*doffs++ = src[blockWid + 1];  // gb = 0
				*doffs++ = src[blockWid + 2];
				*doffs++ = src[blockWid + 5];  // gb = 1
				*doffs++ = src[blockWid + 6];
				*doffs++ = src[blockWid + 9];  // gb = 2
				*doffs++ = src[blockWid + 10];
				*doffs++ = src[blockWid + 13]; // gb = 3
				*doffs++ = src[blockWid + 14];

			}

		}
	}

}

// Internal macros for texture conversion (from Metaphrasis)
#define RGBA_TO_RGB565(x) ((x & 0xf8000000) >> 16) | ((x & 0x00fc0000) >> 13) | ((x & 0x0000f800) >> 11)
#define RGBA_TO_RGB555(x) ((x & 0xf8000000) >> 17) | ((x & 0x00f80000) >> 14) | ((x & 0x0000f800) >> 11) | 0x8000


// Unlike the other conversion macros, I actually wrote this myself because the macro for RGB4A3
// conversion was bugged in Metaphrasis (stupid lazy coders) but at least mine is very readable
#define RGBA_TO_RGB4A3(x)	(((x & 0xff) / 32) << 12) | \
							((((x >> 24) & 0xff) / 16) << 8) | \
							((((x >> 16) & 0xff) / 16) << 4) | \
							((((x >> 8) & 0xff) / 16))

#define RGBA_TO_RGB5A3(x)	(x & 0xff) < 0xe0 ? RGBA_TO_RGB4A3(x) : RGBA_TO_RGB555(x)

static void liy_ConvertRGB5A3(void *inBuff, void *outBuff, short width, short height) {
	short stride = width;

	// Converts the raw image data into a RGB5A3 texture (RGB555 for opaque pixels, RGB4A3 for translucent)

	uint32_t *src = (uint32_t*)inBuff;
	uint16_t *dst = (uint16_t*)outBuff;

	for(uint16_t y=0; y<height; y+=4) {
		for(uint16_t x=0; x<width; x+=4) {

			uint16_t *doffs=dst+(16*((x/4)+((stride/4)*(y/4))));

			for(uint16_t rows = 0; rows < 4; rows++) {

				*doffs++ = RGBA_TO_RGB5A3(src[((y+rows)*width)+(x+0)]);
				*doffs++ = RGBA_TO_RGB5A3(src[((y+rows)*width)+(x+1)]);
				*doffs++ = RGBA_TO_RGB5A3(src[((y+rows)*width)+(x+2)]);
				*doffs++ = RGBA_TO_RGB5A3(src[((y+rows)*width)+(x+3)]);

			}

		}
	}

}

static void liy_ConvertRGB565(void *inBuff, void *outBuff, short width, short height) {
	short stride = width;

	// Convert the raw image data into a RGB565 texture (alpha channel will be discarded)

	uint32_t *src = (uint32_t*)inBuff;
	uint16_t *dst = (uint16_t*)outBuff;

	for(uint16_t y=0; y < height; y += 4) {
		for(uint16_t x = 0; x < width; x += 4) {
			uint16_t *doffs=dst+(16*( (x/4)+((stride/4)*(y/4) )));

			for(uint16_t rows = 0; rows < 4; rows++) {
				*doffs++ = RGBA_TO_RGB565(src[((y+rows)*width)+(x+0)]);
				*doffs++ = RGBA_TO_RGB565(src[((y+rows)*width)+(x+1)]);
				*doffs++ = RGBA_TO_RGB565(src[((y+rows)*width)+(x+2)]);
				*doffs++ = RGBA_TO_RGB565(src[((y+rows)*width)+(x+3)]);
			}
		}
	}
}

static void liy_ConvertI4(uint8_t *in, uint8_t *out, int w, int h) { //i dont remember where i copypasted this from
	int tilesX = w / 8;
	int tilesY = h / 8;
	int outpos = 0;

	for (int ty = 0; ty < tilesY; ty++) {
		for (int tx = 0; tx < tilesX; tx++) {
			for (int y = 0; y < 8; y++) {
				for (int x = 0; x < 8; x += 2) {
					int srcX0 = tx * 8 + x;
					int srcY0 = ty * 8 + y;
					int srcX1 = srcX0 + 1;

					int pixelIndex0 = srcY0 * w + srcX0;
					int pixelIndex1 = srcY0 * w + srcX1;

					uint8_t p0 = (in[pixelIndex0 >> 1] >> ((~pixelIndex0 & 1) * 4)) & 0xF;
					uint8_t p1 = (in[pixelIndex1 >> 1] >> ((~pixelIndex1 & 1) * 4)) & 0xF;

					out[outpos++] = (p0 << 4) | p1;
				}
			}
		}
	}

}

static void liy_ConvertI8(void *inBuff, void *outBuff, short width, short height) {
	short stride = width;
	for(short py=0; py<height; py++) {
		for(short px=0; px<width; px++) {
			int doffs = ((px%8)+(32*(px/8)))+((stride*4)*(py/4))+(8*(py%4));
			int soffs = px+(width*py);
			((u_char*)outBuff)[doffs] = ((u_char*)inBuff)[soffs];
		}
	}
}

#endif
