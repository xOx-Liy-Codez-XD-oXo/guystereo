void lightcap(void * outbuf) {
	Mtx capview, capmodel;
	Mtx44 matpersp;

	u16 matcapsize = 128;
	GX_SetViewport(0, 0, matcapsize, matcapsize, 0, 1);
	GX_SetScissor(0, 0, matcapsize, matcapsize);
	GX_SetTexCopySrc(0, 0, matcapsize, matcapsize);
	GX_SetTexCopyDst(matcapsize, matcapsize, GX_TF_RGBA8, GX_FALSE);

	guVector cam = {0.0F, -2.0F, 0.0F},
			up = {0.0F, 0.0F, 1.0F},
		  look = {0.0F, 0.0F, 0.0F};

	guLookAt(capview, &cam, &up, &look);
	guOrtho(matpersp, 1.0f, -1.0f, -1.0f, 1.0f, 0.1f, 50.0f);
	GX_LoadProjectionMtx(matpersp, GX_ORTHOGRAPHIC);

	guMtxIdentity(capmodel);
	guMtxConcat(capview, capmodel, capmodel);
	GX_LoadPosMtxImm(capmodel, GX_PNMTX0);
	GX_LoadNrmMtxImm(capmodel, GX_PNMTX0);
	GX_SetCurrentMtx(GX_PNMTX0);

	GX_SetCullMode(GX_CULL_NONE);

	GX_SetZMode(GX_FALSE, GX_NEVER, GX_FALSE);

	ag_config_Matcapcone(0);
	ag_draw_Matcapcone(0);

	GX_CopyTex(outbuf, GX_TRUE); //TRUE to clear .. FALSE just looks cool

	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
}

//#define DO_NRMLIGHTDBG
void enableLightNrmDbg() {
	#ifdef DO_NRMLIGHTDBG
	GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0);
	GX_SetNumTevStages(1);
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	#endif
}
