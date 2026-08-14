#ifndef LIGHT_H
#define LIGHT_H

void setlight(Mtx view, GXColor ambcol, int scene) {

	guVector lpos;
	GXLightObj lobj;	

	GX_SetNumChans(1);

	switch(scene) {
		case 0:
			static GXColor grascol = {2, 36, 32, 255}; //5 107 97

			//Sun
			lpos.x = 0.0f;
			lpos.y = -500.0f;
			lpos.z = 432.0f;

			guVecMultiply(view,&lpos,&lpos);

			//i was like, this sucks, but libogc doc says to do this for directional lights
			GX_InitLightPos(&lobj,lpos.x,lpos.y,lpos.z);
			GX_InitLightColor(&lobj, LC_LIGHTERGREY);
   			GX_InitLightDistAttn(&lobj, 0.5f, 0.5f, GX_DA_OFF);
			GX_LoadLightObj(&lobj, GX_LIGHT0);

			//Grass lightbounce
			lpos.x = 0.0f;
			lpos.y = 0.0f;
			lpos.z = -500.0f;

			guVecMultiply(view,&lpos,&lpos);

			lpos.z += 500.0f;

			GX_InitLightPos(&lobj, lpos.x, lpos.y, lpos.z);
			GX_InitLightColor(&lobj, LC_BLACK);
   			GX_InitLightDistAttn(&lobj, 0.5f, 0.5f, GX_DA_OFF);
			GX_LoadLightObj(&lobj, GX_LIGHT1);
	
			GX_SetChanAmbColor(GX_COLOR0A0,LC_GAYSKY);
			GX_SetChanMatColor(GX_COLOR0A0,LC_WHITE);
			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0 | GX_LIGHT1, GX_DF_CLAMP, GX_AF_NONE);
			break;

		case 1: //Hallway
		case 5:
			lpos.x = 0.0f;
			lpos.y = -500.0f;
			lpos.z = 0.0f;

			guVecMultiply(view, &lpos, &lpos);
	
			GX_InitLightPos(&lobj, lpos.x, lpos.y, lpos.z); //Window lighting
			GX_InitLightColor(&lobj, LC_WHITE);
			GX_InitLightDistAttn(&lobj, 0.5f, 0.5f, GX_DA_OFF);
			GX_LoadLightObj(&lobj, GX_LIGHT0);

			lpos.x = 0.0f;
			lpos.y = 0.0f;
			lpos.z = 500.0f;

			guVecMultiply(view, &lpos, &lpos);
	
			GX_InitLightPos(&lobj, lpos.x, lpos.y, lpos.z);
			GX_InitLightColor(&lobj, LC_GREY);
			GX_InitLightDistAttn(&lobj, 0.5f, 0.5f, GX_DA_OFF);
			GX_LoadLightObj(&lobj, GX_LIGHT1);

			GX_SetChanAmbColor(GX_COLOR0A0, LC_DARKGREY); //ambcol
			GX_SetChanMatColor(GX_COLOR0A0, LC_WHITE);
			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0 | GX_LIGHT1, GX_DF_CLAMP, GX_AF_NONE);
			break;

		case 2: //Cafeteria
			lpos.x = -500.0f;
			lpos.y = 200.0f;
			lpos.z = 100.0f;

			guVecMultiply(view, &lpos, &lpos);
	
			GX_InitLightPos(&lobj, lpos.x, lpos.y, lpos.z);
			GX_InitLightColor(&lobj, LC_BLACK);
			GX_InitLightDistAttn(&lobj, 0.5f, 0.5f, GX_DA_OFF);
			GX_LoadLightObj(&lobj, GX_LIGHT0);

			lpos.x = -500.0f;
			lpos.y = -200.0f;
			lpos.z = 100.0f;

			guVecMultiply(view, &lpos, &lpos);
	
			GX_InitLightPos(&lobj, lpos.x, lpos.y, lpos.z);
			GX_InitLightColor(&lobj, LC_WHITE);
			GX_InitLightDistAttn(&lobj, 0.5f, 0.5f, GX_DA_OFF);
			GX_LoadLightObj(&lobj, GX_LIGHT1);

			GX_SetChanAmbColor(GX_COLOR0A0, LC_CAFEAM); 
			GX_SetChanMatColor(GX_COLOR0A0, LC_WHITE);
			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0 | GX_LIGHT1, GX_DF_CLAMP, GX_AF_NONE);
			break;


		case 3: //Gill bedroom
			lpos.x = 0.0f;
			lpos.y = -300.0f;
			lpos.z = 900.0f;

			guVecMultiply(view, &lpos, &lpos);
	
			GX_InitLightPos(&lobj, lpos.x, lpos.y, lpos.z); //overhead
			GX_InitLightColor(&lobj, LC_WHITE);
			GX_InitLightDistAttn(&lobj, 0.1f, 0.1f, GX_DA_OFF);
			GX_LoadLightObj(&lobj, GX_LIGHT0);

			GX_SetChanAmbColor(GX_COLOR0A0, LC_GILLPURPLE);
			GX_SetChanMatColor(GX_COLOR0A0, LC_WHITE);
			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
			break;
		default:
			GX_SetChanAmbColor(GX_COLOR0A0, ambcol);
			GX_SetChanMatColor(GX_COLOR0A0, LC_WHITE);
			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);
	}
}

void nolight() {
	GX_SetChanAmbColor(GX_COLOR0A0, LC_WHITE); //ambcol
	GX_SetChanMatColor(GX_COLOR0A0, LC_WHITE);
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
}

int whichlightmask(int scene) {
	switch(scene) {
		case 0:
		case 1:
		case 2:
		case 5:
			return GX_LIGHT0 | GX_LIGHT1;
			break;
		case 3:
			return GX_LIGHT0;
			break;
		default:
			return GX_LIGHTNULL;
			break;
	}
}

GXColor whatambcol(int scene) {
	switch(scene) {
		case 0:
			return LC_GAYSKY;
			break;
		case 1:
			return LC_DARKGREY;
			break;
		case 2:
			return LC_CAFEAM;
			break;
		case 3:
			return LC_GILLPURPLE;
			break;
		case 5:
			return LC_GILLPURPLE;
			break;
		default:
			return LC_BLACK;
			break;
	}
}

#endif