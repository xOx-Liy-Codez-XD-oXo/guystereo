#ifndef GILL_DRAW_H
#define GILL_DRAW_H
void gillCullMode() {
	GX_SetCullMode((gillReflect) ? GX_CULL_BACK : GX_CULL_FRONT);
}
void drawGill() {
	Mtx * gillMatrices = (gillReflect) ? gillSkeletonMvReflect : gillSkeletonMv;
	Mtx * gillRefMatrices = (gillReflect) ? gillSkeletonMvReflectReflectionmap : gillSkeletonMvReflectionmap;
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);

	GX_SetNumTexGens(1); //Vertex colored non textured matcapped
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);
	GX_LoadTexObj(&matcap1TexObj, GX_TEXMAP0);
	GX_SetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
	GX_SetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
	GX_SetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K0);
	GX_SetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K0);
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
	enableLightNrmDbg();

	GX_SetArray(GX_VA_POS, &gillTorsovertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillTorsovertnor[0], sizeof(f32)*3);

	liy_batchLoadMtx(gillMatrices, gillTorsoMtxidxTable, LM_ALL);
	liy_batchLoadTexMtx(gillRefMatrices, gillTorsoMtxidxTable, LM_ALL);
	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, 0); 
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, gillTorsotricount, 0, 
			NULL, NULL, gillTorsovertposidx,
			NULL, NULL, gillTorsovertnoridx,
			NULL, NULL, NULL,
			gillTorsomtxidx,
			gillTorsovertcol);

	GX_SetArray(GX_VA_POS, &gillHandLvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillHandLvertnor[0], sizeof(f32)*3);

	liy_batchLoadMtx(gillMatrices, gillHandLMtxidxTable, LM_0 | LM_1 | LM_2 | LM_3);
	liy_batchLoadTexMtx(gillRefMatrices, gillHandLMtxidxTable, LM_0 | LM_1 | LM_2 | LM_3);
	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, 0); 
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, gillHandLtricount, 0, 
			NULL, NULL, gillHandLvertposidx,
			NULL, NULL, gillHandLvertnoridx,
			NULL, NULL, NULL,
			gillHandLmtxidx,
			gillHandLvertcol);

	GX_SetArray(GX_VA_POS, &gillHandLfingersvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillHandLfingersvertnor[0], sizeof(f32)*3);

	liy_batchLoadMtx(gillMatrices, gillHandLfingersMtxidxTable, LM_ALL);	
	liy_batchLoadTexMtx(gillRefMatrices, gillHandLfingersMtxidxTable, LM_ALL);
	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, 0); 
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, gillHandLfingerstricount, 0, 
			NULL, NULL, gillHandLfingersvertposidx,
			NULL, NULL, gillHandLfingersvertnoridx,
			NULL, NULL, NULL,
			gillHandLfingersmtxidx,
			gillHandLfingersvertcol);

	GX_SetArray(GX_VA_POS, &gillHandRvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillHandRvertnor[0], sizeof(f32)*3);

	liy_batchLoadMtx(gillMatrices, gillHandRMtxidxTable, LM_0 | LM_1 | LM_2 | LM_3);
	liy_batchLoadTexMtx(gillRefMatrices, gillHandRMtxidxTable, LM_0 | LM_1 | LM_2 | LM_3);
	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, 0); 
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, gillHandRtricount, 0, 
			NULL, NULL, gillHandRvertposidx,
			NULL, NULL, gillHandRvertnoridx,
			NULL, NULL, NULL,
			gillHandRmtxidx,
			gillHandRvertcol);

	GX_SetArray(GX_VA_POS, &gillHandRfingersvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillHandRfingersvertnor[0], sizeof(f32)*3);

	liy_batchLoadMtx(gillMatrices, gillHandRfingersMtxidxTable, LM_ALL);	
	liy_batchLoadTexMtx(gillRefMatrices, gillHandRfingersMtxidxTable, LM_ALL);
	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, 0); 
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, gillHandRfingerstricount, 0, 
			NULL, NULL, gillHandRfingersvertposidx,
			NULL, NULL, gillHandRfingersvertnoridx,
			NULL, NULL, NULL,
			gillHandRfingersmtxidx,
			gillHandRfingersvertcol);

	liy_batchLoadMtx(gillMatrices, gillPresocksMtxidxTable, LM_0 | LM_1);	
	liy_batchLoadTexMtx(gillRefMatrices, gillPresocksMtxidxTable, LM_0 | LM_1);	
	GX_SetArray(GX_VA_POS, &gillPresocksvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillPresocksvertnor[0], sizeof(f32)*3);

	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, 0); 
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, gillPresockstricount, 0, 
			NULL, NULL, gillPresocksvertposidx,
			NULL, NULL, gillPresocksvertnoridx,
			NULL, NULL, NULL,
			gillPresocksmtxidx,
			gillPresocksvertcol);

	liy_batchLoadMtx(gillMatrices, gillSocksMtxidxTable, LM_0 | LM_1 | LM_2 | LM_3 | LM_4 | LM_5);
	liy_batchLoadTexMtx(gillRefMatrices, gillSocksMtxidxTable, LM_0 | LM_1 | LM_2 | LM_3 | LM_4 | LM_5);
	GX_SetArray(GX_VA_POS, &gillSocksvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillSocksvertnor[0], sizeof(f32)*3);

	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, 0); 
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, gillSockstricount, 0, 
			NULL, NULL, gillSocksvertposidx,
			NULL, NULL, gillSocksvertnoridx,
			NULL, NULL, NULL,
			gillSocksmtxidx,
			gillSocksvertcol);

	LX_LoadMtxImm(gillMatrices[4], GX_PNMTX0);
	GX_LoadTexMtxImm(gillRefMatrices[4], GX_TEXMTX0, GX_MTX3x4);
	GX_SetCurrentMtx(GX_PNMTX0);

	GX_SetArray(GX_VA_POS, &gillHeadvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillHeadvertnor[0], sizeof(f32)*3);

	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC, 0); 
	if(!gillheadspecial)
	GX_CallDispList(gillHeadDispList, gillHeadDispListSize);

	LX_LoadMtxImm(gillMatrices[54], GX_PNMTX0);
	LX_LoadMtxImm(gillMatrices[53], GX_PNMTX1);
	GX_LoadTexMtxImm(gillRefMatrices[54], GX_TEXMTX0, GX_MTX3x4);
	GX_LoadTexMtxImm(gillRefMatrices[53], GX_TEXMTX1, GX_MTX3x4);

	GX_SetArray(GX_VA_POS, &gillEarvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillEarvertnor[0], sizeof(f32)*3);

	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, 0); 
	if(!gillheadspecial) {
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, gillEartricount, 0, 
			NULL, NULL, gillEarvertposidx,
			NULL, NULL, gillEarvertnoridx,
			NULL, NULL, NULL,
			gillEarmtxidx,
			gillEarvertcol);

	liy_batchLoadMtx(gillMatrices, gillBrowMtxidxTable, LM_0 | LM_1 | LM_2 | LM_3 | LM_4 | LM_5);
	liy_batchLoadTexMtx(gillRefMatrices, gillBrowMtxidxTable, LM_0 | LM_1 | LM_2 | LM_3 | LM_4 | LM_5);
	GX_SetArray(GX_VA_POS, &gillBrowvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillBrowvertnor[0], sizeof(f32)*3);
	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, 0);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, gillBrowtricount, 0, 
			NULL, NULL, gillBrowvertposidx,
			NULL, NULL, gillBrowvertnoridx,
			NULL, NULL, NULL,
			gillBrowmtxidx,
			gillBrowvertcol);
	}

	//textured objects

	GX_SetNumTexGens(2);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);
	GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);


	GX_SetNumTevStages(4);

	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO); //amb * tex
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
	GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC); //tex
	GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
	GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C1, GX_CC_TEXC, GX_CC_ZERO); //light * tex
	GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
	GX_SetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_ONE, GX_CC_C0, GX_CC_C1); //(light * tex) + (amb * tex)
	GX_SetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP7, GX_COLOR0A0);
	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP7, GX_COLOR0A0);
	GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

	enableLightNrmDbg();

	if(gillheadspecial) {
		Mtx liely, luily;
		guMtxRotRad(model, 'x', -1.57);
		guMtxRotRad(liely, 'z', 3.142);
		guMtxConcat(model, liely, model);
		if(eye)
			guMtxTransApply(model, model, -0.05f - 0.032, -1.3f, 0.075f); //right
		else
			guMtxTransApply(model, model, -0.05f + 0.032, -1.3f, 0.075f); //left
		LX_LoadMtxImm(model, GX_PNMTX0);
		guMtxCopyNopos(model, model);
		guMtxScaleApply(model, model, 0.5f, -0.5f, 0.0f);
		guMtxTransApply(model, model, 0.5f, 0.5f, 1.0f);
		GX_LoadTexMtxImm(model, GX_TEXMTX0, GX_MTX3x4);
	} else { 
		LX_LoadMtxImm(gillMatrices[4], GX_PNMTX0);
		GX_LoadTexMtxImm(gillRefMatrices[4], GX_TEXMTX0, GX_MTX3x4);
	}
	GX_SetCurrentMtx(GX_PNMTX0);
	GX_SetArray(GX_VA_POS, &gillHairvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillHairvertnor[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_TEX0, &gillHairtexcord[0], sizeof(f32)*2);
			
	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16, 0); 
	GX_CallDispList(gillHairDispList, gillHairDispListSize);

	GX_SetArray(GX_VA_POS, &gillPantvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillPantvertnor[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_TEX0, &gillPanttexcord[0], sizeof(f32)*2);

	liy_batchLoadMtx(gillMatrices, gillPantsMtxidxTable, LM_0 | LM_1 | LM_2 | LM_3 | LM_4);
	liy_batchLoadTexMtx(gillRefMatrices, gillPantsMtxidxTable, LM_0 | LM_1 | LM_2 | LM_3 | LM_4);
	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX, 0);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX, gillPanttricount, 0, 
			NULL, NULL, gillPantvertposidx,
			NULL, NULL, gillPantvertnoridx,
			NULL, NULL, gillPanttexcordidx,
			gillPantmtxidx,
			NULL);

	//the shirt is special. it has per vert colors and a texture on the front. 

	GX_SetNumTevStages(4);
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO); 		//vtx * tex , albedo in 0
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
	GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO); 			//(vtx * tex) * light, in 1
	GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
	GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C0, GX_CC_KONST, GX_CC_ZERO); 			//(vtx * tex) * ambient, in 2
	GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);
	GX_SetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_ONE, GX_CC_C1, GX_CC_C2); 			//((vtx * tex) * ambient) + ((vtx * tex) * light)
	GX_SetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP7, GX_COLOR0A0);
	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	enableLightNrmDbg();			

	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_VC | LV_MX, 0);
	liy_batchLoadMtx(gillMatrices, gillShirtMtxidxTable, LM_ALL);
	liy_batchLoadTexMtx(gillRefMatrices, gillShirtMtxidxTable, LM_ALL);
	GX_SetArray(GX_VA_POS, &gillShirtvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &gillShirtvertnor[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_TEX0, &gillShirttexcord[0], sizeof(f32)*2);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_VC | LV_MX, gillShirttricount, 0, 
			NULL, NULL, gillShirtvertposidx,
			NULL, NULL, gillShirtvertnoridx,
			NULL, NULL, gillShirttexcordidx,
			gillShirtmtxidx,
			gillShirtvertcol);
	if(gilljacketed) {
		GX_SetNumTexGens(1);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);
		GX_SetNumTevStages(1);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO); 
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);
		GX_LoadTexObj(&leatherMatcapTexObj, GX_TEXMAP1);
		LX_LoadMtxImm(gillMatrices[1], GX_PNMTX0); GX_LoadTexMtxImm(gillRefMatrices[1], GX_TEXMTX0, GX_MTX3x4);
		LX_LoadMtxImm(gillMatrices[2], GX_PNMTX1); GX_LoadTexMtxImm(gillRefMatrices[2], GX_TEXMTX1, GX_MTX3x4);
		LX_LoadMtxImm(gillMatrices[13], GX_PNMTX2); GX_LoadTexMtxImm(gillRefMatrices[13], GX_TEXMTX2, GX_MTX3x4);
		LX_LoadMtxImm(gillMatrices[14], GX_PNMTX3); GX_LoadTexMtxImm(gillRefMatrices[14], GX_TEXMTX3, GX_MTX3x4);
		LX_LoadMtxImm(gillMatrices[15], GX_PNMTX4); GX_LoadTexMtxImm(gillRefMatrices[15], GX_TEXMTX4, GX_MTX3x4);
		LX_LoadMtxImm(gillMatrices[16], GX_PNMTX5); GX_LoadTexMtxImm(gillRefMatrices[16], GX_TEXMTX5, GX_MTX3x4);
		LX_LoadMtxImm(gillMatrices[28], GX_PNMTX6); GX_LoadTexMtxImm(gillRefMatrices[28], GX_TEXMTX6, GX_MTX3x4);
		LX_LoadMtxImm(gillMatrices[29], GX_PNMTX7); GX_LoadTexMtxImm(gillRefMatrices[29], GX_TEXMTX7, GX_MTX3x4);
		LX_LoadMtxImm(gillMatrices[30], GX_PNMTX8); GX_LoadTexMtxImm(gillRefMatrices[30], GX_TEXMTX8, GX_MTX3x4);
		LX_LoadMtxImm(gillMatrices[31], GX_PNMTX9); GX_LoadTexMtxImm(gillRefMatrices[31], GX_TEXMTX9, GX_MTX3x4);
		ag_config_riverjacketgill(0);
		ag_draw_riverjacketgill(0);
	}
}
#endif
