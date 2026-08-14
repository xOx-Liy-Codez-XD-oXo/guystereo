#ifndef PRIMITIVES_H
#define PRIMITIVES_H

void drawTestaxis() {
	GX_SetLineWidth(24, 24);
	liy_VtxDescConfig(LV_VP | LV_VC, GX_VTXFMT0);
	GX_Begin(GX_LINES, GX_VTXFMT0, 6);
		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Color3f32(1.0f, 0.0f, 0.0f);
		GX_Position3f32(1.0f, 0.0f, 0.0f);
		GX_Color3f32(1.0f, 0.0f, 0.0f);

		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Color3f32(0.0f, 1.0f, 0.0f);
		GX_Position3f32(0.0f, 1.0f, 0.0f);
		GX_Color3f32(0.0f, 1.0f, 0.0f);

		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Color3f32(0.0f, 0.0f, 1.0f);
		GX_Position3f32(0.0f, 0.0f, 1.0f);
		GX_Color3f32(0.0f, 0.0f, 1.0f);
	GX_End();
}




// Functions for sending arrays of data to FIFO in direct mode. Organized in 0 particular order



// FIFO overflows if we send it too many triangles
// in direct mode in a single call, so the Largeformat
// functions split them into multiple calls of
// triangles, the amount of triangles in each batch
// here . 
#define LARGEFORMAT_BATCHSIZE 4000

void drawArrDirectP(float *verts, int tricount, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
		}
	GX_End(); 
}

void drawArrDirectPT(float *verts, float *texCoords, int tricount, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_TexCoord2f32(texCoords[(i*2)],texCoords[(i*2)+1]);
		}
	GX_End(); 
}

void drawArrDirectPNT(float *verts, float *vertNors, float *texCoords, int tricount, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
			GX_TexCoord2f32(texCoords[(i*2)],texCoords[(i*2)+1]);
		}
	GX_End(); 
}

void drawArrDirectPNT_Largeformat(float* verts, float* vertNors, float *texCoords, int tricount, int vtxfmt) {
	int trisleft = tricount;
	int trissubtracted = 0;
	while(trisleft > 0) {
		int tristhisround = liym_min(trisleft, LARGEFORMAT_BATCHSIZE);
		GX_Begin(GX_TRIANGLES, vtxfmt, tristhisround * 3);
			for(int i = (3 * trissubtracted); i < 3 * (trissubtracted + tristhisround); i++) {
				GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
				GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
				GX_TexCoord2f32(texCoords[(i*2)],texCoords[(i*2)+1]);
			}
		GX_End();
		trisleft = trisleft - tristhisround;
		trissubtracted = trissubtracted + tristhisround;
	}
}

void drawArrDirectPN_Largeformat(float* verts, float* vertNors, int tricount, int vtxfmt) {
	int trisleft = tricount;
	int trissubtracted = 0;
	while(trisleft > 0) {
		int tristhisround = liym_min(trisleft, LARGEFORMAT_BATCHSIZE);
		GX_Begin(GX_TRIANGLES, vtxfmt, tristhisround * 3);
			for(int i = (3 * trissubtracted); i < 3 * (trissubtracted + tristhisround); i++) {
				GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
				GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
			}
		GX_End();
		trisleft = trisleft - tristhisround;
		trissubtracted = trissubtracted + tristhisround;
	}
}

void drawArrDirectPNTPmtx_Largeformat(float* verts, float* vertNors, float *texCoords, int *mtxidx, int tricount, int vtxfmt) {
	int trisleft = tricount;
	int trissubtracted = 0;
	while(trisleft > 0) {
		int tristhisround = liym_min(trisleft, LARGEFORMAT_BATCHSIZE);
		GX_Begin(GX_TRIANGLES, vtxfmt, tristhisround * 3);
			for(int i = (3 * trissubtracted); i < 3 * (trissubtracted + tristhisround); i++) {
				GX_MatrixIndex1x8(mtxidx[i]*3);
				GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
				GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
				GX_TexCoord2f32(texCoords[(i*2)],texCoords[(i*2)+1]);
			}
		GX_End();
		trisleft = trisleft - tristhisround;
		trissubtracted = trissubtracted + tristhisround;
	}
}

void drawArrDirectPNTPmtx(float* verts, float* vertNors, float *texCoords, int *mtxidx, int tricount, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_MatrixIndex1x8(mtxidx[i]*3);
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
			GX_TexCoord2f32(texCoords[(i*2)],texCoords[(i*2)+1]);
		}
	GX_End();
}

void drawArrDirectPTPmtx(float* verts, float *texCoords, int *mtxidx, int tricount, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_MatrixIndex1x8(mtxidx[i]*3);
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_TexCoord2f32(texCoords[(i*2)],texCoords[(i*2)+1]);
		}
	GX_End();
}

void drawArrDirectPNTPmtxCidx_Largeformat(float* verts, float* vertNors, float *texCoords, int *mtxidx, int *colidx, int tricount, int vtxfmt) {
	int trisleft = tricount;
	int trissubtracted = 0;
	while(trisleft > 0) {
		int tristhisround = liym_min(trisleft, LARGEFORMAT_BATCHSIZE);
		GX_Begin(GX_TRIANGLES, vtxfmt, tristhisround * 3);
			for(int i = (3 * trissubtracted); i < 3 * (trissubtracted + tristhisround); i++) {
				GX_MatrixIndex1x8(mtxidx[i]*3);
				GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
				GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
				GX_Color1x8(colidx[i]);
				GX_TexCoord2f32(texCoords[(i*2)],texCoords[(i*2)+1]);
			}
		GX_End();
		trisleft = trisleft - tristhisround;
		trissubtracted = trissubtracted + tristhisround;
	}
}

void drawArrDirectPNTPmtxCidx(float* verts, float* vertNors, float *texCoords, int *mtxidx, int *colidx, int tricount, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_MatrixIndex1x8(mtxidx[i]*3);
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
			GX_Color1x8(colidx[i]);
			GX_TexCoord2f32(texCoords[(i*2)],texCoords[(i*2)+1]);
		}
	GX_End();
}

void drawArrDirectPNCidx(float* verts, float* vertNors, int *colidx, int tricount, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
			GX_Color1x8(colidx[i]);
		}
	GX_End(); 
}

void drawArrDirectPNCPmtx_UniformCol(float* verts, float* vertNors, int *mtxidx, int tricount, GXColor *col, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_MatrixIndex1x8(mtxidx[i]*3);
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
			GX_Color3u8(col->r, col->g, col->b);
		}
	GX_End();
}

void drawArrDirectPC_UniformCol(float *verts, int tricount, GXColor *col, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_Color3u8(col->r, col->g, col->b);
		}
	GX_End(); 
}

void drawArrDirectPNC_UniformCol(float* verts, float* vertNors, int tricount, GXColor *col, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
			GX_Color3u8(col->r, col->g, col->b);
		}
	GX_End(); 
}

void drawArrDirectPNC_UniformCol_Largeformat(float* verts, float* vertNors, int tricount, GXColor *col, int vtxfmt) {
	int trisleft = tricount;
	int trissubtracted = 0;
	while(trisleft > 0) {
		int tristhisround = liym_min(trisleft, LARGEFORMAT_BATCHSIZE);
		GX_Begin(GX_TRIANGLES, vtxfmt, tristhisround * 3);
			for(int i = (3 * trissubtracted); i < 3 * (trissubtracted + tristhisround); i++) {
				GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
				GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
				GX_Color3u8(col->r, col->g, col->b);
			}
		GX_End();
		trisleft = trisleft - tristhisround;
		trissubtracted = trissubtracted + tristhisround;
	}
}

void drawArrDirectPCPmtx_UniformCol(float* verts, int *mtxidx, int tricount, GXColor *col, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_MatrixIndex1x8(mtxidx[i]*3);
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_Color3u8(col->r, col->g, col->b);
		}
	GX_End(); 
}

void drawArrDirectPNCPmtx_Largeformat(float* verts, float* vertNors, int *vertCols, int *mtxidx, int tricount, int vtxfmt) {
	int trisleft = tricount;
	int trissubtracted = 0;
	while(trisleft > 0) {
		int tristhisround = liym_min(trisleft, LARGEFORMAT_BATCHSIZE);
		GX_Begin(GX_TRIANGLES, vtxfmt, tristhisround * 3);
			for(int i = (3 * trissubtracted); i < 3 * (trissubtracted + tristhisround); i++) {
				GX_MatrixIndex1x8(mtxidx[i]*3);
				GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
				GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
				GX_Color3u8( (u8)vertCols[(i*3)] , (u8)vertCols[(i*3)+1], (u8)vertCols[(i*3)+2] );
			}
		GX_End();
		trisleft = trisleft - tristhisround;
		trissubtracted = trissubtracted + tristhisround;
	}
}

void drawArrDirectPNCPmtx(float* verts, float* vertNors, int *vertCols, int *mtxidx, int tricount, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_MatrixIndex1x8(mtxidx[i]*3);
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
			GX_Color3u8( (u8)vertCols[(i*3)] , (u8)vertCols[(i*3)+1], (u8)vertCols[(i*3)+2] );
		}
	GX_End(); 
}

void drawArrDirectPNCPmtxT(float* verts, float* vertNors, float* texCoords, int *vertCols, int *mtxidx, int tricount, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_MatrixIndex1x8(mtxidx[i]*3);
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
			GX_Color3u8( (u8)vertCols[(i*3)] , (u8)vertCols[(i*3)+1], (u8)vertCols[(i*3)+2] );
			GX_TexCoord2f32(texCoords[(i*2)],texCoords[(i*2)+1]);
		}
	GX_End(); 
}

void drawArrDirectPNCPmtxT_Largeformat(float* verts, float* vertNors, float* texCoords, int *vertCols, int *mtxidx, int tricount, int vtxfmt) {
	int trisleft = tricount;
	int trissubtracted = 0;
	while(trisleft > 0) {
		int tristhisround = liym_min(trisleft, LARGEFORMAT_BATCHSIZE);
		GX_Begin(GX_TRIANGLES, vtxfmt, tristhisround * 3);
			for(int i = (3 * trissubtracted); i < 3 * (trissubtracted + tristhisround); i++) {
				GX_MatrixIndex1x8(mtxidx[i]*3);
				GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
				GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
				GX_Color3u8( (u8)vertCols[(i*3)] , (u8)vertCols[(i*3)+1], (u8)vertCols[(i*3)+2] );
				GX_TexCoord2f32(texCoords[(i*2)],texCoords[(i*2)+1]);
			}
		GX_End();
		trisleft = trisleft - tristhisround;
		trissubtracted = trissubtracted + tristhisround;
	} 
}

void drawArrDirectPidxNidxCPmtxTidx_Largeformat(uint16_t* verts, uint16_t* vertNors, uint16_t* texCoords, uint8_t *vertCols, uint8_t *mtxidx, int tricount, int vtxfmt) {
	int trisleft = tricount;
	int trissubtracted = 0;
	while(trisleft > 0) {
		int tristhisround = liym_min(trisleft, LARGEFORMAT_BATCHSIZE);
		GX_Begin(GX_TRIANGLES, vtxfmt, tristhisround * 3);
			for(int i = (3 * trissubtracted); i < 3 * (trissubtracted + tristhisround); i++) {
				GX_MatrixIndex1x8(mtxidx[i]*3);
				GX_Position1x16(verts[i]);
				GX_Normal1x16(vertNors[i]);
				GX_Color3u8( (u8)vertCols[(i*3)] , (u8)vertCols[(i*3)+1], (u8)vertCols[(i*3)+2] );
				GX_TexCoord1x16(texCoords[i]);
			}
		GX_End();
		trisleft = trisleft - tristhisround;
		trissubtracted = trissubtracted + tristhisround;
	} 
}

void drawArrDirectPidxNidxPmtxTidx_Largeformat(uint16_t* verts, uint16_t* vertNors, uint16_t* texCoords, uint8_t *mtxidx, int tricount, int vtxfmt) {
	int trisleft = tricount;
	int trissubtracted = 0;
	while(trisleft > 0) {
		int tristhisround = liym_min(trisleft, LARGEFORMAT_BATCHSIZE);
		GX_Begin(GX_TRIANGLES, vtxfmt, tristhisround * 3);
			for(int i = (3 * trissubtracted); i < 3 * (trissubtracted + tristhisround); i++) {
				GX_MatrixIndex1x8(mtxidx[i]*3);
				GX_Position1x16(verts[i]);
				GX_Normal1x16(vertNors[i]);
				GX_TexCoord1x16(texCoords[i]);
			}
		GX_End();
		trisleft = trisleft - tristhisround;
		trissubtracted = trissubtracted + tristhisround;
	} 
}

void drawArrDirectPNC(float* verts, float* vertNors, int *vertCols, int tricount, int vtxfmt) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
		for(int i = 0; i < tricount * 3; i++) {
			GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
			GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
			GX_Color3u8( (u8)vertCols[(i*3)] , (u8)vertCols[(i*3)+1], (u8)vertCols[(i*3)+2] );
		}
	GX_End(); 
}

void drawArrDirectPNC_Largeformat(float* verts, float* vertNors, int *vertCols, int tricount, int vtxfmt) {
	int trisleft = tricount;
	int trissubtracted = 0;
	while(trisleft > 0) {
		int tristhisround = liym_min(trisleft, LARGEFORMAT_BATCHSIZE);
		GX_Begin(GX_TRIANGLES, vtxfmt, tristhisround * 3);
			for(int i = (3 * trissubtracted); i < 3 * (trissubtracted + tristhisround); i++) {
				GX_Position3f32(verts[(i*3)],verts[(i*3)+1],verts[(i*3)+2]);
				GX_Normal3f32(vertNors[(i*3)],vertNors[(i*3)+1],vertNors[(i*3)+2]);
				GX_Color3u8( (u8)vertCols[(i*3)] , (u8)vertCols[(i*3)+1], (u8)vertCols[(i*3)+2] );
			}
		GX_End();
		trisleft = trisleft - tristhisround;
		trissubtracted = trissubtracted + tristhisround;
	}
}

void drawArrConfig(uint16_t config, long tricount, uint8_t vtxfmt, 
                   float *vertpos, uint8_t *vpindex8, uint16_t *vpindex16, 
                   float *vertnor, uint8_t *vnindex8, uint16_t *vnindex16, 
                   float *texcord, uint8_t *tcindex8, uint16_t *tcindex16, 
                   uint8_t *mtxidx, 
                   uint8_t *vertcol) {
	GX_Begin(GX_TRIANGLES, vtxfmt, tricount * 3);
	for(int i = 0; i < tricount * 3; i++) {
		int i3 = i * 3;
		if(config & LV_MX)     { GX_MatrixIndex1x8( mtxidx[i] * 3); }
		if(config & LV_VP)     { GX_Position3f32(vertpos[i3], vertpos[i3+1], vertpos[i3+2]); }
		if(config & LV_VPIDX8) { GX_Position1x8( vpindex8[i] ); }
		if(config & LV_VPIDX16){ GX_Position1x16( vpindex16[i] ); }
		if(config & LV_VN)     { GX_Normal3f32(vertnor[i3], vertnor[i3+1], vertnor[i3+2]); }
		if(config & LV_VNIDX8) { GX_Normal1x8( vnindex8[i] ); }
		if(config & LV_VNIDX16){ GX_Normal1x16( vnindex16[i] ); }
		if(config & LV_VC)     { GX_Color3u8(vertcol[i3], vertcol[i3+1], vertcol[i3+2]); }
		if(config & LV_TC)     { int i2 = i*2; GX_TexCoord2f32(texcord[i2], texcord[i2+1]); }
		if(config & LV_TCIDX8) { GX_TexCoord1x8( tcindex8[i] ); }
		if(config & LV_TCIDX16){ GX_TexCoord1x16( tcindex16[i] ); }
	}
}

#endif