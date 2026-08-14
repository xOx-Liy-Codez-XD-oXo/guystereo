GXTexObj randomATexObj, randomBTexObj;
u8 * randomtexA = NULL;
u8 * randomtexB = NULL;

//Random textures for monte carlo depth of field
//dont know .. any point inside a circle, or points on the edge of a circle?
//circle ought to be aspect ratio corrected

float procedtex_randomFloat() {
	return (float)rand() / (float)RAND_MAX;
}

void procedtex_randomtex_generate() {
	//64x64 look good

	//372736 = 640*480*1 + 65536
	//8192 = 64*64 * 2
	//32768 = 128*128 * 2
	//16384 = 128*128
	uint length = 8192;
	if(!randomtexA) randomtexA = memalign(32, length);
	if(!randomtexB) randomtexB = memalign(32, length);
	//float aspectconst = widescreen ? 1.333f : 1.0f; //widescreen pixels arent square ..
	//YX swap in second pass means this doesnt really work
	//Didn't actually look different to me anyway
	float aspectconst = 1.0f;
	float strength = 127.0f;
	aspectconst *= strength;
	for(int i = 0; i < length; i++) {
		//Edges on circle method .. it looks bad. 
		/*float wa = rand();
		float ba = aspectconst * sin(wa);
		float ca = strength * cos(wa);
		randomtexA[i] = ba;
		randomtexB[i] = ca;*/

		//Ponts in circle method
		float ba, ca, wa;
		wa = procedtex_randomFloat() * 6.283f;
		ca = procedtex_randomFloat();
		ba = ca * cosf(wa);
		ca = ca * sinf(wa);
		ba *= aspectconst;
		ca *= strength;
		randomtexA[i] = ba;
		randomtexB[i] = ca;
	}
	DCFlushRange(randomtexA, length);
	DCFlushRange(randomtexB, length);
}

void procedtex_randomtex_update() {
	int ba = ((rand() % 4096)& ~63); //Use same position for both textures cos they're pairs not just randfoms
	//int ba = 0;

	GX_InitTexObj(&randomATexObj, randomtexA + ba, 64, 64, GX_TF_I8, GX_REPEAT, GX_REPEAT, GX_FALSE);
	GX_InitTexObjLOD(&randomATexObj, GX_LINEAR, GX_LINEAR, 0.5f, 0.5f, 0.5f, GX_DISABLE, GX_DISABLE, GX_ANISO_1);
	GX_InitTexObj(&randomBTexObj, randomtexB + ba, 64, 64, GX_TF_I8, GX_REPEAT, GX_REPEAT, GX_FALSE);
	GX_InitTexObjLOD(&randomBTexObj, GX_LINEAR, GX_LINEAR, 0.5f, 0.5f, 0.5f, GX_DISABLE, GX_DISABLE, GX_ANISO_1);
	GX_InvalidateTexAll();
}