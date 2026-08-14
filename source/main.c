#define GUY_RELEASE
//#define GUY_DEV
#define GUY_CONCON

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <asndlib.h>
#include "ogc/lwp_watchdog.h"
#include <wiikeyboard/keyboard.h>
bool USBKeyboard_IsConnected(void);
#include "ogc/usbmouse.h"

#include <fat.h>

#define QOI_IMPLEMENTATION
//#define QOI_NO_STDIO
#include "qoi.h"

#include "liym/liym.h"
#include "utililiys.h"
#include "light.h"
#include "primitives.h"
//#include "loadings.h"
#include "liyt.h"
#include "puff.h"

#include "randomuncategorizeddata/gillMtxTables.h"
#include "randomuncategorizeddata/lakeMtxTables.h"
#include "randomuncategorizeddata/focuslut.h"

#include "data/allmodels.h"

#include "matcap.h"

int rframe = 1;

//files
#include "skybox_qoi.h"
#include "gilltex_tpl.h"
#include "gilltex.h"
//#include "laketex_tpl.h"
//#include "laketex.h"
#include "foliage1_qoi.h"
#include "hallway1_tpl.h"
#include "hallway1.h"
#include "hallwaytiles_tpl.h"
#include "hallwaytiles.h"
#include "building_qoi.h"
#include "chainlink_tpl.h"
#include "chainlink.h"
#include "rivershirt_qoi.h"
//#include "riverhair_tpl.h"
//#include "riverhair.h"
#include "bark_qoi.h"
#include "flutterbytex_qoi.h"
#include "leatherMatcap_qoi.h"
#include "firematte_tpl.h"
#include "firematte.h"
#include "gillnotebooktex_tpl.h"
#include "gillnotebooktex.h"
#include "lakerivercombine_tpl.h"
#include "lakerivercombine.h"

#include "doodles.h"

//#include "secret.h"

#include "globalvars.h" //texobjs in here

#include "rendermgr.h"

#include "draws/gill.h"
#include "draws/lake.h"
#include "draws/hallway.h"
#include "draws/field.h"
#include "draws/river.h"
#include "draws/doodle.h"

#include "displaylist.h"

#include "gillBonefinalseq_liyabc.h"
#include "lakeBonefinalseq_liyabc.h"
#include "riverBonefinalseq_liyabc.h"
#include "Camerafinalseq_liyabc.h"
#include "guyAllpropAnimfullseq_liyabc.h"
#include "lakeMouthshapeanim_liyabc.h"
#include "gillMouthshapeanim_liyabc.h"
#include "lakeTeethshapeanim_liyabc.h"
#include "gillTeethshapeanim_liyabc.h"
#include "flutterbybonefinalseq_liyabc.h"
#include "gillEyeShapeanimFinalseq_liyabc.h"
#include "riverMouthshapeanim_liyabc.h"
#include "riverjacketpropboneanim_liyabc.h"
#include "guyDoodleAnimfullseq_liyabc.h"
#include "comicCameraSeq_liyabc.h"

#include "randomuncategorizedbullshit.h"
#include "procedtex.h"
#include "inits.h"
#include "liysong.h"

#include "data/coolgill.h"
#include "data/typist.h"
#include "data/dk1lenscorr_liym3q.h"

#include <usb.h>
#define DK1IMU_IMPLEMENTATION
#include "dk1imu.h"

guVector lensOffsets = {0.25f, 0.0f, 0.0f};
float realfov = 111.478f; //dk1 sdk examples project with 111.478

int yawresetrequest = 1;

void resetyaw(unsigned int a, void * b) {
	yawresetrequest = 1;
}

Mtx headsetorient;
float gyroaccum[3] = {0, 0, 1};
float cross[3], forward[3];
float downvectrue[3];
void * dk1integratorthread(void * pee) {
	while(1) {
		DK1IMU_SendKeepalive();
		unsigned char imudat[65] ATTRIBUTE_ALIGN(32) = {0, 0};
		int imudatsize;
		float samplevecs[9];
		float accelcollective[3] = {0, 0, 0}, magnecollective[3] = {0, 0, 0}, gyrocollective[3] = {0, 0, 0};
		int numcollects = 0;
		while(1) {
			if(imudat[1] == 2) break;
			imudatsize = DK1IMU_Dataplease(imudat);
			if(imudatsize == 0) break;
			if(imudatsize < 0) {DK1IMU_Close(); DK1IMU_Open(); break; }
			DK1IMU_Datatosamples(imudat, samplevecs);

			accelcollective[0] += samplevecs[0];
			accelcollective[1] += samplevecs[1];
			accelcollective[2] += samplevecs[2];
			gyrocollective[0] += samplevecs[3];
			gyrocollective[1] += samplevecs[4];
			gyrocollective[2] += samplevecs[5];
			magnecollective[0] = samplevecs[6];
			magnecollective[1] = samplevecs[7];
			magnecollective[2] = samplevecs[8];
			numcollects++;
		}
		
		if(numcollects)
		for(int i = 0; i < 3; i++) {
			gyrocollective[i] /= (float)numcollects;
			accelcollective[i] /= (float)numcollects;
		}

		if(yawresetrequest) {
			gyroaccum[0] = 0;
			gyroaccum[1] = 0;
			gyroaccum[2] = 1;
			yawresetrequest = 0;
		}

		for(int i = 0; i < 3; i++) {
			//printf("%02x, ", imudat[i+52]);
			//printf("%7.4f, ", gyroaccum[i]);
		}

		DK1IMU_rotatevecbygyro(gyroaccum, gyrocollective, numcollects, gyroaccum);
		DK1IMU_DownvecFusion(accelcollective, gyrocollective, numcollects, downvectrue);

		//project gyroaccum onto downvectrue plane
		float magsq = DK1IMU_vecdot(downvectrue, downvectrue);
		float sclfac = DK1IMU_vecdot(gyroaccum, downvectrue) / magsq;
		float parallel[3] = {sclfac * downvectrue[0],
		                     sclfac * downvectrue[1],
		                     sclfac * downvectrue[2]};
		gyroaccum[0] -= parallel[0];
		gyroaccum[1] -= parallel[1];
		gyroaccum[2] -= parallel[2];
		DK1IMU_vecnormalize(gyroaccum);

		DK1IMU_veccross(downvectrue, gyroaccum, cross);
		DK1IMU_vecnormalize(cross);
		DK1IMU_veccross(cross, downvectrue, forward);
		DK1IMU_vecnormalize(forward);

		Mtx m;
		guMtxIdentity(m);
		m[0][0] = cross[0];
		m[0][1] = cross[1];
		m[0][2] = cross[2];
		m[1][0] = downvectrue[0];
		m[1][1] = downvectrue[1];
		m[1][2] = downvectrue[2];
		m[2][0] = forward[0];
		m[2][1] = forward[1];
		m[2][2] = forward[2];
		
		guMtxCopy(m, headsetorient);
		usleep(10000);
	}
}

/*void drawcoolarrow(void) { //Actually draws axis not arrow
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_Begin(GX_LINES, GX_VTXFMT0, 6);
		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Position3f32(1.0f, 0.0f, 0.0f);

		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Position3f32(0.0f, 1.0f, 0.0f);

		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Position3f32(0.0f, 0.0f, 1.0f);
}*/

int main(int argc,char **argv) {
	int frame = 1;
	IOS_ReloadIOS(58);

	Mtx44 perspective;

	Mtx permIdentity;
	Mtx44 permIdentity44;
	guMtxIdentity(permIdentity);
	guMtx44Identity(permIdentity44);

	VIDEO_Init();
	WPAD_Init();
	PAD_Init();
	ASND_Init();

	USB_Initialize();

	fatInitDefault();

	liy_initGlobalConsts();
	liy_initialSystemConfig();
	liy_initialFlipperConfig();

	//hollywood rev.. 17 on both my wiis and dolphin, 33 on my wii u
	switch(*(u32*)0x80003138) {
		case 17:
			printf("loading!\n");
			break;
		case 33:
			printf("loading!!!\n");
			break;
		default:
			printf("loading!!\n");
			break;
	}

	  u8 perfectfilter[7] = { 0, 0, 21, 21, 21, 0, 0 };
	//                           v center line
	//u8 vfilter[7] = { 6, 7, 9, 10, 9, 7, 6 };
	//u8 vfilter[7] = { 0, 0, 0, 63, 0, 0, 0 };
	//u8 vfilter[7] = { 0, 0, 21,21, 21,0, 0 };
	  u8 vfilter[7] = { 0, 0, 63, 40, 21, 0, 0};
	  u8 blurfiltr[7] = { 9, 9, 10,  9, 10, 9, 9 };
	//u8 blurfiltr[7]={ 63, 63, 63, 63, 63, 63, 63 };
	//u8 vfilter[7] = { 0, 31, 0, 0, 0, 31, 0 };
	GX_SetCopyFilter(GX_FALSE, NULL, GX_TRUE, vfilter);
	//GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

	liy_initialDrawConfig();

	unsigned long alala = (720*480)/8;
	unsigned long ababa = headsetwait_doodle_size;
	LotsOfRandoms = malloc(65536 * sizeof(float));
	puff((unsigned char *)LotsOfRandoms, &alala, headsetwait_doodle, &ababa);
	GX_SetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GX_PokeColorUpdate(1);
	GX_SetScissor(0,0,640,528);
	GX_DrawDone();
	GX_Flush();
	for(int i = 0; i < 480; i++) {
		for(int j = 0; j < 640; j++) {
			GXColor hc = ((u8 *)LotsOfRandoms)[((i*704)+j)/8] & (128 >> (j % 8)) ? LC_WHITE : LC_BLACK;
			GX_PokeARGB(j, i, hc);
		}
	}
	for(int j = 0; j < 360*480; j++) {
		((u32 *)(frameBuffer[fb]))[j] = COLOR_BLACK;
	}
	GX_SetDispCopySrc(0,0,640,480);
	GX_SetDispCopyDst(704,480);
	GX_DrawDone();
	GX_Flush();
	GX_CopyDisp(frameBuffer[fb], GX_TRUE);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	GX_SetScissor(0,0,352,480);
	GX_SetDispCopySrc(0,0,352,480);
	GX_SetDispCopyDst(704,480);
	VIDEO_SetBlack(true);
	VIDEO_Flush();
	

	srand(1);
	for(int i = 0; i < 65536; i++) {
		LotsOfRandoms[i] = ((float)(rand() % 100000) / 50000.0f) - 1.0f;
	}

	guMtxIdentity(mv);
	GX_LoadTexMtxImm(mv, GX_TEXMTX0, GX_MTX3x4);

	// setup our camera at the origin
	// looking down the -z axis with y up
	guLookAt(view, &cam, &up, &look);
	guLookAt(overlayView, &overlayCam, &overlayUp, &overlaylook);

	guPerspective(overlayPersp, 45 * 1.5, aspect, 0.1F, 300.0F);
	guPerspective(perspective, 45, aspect, 0.1F, 300.0F); //not th one used @ runtime
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

	// Decompress gill mouth

		float *gillMouthFullsizevertpos[8];
		float *gillMouthFullsizevertnor[8];
		float *gillTeethFullsizevertpos[2];
		float *gillTeethFullsizevertnor[2];
		for(int i = 0; i < 8; i++) {
			gillMouthFullsizevertpos[i] = (float *)malloc((gillMouthBasistricount * 9) * sizeof(float));
			gillMouthFullsizevertnor[i] = (float *)malloc((gillMouthBasistricount * 9) * sizeof(float));
		}
		for(int i = 0; i < 2; i++) {
			gillTeethFullsizevertpos[i] = (float *)malloc((gillTeethClosedtricount * 9) * sizeof(float));
			gillTeethFullsizevertnor[i] = (float *)malloc((gillTeethClosedtricount * 9) * sizeof(float));
		}

		modelIndexedToRaw(gillMouthFullsizevertpos[0], gillMouthFullsizevertnor[0], 
		                  gillMouthBasisvertpos, gillMouthBasisvertposidx, 
				  gillMouthBasisvertnor, gillMouthBasisvertnoridx, 
		                  gillMouthBasistricount);

		modelIndexedToRaw(gillMouthFullsizevertpos[1], gillMouthFullsizevertnor[1], 
		                  gillMouthGenopenvertpos, gillMouthGenopenvertposidx, 
				  gillMouthGenopenvertnor, gillMouthGenopenvertnoridx, 
		                  gillMouthBasistricount);
		
		modelIndexedToRaw(gillMouthFullsizevertpos[2], gillMouthFullsizevertnor[2], 
		                  gillMouthGenwidevertpos, gillMouthGenwidevertposidx, 
				  gillMouthGenwidevertnor, gillMouthGenwidevertnoridx, 
		                  gillMouthBasistricount);

		modelIndexedToRaw(gillMouthFullsizevertpos[3], gillMouthFullsizevertnor[3], 
		                  gillMouthOvertpos, gillMouthOvertposidx, 
				  gillMouthOvertnor, gillMouthOvertnoridx, 
		                  gillMouthBasistricount);

		modelIndexedToRaw(gillMouthFullsizevertpos[4], gillMouthFullsizevertnor[4], 
		                  gillMouthRightsneervertpos, gillMouthRightsneervertposidx, 
				  gillMouthRightsneervertnor, gillMouthRightsneervertnoridx, 
		                  gillMouthBasistricount);

		modelIndexedToRaw(gillMouthFullsizevertpos[5], gillMouthFullsizevertnor[5], 
		                  gillMouthLipthingvertpos, gillMouthLipthingvertposidx, 
				  gillMouthLipthingvertnor, gillMouthLipthingvertnoridx, 
		                  gillMouthBasistricount);
		
		modelIndexedToRaw(gillMouthFullsizevertpos[6], gillMouthFullsizevertnor[6], 
		                  gillMouthTongueupvertpos, gillMouthTongueupvertposidx, 
				  gillMouthTongueupvertnor, gillMouthTongueupvertnoridx, 
		                  gillMouthBasistricount);
		
		modelIndexedToRaw(gillMouthFullsizevertpos[7], gillMouthFullsizevertnor[7], 
		                  gillMouthSmilevertpos, gillMouthSmilevertposidx, 
				  gillMouthSmilevertnor, gillMouthSmilevertnoridx, 
		                  gillMouthBasistricount);
	
		modelIndexedToRaw(gillTeethFullsizevertpos[0], gillTeethFullsizevertnor[0],
		                  gillTeethClosedvertpos, gillTeethClosedvertposidx,
		                  gillTeethClosedvertnor, gillTeethClosedvertnoridx,
		                  gillTeethClosedtricount);

		modelIndexedToRaw(gillTeethFullsizevertpos[1], gillTeethFullsizevertnor[1],
		                  gillTeethOpenvertpos, gillTeethOpenvertposidx,
		                  gillTeethOpenvertnor, gillTeethOpenvertnoridx,
		                  gillTeethOpentricount);		

		float *gillMouthDeltavertpos[7];
		float *gillMouthDeltavertnor[7];
		float *gillTeethDeltavertpos = malloc(gillTeethClosedtricount * 9 * sizeof(float));
		float *gillTeethDeltavertnor = malloc(gillTeethClosedtricount * 9 * sizeof(float));

		for(int i = 0; i < 7; i++) {
			gillMouthDeltavertpos[i] = malloc(gillMouthBasistricount * 9 * sizeof(float));
			gillMouthDeltavertnor[i] = malloc(gillMouthBasistricount * 9 * sizeof(float));
			for(int j = 0; j < gillMouthBasistricount * 9; j++) {
				gillMouthDeltavertpos[i][j] = gillMouthFullsizevertpos[i+1][j] - gillMouthFullsizevertpos[0][j];
				gillMouthDeltavertnor[i][j] = gillMouthFullsizevertnor[i+1][j] - gillMouthFullsizevertnor[0][j];
			}
		}
		for(int i = 0; i < gillTeethClosedtricount * 9; i++) {
			gillTeethDeltavertpos[i] = gillTeethFullsizevertpos[1][i] - gillTeethFullsizevertpos[0][i];
			gillTeethDeltavertnor[i] = gillTeethFullsizevertnor[1][i] - gillTeethFullsizevertnor[0][i];
		}

		for(int i = 1; i < 8; i++) { //dont free 0
			free(gillMouthFullsizevertpos[i]);
			free(gillMouthFullsizevertnor[i]);
		}
		free(gillTeethFullsizevertpos[1]);
		free(gillTeethFullsizevertnor[1]);

		float *gillMouthShapedVertpos = (float *)malloc(gillMouthBasistricount * 9 * sizeof(float));
		float *gillMouthShapedVertnor = (float *)malloc(gillMouthBasistricount * 9 * sizeof(float));
		float *gillTeethShapedvertpos = (float *)malloc(gillTeethClosedtricount * 9 * sizeof(float));
		float *gillTeethShapedvertnor = (float *)malloc(gillTeethClosedtricount * 9 * sizeof(float));
	
	// Decompress gills retarded little sclarae

		float *gillEyeBlackFullsizeVertpos[2];
		float *gillEyeWhiteFullsizeVertpos[2];
		for(int i = 0; i < 2; i++) {
			gillEyeBlackFullsizeVertpos[i] = (float *)malloc((gillEyeBlackBasistricount * 9) * sizeof(float));
			gillEyeWhiteFullsizeVertpos[i] = (float *)malloc((gillEyeWhiteBasistricount * 9) * sizeof(float));
		}
		modelIndexedToRawPos(gillEyeBlackFullsizeVertpos[0],
		                     gillEyeBlackBasisvertpos, gillEyeBlackBasisvertposidx,
		                     gillEyeBlackBasistricount);
		modelIndexedToRawPos(gillEyeBlackFullsizeVertpos[1],
		                     gillEyeBlackClosedvertpos, gillEyeBlackClosedvertposidx,
		                     gillEyeBlackBasistricount);
		modelIndexedToRawPos(gillEyeWhiteFullsizeVertpos[0],
		                     gillEyeWhiteBasisvertpos, gillEyeWhiteBasisvertposidx,
		                     gillEyeWhiteBasistricount);
		modelIndexedToRawPos(gillEyeWhiteFullsizeVertpos[1],
		                     gillEyeWhiteClosedvertpos, gillEyeWhiteClosedvertposidx,
		                     gillEyeWhiteBasistricount);
		float *gillEyeBlackDeltavertpos = malloc(gillEyeBlackBasistricount * 9 * sizeof(float));
		float *gillEyeWhiteDeltavertpos = malloc(gillEyeWhiteBasistricount * 9 * sizeof(float));
		for(int i = 0; i < gillEyeBlackBasistricount * 9; i++) {
			gillEyeBlackDeltavertpos[i] = gillEyeBlackFullsizeVertpos[1][i] - gillEyeBlackFullsizeVertpos[0][i];
		}
		for(int i = 0; i < gillEyeWhiteBasistricount * 9; i++) {
			gillEyeWhiteDeltavertpos[i] = gillEyeWhiteFullsizeVertpos[1][i] - gillEyeWhiteFullsizeVertpos[0][i];
		}
		free(gillEyeBlackFullsizeVertpos[1]);
		free(gillEyeWhiteFullsizeVertpos[1]);

		float *gillEyeBlackShapedVertpos = malloc(gillEyeBlackBasistricount * 9 * sizeof(float));
		float *gillEyeWhiteShapedVertpos = malloc(gillEyeWhiteBasistricount * 9 * sizeof(float));

	// Decompress lake mouth

		float *lakeMouthFullsizevertpos[8];
		float *lakeMouthFullsizevertnor[8];
		float *lakeTeethFullsizevertpos[2];
		float *lakeTeethFullsizevertnor[2];
		for(int i = 0; i < 8; i++) {
			lakeMouthFullsizevertpos[i] = (float *)malloc((lakeMouthBasistricount * 9) * sizeof(float));
			lakeMouthFullsizevertnor[i] = (float *)malloc((lakeMouthBasistricount * 9) * sizeof(float));
		}
		for(int i = 0; i < 2; i++) {
			lakeTeethFullsizevertpos[i] = (float *)malloc((lakeTeethClosedtricount * 9) * sizeof(float));
			lakeTeethFullsizevertnor[i] = (float *)malloc((lakeTeethClosedtricount * 9) * sizeof(float));
		}	

		modelIndexedToRaw(lakeMouthFullsizevertpos[0], lakeMouthFullsizevertnor[0], 
		                  lakeMouthBasisvertpos, lakeMouthBasisvertposidx, 
				  lakeMouthBasisvertnor, lakeMouthBasisvertnoridx, 
		                  lakeMouthBasistricount);
		modelIndexedToRaw(lakeMouthFullsizevertpos[1], lakeMouthFullsizevertnor[1], 
		                  lakeMouthGenopenvertpos, lakeMouthGenopenvertposidx, 
				  lakeMouthGenopenvertnor, lakeMouthGenopenvertnoridx, 
		                  lakeMouthBasistricount);
		modelIndexedToRaw(lakeMouthFullsizevertpos[2], lakeMouthFullsizevertnor[2], 
		                  lakeMouthGenwidevertpos, lakeMouthGenwidevertposidx, 
				  lakeMouthGenwidevertnor, lakeMouthGenwidevertnoridx, 
		                  lakeMouthBasistricount);
		modelIndexedToRaw(lakeMouthFullsizevertpos[3], lakeMouthFullsizevertnor[3], 
		                  lakeMouthOvertpos, lakeMouthOvertposidx, 
				  lakeMouthOvertnor, lakeMouthOvertnoridx, 
		                  lakeMouthBasistricount);
		modelIndexedToRaw(lakeMouthFullsizevertpos[4], lakeMouthFullsizevertnor[4], 
		                  lakeMouthLeftsneervertpos, lakeMouthLeftsneervertposidx, 
				  lakeMouthLeftsneervertnor, lakeMouthLeftsneervertnoridx, 
		                  lakeMouthBasistricount);
		modelIndexedToRaw(lakeMouthFullsizevertpos[5], lakeMouthFullsizevertnor[5], 
		                  lakeMouthRightsneervertpos, lakeMouthRightsneervertposidx, 
				  lakeMouthRightsneervertnor, lakeMouthRightsneervertnoridx, 
		                  lakeMouthBasistricount);
		modelIndexedToRaw(lakeMouthFullsizevertpos[6], lakeMouthFullsizevertnor[6], 
		                  lakeMouthTongueupvertpos, lakeMouthTongueupvertposidx, 
				  lakeMouthTongueupvertnor, lakeMouthTongueupvertnoridx, 
		                  lakeMouthBasistricount);
		modelIndexedToRaw(lakeMouthFullsizevertpos[7], lakeMouthFullsizevertnor[7], 
		                  lakeMouthSmilevertpos, lakeMouthSmilevertposidx, 
				  lakeMouthSmilevertnor, lakeMouthSmilevertnoridx, 
		                  lakeMouthBasistricount);
		modelIndexedToRaw(lakeTeethFullsizevertpos[0], lakeTeethFullsizevertnor[0],
		                  lakeTeethClosedvertpos, lakeTeethClosedvertposidx,
		                  lakeTeethClosedvertnor, lakeTeethClosedvertnoridx,
		                  lakeTeethClosedtricount);
		modelIndexedToRaw(lakeTeethFullsizevertpos[1], lakeTeethFullsizevertnor[1],
		                  lakeTeethOpenvertpos, lakeTeethOpenvertposidx,
		                  lakeTeethOpenvertnor, lakeTeethOpenvertnoridx,
		                  lakeTeethOpentricount);
		

		float *lakeMouthDeltavertpos[7];
		float *lakeMouthDeltavertnor[7];
		float *lakeTeethDeltavertpos = malloc(lakeTeethClosedtricount * 9 * sizeof(float));
		float *lakeTeethDeltavertnor = malloc(lakeTeethClosedtricount * 9 * sizeof(float));

		for(int i = 0; i < 7; i++) {
			lakeMouthDeltavertpos[i] = malloc(lakeMouthBasistricount * 9 * sizeof(float));
			lakeMouthDeltavertnor[i] = malloc(lakeMouthBasistricount * 9 * sizeof(float));
			for(int j = 0; j < lakeMouthBasistricount * 9; j++) {
				lakeMouthDeltavertpos[i][j] = lakeMouthFullsizevertpos[i+1][j] - lakeMouthFullsizevertpos[0][j];
				lakeMouthDeltavertnor[i][j] = lakeMouthFullsizevertnor[i+1][j] - lakeMouthFullsizevertnor[0][j];
			}
		}
		for(int i = 0; i < lakeTeethClosedtricount * 9; i++) {
			lakeTeethDeltavertpos[i] = lakeTeethFullsizevertpos[1][i] - lakeTeethFullsizevertpos[0][i];
			lakeTeethDeltavertnor[i] = lakeTeethFullsizevertnor[1][i] - lakeTeethFullsizevertnor[0][i];
		}

		for(int i = 1; i < 8; i++) {
			free(lakeMouthFullsizevertpos[i]);
			free(lakeMouthFullsizevertnor[i]);
		}
		free(lakeTeethFullsizevertpos[1]);
		free(lakeTeethFullsizevertnor[1]);

		float *lakeMouthShapedVertpos = (float *)malloc(lakeMouthBasistricount * 9 * sizeof(float));
		float *lakeMouthShapedVertnor = (float *)malloc(lakeMouthBasistricount * 9 * sizeof(float));
		float *lakeTeethShapedvertpos = (float *)malloc(lakeTeethClosedtricount * 9 * sizeof(float));
		float *lakeTeethShapedvertnor = (float *)malloc(lakeTeethClosedtricount * 9 * sizeof(float));
		
	// River mouth

		float *riverMouthFullsizevertpos[5];
		float *riverMouthFullsizevertnor[5];
		for(int i = 0; i < 5; i++) {
			riverMouthFullsizevertpos[i] = (float *)malloc((riverMouthBasistricount * 9) * sizeof(float));
			riverMouthFullsizevertnor[i] = (float *)malloc((riverMouthBasistricount * 9) * sizeof(float));
		}
		modelIndexedToRaw(riverMouthFullsizevertpos[0], riverMouthFullsizevertnor[0], 
		                  riverMouthBasisvertpos, riverMouthBasisvertposidx, 
				  riverMouthBasisvertnor, riverMouthBasisvertnoridx, 
		                  riverMouthBasistricount);
		modelIndexedToRaw(riverMouthFullsizevertpos[1], riverMouthFullsizevertnor[1], 
		                  riverMouthClosevertpos, riverMouthClosevertposidx, 
				  riverMouthClosevertnor, riverMouthClosevertnoridx, 
		                  riverMouthBasistricount);
		modelIndexedToRaw(riverMouthFullsizevertpos[2], riverMouthFullsizevertnor[2], 
		                  riverMouthSmilevertpos, riverMouthSmilevertposidx, 
				  riverMouthSmilevertnor, riverMouthSmilevertnoridx, 
		                  riverMouthBasistricount);
		modelIndexedToRaw(riverMouthFullsizevertpos[3], riverMouthFullsizevertnor[3], 
		                  riverMouthWidevertpos, riverMouthWidevertposidx, 
				  riverMouthWidevertnor, riverMouthWidevertnoridx, 
		                  riverMouthBasistricount);
		modelIndexedToRaw(riverMouthFullsizevertpos[4], riverMouthFullsizevertnor[4], 
		                  riverMouthTeethopenvertpos, riverMouthTeethopenvertposidx, 
				  riverMouthTeethopenvertnor, riverMouthTeethopenvertnoridx, 
		                  riverMouthBasistricount);
		float *riverMouthDeltavertpos[4];
		float *riverMouthDeltavertnor[4];
		for(int i = 0; i < 4; i++) {
			riverMouthDeltavertpos[i] = malloc(riverMouthBasistricount * 9 * sizeof(float));
			riverMouthDeltavertnor[i] = malloc(riverMouthBasistricount * 9 * sizeof(float));
			for(int j = 0; j < riverMouthBasistricount * 9; j++) {
				riverMouthDeltavertpos[i][j] = riverMouthFullsizevertpos[i+1][j] - riverMouthFullsizevertpos[0][j];
				riverMouthDeltavertnor[i][j] = riverMouthFullsizevertnor[i+1][j] - riverMouthFullsizevertnor[0][j];
			}
		}
		for(int i = 1; i < 5; i++) {
			free(riverMouthFullsizevertpos[i]);
			free(riverMouthFullsizevertnor[i]);
		}
		float *riverMouthShapedvertpos = (float *)malloc(riverMouthBasistricount * 9 * sizeof(float));
		float *riverMouthShapedvertnor = (float *)malloc(riverMouthBasistricount * 9 * sizeof(float));

	gillSkeletonMv = malloc(58 * sizeof(Mtx));
	gillSkeletonMvReflectionmap = malloc(58 * sizeof(Mtx));
	gillSkeletonMvReflect = malloc(58 * sizeof(Mtx));
	gillSkeletonMvReflectReflectionmap = malloc(58 * sizeof(Mtx));
	float * gillSkelFrame;
	riverSkeletonMv = malloc(51 * sizeof(Mtx));
	riverSkeletonMvReflectionmap = malloc(51 * sizeof(Mtx));
	float * riverSkelFrame;
	lakeSkeletonMv = malloc(59 * sizeof(Mtx));
	lakeSkeletonMvReflectionmap = malloc(59 * sizeof(Mtx));
	float * lakeSkelFrame;
	Mtx * flutterbyMv = malloc(144); //just dug this guy out of the main loop oops lol
	Mtx * jacketpropmv = malloc(4*4*3*4);
	Mtx * jacketpropmvrefmap = malloc(4*4*3*4);
	Mtx * jacketproprefmv = malloc(4*4*3*4);

	// Decompress animation

		liy_unpack_liyabc(Camerafinalseq, 151456);
		liy_unpack_liyabc(gillBonefinalseq, 6832172);
		liy_unpack_liyabc(lakeBonefinalseq, 6949968);
		liy_unpack_liyabc(riverBonefinalseq, 6007600);
		liy_unpack_liyabc(guyAllpropAnimfullseq, 673124);
		liy_unpack_liyabc(gillMouthshapeanim, 134628);
		liy_unpack_liyabc(lakeMouthshapeanim, 117800);
		liy_unpack_liyabc(riverMouthshapeanim, 67316);
		liy_unpack_liyabc(gillTeethshapeanim, 16832);
		liy_unpack_liyabc(lakeTeethshapeanim, 16832);
		liy_unpack_liyabc(flutterbybonefinalseq, 12424);
		liy_unpack_liyabc(gillEyeShapeanimFinalseq, 16832);
		liy_unpack_liyabc(riverjacketpropboneanim, 33604);
		liy_unpack_liyabc(guyDoodleAnimfullseq, 832204);
		liy_unpack_liyabc(comicCameraSeq, 4652);	

	float flowerPos[3000];
	for(int i = 0; i < 1000; i++) {
		int i3 = i * 3;
		flowerPos[i3] = LotsOfRandoms[10000 - i3] * 5.0f;
		flowerPos[i3+1] = (LotsOfRandoms[10000 - (i3+1)] * 5.0f) + 4.0f;
		flowerPos[i3+2] = (LotsOfRandoms[10000 - (i3+2)] * 0.05f) + 0.12;
	} 

	// # Load tree
		f32 *treevertposRaw = malloc((treetricount * 9) * sizeof(f32));
		f32 *treeleavesvertposRaw = malloc((treeleavestricount * 9) * sizeof(f32));
		nullcheck(treevertposRaw);
		nullcheck(treeleavesvertposRaw);

		for(int i = 0; i < treetricount * 3; i++) {
			int idx3 = treevertposidx[i] * 3;
			int i3 = i * 3;
			treevertposRaw[i3] = treevertpos[idx3];
			treevertposRaw[i3+1] = treevertpos[idx3+1];
			treevertposRaw[i3+2] = treevertpos[idx3+2];
		}
		for(int i = 0; i < treeleavestricount * 3; i++) {
			int idx3 = treeleavesvertposidx[i] * 3;
			int i3 = i * 3;
			treeleavesvertposRaw[i3] = treeleavesvertpos[idx3];
			treeleavesvertposRaw[i3+1] = treeleavesvertpos[idx3+1];
			treeleavesvertposRaw[i3+2] = treeleavesvertpos[idx3+2];
		}


		treemtxarr = malloc(treetricount * 3);

		treeleavesmtxarr = malloc(treeleavestricount * 3);

		bs_genMtxArrForTheTree(treevertposRaw, treemtxarr, treetricount);
		bs_genMtxArrForTheTree(treeleavesvertposRaw, treeleavesmtxarr, treeleavestricount);
		
		free(treevertposRaw);
		free(treeleavesvertposRaw);
	
	liy_buildDisplaylist();

	GX_InitTexObj(&hallway0TexObj, flowerPos, 64, 128, GX_TF_I4, GX_REPEAT, GX_REPEAT, GX_FALSE);

	float dialx = (widescreen) ? 7.0f : 5.0f;
	u64 frametime = 0;
	double frametimems = 16.0f;
	double cputimems = 16.0f;
	double framerate = 60.0f;
	u64 realframetime = 0;
	double realframetimems = 16.0f;

	int consoletog = 0;
	int posttog = 1;
	float exposure = 0.50f;
	float Lily_Float = 0.99f;
	u8 stops = 0;
	int Other_Int = 1;
	u8 cameraControl = 0;
	int frameadvance = 1;

	scenelastframe = 5000;

	// Allocate rendertextures for postprocess fx
	GXTexObj DofDefBufrTexObj, DofDefQtrsizeTexObj, DofDefClrTexObj, dofDisplaceATexObj, 
	dofDisplaceBTexObj, dofDisplaceCTexObj, dofDisplaceDTexObj, 
	dofDoublepassHoldingTexObj; 
	int ltw = 640;
	int lth = 528;
	int texsize = GX_GetTexBufferSize(ltw, lth, GX_TF_RGBA8, GX_FALSE, 0);
	void *DofDefClr = memalign(32, GX_GetTexBufferSize(ltw, lth, GX_TF_RGBA8, GX_FALSE, 0));
	void *Comicholding = memalign(32, 1*1024*1024);

	liy_unpackPackedTextures();
	//Persistent texture
	GX_LoadTexObj(&gilltexTexObj, GX_TEXMAP7);

	void * lightcapbuf = memalign(32, 128*128*4);

	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_Flush();

	printf("Searching for dk1 imu\n");

	int secswaitedstart = 0;
	while(!DK1IMU_Open()) {
		for(int i = 0; i < 60; i++) {
			VIDEO_WaitVSync();
		}
		secswaitedstart++;
		if(secswaitedstart == 4) {//printf("try reconnecting your usb cable\n");
			VIDEO_SetBlack(false);
			VIDEO_Flush();
		}
	}
	VIDEO_SetBlack(true);
	VIDEO_Flush();
	lwp_t dk1integratorthreadhandle = LWP_THREAD_NULL;
	LWP_CreateThread(&dk1integratorthreadhandle, dk1integratorthread, NULL, NULL, 0, 70);
	SYS_SetResetCallback(resetyaw);

	realframetime = gettime();

	int keypressed = 0;
	int keypressed2 = 0;

	song_play();
	ASND_Pause(0);
	u64 startplay = gettime();
	double playbacktime = 0.0f;
	int sndpause = 0;
	int framelastframe = 1;
	if(!dolphin) printf("\x1b[1;1H\x1b[2J");
	printf("i wanna be the guy\n");
	printf("by twikipedia\n");
	printf("animation by lliy\n");
	printf("\n");
	while(1) {									 // # # # # Main loop # # # 
		if(dolphin) {
			//dolphin log isnt really a terminal
			if(lyric == 0  && frame > 386 ) {lyric++; printf("i wanna be your favorite boy and girl, the one you think about\n");}
			if(lyric == 1  && frame > 576 ) {lyric++; printf("feel the guilt crawl in my skin and bones whenever you're around\n");}
			if(lyric == 2  && frame > 750 ) {lyric++; printf("and nothing's worse than you with someone else..\n");}
			if(lyric == 3  && frame > 861 ) {lyric++; printf("you're mine, you're mine, you're mine!!\n");}
			if(lyric == 4  && frame > 929 ) {lyric++; printf("\n");}
			if(lyric == 5  && frame > 930 ) {lyric++; printf("(i want, i want, i wanna be the guy)\n");}
			if(lyric == 6  && frame > 1018) {lyric++; printf("(i want, i want, i)\n");}
			if(lyric == 7  && frame > 1065) {lyric++; printf("i wanna be the guy\n");}
			if(lyric == 8  && frame > 1114) {lyric++; printf("\ni love when you try hard to hurt me\n");}
			if(lyric == 9  && frame > 1207) {lyric++; printf("then i feel like i'm real\n");}
			if(lyric == 10 && frame > 1301) {lyric++; printf("(it's hard to feel that way, i'd die a lot just trying to feel something)\n");}
			if(lyric == 11 && frame > 1481) {lyric++; printf("you bash my head against a rock\n");}
			if(lyric == 12 && frame > 1576) {lyric++; printf("when i confess what i feel\n");}
			if(lyric == 13 && frame > 1665) {lyric++; printf("(i love the headache afterwards, i love the ER room with you)\n");}
			if(lyric == 14 && frame > 1868) {lyric++; printf("\nand you know (and you know) how i feel (how i feel)\n");}
			if(lyric == 15 && frame > 1959) {lyric++; printf("but you choose (but you choose) to play hard to get\n");}
			if(lyric == 16 && frame > 2050) {lyric++; printf("before you (before you) met your guy (met your guy)\n");}
			if(lyric == 17 && frame > 2143) {lyric++; printf("you were someone i could understand\n");}
			if(lyric == 18 && frame > 2236) {lyric++; printf("what did he treat you like for you to turn into a psychopath\n");}
			if(lyric == 19 && frame > 2379) {lyric++; printf("who loves to watch me cry?\n");}
			if(lyric == 20 && frame > 2576) {lyric++; printf("\n123\n");}
			if(lyric == 21 && frame > 2622) {lyric++; printf("\n(guy..)\n");}
			if(lyric == 22 && frame > 2750) {lyric++; printf("i wanna be the guy\n");}
			if(lyric == 23 && frame > 2805) {lyric++; printf("(guy)\n");}
			if(lyric == 24 && frame > 2841) {lyric++; printf("if he's allowed to fuck up\n");}
			if(lyric == 25 && frame > 2925) {lyric++; printf("then i wanna be the guy\n");}
			if(lyric == 26 && frame > 2990) {lyric++; printf("(guy)\n");}
			if(lyric == 27 && frame > 3020) {lyric++; printf("so tell me, what's he like? (what's he like\?\?)\n");}
			if(lyric == 28 && frame > 3116) {lyric++; printf("so tell me, what's he like?? (what's he like..)\n");}
			if(lyric == 29 && frame > 3206) {lyric++; printf("i wanna be the guy!!\n");}
		} else {
			if(lyric == 0  && frame > 380 ) {lyric++; printf("i wanna be your favorite boy and girl, "); fflush(stdout);}
			if(lyric == 1  && frame > 496 ) {lyric++; printf("the one you think about "); fflush(stdout);}
			if(lyric == 2  && frame > 560 ) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 3  && frame > 576 ) {lyric++; printf("feel the guilt crawl in my skin and bones "); fflush(stdout);}
			if(lyric == 4  && frame > 677 ) {lyric++; printf("whenever you're around "); fflush(stdout);}
			if(lyric == 5  && frame > 747 ) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 6  && frame > 750 ) {lyric++; printf("and nothing's worse than you with someone else.. "); fflush(stdout);}
			if(lyric == 7  && frame > 855 ) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 8  && frame > 861 ) {lyric++; printf("you're mine, "); fflush(stdout);}
			if(lyric == 9  && frame > 883 ) {lyric++; printf("you're mine, "); fflush(stdout);}
			if(lyric == 10 && frame > 910 ) {lyric++; printf("you're mine!! "); fflush(stdout);}
			if(lyric == 11 && frame > 928 ) {lyric++; printf("\n"); fflush(stdout);} // \n\n doesnt work okay
			if(lyric == 12 && frame > 929 ) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 13 && frame > 930 ) {lyric++; printf("(i want, i want, i wanna be the guy) "); fflush(stdout);}
			if(lyric == 14 && frame > 1012) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 15 && frame > 1018) {lyric++; printf("(i want, i want, i) "); fflush(stdout);}
			if(lyric == 16 && frame > 1060) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 17 && frame > 1065) {lyric++; printf("i wanna be the guy "); fflush(stdout);}
			if(lyric == 18 && frame > 1102) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 19 && frame > 1103) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 20 && frame > 1114) {lyric++; printf("i love when you "); fflush(stdout);}
			if(lyric == 21 && frame > 1160) {lyric++; printf("try hard to hurt "); fflush(stdout);}
			if(lyric == 22 && frame > 1207) {lyric++; printf("me then "); fflush(stdout);}
			if(lyric == 23 && frame > 1231) {lyric++; printf("i feel like i'm real "); fflush(stdout);}
			if(lyric == 24 && frame > 1285) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 25 && frame > 1301) {lyric++; printf("(it's hard to feel that way, i'd die a lot just trying to feel something) "); fflush(stdout);}
			if(lyric == 26 && frame > 1478) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 27 && frame > 1481) {lyric++; printf("you bash "); fflush(stdout);}
			if(lyric == 28 && frame > 1505) {lyric++; printf("my head "); fflush(stdout);}
			if(lyric == 29 && frame > 1528) {lyric++; printf("against "); fflush(stdout);}
			if(lyric == 30 && frame > 1553) {lyric++; printf("a rock "); fflush(stdout);}
			if(lyric == 31 && frame > 1576) {lyric++; printf("when i confess what i feel "); fflush(stdout);}
			if(lyric == 32 && frame > 1655) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 33 && frame > 1665) {lyric++; printf("(i love the headache afterwards, i love the ER room with you) "); fflush(stdout);}
			if(lyric == 34 && frame > 1846) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 35 && frame > 1847) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 36 && frame > 1868) {lyric++; printf("and you know "); fflush(stdout);}
			if(lyric == 37 && frame > 1886) {lyric++; printf("(and you know) "); fflush(stdout);}
			if(lyric == 38 && frame > 1910) {lyric++; printf("how i feel "); fflush(stdout);}
			if(lyric == 39 && frame > 1935) {lyric++; printf("(how i feel) "); fflush(stdout);}
			if(lyric == 40 && frame > 1958) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 41 && frame > 1959) {lyric++; printf("but you choose "); fflush(stdout);}
			if(lyric == 42 && frame > 1983) {lyric++; printf("(but you choose) "); fflush(stdout);}
			if(lyric == 43 && frame > 2006) {lyric++; printf("to play hard to get "); fflush(stdout);}
			if(lyric == 44 && frame > 2049) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 45 && frame > 2050) {lyric++; printf("before you "); fflush(stdout);}
			if(lyric == 46 && frame > 2072) {lyric++; printf("(before you) "); fflush(stdout);}
			if(lyric == 47 && frame > 2100) {lyric++; printf("met your guy "); fflush(stdout);}
			if(lyric == 48 && frame > 2122) {lyric++; printf("(met your guy) "); fflush(stdout);}
			if(lyric == 49 && frame > 2142) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 50 && frame > 2143) {lyric++; printf("you were someone i could understand "); fflush(stdout);}
			if(lyric == 51 && frame > 2233) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 52 && frame > 2236) {lyric++; printf("what did he treat you like "); fflush(stdout);}
			if(lyric == 53 && frame > 2280) {lyric++; printf("for you to turn into a psychopath "); fflush(stdout);}
			if(lyric == 54 && frame > 2373) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 55 && frame > 2379) {lyric++; printf("who loves to watch me cry? "); fflush(stdout);}
			if(lyric == 56 && frame > 2502) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 57 && frame > 2503) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 58 && frame > 2576) {lyric++; printf("1"); fflush(stdout);}
			if(lyric == 59 && frame > 2587) {lyric++; printf("2"); fflush(stdout);}
			if(lyric == 60 && frame > 2597) {lyric++; printf("3"); fflush(stdout);}
			if(lyric == 61 && frame > 2609) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 62 && frame > 2610) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 63 && frame > 2622) {lyric++; printf("(guy..) "); fflush(stdout);}
			if(lyric == 64 && frame > 2725) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 65 && frame > 2750) {lyric++; printf("i wanna be the guy "); fflush(stdout);}
			if(lyric == 66 && frame > 2805) {lyric++; printf("(guy) "); fflush(stdout);}
			if(lyric == 67 && frame > 2820) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 68 && frame > 2841) {lyric++; printf("if he's allowed to fuck up "); fflush(stdout);}
			if(lyric == 69 && frame > 2916) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 70 && frame > 2925) {lyric++; printf("then i wanna be the guy "); fflush(stdout);}
			if(lyric == 71 && frame > 2990) {lyric++; printf("(guy) "); fflush(stdout);}
			if(lyric == 72 && frame > 3018) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 73 && frame > 3020) {lyric++; printf("so tell me, what's he like? "); fflush(stdout);}
			if(lyric == 74 && frame > 3086) {lyric++; printf("(what's he like\?\?) "); fflush(stdout);}
			if(lyric == 75 && frame > 3115) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 76 && frame > 3116) {lyric++; printf("so tell me, what's he like?? "); fflush(stdout);}
			if(lyric == 77 && frame > 3178) {lyric++; printf("(what's he like..) "); fflush(stdout);}
			if(lyric == 78 && frame > 3205) {lyric++; printf("\n"); fflush(stdout);}
			if(lyric == 79 && frame > 3206) {lyric++; printf("i "); fflush(stdout);}
			if(lyric == 80 && frame > 3215) {lyric++; printf("wa-"); fflush(stdout);}
			if(lyric == 81 && frame > 3230) {lyric++; printf("\r"); fflush(stdout); printf("i wanna "); fflush(stdout);}
			if(lyric == 82 && frame > 3235) {lyric++; printf("be "); fflush(stdout);}
			if(lyric == 83 && frame > 3253) {lyric++; printf("the "); fflush(stdout);}
			if(lyric == 84 && frame > 3260) {lyric++; printf("guy!! "); fflush(stdout);}
			if(lyric == 85 && frame > 3280) {lyric++; printf("\n"); fflush(stdout);}
		}
		
		if(!istypist)
		if(frame < 350)
		if(USBKeyboard_IsConnected()) {
			istypist++;
			for(int i = 0; i < sizeof(typist); i++) putchar(typist[i]);
		}
		
		basicStringsFrame = 0;
		uint8_t buttons = 0;
		u64 startframe = gettime();

		//wiimote
		WPAD_ScanPads();
		int wbuttonsdown = WPAD_ButtonsDown(0);
		int wbuttonsheld = WPAD_ButtonsHeld(0);
		if(wbuttonsdown & WPAD_BUTTON_HOME) pleaseexit++;
		if (wbuttonsheld&WPAD_BUTTON_UP) buttons = buttons | LIYB_UP;
		if (wbuttonsheld&WPAD_BUTTON_DOWN) buttons = buttons | LIYB_DOWN;
		if (wbuttonsheld&WPAD_BUTTON_LEFT) buttons = buttons | LIYB_LEFT;
		if (wbuttonsheld&WPAD_BUTTON_RIGHT) buttons = buttons | LIYB_RIGHT;
		if (wbuttonsheld&WPAD_BUTTON_1) { buttons = buttons | LIYB_1; }
		if (wbuttonsheld&WPAD_BUTTON_2) { buttons = buttons | LIYB_2; }
		if (wbuttonsheld&WPAD_BUTTON_A) { buttons = buttons | LIYB_A; }
		if (wbuttonsheld&WPAD_BUTTON_B) { buttons = buttons | LIYB_B; }
		if(wbuttonsdown & WPAD_BUTTON_MINUS) yawresetrequest = 1;

		//gc
		int gcconnected = PAD_ScanPads();
		int gbuttonsheld = 0;
		int gbuttonsdown = 0;

		#define KEYB_SPRINT 1
		#define KEYB_SLOW   2
		#define KEYB_CTRTOG 4
		#define KEYB_FOCUP  8
		#define KEYB_FOCDN  16
		#define KEYB_EXPUP  32
		#define KEYB_EXPDN  64
		#define KEYB_ANMFFW 128
		#define KEYB_ANMREW 256
		#define KEYB_UP     512
		#define KEYB_DOWN   1024
		#define KEYB_FOVUP  2048
		#define KEYB_FOVDN  4096

		if(USBKeyboard_IsConnected()){
		int key;
		keyboard_event keyevent;
		while(KEYBOARD_GetEvent(&keyevent)) {
			key = keyevent.keycode;
			if(keyevent.type == KEYBOARD_PRESSED) {
				if(key == 0x1a) keypressed2 |= LIYB_RIGHT;    //w
				if(key == 0x04) keypressed2 |= LIYB_UP;   //a
				if(key == 0x16) keypressed2 |= LIYB_LEFT;  //s
				if(key == 0x07) keypressed2 |= LIYB_DOWN;  //d

				if(key == 225) keypressed |= KEYB_SPRINT; //lshift
				if(key == 229) keypressed |= KEYB_SLOW;   //rshift
				if(key == 21) keypressed |= KEYB_FOCDN;   //r
				if(key == 23) keypressed |= KEYB_FOCUP;   //t
				if(key == 9) keypressed |= KEYB_EXPDN;   //f
				if(key == 10) keypressed |= KEYB_EXPUP;   //g
				if(key == 47) keypressed |= KEYB_ANMREW; //[
				if(key == 48) keypressed |= KEYB_ANMFFW; //]
				if(key == 0x1d) keypressed |= KEYB_DOWN; //z
				if(key == 0x1b) keypressed |= KEYB_UP; //x
				if(key == 0x1c) keypressed |= KEYB_FOVDN; //y
				if(key == 0x0b) keypressed |= KEYB_FOVUP; //h
				if(key == 41) pleaseexit = 1;
			}
			if(keyevent.type == KEYBOARD_RELEASED) {
				if(key == 0x1a) keypressed2 &= ~LIYB_RIGHT;
				if(key == 0x04) keypressed2 &= ~LIYB_UP;
				if(key == 0x16) keypressed2 &= ~LIYB_LEFT;
				if(key == 0x07) keypressed2 &= ~LIYB_DOWN;

				if(key == 225) keypressed &= ~KEYB_SPRINT; //lshift
				if(key == 229) keypressed &= ~KEYB_SLOW;   //rshift
				if(key == 224) cameraControl ^= 1; //ctrl
				if(key == 19) posttog ^= 1;                 //p
				if(key == 21) keypressed &= ~KEYB_FOCDN;   //r
				if(key == 23) keypressed &= ~KEYB_FOCUP;   //t
				if(key == 9) keypressed &= ~KEYB_EXPDN;   //f
				if(key == 10) keypressed &= ~KEYB_EXPUP;   //g
				if(key == 47) keypressed &= ~KEYB_ANMREW; //[
				if(key == 48) keypressed &= ~KEYB_ANMFFW; //]
				if(key == 0x1d) keypressed &= ~KEYB_DOWN; //z
				if(key == 0x1b) keypressed &= ~KEYB_UP; //x
				if(key == 0x1c) keypressed &= ~KEYB_FOVDN; //y
				if(key == 0x0b) keypressed &= ~KEYB_FOVUP; //h
				if(key == 80) frame--; //larrow
				if(key == 79) frame++; //rarrow
				if(key == 98) { sndpause ^=1; ASND_Pause(sndpause); } //numpad 0
				if(key == 99) frameadvance ^= 1; //numpad .
			}
		}
		buttons |= keypressed2;
		}

		float movespeed = 0.01f;
		float sprintspeed = 0.16f;
		float slowspeed = 0.0025f;		

		u8 movedalready = 0;

		if(keypressed & KEYB_SPRINT) movespeed = sprintspeed;
		if(keypressed & KEYB_SLOW) movespeed = slowspeed;
		if(keypressed & KEYB_FOCUP) Lily_Float += 0.0025f;
		if(keypressed & KEYB_FOCDN) Lily_Float -= 0.0025f;
		if(keypressed & KEYB_EXPUP) exposure += 0.02f;
		if(keypressed & KEYB_EXPDN) exposure -= 0.02f;
		if(keypressed & KEYB_ANMREW) frame -= 4;
		if(keypressed & KEYB_ANMFFW) frame += 4;

		if(keypressed & KEYB_FOVDN) camFov -= 0.03f * camFov;
		if(keypressed & KEYB_FOVUP) camFov += 0.03f * camFov;
		if(keypressed & KEYB_DOWN) cam.z -= movespeed;
		if(keypressed & KEYB_UP) cam.z += movespeed;

		if(MOUSE_IsConnected()) {
		mouse_event mouseevent;
		mouseevent.rx = 0;
		mouseevent.ry = 0;
		mouseevent.rz = 0;
		double mousesens = 0.00005f;
		while(MOUSE_GetEvent(&mouseevent)) {
			camrot.z -= (float)mouseevent.rx * mousesens * camFov;
			camrot.x -= (float)mouseevent.ry * (mousesens * 2.0f) * camFov;
		}
		}

		if(cameraControl) {
		//the dpad is setup for horizontal wii remote
		if(buttons & LIYB_RIGHT) {  //vert. up
			cam.y += movespeed * cos(-camrot.z);
			cam.x += movespeed * sin(-camrot.z);
			movedalready = 1;
		}
		if(buttons & LIYB_DOWN) {//vert. right
			cam.y += movespeed * sin(camrot.z);
			cam.x += movespeed * cos(camrot.z);
			movedalready = 1;
		}
		if(buttons & LIYB_LEFT) { //vert. down
			cam.y -= movespeed * cos(-camrot.z);
			cam.x -= movespeed * sin(-camrot.z);
			movedalready = 1;
		}
		if(buttons & LIYB_UP) { //vert. left
			cam.y -= movespeed * sin(camrot.z);
			cam.x -= movespeed * cos(camrot.z);
			movedalready = 1;
		}
		if(buttons & LIYB_1) {
			camrot.z += movespeed * camFov * 0.02f;
		}
		if(buttons & LIYB_2) {
			camrot.z -= movespeed * camFov * 0.02f;
		}
		if(WPAD_ButtonsDown(0)&WPAD_BUTTON_B) {
			//Other_Int--;
			cameraControl ^= 1;
		}
		
		if(gcconnected) {
			gbuttonsheld = PAD_ButtonsHeld(0);
			gbuttonsdown = PAD_ButtonsDown(0);
			if(gbuttonsdown & PAD_BUTTON_START) pleaseexit++;
			
			float looksens = 0.05f;
			float oneslashonetwentyeight = 1.0f / 128.0f;
			float oneslashtwohundredfiftysix = 1.0f / 256.0f;
			//camrot.x += (float)PAD_SubStickY(0) * oneslashonetwentyeight * looksens * camFov * 0.02f;
			camrot.z -= (float)PAD_SubStickX(0) * oneslashonetwentyeight * looksens * camFov * 0.02f;
			//camrot.x = liym_min(camrot.x, 3.14f);
			//camrot.x = liym_max(camrot.x, 0.0f);
			if(!movedalready) {
				//updown
				cam.y += movespeed * cos(-camrot.z) * oneslashonetwentyeight * (float)PAD_StickY(0);
				cam.x += movespeed * sin(-camrot.z) * oneslashonetwentyeight * (float)PAD_StickY(0);
				//lr
				cam.y += movespeed * sin(camrot.z) * oneslashonetwentyeight * (float)PAD_StickX(0);
				cam.x += movespeed * cos(camrot.z) * oneslashonetwentyeight * (float)PAD_StickX(0);
			}
			if(gbuttonsdown&PAD_BUTTON_A) Other_Int++;
			if(gbuttonsdown&PAD_BUTTON_B) Other_Int--;
			if(gbuttonsdown&PAD_TRIGGER_Z) cameraControl ^= 1;
			if(gbuttonsheld&PAD_BUTTON_X) Lily_Float += 0.0025f;
			if(gbuttonsheld&PAD_BUTTON_Y) Lily_Float -= 0.0025f;
			if(gbuttonsheld&PAD_BUTTON_UP) camFov -= 0.03f * camFov;
			if(gbuttonsheld&PAD_BUTTON_DOWN) camFov += 0.03f * camFov;
			if(gbuttonsheld&PAD_BUTTON_RIGHT) exposure += 0.02f;
			if(gbuttonsheld&PAD_BUTTON_LEFT) exposure -= 0.02f;
			//if(gbuttonsheld) printf("%f, %d\n", Lily_Float, frame);
		}

		} else { //if !cameracontrol, buttons control playback instead of camera
			#ifdef GUY_CONCON
			if(wbuttonsdown & WPAD_BUTTON_A) frameadvance ^= 1;
			if(wbuttonsdown & WPAD_BUTTON_B) cameraControl ^= 1;
			if(wbuttonsdown & WPAD_BUTTON_UP) frame--;
			if(wbuttonsdown & WPAD_BUTTON_DOWN) frame++;
			if(wbuttonsheld & WPAD_BUTTON_1) frame -= 4;
			if(wbuttonsheld & WPAD_BUTTON_2) frame += 4;
			#endif

			if(gcconnected) {
				gbuttonsheld = PAD_ButtonsHeld(0);
				gbuttonsdown = PAD_ButtonsDown(0);
				if(gbuttonsdown & PAD_BUTTON_START) pleaseexit++;
				#ifdef GUY_CONCON
				if(gbuttonsdown & PAD_BUTTON_A) frameadvance ^= 1;
				if(gbuttonsdown & PAD_BUTTON_LEFT) frame--;
				if(gbuttonsdown & PAD_BUTTON_RIGHT) frame++;
				if(gbuttonsheld & PAD_BUTTON_Y) frame -= 4;
				if(gbuttonsheld & PAD_BUTTON_X) frame += 4;
				//if(gbuttonsdown & PAD_BUTTON_Y) whatever -= 1;
				//if(gbuttonsdown & PAD_BUTTON_X) whatever += 1;
				if(gbuttonsdown&PAD_TRIGGER_Z) cameraControl ^= 1;
				#endif
				//if(gbuttonsheld & PAD_TRIGGER_L) Lily_Float -= 0.02f;
				//if(gbuttonsheld & PAD_TRIGGER_R) Lily_Float += 0.02f;
				//if(gbuttonsheld) printf("%f, %d\n", Lily_Float, frame);



				//lala
				gbuttonsheld = PAD_ButtonsHeld(1);
				gbuttonsdown = PAD_ButtonsDown(1);
				//if(gbuttonsheld & PAD_BUTTON_Y) realfov -= 0.05f;
				//if(gbuttonsheld & PAD_BUTTON_X) realfov += 0.05f;
			}
			float * camframe = &CamerafinalseqData[frame * CamerafinalseqPrimcount];
			cam.x = camframe[0]; cam.y = camframe[1]; cam.z = camframe[2];
			camrot.x = camframe[3]; camrot.y = camframe[4]; camrot.z = camframe[5];
			camFov = camframe[6]; Lily_Float = camframe[7]; exposure = camframe[8];
		}
		if(pleaseexit) {ASND_Pause(1);break;}
		
		//lensOffsets.z += 0.00001f * (float)PAD_StickY(1);
		//lensOffsets.x += 0.00001f * (float)PAD_StickX(1);
		//printf("{%f, %f, %f}\n", lensOffsets.x, lensOffsets.y, lensOffsets.z);
		//printf("%f\n", realfov);

		//Scene control
		if(cameraControl) {
			scene = Other_Int;
			//scene = 3;
		} else {
			scene = 0; //field
			if(frame > 183) scene = 6; //field (skybox only)
			if(frame > 367) scene = 0; //field
			if(frame > 1017) scene = 2; //cafeteria
			if(frame > 1851) scene = 4; //doodle
			if(frame > 2574) scene = 5; //ether
			if(frame > 2620) scene = 1; //lockers
			if(frame > 2822) scene = 3; //bedroom
			if(frame > 2939) scene = 0; //field
			if(frame > 3017) scene = 1; //lockers
			if(frame > 3114) scene = 0; //field
			if(frame > 3287) scene = 5; //ether
			if(frame > 3361) scene = 3; //bedroom
			if(frame > 3730) scene = 1;
			Other_Int = scene;
		}
		
		exposure = (scene == 4) ? 0.50 : 0.65;
		exposure = liym_max(exposure, 0.009f);
		exposure = liym_min(exposure, 0.992f);
		Lily_Float = liym_max(Lily_Float, 0.009f);
		Lily_Float = liym_min(Lily_Float, 0.992f);
		camFov = liym_max(camFov, 0.001);
		camFov = liym_min(camFov, 179.9f);
		frame = (frame > 5000) ? 0 : frame;

		//Precorr resolution control
		//stats...
		//do we target 60fps or 30fps?
		//field is below 30fps at default res
		//Lockers is max like 45fps at default res
		//Cafeteria is over 60fps at default res
		//Doodle is over 60fps at maximum res
		//bedroom is like 45fps at default res
		
		int precorrx, precorry;
		switch(scene) {
			case 0: //field
				precorrx = 336;
				precorry = 240;
				break;
			case 1: //lockers
				precorrx = 400;
				precorrx = 528;
			case 2: //cafeteria
			case 3: //bedroom
			case 4: //doodle
			case 5: //ether
			case 6: //notebookonly
				precorrx = 640; //fking maximum
				precorry = 528;
				break;
			default:
				precorrx = 336;
				precorry = 400;
				break;
		}
		
		//Raise the initial render resolution, so the text at the start is fking readable
		//This puts fps very low and makes very poor impression.. is this worth it?
		if(frame < 184) {
			precorrx = 640;
			precorry = 528;
		}
		precorrx = 352;
		precorry = 480;

		//Stop control
		stops = 0;
		if(frame >= 908 && frame < 1018) stops = 2;
		if(frame >= 1065 && frame < 1289) stops = 4;
		if(frame >= 3818 && frame < 3915) stops = 2;

		doGillReflect = (scene == 3) ? 1 : 0;

		//Comic scene control
		comic = frame - 2822;
		if(comic < 0) comic = 0;
		if(comic > 166) comic = 0;

		//Firstperson gill
		gillheadspecial = 0;
		if(frame > 574) gillheadspecial = 1;
		if(frame > 673) gillheadspecial = 0;
		if(frame > 770) gillheadspecial = 1;
		if(frame > 879) gillheadspecial = 0;

		//if(frame > 1529) gillheadspecial = 1;
		//if(frame > 1551) gillheadspecial = 0;
		if(frame > 1575) gillheadspecial = 1;
		if(frame > 1656) gillheadspecial = 0;

		//BLENDSHAPING HERE
		u64 startblendtime = gettime();
		float blendtimems = 420.69f;

			//Gill
			//Mouth
			float * gillMouthBlendshapeWeights = &gillMouthshapeanimData[frame * gillMouthshapeanimPrimcount];
			for(int i = 0; i < gillMouthBasistricount * 9; i++) {
				gillMouthShapedVertpos[i] = gillMouthFullsizevertpos[0][i];
				gillMouthShapedVertnor[i] = gillMouthFullsizevertnor[0][i];
			} //That set the output to the basis
			for(int i = 0; i < 7; i++) { //7 blendshapes
				float wght = gillMouthBlendshapeWeights[i];
				if(wght < 0.01f && wght > -0.01f) continue;
				for(int j = 0; j < gillMouthBasistricount * 9; j++) {
					gillMouthShapedVertpos[j] += gillMouthDeltavertpos[i][j] * wght;
					gillMouthShapedVertnor[j] += gillMouthDeltavertnor[i][j] * wght;
				}
			}
			//Teeth
			float gillToothWeight = gillTeethshapeanimData[frame];
			if(gillToothWeight > 0.01f || gillToothWeight < -0.01f) {
				for(int i = 0; i < gillTeethClosedtricount * 9; i++) {
					gillTeethShapedvertpos[i] = gillTeethFullsizevertpos[0][i] + (gillTeethDeltavertpos[i] * gillToothWeight);
					gillTeethShapedvertnor[i] = gillTeethFullsizevertnor[0][i] + (gillTeethDeltavertnor[i] * gillToothWeight);
				}
			} else { 
				for(int i = 0; i < gillTeethClosedtricount * 9; i++) {
					gillTeethShapedvertpos[i] = gillTeethFullsizevertpos[0][i];
					gillTeethShapedvertnor[i] = gillTeethFullsizevertnor[0][i];
				}
			}
			//Eyes
			float gilleyewght = gillEyeShapeanimFinalseqData[frame];
			if(gilleyewght > 0.01f || gilleyewght < -0.01f) {
				for(int i = 0; i < gillEyeBlackBasistricount * 9; i++) {
					gillEyeBlackShapedVertpos[i] = gillEyeBlackFullsizeVertpos[0][i] + (gillEyeBlackDeltavertpos[i] * gilleyewght);
				}
				for(int i = 0; i < gillEyeWhiteBasistricount * 9; i++) {
					gillEyeWhiteShapedVertpos[i] = gillEyeWhiteFullsizeVertpos[0][i] + (gillEyeWhiteDeltavertpos[i] * gilleyewght);
				}
			} else { 
				for(int i = 0; i < gillEyeBlackBasistricount * 9; i++) {
					gillEyeBlackShapedVertpos[i] = gillEyeBlackFullsizeVertpos[0][i];
				}
				for(int i = 0; i < gillEyeWhiteBasistricount * 9; i++) {
					gillEyeWhiteShapedVertpos[i] = gillEyeWhiteFullsizeVertpos[0][i];
				}
			}

			//lake
			//Mouth
			float * lakeMouthBlendshapeWeights = &lakeMouthshapeanimData[frame * lakeMouthshapeanimPrimcount];
			for(int i = 0; i < lakeMouthBasistricount * 9; i++) {
				lakeMouthShapedVertpos[i] = lakeMouthFullsizevertpos[0][i];
				lakeMouthShapedVertnor[i] = lakeMouthFullsizevertnor[0][i];
			} //That set the output to the basis
			for(int i = 0; i < 7; i++) { //7 blendshapes
				float wght = lakeMouthBlendshapeWeights[i];
				if(wght < 0.01f && wght > -0.01f) continue;
				for(int j = 0; j < lakeMouthBasistricount * 9; j++) {
					lakeMouthShapedVertpos[j] += lakeMouthDeltavertpos[i][j] * wght;
					lakeMouthShapedVertnor[j] += lakeMouthDeltavertnor[i][j] * wght;
				}
			}
			//Teeth
			float lakeToothWeight = lakeTeethshapeanimData[frame];
			if(lakeToothWeight > 0.01f || lakeToothWeight < -0.01f) {
				for(int i = 0; i < lakeTeethClosedtricount * 9; i++) {
					lakeTeethShapedvertpos[i] = lakeTeethFullsizevertpos[0][i] + (lakeTeethDeltavertpos[i] * lakeToothWeight);
					lakeTeethShapedvertnor[i] = lakeTeethFullsizevertnor[0][i] + (lakeTeethDeltavertnor[i] * lakeToothWeight);
				}
			} else {
				for(int i = 0; i < lakeTeethClosedtricount * 9; i++) {
					lakeTeethShapedvertpos[i] = lakeTeethFullsizevertpos[0][i];
					lakeTeethShapedvertnor[i] = lakeTeethFullsizevertnor[0][i];
				}
			}
			//river
			//Mouth
			float * riverMouthBlendshapeWeights = &riverMouthshapeanimData[frame * riverMouthshapeanimPrimcount];
			u8 calcrivermouth = 1;
			if(frame > 771 && frame < 881) calcrivermouth = 1;
			if(frame > 1530) calcrivermouth = 1;
			if(calcrivermouth) {
				for(int i = 0; i < riverMouthBasistricount * 9; i++) {
					riverMouthShapedvertpos[i] = riverMouthFullsizevertpos[0][i];
					riverMouthShapedvertnor[i] = riverMouthFullsizevertnor[0][i];
				} //That set the output to the basis
				for(int i = 0; i < 4; i++) {
					float wght = riverMouthBlendshapeWeights[i];
					//if(i == 0) wght = 1.0f;
					if(wght < 0.01f && wght > -0.01f) continue;
					for(int j = 0; j < riverMouthBasistricount * 9; j++) {
						riverMouthShapedvertpos[j] += riverMouthDeltavertpos[i][j] * wght;
						riverMouthShapedvertnor[j] += riverMouthDeltavertnor[i][j] * wght;
					}
				}
			} //Just using last frame's calculation is fine
		u64 blendtime = gettime() - startblendtime;	
		blendtimems = (float)ticks_to_microsecs(blendtime) / 1000.0f;		

		for(eye = 0; eye < 2; eye++) {
		if(!cameraControl) {
			float * camframe = &CamerafinalseqData[frame * CamerafinalseqPrimcount];
			cam.x = camframe[0]; cam.y = camframe[1]; cam.z = camframe[2];
			camrot.x = camframe[3]; camrot.y = camframe[4]; camrot.z = camframe[5];
			camFov = camframe[6]; Lily_Float = camframe[7]; exposure = camframe[8];
		}
		Mtx peee;
		guMtxIdentity(peee);
		//guMtxRotRad(model, 'x', -1.5707);
		//guMtxScale(peee, 1.0f, 1.0f, -1.0f);
		//guMtxConcat(peee, model, peee);
		//guMtxConcat(headsetorient, peee, model);
		if(comic) guMtxIdentity(model); else guMtxCopy(headsetorient, model);
		liyt_genMtxPosRotZyx(view, cam.x, cam.y, cam.z, camrot.x, camrot.y, camrot.z);
		guMtxConcat(view, model, view);
		guMtxInverse(view, view);
		guMtxCopyNopos(view, viewreflec);
		guLookAt(overlayView, &overlayCam, &overlayUp, &overlaylook);
		if(comic) guPerspective(perspective, camFov * 1.15, aspect, 0.1F, 300.0F); 
		else guPerspective(perspective, realfov, aspect, 0.01F, 60.0F);
		guPerspective(overlayPersp, 100, aspect, 0.1F, 300.0F);

		//We shift viewpoint, and shift perspective leftright
		//With viewpoint shift, image is pop in and pop out
		//With pespective shift added, we can shift the .. pop point ?

		//Ipd 68mm, 0.034
		if(!comic)
		switch(eye) {
			case 0:
				guMtxTransApply(view, view, 0.034f, 0.0f, 0.0f);
				//guMtxTransApply(overlayView, overlayView, 0.005f, 0.0f, 0.0f);
				//perspective[0][2] = -lensOffsets.x;
				perspective[0][2] = -0.25f;
				overlayPersp[0][2] = -0.3;
				break;
			case 1:
				guMtxTransApply(view, view, -0.034f, 0.0f, 0.0f);
				//guMtxTransApply(overlayView, overlayView,-0.005f, 0.0f, 0.0f);
				//perspective[0][2] = lensOffsets.x;
				perspective[0][2] = 0.25f;
				overlayPersp[0][2] = 0.3;
		}

		GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

		//Skeleton here
		u64 startskeletime = gettime();
		float skeletimems = 420.69f;

		gillSkelFrame = &gillBonefinalseqData[frame * gillBonefinalseqPrimcount];
		if(doGillReflect) {
			c_guMtxReflect(model, &(guVector){2.807f, 0.0f, 0.0f}, &(guVector){1.0f, 0.0f, 0.0f});
			for(int i = 0; i < 58; i++) {
				int i7 = i*7;
				liyt_genMtxPosRotZyxScale(gillSkeletonMv[i], gillSkelFrame[i7], gillSkelFrame[i7+1], gillSkelFrame[i7+2],
				                                             gillSkelFrame[i7+3], gillSkelFrame[i7+4], gillSkelFrame[i7+5],
				                                             gillSkelFrame[i7+6], gillSkelFrame[i7+6], gillSkelFrame[i7+6]);
				guMtxConcat(model, gillSkeletonMv[i], gillSkeletonMvReflect[i]);
				guMtxCopyNopos(gillSkeletonMv[i], gillSkeletonMvReflectionmap[i]);
				guMtxCopyNopos(gillSkeletonMvReflect[i], gillSkeletonMvReflectReflectionmap[i]);
				guMtxConcat(view, gillSkeletonMv[i], gillSkeletonMv[i]);
				guMtxConcat(view, gillSkeletonMvReflect[i], gillSkeletonMvReflect[i]);
				guMtxConcat(viewreflec, gillSkeletonMvReflectionmap[i], gillSkeletonMvReflectionmap[i]);
				guMtxConcat(viewreflec, gillSkeletonMvReflectReflectionmap[i], gillSkeletonMvReflectReflectionmap[i]);
				guMtxScaleApply(gillSkeletonMvReflectionmap[i], gillSkeletonMvReflectionmap[i], 0.5f, -0.5f, 0.0f);
				guMtxTransApply(gillSkeletonMvReflectionmap[i], gillSkeletonMvReflectionmap[i], 0.5f, 0.5f, 1.0f);
				guMtxScaleApply(gillSkeletonMvReflectReflectionmap[i], gillSkeletonMvReflectReflectionmap[i], 0.5f, -0.5f, 0.0f);
				guMtxTransApply(gillSkeletonMvReflectReflectionmap[i], gillSkeletonMvReflectReflectionmap[i], 0.5f, 0.5f, 1.0f);
			}
		} else {
			for(int i = 0; i < 58; i++) {
				int i7 = i*7;
				liyt_genMtxPosRotZyxScale(gillSkeletonMv[i], gillSkelFrame[i7], gillSkelFrame[i7+1], gillSkelFrame[i7+2],
				                                             gillSkelFrame[i7+3], gillSkelFrame[i7+4], gillSkelFrame[i7+5],
				                                             gillSkelFrame[i7+6], gillSkelFrame[i7+6], gillSkelFrame[i7+6]);
				guMtxCopyNopos(gillSkeletonMv[i], gillSkeletonMvReflectionmap[i]);
				guMtxConcat(view, gillSkeletonMv[i], gillSkeletonMv[i]);
				guMtxConcat(viewreflec, gillSkeletonMvReflectionmap[i], gillSkeletonMvReflectionmap[i]);
				guMtxScaleApply(gillSkeletonMvReflectionmap[i], gillSkeletonMvReflectionmap[i], 0.5f, -0.5f, 0.0f);
				guMtxTransApply(gillSkeletonMvReflectionmap[i], gillSkeletonMvReflectionmap[i], 0.5f, 0.5f, 1.0f);
			}

			riverSkelFrame = &riverBonefinalseqData[frame * riverBonefinalseqPrimcount];
			for(int i = 0; i < 51; i++) {
				int i7 = i*7;
				liyt_genMtxPosRotZyxScale(riverSkeletonMv[i], riverSkelFrame[i7], riverSkelFrame[i7+1], riverSkelFrame[i7+2],
				                                              riverSkelFrame[i7+3], riverSkelFrame[i7+4], riverSkelFrame[i7+5],
				                                              riverSkelFrame[i7+6], riverSkelFrame[i7+6], riverSkelFrame[i7+6]);
				guMtxCopyNopos(riverSkeletonMv[i], riverSkeletonMvReflectionmap[i]);
				guMtxConcat(view, riverSkeletonMv[i], riverSkeletonMv[i]);
				guMtxConcat(viewreflec, riverSkeletonMvReflectionmap[i], riverSkeletonMvReflectionmap[i]);
				guMtxScaleApply(riverSkeletonMvReflectionmap[i], riverSkeletonMvReflectionmap[i], 0.5f, -0.5f, 0.0f);
				guMtxTransApply(riverSkeletonMvReflectionmap[i], riverSkeletonMvReflectionmap[i], 0.5f, 0.5f, 1.0f);
			}

			lakeSkelFrame = &lakeBonefinalseqData[frame * lakeBonefinalseqPrimcount];
			for(int i = 0; i < 59; i++) {
				int i7 = i*7;
				liyt_genMtxPosRotZyxScale(lakeSkeletonMv[i], lakeSkelFrame[i7], lakeSkelFrame[i7+1], lakeSkelFrame[i7+2],
				                                             lakeSkelFrame[i7+3], lakeSkelFrame[i7+4], lakeSkelFrame[i7+5],
				                                             lakeSkelFrame[i7+6], lakeSkelFrame[i7+6], lakeSkelFrame[i7+6]);
				guMtxCopyNopos(lakeSkeletonMv[i], lakeSkeletonMvReflectionmap[i]);
				guMtxConcat(view, lakeSkeletonMv[i], lakeSkeletonMv[i]);
				guMtxConcat(viewreflec, lakeSkeletonMvReflectionmap[i], lakeSkeletonMvReflectionmap[i]);
				guMtxScaleApply(lakeSkeletonMvReflectionmap[i], lakeSkeletonMvReflectionmap[i], 0.5f, -0.5f, 0.0f);
				guMtxTransApply(lakeSkeletonMvReflectionmap[i], lakeSkeletonMvReflectionmap[i], 0.5f, 0.5f, 1.0f);
			}
		}
		if(frame > 415 && frame < 530) {
			float * flutterbyFrame = &flutterbybonefinalseqData[(frame - 415) * flutterbybonefinalseqPrimcount];
			for(int i = 0; i < 3; i++) {
				int i9 = i*9;
				liyt_genMtxPosRotZyx(flutterbyMv[i], flutterbyFrame[i9], flutterbyFrame[i9+1], flutterbyFrame[i9+2],
				                                     flutterbyFrame[i9+3], flutterbyFrame[i9+4], flutterbyFrame[i9+5]);
				guMtxConcat(view, flutterbyMv[i], flutterbyMv[i]);
			}
		}

		if(frame > 2695 && frame < 2989) {
			float * jacketpropframe = &riverjacketpropboneanimData[(frame - 2689) * riverjacketpropboneanimPrimcount];
			for(int i = 0; i < 4; i++) {
				int i7 = i*7;
				c_guMtxReflect(model, &(guVector){2.807f, 0.0f, 0.0f}, &(guVector){1.0f, 0.0f, 0.0f});
				liyt_genMtxPosRotZyx(jacketpropmv[i], jacketpropframe[i7], jacketpropframe[i7+1], jacketpropframe[i7+2],
				                                      jacketpropframe[i7+3], jacketpropframe[i7+4], jacketpropframe[i7+5]);
				guMtxCopyNopos(jacketpropmv[i], jacketpropmvrefmap[i]);
				guMtxConcat(model, jacketpropmv[i], jacketproprefmv[i]);
				guMtxConcat(view, jacketpropmv[i], jacketpropmv[i]);
				guMtxConcat(view, jacketproprefmv[i], jacketproprefmv[i]);
				guMtxConcat(viewreflec, jacketpropmvrefmap[i], jacketpropmvrefmap[i]);
				guMtxScaleApply(jacketpropmvrefmap[i], jacketpropmvrefmap[i], 0.5f, -0.5f, 0.0f);
				guMtxTransApply(jacketpropmvrefmap[i], jacketpropmvrefmap[i], 0.5f, 0.5f, 1.0f);
			}
		}

		//Process prop anim data here why not
	
		float * allpropsFrame = &guyAllpropAnimfullseqData[frame * guyAllpropAnimfullseqPrimcount];
		float * gillpenciltransform = allpropsFrame;
		float * gillnotebooktransform = &allpropsFrame[6];
		float * cafedoortransform = &allpropsFrame[12];
		float * openablelockers = &allpropsFrame[18];
		float * matchtransform = &allpropsFrame[20];
		float * matchboxtransform = &allpropsFrame[26];
		float * gillBedroomdoorrot = &allpropsFrame[32];
		int * notebookmatcomp = (int *)&allpropsFrame[33];
		float * trashtransform = &allpropsFrame[34];

		u64 skeletime = gettime() - startskeletime;	
		skeletimems = (float)ticks_to_microsecs(skeletime) / 1000.0f;	

		GX_SetNumTexGens(1);

		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);	
		GX_SetColorUpdate(GX_TRUE);
		GX_SetAlphaUpdate(GX_FALSE);

		setlight(view, background, scene);

		GX_SetTevOp(GX_TEVSTAGE0,GX_REPLACE);

		//GX_TEXMAP0 - dynamic, required by dof
		//GX_TEXMAP1 - dynamic, required by dof
		//GX_TEXMAP2 - dynamic, required by dof
		//GX_TEXMAP3 - dynamic, required by dof
		//GX_TEXMAP4 - bark / hallway0TexObj / used by dof anyway lol
		//GX_TEXMAP5 - skyboxTexObj / hallwaytilesTexObj / used by dof anyway lol
		//GX_TEXMAP6 - foliage1TexObj / hallway1TexObj
		//GX_TEXMAP7 - gill texture

		switch(scene) {
			case 0:
				GX_LoadTexObj(&barkTexObj, GX_TEXMAP4);
				GX_LoadTexObj(&skyboxTexObj, GX_TEXMAP5);
				GX_LoadTexObj(&foliage1TexObj, GX_TEXMAP6);
				break;
			case 1:
				GX_LoadTexObj(&hallway0TexObj, GX_TEXMAP4);
				GX_LoadTexObj(&hallwaytilesTexObj, GX_TEXMAP5);
				GX_LoadTexObj(&hallway1TexObj, GX_TEXMAP6);
				break;
		}
	
		procedtex_randomtex_update();

		if(!eye) {
		//Create matcap
		//if you take this code, uncomment this line
		//GX_SetCopyFilter(GX_FALSE, NULL, GX_TRUE, perfectfilter);
		//i wont because it'd significantly change the appearance of the demo from as it was released
		//but it is objectively correct
		GX_SetCopyFilter(GX_FALSE, NULL, GX_TRUE, vfilter);

		setlight(view, background, scene);
		GX_SetChanAmbColor(GX_COLOR0A0,LC_BLACK); //Add ambient back in tev ..
		GX_SetChanMatColor(GX_COLOR0A0,LC_WHITE);
		GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, whichlightmask(scene), GX_DF_CLAMP, GX_AF_NONE);
		GX_SetNumTevStages(1);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_RASC, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		lightcap(lightcapbuf);
		GX_PixModeSync();
		GX_InitTexObj(&matcap1TexObj, lightcapbuf, 128, 128, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
		GX_InvalidateTexAll();

		//Finalize matcap with ambient-based rim light
		GX_SetNumTexGens(1);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
		GX_SetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
		GX_SetTevKColor(GX_KCOLOR0, colorMul(whatambcol(scene), LC_RIMTILT));
		GX_SetNumTevStages(2);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
		GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_C0);
		GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);
		//GX_SetZMode(GX_FALSE, GX_NEVER, GX_FALSE);
		GX_SetZMode(GX_ALWAYS, GX_TRUE, GX_TRUE);
		GX_LoadTexObj(&rimlightTexObj, GX_TEXMAP0);
		GX_LoadTexObj(&matcap1TexObj, GX_TEXMAP1);
		GX_LoadPosMtxImm(fullQuadMv, GX_PNMTX0);
		GX_SetCurrentMtx(GX_PNMTX0);
		liyrub_drawScreenquad(1.0f);
		GX_CopyTex(lightcapbuf, GX_TRUE);
		GX_PixModeSync();
		GX_InitTexObj(&matcap1TexObj, lightcapbuf, 128, 128, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
		GX_InvalidateTexAll();
		}
		resetViewport();
		GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

		//GX_SetViewport(0,0,352,480,0,1);
		//GX_SetScissor(0,0,352,480);
		GX_SetViewport(0,0, precorrx, precorry,0,1);
		GX_SetScissor(0,0, precorrx, precorry);

		if(comic) {
			Mtx44 eohfeghau;
			guPerspective(eohfeghau, 45, 1.0f, 0.1F, 300.0F);
			GX_LoadProjectionMtx(eohfeghau, GX_PERSPECTIVE);
			GX_SetViewport(0, 0, 640, 528, 0, 1);
			GX_SetScissor(0, 0, 640, 528);
			GX_SetColorUpdate(GX_FALSE);
			//bandaid .. clear for whole efb
			guMtxTrans(model, 0.0f, 0.0f, -100.0f);
			GX_LoadPosMtxImm(model, GX_PNMTX0);
			GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
			GX_SetNumTevStages(1);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
			liy_VtxDescConfig(LV_VP, 0);
			GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
			GX_Position3f32(-1000.0f, -1000.0f, 0.0f);	
			GX_Position3f32(1000.0f, -1000.0f, 0.0f);
			GX_Position3f32(1000.0f, 1000.0f, 0.0f);
			GX_Position3f32(-1000.0f, 1000.0f, 0.0f);
			GX_SetColorUpdate(GX_TRUE);

			GX_SetViewport(0,0,256,256,0,1);
			GX_SetScissor(0,0,256,256);
			guPerspective(perspective, camFov * 1.15, (frame - 2822) > 115 ? 2.0f : 1.0f, 0.1F, 300.0F);
			GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
			if((frame - 2822) > 115) {
				GX_SetViewport(0,0,512,256,0,1);
				GX_SetScissor(0,0,512,256);
			}
		}

		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

		if ((scene == 0 || scene == 6) && !(comic && eye)) {
			//A skybox

			GX_SetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
			GX_SetNumTevStages(1);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_ONE, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP5, GX_COLOR0A0);
			GX_SetNumTexGens(1);
			GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
			GX_SetZMode(GX_FALSE, GX_NEVER, GX_FALSE);	
			liy_VtxDescConfig(LV_VPIDX16 | LV_TCIDX16, 0);
			guMtxIdentity(model);
			guMtxRotDeg(model, 'z', 0.025f * (float)rframe);
			guMtxTransApply(model, model, cam.x, cam.y, cam.z);
			guMtxConcat(view,model,modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			GX_SetCurrentMtx(GX_PNMTX0);

			GX_SetCullMode(GX_CULL_NONE);
			ag_draw_skybox(0);

			GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);	
		}
		if(scene == 2) {
			nolight();
			GX_SetNumTevStages(1);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
			guMtxIdentity(model);
			guMtxConcat(view, model, model);
			GX_LoadPosMtxImm(model, GX_PNMTX0);
			GX_SetCullMode(GX_CULL_NONE);
			ag_config_gaywindows(0);
			ag_draw_gaywindows(0);
			GX_SetCullMode(GX_CULL_FRONT);
			GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		}
		if(scene == 3 && !(comic && eye)) {
			
			GX_SetCurrentMtx(GX_PNMTX0);
			GX_SetNumTevStages(1);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_RASC, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
			nolight();
			ag_config_gillBedroom(0);

			guMtxIdentity(model);
			guMtxConcat(view, model, modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			GX_SetCullMode(GX_CULL_FRONT);
			ag_draw_gillBedroom(0);

			c_guMtxReflect(model, &(guVector){2.807f, 0.0f, 0.0f}, &(guVector){1.0f, 0.0f, 0.0f}); // Paired single one is just broken lawl
			guMtxConcat(view, model, modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			GX_SetCullMode(GX_CULL_BACK);
			ag_draw_gillBedroom(0);


		}

		// Rigged gill	
		if(!(comic && eye)) {
			if(frame > 2857 && frame < 2941) { gilljacketed = YES; }
			else { gilljacketed = NO; }
			gillReflect = 0;
			for(int i = 0; i < doGillReflect + 1; i++) {
				setlight(view, background, scene);
				gillCullMode();
				Mtx * gillMatrices = (gillReflect) ? gillSkeletonMvReflect : gillSkeletonMv;
				Mtx * gillRefMatrices = (gillReflect) ? gillSkeletonMvReflectReflectionmap : gillSkeletonMvReflectionmap;

				GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
				GX_SetNumTexGens(1); //Vertex colored non textured matcapped
				GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);
				GX_LoadTexObj(&matcap1TexObj, GX_TEXMAP0);
				GX_SetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
				GX_SetTevKColor(GX_KCOLOR0, whatambcol(scene));
				GX_SetNumTevStages(3);
				GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_KONST, GX_CC_ZERO);
				GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
				GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
				GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ONE, GX_CC_C0, GX_CC_C1);
				GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
				GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
				GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

				if(!gillheadspecial) {

				GX_SetCurrentMtx(GX_PNMTX0);
				LX_LoadMtxImm(gillMatrices[4], GX_PNMTX0);
				GX_LoadTexMtxImm(gillRefMatrices[4], GX_TEXMTX0, GX_MTX3x4);
				liy_VtxDescConfig(LV_VP | LV_VN | LV_VC, 0);
				drawArrConfig(LV_VP | LV_VN | LV_VC, gillMouthBasistricount, 0, 
						gillMouthShapedVertpos, NULL, NULL,
						gillMouthShapedVertnor, NULL, NULL,
						NULL, NULL, NULL,
						NULL,
						gillMouthBasisvertcol);
				LX_LoadMtxImm(gillMatrices[55], GX_PNMTX0);
				drawArrConfig(LV_VP | LV_VN | LV_VC, gillTeethClosedtricount, 0, 
						gillTeethShapedvertpos, NULL, NULL,
						gillTeethShapedvertnor, NULL, NULL,
						NULL, NULL, NULL,
						NULL,
						gillTeethClosedvertcol);

				//Vertex lighting for these
				GX_SetNumTexGens(1);
				GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
				GX_SetNumTevStages(1);
				GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_RASC, GX_CC_ZERO);
				GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

				//for pupil layering:
				//draw eyewhite, turn on compare but turn off z write, draw pupils, then draw eyeblack
				GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, whichlightmask(scene), GX_DF_CLAMP, GX_AF_NONE);
				LX_LoadMtxImm(gillMatrices[50], GX_PNMTX0);
				GX_SetChanMatColor(GX_COLOR0A0, LC_WHITE);
				GX_SetArray(GX_VA_NRM, &gillEyeWhiteBasisvertnor[0], sizeof(f32)*3);
				liy_VtxDescConfig(LV_VP | LV_VNIDX16, 0);
				drawArrConfig(LV_VP | LV_VNIDX16, gillEyeWhiteBasistricount, 0,
						gillEyeWhiteShapedVertpos, NULL, NULL,
						NULL, NULL, gillEyeWhiteBasisvertnoridx,
						NULL, NULL, NULL,
						NULL,
						NULL);

				GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
				GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP7, GX_COLOR0A0);

				GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);

				LX_LoadMtxImm(gillMatrices[52], GX_PNMTX0);
				LX_LoadMtxImm(gillMatrices[51], GX_PNMTX1);
				LX_LoadMtxImm(gillMatrices[56], GX_PNMTX2);
				LX_LoadMtxImm(gillMatrices[57], GX_PNMTX3);
				GX_SetArray(GX_VA_POS, &gillPupilsvertpos[0], sizeof(f32)*3);
				GX_SetArray(GX_VA_NRM, &gillPupilsvertnor[0], sizeof(f32)*3);
				GX_SetArray(GX_VA_TEX0, &gillPupilstexcord[0], sizeof(f32)*2);
				liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX, 0);
				drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX, gillPupilstricount, 0, 
						NULL, NULL, gillPupilsvertposidx,
						NULL, NULL, gillPupilsvertnoridx,
						NULL, NULL, gillPupilstexcordidx,
						gillPupilsmtxidx,
						NULL);

				GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
				GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_ONE, GX_CC_ZERO);
				GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);

				GX_SetChanMatColor(GX_COLOR0A0, LC_BLACK);
				GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, whichlightmask(scene), GX_DF_CLAMP, GX_AF_NONE);
				LX_LoadMtxImm(gillMatrices[50], GX_PNMTX0);
				GX_SetCurrentMtx(GX_PNMTX0);
				GX_SetArray(GX_VA_NRM, &gillEyeBlackBasisvertnor[0], sizeof(f32)*3);
				liy_VtxDescConfig(LV_VP | LV_VNIDX16, 0);
				drawArrConfig(LV_VP | LV_VNIDX16, gillEyeBlackBasistricount, 0,
						gillEyeBlackShapedVertpos, NULL, NULL,
						NULL, NULL, gillEyeBlackBasisvertnoridx,
						NULL, NULL, NULL,
						NULL,
						NULL);
				}

				drawGill();
				gillReflect = (doGillReflect) ? 1 : 0;
			}
			GX_SetNumTexGens(1);
			GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
		}
			
		// Rigged lake
		if(scene != 3) { //we dont make matrices for them on these frames, so itd be pibby glitch if we drew them
			GX_SetCullMode(GX_CULL_FRONT);
			GX_SetNumTexGens(1); //Vertex colored non textured matcapped
			GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);
			GX_SetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
			GX_SetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
			GX_SetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K0);
			GX_SetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K0);
			GX_SetNumTevStages(3);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_KONST, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
			GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
			GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ONE, GX_CC_C0, GX_CC_C1);
			GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

			LX_LoadMtxImm(lakeSkeletonMv[4], GX_PNMTX0);
			GX_LoadTexMtxImm(lakeSkeletonMvReflectionmap[4], GX_TEXMTX0, GX_MTX3x4);
			GX_SetCurrentMtx(GX_PNMTX0);
			liy_VtxDescConfig(LV_VP | LV_VN | LV_VC, 0);
			drawArrConfig(LV_VP | LV_VN | LV_VC, lakeMouthBasistricount, 0, 
					lakeMouthShapedVertpos, NULL, NULL,
					lakeMouthShapedVertnor, NULL, NULL,
					NULL, NULL, NULL,
					NULL,
					lakeMouthBasisvertcol);
			drawArrConfig(LV_VP | LV_VN | LV_VC, lakeTeethClosedtricount, 0, 
					lakeTeethShapedvertpos, NULL, NULL,
					lakeTeethShapedvertnor, NULL, NULL,
					NULL, NULL, NULL,
					NULL,
					lakeTeethClosedvertcol);
			drawLake();

		// River
			GX_SetNumTexGens(1); //Vertex colored non textured matcapped
			GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);
			GX_SetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
			GX_SetNumTevStages(3);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_KONST, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
			GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
			GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ONE, GX_CC_C0, GX_CC_C1);
			GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
			GX_LoadTexObj(&matcap1TexObj, GX_TEXMAP0);
			LX_LoadMtxImm(riverSkeletonMv[5], GX_PNMTX0);
			GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[5], GX_TEXMTX0, GX_MTX3x4);
			GX_SetCurrentMtx(GX_PNMTX0);
			liy_VtxDescConfig(LV_VP | LV_VN | LV_VC, 0);
			drawArrConfig(LV_VP | LV_VN | LV_VC, riverMouthBasistricount, 0, 
					riverMouthShapedvertpos, NULL, NULL,
					riverMouthShapedvertnor, NULL, NULL,
					NULL, NULL, NULL,
					NULL,
					riverMouthBasisvertcol);
			if(frame > 1530 && frame < 1674) {
				LX_LoadMtxImm(riverSkeletonMv[27], GX_PNMTX0);
				GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[27], GX_TEXMTX0, GX_MTX3x4);
				ag_config_retardedlittlesandwich(0);
				ag_draw_retardedlittlesandwich(0);
			}
			riverhappyornot = NOPE;
			if(frame > 832 && frame < 1027) riverhappyornot = SURE;
			if(frame > 2741) { riverjacketed = NUHUH;
			} else { riverjacketed = FINE; }
			drawRiver();
		}
		if(scene == 0 && !(comic && eye)) {
			GX_SetCullMode(GX_CULL_FRONT);
			drawField();

			//field props
			GX_SetChanAmbColor(GX_COLOR0A0, LC_WHITE);
			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);
			GX_SetCurrentMtx(GX_PNMTX0);
			GX_SetNumTevStages(1);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_ONE, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			setlight(view, background, scene);
			if(frame < 1018) {
			liyt_genMtxPosRotZyx(model, gillpenciltransform[0], gillpenciltransform[1], gillpenciltransform[2],
			                            gillpenciltransform[3], gillpenciltransform[4], gillpenciltransform[5]);
			guMtxConcat(view, model, modelview);
			LX_LoadMtxImm(modelview, GX_PNMTX0);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_ONE, GX_CC_ZERO);
			ag_config_gillpencil(0);
			ag_draw_gillpencil(0);
			}
			//maybe demeaning to call a flutterby a prop but its goin in the section yerp i said it
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
			if(frame > 415 && frame < 530) {
				GX_LoadTexObj(&flutterbytexTexObj, GX_TEXMAP0);
				LX_LoadMtxImm(flutterbyMv[0], GX_PNMTX0); 
				LX_LoadMtxImm(flutterbyMv[1], GX_PNMTX1); 
				LX_LoadMtxImm(flutterbyMv[2], GX_PNMTX2);
				GX_SetCurrentMtx(GX_PNMTX0);
				ag_config_flutterby(0);
				ag_draw_flutterby(0);
			}

			int renfield = 1;
			//if(frame > 184 && frame < 575) renfield = 0;
			//if(frame > 673 && frame < 908) renfield = 0;
			if(renfield) { //blocks the aaaction on thes frames

			GX_SetCullMode(GX_CULL_NONE);

			GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);

			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP6, GX_COLOR0A0);

			GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
			GX_SetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP); // no blending for these
			GX_SetAlphaCompare(GX_GEQUAL, 64, GX_AOP_AND, GX_ALWAYS, 0);
			GX_SetAlphaUpdate(GX_FALSE);
			GX_SetZCompLoc(GX_FALSE);

			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);

			float fieldsize = 17.0f;

			ag_config_grassplaneclump(0);

			GX_SetArray(GX_VA_POS, &grassplaneclumpvertpos[0], 12);
			GX_SetArray(GX_VA_TEX0, &grassplaneclumptexcord[0], 8);

			#define GRASSMANY 30
			#define NOTGRASSMANY 20

			float goffx = -4.0f;
			float goffy = 6.0f;
			float goffz = 0.0f; //more of a length tuner

			for(int i = 0; i < GRASSMANY; i++) {
				int i3 = i*3;
				float waveval = sinf(1.42f + (((float)rframe * 0.02) + ((float)i))) * 0.05f;
			
				float grassx = (LotsOfRandoms[i3]*8.0f);
				if(grassx > -10.0f && grassx < -6.5f || grassx < -19.5f) continue;
				float grassy = (LotsOfRandoms[i3+1]*12.0f)+goffy;
				float grassz = (grassx < -7.0f) ? 0.081633f : 0.0f;

				guMtxTrans(model, grassx, grassy, grassz);
				guMtxConcat(view, model, modelview);
				GX_LoadPosMtxImm(modelview, GX_PNMTX0);

				guMtxTrans(model, grassx + waveval, grassy, goffz + (LotsOfRandoms[i3+2]*0.1f));
				guMtxConcat(view, model, modelview);
				GX_LoadPosMtxImm(modelview, GX_PNMTX1);

				GX_CallDispList(grassclumpDispList, grassclumpDispListSize);

			}

			//Greenery

			GX_SetCurrentMtx(GX_PNMTX0);

			//Clipped texture

			liy_VtxDescConfig(LV_VPIDX16 | LV_TCIDX16, 0);
		
			for(int i = 0; i < NOTGRASSMANY; i++) {
				int i9 = i * 12;
				guMtxIdentity(model);
				guMtxTransApply(model, model, flowerPos[i9], flowerPos[i9+1], flowerPos[i9+2]);
				guMtxConcat(view,model,modelview);
				GX_LoadPosMtxImm(modelview, GX_PNMTX0);
				GX_LoadTexMtxImm(daisyPatchTexmtx, GX_TEXMTX0, GX_MTX3x4);
				GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_TEXMTX0);
				ag_draw_daisy_patch(0);

				Mtx butt;
				guMtxRotRad(butt, 'y', 0.2f*sin(flowerPos[i9]));
				guMtxRotRad(model, 'z', flowerPos[i]);
				guMtxConcat(butt, model, model);
				guMtxTransApply(model, model, flowerPos[i9+3], flowerPos[i9+4], flowerPos[i9+5]+0.2f);
				guMtxConcat(view,model,modelview);
				GX_LoadPosMtxImm(modelview, GX_PNMTX0);
				GX_LoadTexMtxImm(dandelionUnfertilizedTexmtx, GX_TEXMTX0, GX_MTX3x4);
				ag_draw_dandelion_unfertilized_flower(0);
	
				guMtxIdentity(model);
				guMtxTransApply(model, model, flowerPos[i9+6], flowerPos[i9+7], flowerPos[i9+8]);
				guMtxConcat(view,model,modelview);
				GX_LoadPosMtxImm(modelview, GX_PNMTX0);
				GX_LoadTexMtxImm(idkflowerTexmtx, GX_TEXMTX0, GX_MTX3x4);
				ag_draw_idkflower(0);
			}

			GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);

			GX_SetAlphaUpdate(GX_FALSE);
			GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
			GX_SetZCompLoc(GX_TRUE);
			GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
			GX_SetCullMode(GX_CULL_NONE);

			//Vertex color

			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_RASC, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
			
			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);

			liy_VtxDescConfig(LV_VPIDX16 | LV_VC, 0);
		
			for(int i = 0; i < NOTGRASSMANY; i++) {
				int i9 = i * 12;

				Mtx butt;
				guMtxRotRad(butt, 'y', 0.2f*sin(flowerPos[i9]));
				guMtxRotRad(model, 'z', flowerPos[i]);
				guMtxConcat(butt, model, model);
				guMtxTransApply(model, model, flowerPos[i9+3], flowerPos[i9+4], flowerPos[i9+5]+0.2f);
				guMtxConcat(view,model,modelview);
				GX_LoadPosMtxImm(modelview, GX_PNMTX0);
				ag_draw_dandelion_unfertilized(0);
	
				guMtxIdentity(model);
				guMtxTransApply(model, model, flowerPos[i9+6], flowerPos[i9+7], flowerPos[i9+8]);
				guMtxConcat(view,model,modelview);
				GX_LoadPosMtxImm(modelview, GX_PNMTX0);
				ag_draw_idkflower_reproductive(0);
			}

			for(int i = 0; i < NOTGRASSMANY; i++) {
				Mtx butt;
				int i9 = i * 12;
				if(flowerPos[i9+10]-2.0f > 6.0f) continue;
				guMtxRotRad(butt, 'y', 0.2f*sin(flowerPos[i9+20]));
				guMtxRotRad(model, 'z', flowerPos[i+3]);
				guMtxConcat(butt, model, model);
				guMtxTransApply(model, model, flowerPos[i9+9], flowerPos[i9+10]-2.0f, flowerPos[i9+11]+0.23f);
				guMtxConcat(view,model,modelview);
				GX_LoadPosMtxImm(modelview, GX_PNMTX0);
				ag_draw_dandelion_seeding(0);
			}
			

			GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);

			GX_SetNumTevStages(1);

			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

			GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
			GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP6, GX_COLOR0A0);

			GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

			liy_VtxDescConfig(LV_VPIDX16 | LV_TCIDX16, 0);

			Mtx dandebillboard;
			guMtxRotRad(dandebillboard, 'x', -1.57f);

			GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);	 //dont update z

			GX_SetAlphaUpdate(GX_TRUE);


			GX_SetCullMode(GX_CULL_NONE);	

			GX_SetArray(GX_VA_POS, &dandelion_seeding_seedsvertpos[0], 12);
			GX_SetArray(GX_VA_TEX0, &dandelion_seeding_seedstexcord[0], 8);
			for(int i = 0; i < NOTGRASSMANY; i++) { //render seeds basically last
				int i9 = i * 12;
				if(flowerPos[i9+10]-2.0f > 6.0f) continue;
				guMtxTrans(model, flowerPos[i9+9], flowerPos[i9+10]-2.0f, flowerPos[i9+11]+0.23f);
				c_guMtxConcatTransOnly(view,model,modelview, 1.0f);
				guMtxConcat(modelview, dandebillboard, modelview);
				GX_LoadPosMtxImm(modelview, GX_PNMTX0);
				ag_draw_dandelion_seeding_seeds(0);
			}


			GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);	

			GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
			GX_SetCullMode(GX_CULL_FRONT);	

			} //end frame limit

		} //endof outsidescene

		if(scene == 1) {
			drawHallway(openablelockers[0], openablelockers[1]);
		}
		if(scene == 2) {
			liyt_genMtxPosRotZyx(model, cafedoortransform[0], cafedoortransform[1], cafedoortransform[2],
			                            cafedoortransform[3], cafedoortransform[4], cafedoortransform[5]);
			GX_SetChanMatColor(GX_COLOR0A0, LC_LIGHTGREY);
			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, whichlightmask(scene), GX_DF_CLAMP, GX_AF_NONE);
			guMtxConcat(view, model, modelview);
			GX_SetCurrentMtx(GX_PNMTX0);
			LX_LoadMtxImm(modelview, GX_PNMTX0);
			GX_SetNumTevStages(1);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_ONE, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			ag_config_cafedoor_left(0);
			ag_draw_cafedoor_left(0);
			guMtxTrans(model, 8.05f, 9.55f, 1.0f);
			guMtxConcat(view, model, modelview);
			LX_LoadMtxImm(modelview, GX_PNMTX0);
			ag_draw_cafedoor_right(0);

			GX_SetChanMatColor(GX_COLOR0A0, LC_WHITE);
			GX_SetChanAmbColor(GX_COLOR0A0, LC_WHITE);
			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);
			guMtxIdentity(model);
			guMtxConcat(view, model, modelview);
			LX_LoadMtxImm(modelview, GX_PNMTX0);
			GX_SetCullMode(GX_CULL_FRONT);
			ag_config_table(0);
			ag_draw_table(0);
			guMtxIdentity(model);
			c_guMtxReflect(model, &(guVector){0.0f, 0.0f, 0.0f}, &(guVector){0.0f, 1.0f, 0.0f});
			guMtxConcat(view, model, modelview);
			LX_LoadMtxImm(modelview, GX_PNMTX0);
			GX_SetCullMode(GX_CULL_BACK);
			ag_draw_table(0);
		}

		if(scene == 4) {
			drawDoodle(&(guyDoodleAnimfullseqData[(frame - 1852) * (guyDoodleAnimfullseqPrimcount)])); //primcount started being read as 0... too lazy to fix
		}

		//Jacket prop
		if((frame > 2695 && frame < 2965) && !(comic && eye)) {
			GX_SetNumTexGens(1);
			GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);
			GX_SetNumTevStages(1);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO); 
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);
			GX_LoadTexObj(&leatherMatcapTexObj, GX_TEXMAP1);
			for(int i = 0; i < doGillReflect + 1; i++) {
				Mtx * jacketmtxs = i ? jacketproprefmv : jacketpropmv;
				LX_LoadMtxImm(jacketmtxs[0], GX_PNMTX0); GX_LoadTexMtxImm(jacketpropmvrefmap[0], GX_TEXMTX0, GX_MTX3x4);
				LX_LoadMtxImm(jacketmtxs[1], GX_PNMTX1); GX_LoadTexMtxImm(jacketpropmvrefmap[1], GX_TEXMTX1, GX_MTX3x4);
				LX_LoadMtxImm(jacketmtxs[2], GX_PNMTX2); GX_LoadTexMtxImm(jacketpropmvrefmap[2], GX_TEXMTX2, GX_MTX3x4);
				LX_LoadMtxImm(jacketmtxs[3], GX_PNMTX3); GX_LoadTexMtxImm(jacketpropmvrefmap[3], GX_TEXMTX3, GX_MTX3x4);
				GX_SetCullMode(GX_CULL_NONE);
				ag_config_riverjacketprop(0);
				ag_draw_riverjacketprop(0);
			}
		}

		//gil notbok
			GX_SetCullMode(GX_CULL_FRONT);
			liyt_genMtxPosRotZyxScale(model, gillnotebooktransform[0], gillnotebooktransform[1], gillnotebooktransform[2],
			                                 gillnotebooktransform[3], gillnotebooktransform[4], gillnotebooktransform[5],
							 0.15f, 0.15f, 0.15f);
			GX_SetChanAmbColor(GX_COLOR0A0, LC_WHITE);
			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);
			guMtxConcat(view, model, modelview);
			GX_SetCurrentMtx(GX_PNMTX0);
			LX_LoadMtxImm(modelview, GX_PNMTX0);
			GX_SetNumTevStages(1);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_ONE, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			GX_SetNumTexGens(1);
			GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
			ag_config_gillnotebook_bind(0);
			ag_draw_gillnotebook_bind(0);
			
			GX_LoadTexObj(&gillnotebooktexTexObj, GX_TEXMAP0);
			if(scene == 3) {
				GX_LoadTexObj(&firematteTexObj, GX_TEXMAP1);
				int burnval = notebookmatcomp[0];
				int burnvald = burnval - 3;
				int burnvali = burnval + 3;
				int burnvaldd = burnval - 30;
				if(burnvald < 0) burnvald = 0;
				if(burnvali > 255) burnvali = 255;
				if(burnvaldd < 0) burnvaldd = 0;
				GX_SetTevKColor(GX_KCOLOR0, (GXColor){burnval, burnvald, burnvali, burnvaldd});
				GX_SetTevKColor(GX_KCOLOR1, (GXColor){255, 150, 32, 255}); //just a firey orange
				GX_SetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0_A);
				GX_SetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K0_R);
				GX_SetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K0_R);
				GX_SetTevKColorSel(GX_TEVSTAGE4, GX_TEV_KCSEL_K0_G);
				GX_SetTevKColorSel(GX_TEVSTAGE5, GX_TEV_KCSEL_K0_B);
				GX_SetTevKColorSel(GX_TEVSTAGE6, GX_TEV_KCSEL_K1);
	
				GX_SetNumTevStages(7);
				GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
				GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
				GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

				GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
				GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);

				GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C1, GX_CC_CPREV, GX_CC_ZERO);
				GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

				GX_SetTevColorIn(GX_TEVSTAGE3, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV, GX_CC_ZERO);
				GX_SetTevColorOp(GX_TEVSTAGE3, GX_TEV_COMP_R8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);

				GX_SetTevColorIn(GX_TEVSTAGE4, GX_CC_TEXC, GX_CC_KONST, GX_CC_ONE, GX_CC_ZERO);
				GX_SetTevColorOp(GX_TEVSTAGE4, GX_TEV_COMP_R8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GX_SetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);

				GX_SetTevColorIn(GX_TEVSTAGE5, GX_CC_KONST, GX_CC_TEXC, GX_CC_CPREV, GX_CC_ZERO);
				GX_SetTevColorOp(GX_TEVSTAGE5, GX_TEV_COMP_R8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GX_SetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);
		
				GX_SetTevColorIn(GX_TEVSTAGE6, GX_CC_C0, GX_CC_KONST, GX_CC_CPREV, GX_CC_ZERO);
				GX_SetTevColorOp(GX_TEVSTAGE6, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GX_SetTevOrder(GX_TEVSTAGE6, GX_TEXCOORD0, GX_TEXMAP_NULL, GX_COLOR0A0);
			} else {
				GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
			}

			ag_config_gillnotebook_pages(0);
			ag_draw_gillnotebook_pages(0);
			
		if(scene == 3 && !(comic && eye)) {
			GX_SetCullMode(GX_CULL_NONE);
			setlight(view, LC_BLACK, 3);
			GX_SetNumTevStages(1);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
			liyt_genMtxPosRotZyx(model, matchtransform[0], matchtransform[1], matchtransform[2],
			                            matchtransform[3], matchtransform[4], matchtransform[5]);
			guMtxConcat(view, model, model);
			LX_LoadMtxImm(model, GX_PNMTX0);
			ag_config_match(0);
			ag_draw_match(0);
			GX_SetCullMode(GX_CULL_FRONT);
			liyt_genMtxPosRotZyx(model, matchboxtransform[0], matchboxtransform[1], matchboxtransform[2],
			                            matchboxtransform[3], matchboxtransform[4], matchboxtransform[5]);
			guMtxConcat(view, model, model);
			LX_LoadMtxImm(model, GX_PNMTX0);
			ag_config_matchbox(0);
			ag_draw_matchbox(0);
			nolight();
			liyt_genMtxPosRotZyx(model, 2.15948, 2.27709, 1.18844,
			                            0.0f, 0.0f, gillBedroomdoorrot[0]);
			guMtxConcat(view, model, model);
			LX_LoadMtxImm(model, GX_PNMTX0);
			ag_config_gillBedroomDoor(0);
			ag_draw_gillBedroomDoor(0);
	
			
		}

		if(scene == 0 && !(comic && eye)) {
			GX_SetCullMode(GX_CULL_FRONT);
			setlight(view, LC_BLACK, 0);
			GX_SetNumTevStages(1);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
			liyt_genMtxPosRotZyx(model, trashtransform[0], trashtransform[1], trashtransform[2],
			                            trashtransform[3], trashtransform[4], trashtransform[5]);
			guMtxConcat(view, model, model);
			LX_LoadMtxImm(model, GX_PNMTX0);
			ag_config_trashbin(0);
			ag_draw_trashbin(0);
		}

		/*GX_SetNumTevStages(1);
		GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_KONST);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
		GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
		GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
		Mtx44 persp;
		guPerspective(persp, 45, 1.333f, 0.1F, 300.0F);
		GX_LoadProjectionMtx(persp, GX_PERSPECTIVE);
		Mtx model;
		guMtxTrans(model, downvectrue[0], downvectrue[1], downvectrue[2] - 4.0f);
		GX_LoadPosMtxImm(model, GX_PNMTX0);
		GX_SetTevKColor(GX_KCOLOR0, (GXColor){0, 0, 255, 255});
		drawcoolarrow();
		guMtxTrans(model, gyroaccum[0], gyroaccum[1], gyroaccum[2] - 4.0f);
		GX_LoadPosMtxImm(model, GX_PNMTX0);
		GX_SetTevKColor(GX_KCOLOR0, (GXColor){0, 255, 0, 255});
		drawcoolarrow();
		guMtxTrans(model, forward[0], forward[1], forward[2] - 4.0f);
		GX_LoadPosMtxImm(model, GX_PNMTX0);
		GX_SetTevKColor(GX_KCOLOR0, (GXColor){0, 0, 0, 255});
		drawcoolarrow();
		guMtxTrans(model, cross[0], cross[1], cross[2] - 4.0f);
		GX_LoadPosMtxImm(model, GX_PNMTX0);
		GX_SetTevKColor(GX_KCOLOR0, (GXColor){255, 0, 0, 255});
		drawcoolarrow();*/

		//Postprocess
		
		GX_SetDispCopyGamma((f32)GX_GM_1_0);

		GX_SetCopyFilter(GX_FALSE, NULL, GX_TRUE, perfectfilter);
		
		GX_SetCullMode(GX_CULL_NONE);

		GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);

		int dof = (scene == 4) ? 0 : 1;
		dof = (frame > 91) ? dof : 0;
		dof &= posttog;
		if(frame > 2822 && frame < 2832) dof = 0;
		if(comic) dof = 0;
		dof = 0; //doesnt work well in stereo

		//Random sample dither display before copy to rgb565
		//Use cos we do correction rgb565
		if(!(comic && eye)) {
			GX_SetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
			GX_LoadTexObj(&randomATexObj, GX_TEXMAP2);
			GX_SetNumTevStages(1);
			GX_SetTevKColor(GX_KCOLOR0, (GXColor){5, 5, 5, 5});
			GX_SetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP2, GX_COLORNULL);
			GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
			GX_SetNumTexGens(2);
			GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
			GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
		

			if(dolphin) {
				guMtxScale(model, 11.25f, 9.0f, 9.0f); 
				//guMtxScale(model, 22.5f, 30.0f, 30.0f); 
				//guMtxScale(model, 2.8125f, 3.75f, 3.75f); 
			} else { 
				guMtxScale(model, 5.625f, 7.5f, 7.5f); 
			}
			GX_LoadTexMtxImm(model, GX_TEXMTX0, GX_MTX2x4);
			guMtxTrans(model, 0.0f, 0.0f, -1.0f);
			GX_LoadPosMtxImm(model, GX_PNMTX0);
			GX_LoadProjectionMtx(permIdentity44, GX_PERSPECTIVE);
			liy_VtxDescConfig(LV_VP | LV_TC, 0);
			GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
			GX_Position3f32(-2.0f, -1.0f, 0.0f);
			GX_TexCoord2f32(-0.5f, 1.0f);	
			GX_Position3f32(2.0f, -1.0f, 0.0f);
			GX_TexCoord2f32(1.5f, 1.0f);
			GX_Position3f32(0.0f, 3.0f, 0.0f);
			GX_TexCoord2f32(0.5f, -1.0f);
			GX_TexCoord2f32(0.0f, 0.0f);
		}

		GX_SetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
		GX_SetNumTexGens(1);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

		GX_LoadProjectionMtx(permIdentity44, GX_PERSPECTIVE);
		GX_SetCurrentMtx(GX_PNMTX0);
		LX_LoadMtxImm(fullQuadMv, GX_PNMTX0);
		GX_SetNumTevStages(1);
		GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);

		if(dof) {
		float focuspoint = sqrt(Lily_Float);	//80041960
		float inverse = 1.0f / focuspoint;

		int inverseInt = (int)((float)inverse * 255.0f);
		int focusInt = (int)((float)focuspoint * 255.0f);
		//int exposureInt = (int)((float)exposure * 255.0f);
		//int exposureInt = 128;

		inverseInt += inverseIntEpsilonLut[focusInt];

		GXColor dofColA = {inverseInt, 133, focusInt, 255};

		GX_SetTexCopySrc(0, 0, ltw, lth);
		GX_SetTexCopyDst(ltw, lth, GX_TF_Z8, GX_FALSE);
		//GX_CopyTex(DofDefBufr, GX_FALSE); 
		GX_PixModeSync();
		//GX_InitTexObj(&DofDefBufrTexObj, DofDefBufr, ltw, lth, GX_TF_Z8, GX_CLAMP, GX_CLAMP, GX_FALSE);

		GX_SetTexCopySrc(0, 0, ltw, lth);
		GX_SetTexCopyDst(ltw, lth, GX_TF_RGB565, GX_FALSE);
		//GX_CopyTex(DofDefClr, GX_FALSE); 
		GX_PixModeSync();
		//GX_InitTexObj(&DofDefClrTexObj, DofDefClr, ltw, lth, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
		GX_InvalidateTexAll();
		//GX_LoadTexObj(&DofDefClrTexObj, GX_TEXMAP0);

		GX_SetCopyFilter(GX_FALSE, NULL, GX_TRUE, perfectfilter);

		//GX_LoadTexObj(&DofDefBufrTexObj, GX_TEXMAP0);
		//GX_LoadTexObj(&DofDefClrTexObj, GX_TEXMAP1);
		//GX_LoadTexObj(&randomATexObj, GX_TEXMAP2);
		//GX_LoadTexObj(&randomBTexObj, GX_TEXMAP3);
	
		GX_SetNumTexGens(2);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
		GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
		//For 640x480 and 128, 5.0 and 3.75 are right numbers
		//For dolphin just assume 12x resolution whatever
		//If you dont like the weird emulator-exclusive repeating patterns
		//make a dolphin fork that doesnt expose /dev/dolphin
		//You might make ctgp cheaters happy
		if(dolphin) {
			guMtxScale(model, 11.25f, 9.0f, 9.0f); 
			//guMtxScale(model, 22.5f, 30.0f, 30.0f); 
			//guMtxScale(model, 5.0f, 3.75f, 3.75f); 
		} else { 
			guMtxScale(model, 5.625f, 7.5f, 7.5f); 
		}
		GX_LoadTexMtxImm(model, GX_TEXMTX0, GX_MTX2x4);

		//TEV for a

		GX_SetNumTevStages(8); 
		GX_SetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0_R); //A band
		GX_SetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K0_B); //B band
		GX_SetTevKColorSel(GX_TEVSTAGE7, GX_TEV_KCSEL_K0_G); //exposure
		GX_SetTevKColor(GX_KCOLOR0, dofColA);

		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0); //multiply first half of the curve by a konstant

		// output = d + ((a OP b) ? c : 0)
		GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_C0, GX_CC_ONE, GX_CC_ONE, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_COMP_R8_EQ, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV); //make white areas black
						//subhalf scale2 is to increase contrast in the matte
		GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_C0, GX_CC_ZERO, GX_CC_CPREV, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0); //fisrt half of the curve is done
						//subhalf scale2 is to increase contrast in the matte
		//TEV for b

		GX_SetTevColorIn(GX_TEVSTAGE3, GX_CC_ONE, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST);
		GX_SetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1); //invert and move up 

		// output = d + ((a OP b) ? c : 0)
		GX_SetTevColorIn(GX_TEVSTAGE4, GX_CC_C1, GX_CC_ONE, GX_CC_ONE, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE4, GX_TEV_COMP_R8_EQ, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV); //make white areas black
						//subhalf scale2 is to increase contrast in the matte
		GX_SetTevColorIn(GX_TEVSTAGE5, GX_CC_C1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE5, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1); //make the second half of the curve
						//subhalf scale2 is to increase contrast in the matte
		GX_SetTevColorIn(GX_TEVSTAGE6, GX_CC_ZERO, GX_CC_ONE, GX_CC_C0, GX_CC_C1);      //TEVREG0
		GX_SetTevColorOp(GX_TEVSTAGE6, GX_TEV_ADD, GX_TB_SUBHALF, GX_CS_SCALE_2, GX_TRUE, GX_TEVREG0); //add two curves together
						//subhalf scale2 is to increase contrast in the matte
		//matte is now done and in c0

		//GX_SetTevColorIn(GX_TEVSTAGE7, GX_CC_TEXC, GX_CC_KONST, GX_CC_C0, GX_CC_ZERO); //mix noise with grey based on matte
		GX_SetTevColorIn(GX_TEVSTAGE7, GX_CC_TEXC, GX_CC_KONST, GX_CC_C0, GX_CC_ZERO); //mix noise with grey based on matte
		GX_SetTevColorOp(GX_TEVSTAGE7, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		//displacement texture now in c1

		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
		GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP_NULL, GX_COLORNULL);
		GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP_NULL, GX_COLORNULL);
		GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
		GX_SetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD0, GX_TEXMAP_NULL, GX_COLORNULL);
		GX_SetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD0, GX_TEXMAP_NULL, GX_COLORNULL);
		GX_SetTevOrder(GX_TEVSTAGE6, GX_TEXCOORD0, GX_TEXMAP_NULL, GX_COLORNULL);
		GX_SetTevOrder(GX_TEVSTAGE7, GX_TEXCOORD1, GX_TEXMAP2, GX_COLORNULL);

		liy_VtxDescConfig(LV_VP | LV_TC, 0);
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
		GX_Position3f32(-2.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(-0.5f, 1.0f);	
		GX_Position3f32(2.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(1.5f, 1.0f);
		GX_Position3f32(0.0f, 3.0f, 0.0f);
		GX_TexCoord2f32(0.5f, -1.0f);

		GX_SetTexCopySrc(0, 0, ltw, lth); //X for first pass
		GX_SetTexCopyDst(ltw, lth, GX_TF_I8, GX_FALSE);
		//GX_CopyTex(DofDisplaceA, GX_FALSE); 
		GX_PixModeSync();
		//GX_InitTexObj(&dofDisplaceATexObj, DofDisplaceA, ltw, lth, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
		GX_InvalidateTexAll();

		//We repeat all the stages with a differnt texture
		GX_SetTevOrder(GX_TEVSTAGE7, GX_TEXCOORD1, GX_TEXMAP3, GX_COLORNULL); //Y for first pass

		liy_VtxDescConfig(LV_VP | LV_TC, 0);
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
		GX_Position3f32(-2.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(-0.5f, 1.0f);	
		GX_Position3f32(2.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(1.5f, 1.0f);
		GX_Position3f32(0.0f, 3.0f, 0.0f);
		GX_TexCoord2f32(0.5f, -1.0f);

		GX_SetTexCopySrc(0, 0, ltw, lth);
		GX_SetTexCopyDst(ltw, lth, GX_TF_I8, GX_FALSE);
		//GX_CopyTex(DofDisplaceB, GX_FALSE); 
		GX_PixModeSync();
		//GX_InitTexObj(&dofDisplaceBTexObj, DofDisplaceB, ltw, lth, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
		GX_InvalidateTexAll();

		//GX_LoadTexObj(&DofDefClrTexObj, GX_TEXMAP1);
		//GX_LoadTexObj(&dofDisplaceATexObj, GX_TEXMAP0);
		//GX_LoadTexObj(&dofDisplaceBTexObj, GX_TEXMAP2);

		GX_SetNumTexGens(2);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
		GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);

		GX_SetNumTevStages(2);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLORNULL);
		GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLORNULL);

		GX_SetNumIndStages(2);
		GX_SetTevIndirect(GX_TEVSTAGE0, GX_INDTEXSTAGE0, GX_ITF_8, GX_ITB_STU, GX_ITM_0, GX_ITW_OFF, GX_ITW_OFF, GX_FALSE, GX_FALSE, GX_ITBA_OFF);
		GX_SetTevIndirect(GX_TEVSTAGE1, GX_INDTEXSTAGE1, GX_ITF_8, GX_ITB_STU, GX_ITM_1, GX_ITW_0, GX_ITW_0, GX_TRUE, GX_FALSE, GX_ITBA_OFF);
		GX_SetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD0, GX_TEXMAP0);
		GX_SetIndTexOrder(GX_INDTEXSTAGE1, GX_TEXCOORD1, GX_TEXMAP2);
		float hozIndMtx[2][3] = {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}};
		float verIndMtx[2][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.0f}};
		GX_SetIndTexMatrix(GX_ITM_0, hozIndMtx, stops);
		GX_SetIndTexMatrix(GX_ITM_1, verIndMtx, stops);

		liy_VtxDescConfig(LV_VP | LV_TC, 0);
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
		GX_Position3f32(-2.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(-0.5f, 1.0f);	
		GX_Position3f32(2.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(1.5f, 1.0f);
		GX_Position3f32(0.0f, 3.0f, 0.0f);
		GX_TexCoord2f32(0.5f, -1.0f);

		GX_SetTexCopySrc(0, 0, ltw, lth); //First pass
		GX_SetTexCopyDst(ltw, lth, GX_TF_RGB565, GX_FALSE);
		//GX_CopyTex(dofDoublepassHolding, GX_FALSE); 
		GX_PixModeSync();
		//GX_InitTexObj(&dofDoublepassHoldingTexObj, dofDoublepassHolding, ltw, lth, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
		GX_InvalidateTexAll();

		//GX_LoadTexObj(&dofDisplaceBTexObj, GX_TEXMAP0);
		//GX_LoadTexObj(&dofDisplaceATexObj, GX_TEXMAP2);

		liy_VtxDescConfig(LV_VP | LV_TC, 0);
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
		GX_Position3f32(-2.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(-0.5f, 1.0f);	
		GX_Position3f32(2.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(1.5f, 1.0f);
		GX_Position3f32(0.0f, 3.0f, 0.0f);
		GX_TexCoord2f32(0.5f, -1.0f);

		GX_SetTexCopySrc(0, 0, ltw, lth); //Second pass .. reuse dofdefclr memory area
		GX_SetTexCopyDst(ltw, lth, GX_TF_RGB565, GX_FALSE);
		GX_CopyTex(DofDefClr, GX_FALSE); 
		GX_PixModeSync();
		GX_InitTexObj(&DofDefClrTexObj, DofDefClr, ltw, lth, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
		GX_InvalidateTexAll();

		GX_SetNumIndStages(0);
		GX_SetIndTexCoordScale(GX_INDTEXSTAGE0,GX_ITS_1,GX_ITS_1);
		GX_SetIndTexCoordScale(GX_INDTEXSTAGE1,GX_ITS_1,GX_ITS_1);
		for(int i = 0; i < 16; i++) {
			GX_SetTevOp(i, GX_REPLACE);
			GX_SetTevOrder(i, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
			GX_SetTevDirect(i);
		}

		//GX_LoadTexObj(&dofDoublepassHoldingTexObj, GX_TEXMAP0);
		//GX_LoadTexObj(&DofDefClrTexObj, GX_TEXMAP1);

		GX_SetNumTexGens(1);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

		GX_SetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);	
		GX_SetTevKColor(GX_KCOLOR0, (GXColor){128, 128, 128, 255});

		GX_SetNumTevStages(2);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
		GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP1, GX_COLORNULL);

		liy_VtxDescConfig(LV_VP | LV_TC, 0);
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
		GX_Position3f32(-2.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(-0.5f, 1.0f);	
		GX_Position3f32(2.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(1.5f, 1.0f);
		GX_Position3f32(0.0f, 3.0f, 0.0f);
		GX_TexCoord2f32(0.5f, -1.0f);
		}

		GX_DrawDone();
		GX_Flush();

		// Comic
		if(comic) {
			Mtx44 eohfeghau;
			guPerspective(eohfeghau, 45, 1.0f, 0.1F, 300.0F);
			GX_LoadProjectionMtx(eohfeghau, GX_PERSPECTIVE);
			GX_SetViewport(0, 0, 640, 528, 0, 1);
			GX_SetScissor(0, 0, 640, 528);
			GX_SetColorUpdate(GX_FALSE);
			//bandaid .. clear for whole efb
			guMtxTrans(model, 0.0f, 0.0f, -100.0f);
			GX_LoadPosMtxImm(model, GX_PNMTX0);
			GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
			GX_SetNumTevStages(1);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
			liy_VtxDescConfig(LV_VP, 0);
			GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
			GX_Position3f32(-1000.0f, -1000.0f, 0.0f);	
			GX_Position3f32(1000.0f, -1000.0f, 0.0f);
			GX_Position3f32(1000.0f, 1000.0f, 0.0f);
			GX_Position3f32(-1000.0f, 1000.0f, 0.0f);
			GX_SetColorUpdate(GX_TRUE);

			GX_SetViewport(0,0,256,256,0,1);
			GX_SetScissor(0,0,256,256);
			guPerspective(perspective, camFov * 1.15, (frame - 2822) > 115 ? 2.0f : 1.0f, 0.1F, 300.0F);
			GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
			if((frame - 2822) > 115) {
				GX_SetViewport(0,0,512,256,0,1);
				GX_SetScissor(0,0,512,256);
			}

			GX_PixModeSync();
			if(comic > 165) comic = 165;
			float * comcamframe = &comicCameraSeqData[comic * comicCameraSeqPrimcount];
				cam.x = comcamframe[0]; cam.y = comcamframe[1]; cam.z = comcamframe[2];
				camrot.x = comcamframe[3]; camrot.y = comcamframe[4]; camrot.z = comcamframe[5];
				camFov = comcamframe[6];
			liyt_genMtxPosRotZyx(view, cam.x, cam.y, cam.z, camrot.x, camrot.y, camrot.z);
			guMtxCopy(headsetorient, model);
			guMtxConcat(view, model, view);
			guMtxInverse(view, view);
			guPerspective(perspective, realfov, aspect, 0.01F, 60.0F);

			int eyetexoffset = 0; //why are we even rendering the scenes twice at all

			switch(eye) {
				case 0:
					guMtxTransApply(view, view, 0.034f, 0.0f, 0.0f);
					//guMtxTransApply(overlayView, overlayView, 0.005f, 0.0f, 0.0f);
					//perspective[0][2] = -lensOffsets.x;
					perspective[0][2] = -0.25f;
					eyetexoffset = 0;
					break;
				case 1:
					guMtxTransApply(view, view, -0.034f, 0.0f, 0.0f);
					//guMtxTransApply(overlayView, overlayView,-0.005f, 0.0f, 0.0f);
					//perspective[0][2] = lensOffsets.x;
					perspective[0][2] = 0.25f;
			}
			GX_SetCopyClear(LC_GILLPURPLE, 0x00ffffff);
			if(comiclear < 2) {
				GX_SetTexCopySrc(256, 0, 256, 256);//off to the right
				GX_SetTexCopyDst(256, 256, GX_TF_RGB565, GX_FALSE);
				GX_CopyTex(Comicholding + eyetexoffset, GX_TRUE); //color that area off to the right purple. texture is rewritten immediatly
				GX_PixModeSync(); //sprinkling these innn right now..
				GX_CopyTex(Comicholding + 131072 + eyetexoffset, GX_FALSE); //copy the purple to the tex
				GX_CopyTex(Comicholding + 262144 + eyetexoffset, GX_FALSE); 
				GX_CopyTex(Comicholding + 393216 + eyetexoffset, GX_FALSE); 
				GX_CopyTex(Comicholding + 524288 + eyetexoffset, GX_FALSE); 
				GX_CopyTex(Comicholding + 655360 + eyetexoffset, GX_FALSE); 
				GX_PixModeSync();
				comiclear++;
				GX_SetCopyClear((GXColor){0x7f, 0x4d, 0x90, 255}, 0x00ffffff); //light pink
				GX_CopyTex(Comicholding + eyetexoffset, GX_TRUE); //fix color of area used for copy
				GX_PixModeSync();
			}
			GX_SetCopyClear((GXColor){0x7f, 0x4d, 0x90, 255}, 0x00ffffff);
			GXTexObj comicATex, comicBTex, comicCTex, comicDTex, comicETex;
			GX_InitTexObj(&comicATex, Comicholding + eyetexoffset, 256, 256, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
			GX_InitTexObj(&comicBTex, Comicholding + 131072 + eyetexoffset, 256, 256, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
			GX_InitTexObj(&comicCTex, Comicholding + 262144 + eyetexoffset, 256, 256, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
			GX_InitTexObj(&comicDTex, Comicholding + 393216 + eyetexoffset, 256, 256, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
			GX_InitTexObj(&comicETex, Comicholding + 524288 + eyetexoffset, 512, 256, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
			int whichtoupdate = 0;
			if(comic > 35) whichtoupdate = 1;
			if(comic > 64) whichtoupdate = 2;
			if(comic > 91) whichtoupdate = 3;
			if(comic > 114) whichtoupdate = 4;
			if(!eye)
			switch(whichtoupdate) {
				case 0:
					GX_SetTexCopySrc(0, 0, 256, 256);
					GX_SetTexCopyDst(256, 256, GX_TF_RGB565, GX_FALSE);
					GX_CopyTex(Comicholding + eyetexoffset, GX_TRUE); 
					break;
				case 1:
					GX_SetTexCopySrc(0, 0, 256, 256);
					GX_SetTexCopyDst(256, 256, GX_TF_RGB565, GX_FALSE);
					GX_CopyTex(Comicholding + 131072 + eyetexoffset, GX_TRUE); 
					break;
				case 2:
					GX_SetTexCopySrc(0, 0, 256, 256);
					GX_SetTexCopyDst(256, 256, GX_TF_RGB565, GX_FALSE);
					GX_CopyTex(Comicholding + 262144 + eyetexoffset, GX_TRUE); 
					break;
				case 3:
					GX_SetTexCopySrc(0, 0, 256, 256);
					GX_SetTexCopyDst(256, 256, GX_TF_RGB565, GX_FALSE);
					GX_CopyTex(Comicholding + 393216 + eyetexoffset, GX_TRUE); 
					break;
				case 4:
					GX_SetTexCopySrc(0, 0, 512, 256);
					GX_SetTexCopyDst(512, 256, GX_TF_RGB565, GX_FALSE);
					GX_CopyTex(Comicholding + 524288 + eyetexoffset, GX_TRUE); 
					break;
			}
		
			GX_PixModeSync();
			GX_InvalidateTexAll();

			GX_DrawDone();
			GX_Flush();

			GX_SetViewport(0, 0, precorrx, precorry, 0, 1);
			GX_SetScissor(0, 0, precorrx, precorry);
			GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

			guMtxIdentity(model);
			guMtxConcat(view, model, model);
			GX_LoadPosMtxImm(model, GX_PNMTX0);
			GX_SetCurrentMtx(GX_PNMTX0);
			GX_SetCullMode(GX_CULL_NONE);
			GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

			GX_SetNumTevStages(1);
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_RASC, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			GX_SetNumChans(1);
			GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);

			ag_config_comicbook(0);
			ag_draw_comicbook(0);

			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	
			ag_config_comicpanelA(0);
			GX_LoadTexObj(&comicATex, GX_TEXMAP0);
			ag_draw_comicpanelA(0);
			GX_LoadTexObj(&comicBTex, GX_TEXMAP0);
			ag_draw_comicpanelB(0);
			GX_LoadTexObj(&comicCTex, GX_TEXMAP0);
			ag_draw_comicpanelC(0);
			GX_LoadTexObj(&comicDTex, GX_TEXMAP0);
			ag_draw_comicpanelD(0);
			GX_LoadTexObj(&comicETex, GX_TEXMAP0);
			ag_draw_comicpanelE(0);
			
		}

		// Overlay

		GX_SetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
		GX_SetCurrentMtx(GX_PNMTX0);

		GX_LoadProjectionMtx(overlayPersp, GX_PERSPECTIVE);


		#ifdef GUY_DEV
			basicRenderString("guy dev");
		#endif 
		#ifdef GUY_RELEASE
			basicRenderString("guy prerelease");
		#endif
		//basicRenderString("i wanna be your favorite boy and girl");
		//basicRenderString("the one you think about");
		//basicRenderString("feel the guilt crawl in my skin and bones");
		//basicRenderString("whenever you're around");
		//basicRenderString("and nothing's worse than you with someone else");
		//basicRenderString("you're mine, you're mine, you're mine!");
		//basicRenderString("llllmmmm()()?''");
		
					
		//dials
		liy_VtxDescConfig(LV_VP | LV_VC, 0);
		GX_SetChanAmbColor(GX_COLOR0A0, LC_WHITE);
		GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);	

		GX_SetNumTevStages(1);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);

		if(cameraControl) {
			guMtxIdentity(model);
			guMtxRotRad(model, 'z', (float)pi+(framerate/9.554140f));
			guMtxTransApply(model, model, dialx, -3.7f, -10.0f);
			guMtxConcat(overlayView,model,modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			GX_LoadNrmMtxImm(modelview, GX_PNMTX0);
			drawDial(0, 0, 0);

			guMtxIdentity(model);
			guMtxRotRad(model, 'z', (float)pi+(blendtimems/5.30780254f));
			guMtxTransApply(model, model, dialx, -1.2f, -10.0f);
			guMtxConcat(overlayView,model,modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			drawDial(255, 0, 0);

			guMtxIdentity(model);
			guMtxRotRad(model, 'z', (float)pi+(skeletimems/5.30780254f));
			guMtxTransApply(model, model, dialx, -1.2f, -10.0f);
			guMtxConcat(overlayView,model,modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			drawDial(255, 0, 255);

			guMtxIdentity(model);
			guMtxRotRad(model, 'z', (float)pi+(frametimems/5.30780254f));
			guMtxTransApply(model, model, dialx, -1.2f, -10.0f);
			guMtxConcat(overlayView,model,modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			drawDial(0, 0, 0);
			
			guMtxIdentity(model);
			guMtxRotRad(model, 'z', (float)pi+(cputimems/5.30780254f));
			guMtxTransApply(model, model, dialx, -1.2f, -10.0f);
			guMtxConcat(overlayView,model,modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			drawDial(0, 0, 255);


			/*guMtxIdentity(model);
			guMtxRotRad(model, 'z', (float)pi+(accumulatedbehindtime/5.30780254f));
			guMtxTransApply(model, model, dialx, 1.3f, -10.0f);
			guMtxConcat(overlayView,model,modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			drawDial(0, 0, 0);*/
		}
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
		if(frame < 170) {
			if(frame < 92) {
				guMtxTrans(model, 0.3f, 0.12f, 0.3f);
				guMtxConcat(overlayView, model, modelview);
				GX_LoadPosMtxImm(modelview, GX_PNMTX0);
				ag_config_introtext1(0);
				ag_draw_introtext1(0);
			} else {
				guMtxTrans(model, -0.3f, -0.028f, 0.3f);
				guMtxConcat(overlayView, model, modelview);
				GX_LoadPosMtxImm(modelview, GX_PNMTX0);
				ag_config_introtext2(0);
				ag_draw_introtext2(0);
			}
		}
		// End overlay
		
		//Barrel correction here
		GX_SetCullMode(GX_CULL_NONE);
		GX_SetTexCopySrc(0, 0, precorrx, precorry);
		GX_SetTexCopyDst(precorrx, precorry, GX_TF_RGB565, GX_FALSE);
		GX_CopyTex(DofDefClr, GX_TRUE); 
		GX_PixModeSync();
		GX_InitTexObj(&DofDefClrTexObj, DofDefClr, precorrx, precorry, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
		GX_InvalidateTexAll();
		GX_LoadTexObj(&DofDefClrTexObj, GX_TEXMAP0);

		GX_LoadProjectionMtx(permIdentity44, GX_PERSPECTIVE);
		GX_SetCurrentMtx(GX_PNMTX0);
		
		Mtx eyemtx;

		switch(eye) {
			case 0:
				//guMtxTransApply(fullQuadMv, model, lensOffsets.x, lensOffsets.y, lensOffsets.z);
				GX_LoadPosMtxImm(fullQuadMv, GX_PNMTX0);
 
				guMtxScale(model, 1.0f, -1.0f, 1.0f); 
				guMtxTransApply(model, model, 0.0f, 1.0f, 0.0f);
				GX_LoadTexMtxImm(model, GX_TEXMTX0, GX_MTX2x4);
				break;
			case 1:
				guMtxScaleApply(fullQuadMv, model, -1.0f, 1.0f, 1.0f);
				//guMtxTransApply(model, model, -lensOffsets.x, lensOffsets.y, lensOffsets.z);
				GX_LoadPosMtxImm(model, GX_PNMTX0);

				guMtxScale(model, -1.0f, -1.0f, 1.0f); 
				guMtxTransApply(model, model, 1.0f, 1.0f, 0.0f);
				GX_LoadTexMtxImm(model, GX_TEXMTX0, GX_MTX2x4);
				break;
		}

		GX_SetNumTexGens(1);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);

		GX_SetNumTevStages(1);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_ZERO, GX_CC_ZERO);
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);

		GX_SetViewport(0,0,352, 480,0,1); //Render in copyout area only
		GX_SetScissor(0,0,352, 480);

		//Clear out area

		liy_VtxDescConfig(LV_VP | LV_TC, 0);
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-1.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(0.0f, 1.0f);	
		GX_Position3f32(1.0f, -1.0f, 0.0f);
		GX_TexCoord2f32(1.0f, 1.0f);
		GX_Position3f32(1.0f, 1.0f, 0.0f);
		GX_TexCoord2f32(1.0f, 0.0f);
		GX_Position3f32(-1.0f, 1.0f, 0.0f);
		GX_TexCoord2f32(0.0f, 0.0f);

		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);

		ag_config_dk1lenscorr(0);
		ag_draw_dk1lenscorr(0);

		//End rendering

		exposure = (scene == 4) ? 0.50 : 0.65;
		exposure = liym_max(exposure, 0.009f);
		exposure = liym_min(exposure, 0.992f);
		u8 blurfinalcopy = 0;
		if(blurfinalcopy) {
			//div by 7
			int exposureInt = (int)(exposure * 124.0f * 0.142857142f);
			memset(vfilter, exposureInt, 7);
		} else {
			//0-127 basically but cap to 124 cos postprocess goes a little over unintentionally oops lol
			int exposureInt = (int)(exposure * 124.0f);
			memset(vfilter, 0, 7);
			if(exposureInt > 63) { 
				vfilter[2] = 63; 
				exposureInt -= 63;
				if(exposureInt > 63) { 
					vfilter[3] = 63; 
					exposureInt -= 63;
				} else {
					vfilter[3] = exposureInt;
				}
			} else {
				vfilter[2] = exposureInt;
			}
		}

		GX_SetCopyFilter(GX_FALSE, NULL, GX_TRUE, vfilter);
		GX_SetDispCopyGamma((f32)GX_GM_1_0);

		background = LC_GAYSKY;
		if(frame > 1017) background = LC_CAFEAM;
		if(frame > 1850) background = LC_WHITE;
		if(frame > 2573) background = LC_GILLPURPLE;
		if(frame > 2620) background = LC_WHITE;
		if(frame > 2821) background = (GXColor){0x7f, 0x4d, 0x90, 255};
		if(frame > 2986) background = LC_GAYSKY;
		if(frame > 3017) background = LC_WHITE;
		GX_SetCopyClear(background, 0x00ffffff);
		GX_CopyDisp((u8 *)(frameBuffer[fb]) + (eye?704:0), GX_TRUE); //fb is cleared after copy when true

		u64 cputime = gettime() - startframe;
		cputimems = (float)ticks_to_microsecs(cputime) / 1000.0f;

		}//end eye

		GX_DrawDone();

		u64 endframe = gettime();
		frametime = endframe - startframe;
		frametimems = (float)ticks_to_microsecs(frametime) / 1000.0f;
	
		realframetimems = (float)ticks_to_microsecs(gettime() - realframetime) / 1000.0f;
		realframetime = gettime();
		
		if(frameadvance) {
			u64 playbacktimeticks = gettime() - startplay;
			playbacktime = (float)ticks_to_microsecs(playbacktimeticks) / 1000.0f;
			playbacktime /= 33.3667000334f;
			frame = playbacktime;
		}

		VIDEO_SetNextFramebuffer(frameBuffer[fb]);

		#ifdef MINUSCONSOLE
		if(consoletog) {
			VIDEO_SetNextFramebuffer(consoleTex);
		}
		#endif
		VIDEO_Flush();		

		//if(rframe == 1) dumpXfbToSD(frameBuffer[fb]);
		if(rframe == 2) VIDEO_SetBlack(false);

		fb ^= 1;

		scenelastframe = scene;

		//rquad -= 1.5f;

		framerate = liym_min((1000.0f / frametimems), 30.0f);

		if(dolphin)
			if(frameadvance | !cameraControl) {
				if(frametimems < (100.0f / 3.0f)) VIDEO_WaitVSync();
				if(frametimems < (100.0f / 6.0f)) VIDEO_WaitVSync(); 
			}

		#ifdef GUY_DEV
			if(rframe % 20 == 0) {
				/*printf("%f ms for the blendshapes\n", blendtimems); 
				printf("%f ms for skeletal animation\n", skeletimems);
				printf("%f ms for cpu finish\n", cputimems);
				printf("%f ms for full real frametime\n", realframetimems);
				printf("%f ms for accumulated behindtime\n", accumulatedbehindtime);
				printf("%f ms for entire loop\n%f FPS\n", frametimems, framerate);
				printf("Frame is %d, Other int is %d, Lily Float is %f\n\n", frame + 1, Other_Int, Lily_Float);*/
			}
		#endif		

		rframe++;
		
		if(frame > 4102) VIDEO_SetBlack(true);
		if(frame > 4204) pleaseexit = 1;
	}

	printf("\n");
	for(int i = 0; i < sizeof(coolgill); i++) {
		putchar(coolgill[i]);
	}
	if(frame > 4200) 
	if(!istypist && !dolphin) { printf("psst.. connect a keyboard and mouse\n");
	} else { printf("psst.. .. thank you\n"); }
}
