#ifndef LAKE_DRAW_H
#define LAKE_DRAW_H
void drawLake() {
	GX_SetCullMode(GX_CULL_FRONT);
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);

	GX_LoadTexObj(&matcap1TexObj, GX_TEXMAP0);
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

	GX_SetCurrentMtx(GX_PNMTX0);
	
	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, 0); 

	GX_SetArray(GX_VA_POS, &lakeTorsovertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeTorsovertnor[0], sizeof(f32)*3);
	liy_batchLoadMtx(lakeSkeletonMv, lakeTorsoMtxidxTable, 63);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakeTorsoMtxidxTable, 63);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, lakeTorsotricount, 0, 
		NULL, NULL, lakeTorsovertposidx,
		NULL, NULL, lakeTorsovertnoridx,
		NULL, NULL, NULL,
		lakeTorsomtxidx,
		lakeTorsovertcol);

	GX_SetArray(GX_VA_POS, &lakeHandLvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeHandLvertnor[0], sizeof(f32)*3);
	liy_batchLoadMtx(lakeSkeletonMv, lakeHandLMtxidxTable, 15);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakeHandLMtxidxTable, 15);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, lakeHandLtricount, 0, 
		NULL, NULL, lakeHandLvertposidx,
		NULL, NULL, lakeHandLvertnoridx,
		NULL, NULL, NULL,
		lakeHandLmtxidx,
		lakeHandLvertcol);
	GX_SetArray(GX_VA_POS, &lakeHandRvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeHandRvertnor[0], sizeof(f32)*3);
	liy_batchLoadMtx(lakeSkeletonMv, lakeHandRMtxidxTable, 15);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakeHandRMtxidxTable, 15);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, lakeHandRtricount, 0, 
		NULL, NULL, lakeHandRvertposidx,
		NULL, NULL, lakeHandRvertnoridx,
		NULL, NULL, NULL,
		lakeHandRmtxidx,
		lakeHandRvertcol);

	GX_SetArray(GX_VA_POS, &lakeHandRFingersvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeHandRFingersvertnor[0], sizeof(f32)*3);
	liy_batchLoadMtx(lakeSkeletonMv, lakeHandRFingersMtxidxTable, LM_ALL);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakeHandRFingersMtxidxTable, LM_ALL);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, lakeHandRFingerstricount, 0, 
		NULL, NULL, lakeHandRFingersvertposidx,
		NULL, NULL, lakeHandRFingersvertnoridx,
		NULL, NULL, NULL,
		lakeHandRFingersmtxidx,
		lakeHandRFingersvertcol);
	GX_SetArray(GX_VA_POS, &lakeHandLFingersvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeHandLFingersvertnor[0], sizeof(f32)*3);
	liy_batchLoadMtx(lakeSkeletonMv, lakeHandLFingersMtxidxTable, LM_ALL);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakeHandLFingersMtxidxTable, LM_ALL);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, lakeHandLFingerstricount, 0, 
		NULL, NULL, lakeHandLFingersvertposidx,
		NULL, NULL, lakeHandLFingersvertnoridx,
		NULL, NULL, NULL,
		lakeHandLFingersmtxidx,
		lakeHandLFingersvertcol);

	GX_SetArray(GX_VA_POS, &lakeShoevertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeShoevertnor[0], sizeof(f32)*3);
	liy_batchLoadMtx(lakeSkeletonMv, lakeShoeMtxidxTable, 63);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakeShoeMtxidxTable, 63);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, lakeShoetricount, 0, 
		NULL, NULL, lakeShoevertposidx,
		NULL, NULL, lakeShoevertnoridx,
		NULL, NULL, NULL,
		lakeShoemtxidx,
		lakeShoevertcol);
	GX_SetArray(GX_VA_POS, &lakeShoelacevertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeShoelacevertnor[0], sizeof(f32)*3);
	liy_batchLoadMtx(lakeSkeletonMv, lakeShoelaceMtxidxTable, 3);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakeShoelaceMtxidxTable, 3);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, lakeShoelacetricount, 0, 
		NULL, NULL, lakeShoelacevertposidx,
		NULL, NULL, lakeShoelacevertnoridx,
		NULL, NULL, NULL,
		lakeShoelacemtxidx,
		lakeShoelacevertcol);
	GX_SetArray(GX_VA_POS, &lakeShoeLoopsvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeShoeLoopsvertnor[0], sizeof(f32)*3);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, lakeShoeLoopstricount, 0, 
		NULL, NULL, lakeShoeLoopsvertposidx,
		NULL, NULL, lakeShoeLoopsvertnoridx,
		NULL, NULL, NULL,
		lakeShoeLoopsmtxidx,
		lakeShoeLoopsvertcol);

	GX_SetArray(GX_VA_POS, &lakeBrowvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeBrowvertnor[0], sizeof(f32)*3);
	liy_batchLoadMtx(lakeSkeletonMv, lakeBrowMtxidxTable, 63);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakeBrowMtxidxTable, 63);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, lakeBrowtricount, 0, 
		NULL, NULL, lakeBrowvertposidx,
		NULL, NULL, lakeBrowvertnoridx,
		NULL, NULL, NULL,
		lakeBrowmtxidx,
		lakeBrowvertcol);

	GX_SetArray(GX_VA_POS, &lakePupilvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakePupilvertnor[0], sizeof(f32)*3);
	liy_batchLoadMtx(lakeSkeletonMv, lakePupilMtxidxTable, 63);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakePupilMtxidxTable, 63);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC | LV_MX, lakePupiltricount, 0, 
		NULL, NULL, lakePupilvertposidx,
		NULL, NULL, lakePupilvertnoridx,
		NULL, NULL, NULL,
		lakePupilmtxidx,
		lakePupilvertcol);

	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC, 0); 

	LX_LoadMtxImm(lakeSkeletonMv[4], GX_PNMTX0);
	GX_LoadTexMtxImm(lakeSkeletonMvReflectionmap[4], GX_TEXMTX0, GX_MTX3x4);
	GX_SetCurrentMtx(GX_PNMTX0);
	GX_SetArray(GX_VA_POS, &lakeHeadvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeHeadvertnor[0], sizeof(f32)*3);
	GX_CallDispList(lakeHeadDispList, lakeHeadDispListSize);

	LX_LoadMtxImm(lakeSkeletonMv[43], GX_PNMTX0);
	GX_LoadTexMtxImm(lakeSkeletonMvReflectionmap[43], GX_TEXMTX0, GX_MTX3x4);
	GX_SetCurrentMtx(GX_PNMTX0);
	GX_SetArray(GX_VA_POS, &lakeSclaraevertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeSclaraevertnor[0], sizeof(f32)*3);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC, lakeSclaraetricount, 0, 
		NULL, NULL, lakeSclaraevertposidx,
		NULL, NULL, lakeSclaraevertnoridx,
		NULL, NULL, NULL,
		NULL,
		lakeSclaraevertcol);

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
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
	GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

	GX_LoadTexObj(&lakerivercombineTexObj, GX_TEXMAP1);
	GX_SetTevSwapModeTable(GX_TEV_SWAP1, 1, 1, 1, 1);
	GX_SetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP1);
	GX_SetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP1);
	GX_SetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GX_SetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);

	GX_SetArray(GX_VA_POS, &lakePantvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakePantvertnor[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_TEX0, &lakePanttexcord[0], sizeof(f32)*2);
	liy_batchLoadMtx(lakeSkeletonMv, lakePantMtxidxTable, LM_ALL);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakePantMtxidxTable, LM_ALL);
	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX, 0);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX, lakePanttricount, 0, 
		NULL, NULL, lakePantvertposidx,
		NULL, NULL, lakePantvertnoridx,
		NULL, NULL, lakePanttexcordidx,
		lakePantmtxidx,
		NULL);

	GX_SetArray(GX_VA_POS, &lakeShirtTopvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeShirtTopvertnor[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_TEX0, &lakeShirtToptexcord[0], sizeof(f32)*2);
	liy_batchLoadMtx(lakeSkeletonMv, lakeShirtTopMtxidxTable, LM_ALL);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakeShirtTopMtxidxTable, LM_ALL);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX, lakeShirtToptricount, 0, 
		NULL, NULL, lakeShirtTopvertposidx,
		NULL, NULL, lakeShirtTopvertnoridx,
		NULL, NULL, lakeShirtToptexcordidx,
		lakeShirtTopmtxidx,
		NULL);
	GX_SetArray(GX_VA_POS, &lakeShirtBottomvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeShirtBottomvertnor[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_TEX0, &lakeShirtBottomtexcord[0], sizeof(f32)*2);
	liy_batchLoadMtx(lakeSkeletonMv, lakeShirtBottomMtxidxTable, LM_ALL);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakeShirtBottomMtxidxTable, LM_ALL);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX, lakeShirtBottomtricount, 0, 
		NULL, NULL, lakeShirtBottomvertposidx,
		NULL, NULL, lakeShirtBottomvertnoridx,
		NULL, NULL, lakeShirtBottomtexcordidx,
		lakeShirtBottommtxidx,
		NULL);

	GX_SetArray(GX_VA_POS, &lakeEarvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeEarvertnor[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_TEX0, &lakeEartexcord[0], sizeof(f32)*2);
	liy_batchLoadMtx(lakeSkeletonMv, lakeEarMtxidxTable, 3);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakeEarMtxidxTable, 3);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX, lakeEartricount, 0, 
		NULL, NULL, lakeEarvertposidx,
		NULL, NULL, lakeEarvertnoridx,
		NULL, NULL, lakeEartexcordidx,
		lakeEarmtxidx,
		NULL);
	
	GX_SetArray(GX_VA_POS, &lakeSockvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeSockvertnor[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_TEX0, &lakeSocktexcord[0], sizeof(f32)*2);
	liy_batchLoadMtx(lakeSkeletonMv, lakeSockMtxidxTable, 3);
	liy_batchLoadTexMtx(lakeSkeletonMvReflectionmap, lakeSockMtxidxTable, 3);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX, lakeSocktricount, 0, 
		NULL, NULL, lakeSockvertposidx,
		NULL, NULL, lakeSockvertnoridx,
		NULL, NULL, lakeSocktexcordidx,
		lakeSockmtxidx,
		NULL);

	liy_VtxDescConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16, 0);
	GX_SetArray(GX_VA_POS, &lakeHairvertpos[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_NRM, &lakeHairvertnor[0], sizeof(f32)*3);
	GX_SetArray(GX_VA_TEX0, &lakeHairtexcord[0], sizeof(f32)*2);
	LX_LoadMtxImm(lakeSkeletonMv[4], GX_PNMTX0);
	GX_LoadTexMtxImm(lakeSkeletonMvReflectionmap[4], GX_TEXMTX0, GX_MTX3x4);
	GX_SetCurrentMtx(GX_PNMTX0);
	drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16, lakeHairtricount, 0, 
		NULL, NULL, lakeHairvertposidx,
		NULL, NULL, lakeHairvertnoridx,
		NULL, NULL, lakeHairtexcordidx,
		NULL,
		NULL);
	
	GX_SetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GX_SetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
}
#endif
