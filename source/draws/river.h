#ifndef RIVER_DRAW_H
#define RIVER_DRAW_H

void drawRiver() {
	GX_SetCullMode(GX_CULL_FRONT);
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);

	GX_SetNumTexGens(2); //Light and vertex color and texture
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);
	GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
	GX_LoadTexObj(&matcap1TexObj, GX_TEXMAP0);
	GX_SetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
	GX_SetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
	GX_SetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K0);
	GX_SetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K0);
	GX_SetTevKColor(GX_KCOLOR0, whatambcol(scene));
	GX_SetNumTevStages(4);
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO); 		//vtx * tex , albedo in 0
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
	GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO); 			//(vtx * tex) * light, in 1
	GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
	GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C0, GX_CC_KONST, GX_CC_ZERO); 			//(vtx * tex) * ambient, in 2
	GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);
	GX_SetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_ONE, GX_CC_C1, GX_CC_C2); 			//((vtx * tex) * ambient) + ((vtx * tex) * light)
	GX_SetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	
	LX_LoadMtxImm(riverSkeletonMv[1], GX_PNMTX0); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[1], GX_TEXMTX0, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[2], GX_PNMTX1); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[2], GX_TEXMTX1, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[3], GX_PNMTX2); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[3], GX_TEXMTX2, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[6], GX_PNMTX3); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[6], GX_TEXMTX3, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[24], GX_PNMTX4); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[24], GX_TEXMTX4, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[7], GX_PNMTX5); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[7], GX_TEXMTX5, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[25], GX_PNMTX6); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[25], GX_TEXMTX6, GX_MTX3x4);
	GX_LoadTexObj(&rivershirtTexObj, GX_TEXMAP1);
	ag_config_riverShirt(0);
	if(riverjacketed) {
		ag_draw_riverShirtCovered(0);
		GX_SetNumTexGens(1);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);
		GX_SetNumTevStages(1);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO); 
		GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);
		GX_LoadTexObj(&leatherMatcapTexObj, GX_TEXMAP1);
		LX_LoadMtxImm(riverSkeletonMv[7], GX_PNMTX0); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[7], GX_TEXMTX0, GX_MTX3x4);
		LX_LoadMtxImm(riverSkeletonMv[25], GX_PNMTX1); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[25], GX_TEXMTX1, GX_MTX3x4);
		LX_LoadMtxImm(riverSkeletonMv[1], GX_PNMTX2); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[1], GX_TEXMTX2, GX_MTX3x4);
		LX_LoadMtxImm(riverSkeletonMv[2], GX_PNMTX3); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[2], GX_TEXMTX3, GX_MTX3x4);
		LX_LoadMtxImm(riverSkeletonMv[3], GX_PNMTX4); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[3], GX_TEXMTX4, GX_MTX3x4);
		LX_LoadMtxImm(riverSkeletonMv[6], GX_PNMTX5); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[6], GX_TEXMTX5, GX_MTX3x4);
		LX_LoadMtxImm(riverSkeletonMv[24], GX_PNMTX6); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[24], GX_TEXMTX6, GX_MTX3x4);
		ag_config_riverJacket(0);
		ag_draw_riverJacket(0);
	} else {
		ag_draw_riverShirt(0);
	}

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

	LX_LoadMtxImm(riverSkeletonMv[1], GX_PNMTX0); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[1], GX_TEXMTX0, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[2], GX_PNMTX1); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[2], GX_TEXMTX1, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[3], GX_PNMTX2); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[3], GX_TEXMTX2, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[6], GX_PNMTX3); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[6], GX_TEXMTX3, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[24], GX_PNMTX4); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[24], GX_TEXMTX4, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[5], GX_PNMTX5); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[5], GX_TEXMTX5, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[7], GX_PNMTX6); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[7], GX_TEXMTX6, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[25], GX_PNMTX7); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[25], GX_TEXMTX7, GX_MTX3x4);
	ag_config_riverSkinTorso(0);
	ag_draw_riverSkinTorso(0);
	LX_LoadMtxImm(riverSkeletonMv[7], GX_PNMTX0); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[7], GX_TEXMTX0, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[8], GX_PNMTX1); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[8], GX_TEXMTX1, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[25], GX_PNMTX2); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[25], GX_TEXMTX2, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[26], GX_PNMTX3); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[26], GX_TEXMTX3, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[9], GX_PNMTX4); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[9], GX_TEXMTX4, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[19], GX_PNMTX5); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[19], GX_TEXMTX5, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[20], GX_PNMTX6); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[20], GX_TEXMTX6, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[27], GX_PNMTX7); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[27], GX_TEXMTX7, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[37], GX_PNMTX8); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[37], GX_TEXMTX8, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[38], GX_PNMTX9); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[38], GX_TEXMTX9, GX_MTX3x4);
	ag_draw_riverSkinArms(0);
	LX_LoadMtxImm(riverSkeletonMv[1], GX_PNMTX0); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[1], GX_TEXMTX0, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[21], GX_PNMTX1); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[21], GX_TEXMTX1, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[22], GX_PNMTX2); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[22], GX_TEXMTX2, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[39], GX_PNMTX3); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[39], GX_TEXMTX3, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[40], GX_PNMTX4); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[40], GX_TEXMTX4, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[23], GX_PNMTX5); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[23], GX_TEXMTX5, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[41], GX_PNMTX6); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[41], GX_TEXMTX6, GX_MTX3x4);
	ag_draw_riverSkinLegs(0);
	ag_draw_riverPants(0);
	LX_LoadMtxImm(riverSkeletonMv[9], GX_PNMTX0); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[9], GX_TEXMTX0, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[13], GX_PNMTX1); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[13], GX_TEXMTX1, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[14], GX_PNMTX2); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[14], GX_TEXMTX2, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[15], GX_PNMTX3); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[15], GX_TEXMTX3, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[10], GX_PNMTX4); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[10], GX_TEXMTX4, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[11], GX_PNMTX5); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[11], GX_TEXMTX5, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[12], GX_PNMTX6); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[12], GX_TEXMTX6, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[16], GX_PNMTX7); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[16], GX_TEXMTX7, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[17], GX_PNMTX8); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[17], GX_TEXMTX8, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[18], GX_PNMTX9); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[18], GX_TEXMTX9, GX_MTX3x4);
	ag_draw_riverSkinHandL(0);
	LX_LoadMtxImm(riverSkeletonMv[27], GX_PNMTX0); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[27], GX_TEXMTX0, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[31], GX_PNMTX1); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[31], GX_TEXMTX1, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[32], GX_PNMTX2); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[32], GX_TEXMTX2, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[33], GX_PNMTX3); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[33], GX_TEXMTX3, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[28], GX_PNMTX4); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[28], GX_TEXMTX4, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[29], GX_PNMTX5); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[29], GX_TEXMTX5, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[30], GX_PNMTX6); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[30], GX_TEXMTX6, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[34], GX_PNMTX7); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[34], GX_TEXMTX7, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[35], GX_PNMTX8); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[35], GX_TEXMTX8, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[36], GX_PNMTX9); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[36], GX_TEXMTX9, GX_MTX3x4);
	ag_draw_riverSkinHandR(0);
	LX_LoadMtxImm(riverSkeletonMv[42], GX_PNMTX0); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[42], GX_TEXMTX0, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[43], GX_PNMTX1); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[43], GX_TEXMTX1, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[44], GX_PNMTX2); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[44], GX_TEXMTX2, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[45], GX_PNMTX3); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[45], GX_TEXMTX3, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[46], GX_PNMTX4); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[46], GX_TEXMTX4, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[47], GX_PNMTX5); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[47], GX_TEXMTX5, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[48], GX_PNMTX6); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[48], GX_TEXMTX6, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[49], GX_PNMTX7); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[49], GX_TEXMTX7, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[50], GX_PNMTX8); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[50], GX_TEXMTX8, GX_MTX3x4);
	if(riverhappyornot) {
		ag_draw_riverEyeClusterHappy(0);
	} else { 
		ag_draw_riverEyeCluster(0);
	}
	LX_LoadMtxImm(riverSkeletonMv[23], GX_PNMTX0); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[23], GX_TEXMTX0, GX_MTX3x4);
	LX_LoadMtxImm(riverSkeletonMv[41], GX_PNMTX1); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[41], GX_TEXMTX1, GX_MTX3x4);
	ag_draw_riverShoes(0);
	LX_LoadMtxImm(riverSkeletonMv[5], GX_PNMTX0); GX_LoadTexMtxImm(riverSkeletonMvReflectionmap[5], GX_TEXMTX0, GX_MTX3x4);
	GX_SetCurrentMtx(GX_PNMTX0);
	ag_config_riverHead(0);
	ag_draw_riverHead(0);

	GX_SetNumTevStages(1); //shade hair traditionally ... its dark color sidesteps the tone shortcomings of vtx shading
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
	GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP2, GX_COLOR0A0);

	GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
	
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP); // no blending for these
	GX_SetAlphaCompare(GX_GEQUAL, 192, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetAlphaUpdate(GX_FALSE);
	GX_SetZCompLoc(GX_FALSE);

	GX_LoadTexObj(&lakerivercombineTexObj, GX_TEXMAP2);
	GX_SetTevSwapModeTable(GX_TEV_SWAP1, 0, 0, 0, 2);
	GX_SetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP1);

	GX_SetChanAmbColor(GX_COLOR0A0, whatambcol(scene));
	GX_SetChanMatColor(GX_COLOR0A0, LC_DARKER);
	GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, whichlightmask(scene), GX_DF_CLAMP, GX_AF_NONE);

	ag_config_riverHair(0);
	ag_draw_riverHair(0);	

	GX_SetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetAlphaUpdate(GX_FALSE);
	GX_SetZCompLoc(GX_TRUE);
}
#endif