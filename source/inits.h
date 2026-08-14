//Inits h

void liy_initGlobalConsts() {
	fullQuadMv[0][0] = 1.0f;
	fullQuadMv[1][0] = 0.0f;
	fullQuadMv[2][0] = 0.0f;

	fullQuadMv[0][1] = 0.0f;
	fullQuadMv[1][1] = 1.0f;
	fullQuadMv[2][1] = 0.0f;
	
	fullQuadMv[0][2] = 0.0f;
	fullQuadMv[1][2] = 0.0f;
	fullQuadMv[2][2] = 1.0f;

	fullQuadMv[0][3] = 0.0f;
	fullQuadMv[1][3] = 0.0f;
	fullQuadMv[2][3] = -1.0f;
}

void blowpopsiclestand() {
	pleaseexit = 1;
}

void liy_initialSystemConfig() {
	SYS_SetPowerCallback(blowpopsiclestand);

	int dolphinfd = IOS_Open("/dev/dolphin", IPC_OPEN_NONE);

	if(dolphinfd < 0) {
		CON_EnableGecko(1, 0);
		USB_Initialize();
		MOUSE_Init();
		KEYBOARD_Init(NULL);
	} else {
		SYS_STDIO_Report(true);
		IOS_Close(dolphinfd);
		dolphin = 1;
		#ifdef GUY_DEV
		//printf("That's dolpfy\n");
		#endif
	}
}

void liy_initialFlipperConfig() {
	//rmode = VIDEO_GetPreferredMode(NULL);
	//rmode = &TVPal528Prog;
	rmode = &TVNtsc480Prog;
	
	// allocate the fifo buffer
	gpfifo = memalign(32,DEFAULT_FIFO_SIZE);
	memset(gpfifo,0,DEFAULT_FIFO_SIZE);

	frameBuffer[0] = memalign(32, 720*480*2);
	frameBuffer[1] = memalign(32, 720*480*2);

		// configure video
	//if (CONF_GetAspectRatio() == CONF_ASPECT_16_9) {
	//	aspect = 16.0f/9.0f;
	//	widescreen = 1;
	//} else {
	//	aspect = 4.0f/3.0f;
	//}
	aspect = 4.0f/5.0f;
	rmode->aa = 0; //no way aa

	rmode->fbWidth = 704;
	rmode->viWidth = 720;
	rmode->viXOrigin = 0;

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	//fb ^= 1; uncomment to break rendering on progressive display

	// init the flipper
	GX_Init(gpfifo,DEFAULT_FIFO_SIZE);

	GX_SetCopyClear(LC_BLACK, 0x00ffffff);

	fbwidth = rmode->fbWidth;
	fbheight = rmode->efbHeight;
	if((fbheight != 240) && (fbheight != 480)) { european = 1; }

	// other gx setup
	GX_SetViewport(0,0,352,480,0,1);
	u32 xfbHeight = GX_SetDispCopyYScale(1.0f);
	GX_SetScissor(0,0,352,480);
	GX_SetDispCopySrc(0,0,352,480);
	GX_SetDispCopyDst(704,480);	
}

void liy_initialDrawConfig() {
	GX_SetCullMode(GX_CULL_FRONT);
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);
	GX_SetDispCopyGamma((f32)GX_GM_1_0);

	GX_SetNumTexGens(1);

	// setup texture coordinate generation
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);

	//VIDEO_SetBlack(true);
	VIDEO_SetBlack(false);

	//Oh leave me alone i already gave a spiel about this
	guMtxScale(dandelionUnfertilizedTexmtx, 2.0f, 0.5f, 1.0f);
	guMtxScale(daisyPatchTexmtx, 4.0f, 1.0f, 1.0f);
	guMtxTransApply(daisyPatchTexmtx, idkflowerTexmtx, 0.5f, 0.125f, 0.0f);
	guMtxTransApply(dandelionUnfertilizedTexmtx, dandelionUnfertilizedTexmtx, 0.0f, 0.5f, 0.0f);
}

void liy_unpackPackedTextures() {
	TPLFile tplfileFromMem;
	void *qoiParse;
	qoi_desc qoiParseDesc;

	initdumbrand();
	liy_unpackDoodle(gillDoodleTexA, 512, 1024);
	liy_unpackDoodle(gilldoodlescramble, 1024, 1024);

	liy_unpackDoodle(laketiltdown1, 168, 480);
	liy_unpackDoodle(laketiltdown2, 168, 480);
	liy_unpackDoodle(laketiltdown3, 168, 480);
	liy_unpackDoodle(laketiltdown4, 168, 480);
	liy_unpackDoodle(laketiltdown5, 168, 480);

	liy_unpackDoodle(lakeusquat2, 256, 512);
	liy_unpackDoodle(lakeusquat3, 256, 512);
	liy_unpackDoodle(lakeusquat4, 256, 256);
	liy_unpackDoodle(lakeusquat5, 256, 256);
	
	liy_unpackDoodle(lakeusquat5ear0, 64, 64);
	liy_unpackDoodle(lakeusquat5ear1, 64, 64);
	liy_unpackDoodle(lakeusquat5ear2, 64, 64);
	liy_unpackDoodle(lakeusquat5ear3, 64, 64);

	liy_unpackDoodle(lakeusquat5eye0, 64, 64);
	liy_unpackDoodle(lakeusquat5eye1, 64, 64);
	liy_unpackDoodle(lakeusquat5eye2, 64, 64);
	
	liy_unpackDoodle(lakevstand2, 224, 480);
	liy_unpackDoodle(lakevstand3, 184, 480);
	liy_unpackDoodle(lakevstand4, 176, 480);
	liy_unpackDoodle(lakevstand45, 160, 480);
	liy_unpackDoodle(lakevstand5, 152, 480);

	liy_unpackDoodle(lakewalk1, 152, 480);
	liy_unpackDoodle(lakewalk2, 184, 480);
	liy_unpackDoodle(lakewalk3, 144, 480);
	liy_unpackDoodle(lakewalk4, 232, 480);

	liy_unpackDoodlegrey(riverturn1, 648, 480);
	liy_unpackDoodlegrey(riverturn2, 640, 480);
	liy_unpackDoodlegrey(riverturn3, 624, 480);
	liy_unpackDoodlegrey(riverturn4, 544, 480);
	liy_unpackDoodlegrey(riverturn5, 536, 480);
	liy_unpackDoodlegrey(riverturn6, 536, 480);
	liy_unpackDoodlegrey(riverturn7, 552, 480);

	liy_unpackDoodle(lakeoutstretch1, 256, 480);
	liy_unpackDoodle(lakeoutstretch2, 128, 480);
	liy_unpackDoodle(lakeoutstretch3, 128, 480);
	liy_unpackDoodle(lakeoutstretch4, 128, 480);
	liy_unpackDoodle(lakeoutstretch5, 256, 480);
	liy_unpackDoodle(lakeoutstretch6, 256, 480);
	liy_unpackDoodle(lakeoutstretch7, 128, 480);
	liy_unpackDoodle(lakeoutstretch8, 128, 480);
	liy_unpackDoodle(lakeoutstretch9, 128, 480);
	liy_unpackDoodle(lakeoutstretch10, 256, 480);
	liy_unpackDoodle(lakeoutstretch11, 256, 480);

	liy_unpackDoodle(hand1, 1024, 512);
	liy_unpackDoodle(hand2, 1024, 512);
	liy_unpackDoodle(hand3, 1024, 512);
	liy_unpackDoodle(hand4, 1024, 512);
	liy_unpackDoodle(hand5, 1024, 512);
	liy_unpackDoodle(hand6, 1024, 512);
	liy_unpackDoodle(hand7, 1024, 512);
	liy_unpackDoodle(hand8, 1024, 512);
	liy_unpackDoodle(hand9, 1024, 512);
	liy_unpackDoodle(hand10, 1024, 512);
	liy_unpackDoodle(hand11, 512, 512);
	liy_unpackDoodle(hand12, 512, 512);
	liy_unpackDoodle(hand13, 512, 512);
	liy_unpackDoodle(hand14, 512, 512);
	liy_unpackDoodle(hand15, 512, 512);
	liy_unpackDoodle(hand16, 512, 512);
	liy_unpackDoodle(hand17, 512, 512);	

	liy_unpackDoodle(guycarry1, 304, 512);
	liy_unpackDoodle(guycarry2, 304, 512);
	liy_unpackDoodle(guycarry3, 304, 512);
	liy_unpackDoodle(guycarry4, 304, 512);
	liy_unpackDoodle(guycarry5, 304, 512);
	liy_unpackDoodle(guycarry6, 304, 512);
	liy_unpackDoodle(guycarry7, 304, 512);
	liy_unpackDoodle(guycarry8, 304, 512);
	liy_unpackDoodle(guycarry9, 304, 512);
	liy_unpackDoodle(guycarry10, 304, 512);
	liy_unpackDoodle(guycarry11, 304, 512);
	liy_unpackDoodle(guycarry12, 304, 512);
	liy_unpackDoodle(guycarry13, 304, 512);
	liy_unpackDoodle(guycarry14, 304, 512);
	liy_unpackDoodle(guycarry15, 304, 512);
	liy_unpackDoodle(guycarry16, 304, 512);
	liy_unpackDoodle(guycarry17, 304, 512);
	liy_unpackDoodle(guycarry18, 304, 512);
	liy_unpackDoodle(guycarry19, 304, 512);
	liy_unpackDoodle(guycarry20, 304, 512);
	liy_unpackDoodle(guycarry21, 304, 512);

	liy_unpackDoodle(riveroutstretch1, 256, 512);
	liy_unpackDoodle(riveroutstretch2, 256, 512);
	liy_unpackDoodle(riveroutstretch3, 256, 512);
	liy_unpackDoodle(riveroutstretch4, 256, 512);
	liy_unpackDoodle(riveroutstretch5, 256, 512);
	
	//liy_unpackQOI_RGB565(gilltex);
	//liy_unpackQOI_RGB565(gillnotebooktex);
	liy_unpackQOI_RGB5A3(building); 
	GX_InitTexObjLOD(&buildingTexObj, GX_LINEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_DISABLE, GX_DISABLE, GX_ANISO_1);
	liy_unpackQOI_RGBA8(foliage1);
	liy_unpackQOI_RGB565(skybox);

	liy_unpackQOI_RGB565(rivershirt);
	liy_unpackQOI_RGB565(bark);
	liy_unpackQOI_RGB565(flutterbytex);
	liy_unpackQOI_RGB565(leatherMatcap);

	liy_unpackTPL(gilltex);
	liy_unpackTPL(gillnotebooktex);
	//liy_unpackTPL(laketex);
	liy_unpackTPL(hallway1);
	liy_unpackTPL(hallwaytiles);
	liy_unpackTPL(chainlink);
	//liy_unpackTPL(riverhair);
	liy_unpackTPL(lakerivercombine);
	liy_unpackTPL(firematte);

	procedtex_randomtex_generate();
	procedtex_randomtex_update();

	unsigned char * rimbuftemp = memalign(32, 128*128);
	genrimtex(rimbuftemp, 3);
	rimlighttex = memalign(32, 128*128);
	liy_ConvertI8(rimbuftemp, rimlighttex, 128, 128);
	free(rimbuftemp);
	DCFlushRange(rimlighttex, 128*128);
	GX_InitTexObj(&rimlightTexObj, rimlighttex, 128, 128, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);

	GX_InvalidateTexAll();
}