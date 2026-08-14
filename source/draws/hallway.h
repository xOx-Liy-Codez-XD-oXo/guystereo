#ifndef HALLWAY_DRAW_H
#define HALLWAY_DRAW_H

f32 hallwayFakeRealLockerXPoss[14] = {-51.826, -40.215, -17.589, -5.637, 5.215, 49.037, 71.2, 82.92, 95.005, 117.27, 128.32, 139.57, 161.24, 173.03};

void drawHallway(float l1, float l2) {
	GX_SetCurrentMtx(GX_PNMTX0);

	float hallscale = 0.2f; //1 / 5
			Mtx hallOrigin, hallOriginMv;
			guMtxIdentity(hallOrigin);
			guMtxScaleApply(hallOrigin, hallOrigin, hallscale, hallscale, hallscale);
			guMtxConcat(view, hallOrigin, hallOriginMv);

			nolight();
			
			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);	

			GX_SetNumTevStages(1);

			GX_SetTevOp(GX_TEVSTAGE0,GX_PASSCLR);
	
			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_RASC, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);


			LX_LoadMtxImm(hallOriginMv, GX_PNMTX0);

			GX_SetArray(GX_VA_POS, &hallwayvcolexvertpos[0], sizeof(f32)*3);

			liy_VtxDescConfig(LV_VPIDX16 | LV_VC, 0); 
			drawArrConfig(LV_VPIDX16 | LV_VC, hallwayvcolextricount, 0, 
					NULL, NULL, hallwayvcolexvertposidx,
					NULL, NULL, NULL,
					NULL, NULL, NULL,
					NULL,
					hallwayvcolexvertcol);

			for(int i = 0; i < 14; i++) { //Draw fake real lockers
				guMtxIdentity(model);
				//guMtxTransApply(model, model, hallwayFakeRealLockerXPoss[i], 0.97086f, -2.7566f);
				guMtxTransApply(model, model, hallwayFakeRealLockerXPoss[i], 6.8f, 0.586f);
				guMtxScaleApply(model, model, hallscale, hallscale, hallscale);
				guMtxConcat(view, model, modelview);

				GX_LoadPosMtxImm(modelview, GX_PNMTX0); //why was i loading nrm mtxs elsewhere. too lazy to change

				GX_CallDispList(lockerDispList, lockerDispListSize);
			}

			guMtxIdentity(model);
			guMtxRotAxisRad(model, &zAxis, l1);
			guMtxTransApply(model, model, 34.54f, 5.73f, 7.63f);
			guMtxScaleApply(model, model, hallscale, hallscale, hallscale);
			guMtxConcat(view, model, modelview);
			LX_LoadMtxImm(modelview, GX_PNMTX0);

			GX_SetArray(GX_VA_POS, &hallwayoplockervertpos[0], sizeof(f32)*3);
			
			drawArrConfig(LV_VPIDX16 | LV_VC, hallwayoplockertricount, 0, 
					NULL, NULL, hallwayoplockervertposidx,
					NULL, NULL, NULL,
					NULL, NULL, NULL,
					NULL,
					hallwayoplockervertcol);

			guMtxIdentity(model);
			guMtxRotAxisRad(model, &zAxis, l2);
			guMtxTransApply(model, model, 42.61f, 5.73f, 7.63f);
			guMtxScaleApply(model, model, hallscale, hallscale, hallscale);
			guMtxConcat(view, model, modelview);
			LX_LoadMtxImm(modelview, GX_PNMTX0);
			
			drawArrConfig(LV_VPIDX16 | LV_VC, hallwayoplockertricount, 0, 
					NULL, NULL, hallwayoplockervertposidx,
					NULL, NULL, NULL,
					NULL, NULL, NULL,
					NULL,
					hallwayoplockervertcol);

			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);

			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP4, GX_COLOR0A0);

			LX_LoadMtxImm(hallOriginMv, GX_PNMTX0);

			liy_VtxDescConfig(LV_VPIDX16 | LV_TCIDX16, 0); 

			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP6, GX_COLOR0A0);
	
			GX_SetArray(GX_VA_POS, &hallwayendcapvertpos[0], sizeof(f32)*3);
			GX_SetArray(GX_VA_TEX0, &hallwayendcaptexcord[0], sizeof(f32)*2);

			drawArrConfig(LV_VPIDX16 | LV_TCIDX16, hallwayendcaptricount, 0, 
					NULL, NULL, hallwayendcapvertposidx,
					NULL, NULL, NULL,
					NULL, NULL, hallwayendcaptexcordidx,
					NULL,
					NULL);

			GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);

			GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP5, GX_COLOR0A0);

			GX_SetArray(GX_VA_POS, &hallwayfloorceilvertpos[0], sizeof(f32)*3);
			GX_SetArray(GX_VA_TEX0, &hallwayfloorceiltexcord[0], sizeof(f32)*2);

			liy_VtxDescConfig(LV_VPIDX16 | LV_TCIDX16 | LV_VC, 0); 
			drawArrConfig(LV_VPIDX16 | LV_TCIDX16 | LV_VC, hallwayfloorceiltricount, 0, 
					NULL, NULL, hallwayfloorceilvertposidx,
					NULL, NULL, NULL,
					NULL, NULL, hallwayfloorceiltexcordidx,
					NULL,
					hallwayfloorceilvertcol);
			
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP4, GX_COLOR0A0);

			GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);

			GX_SetArray(GX_VA_POS, &hallwaycorkboardvertpos[0], sizeof(f32)*3);
			GX_SetArray(GX_VA_TEX0, &hallwaycorkboardtexcord[0], sizeof(f32)*2);
	
			drawArrConfig(LV_VPIDX16 | LV_TCIDX16 | LV_VC, hallwaycorkboardtricount, 0, 
					NULL, NULL, hallwaycorkboardvertposidx,
					NULL, NULL, NULL,
					NULL, NULL, hallwaycorkboardtexcordidx,
					NULL,
					hallwaycorkboardvertcol);
}
#endif
