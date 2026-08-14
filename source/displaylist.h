void liy_buildDisplaylist() {
	//Make tree disp lists

		treeDispListSize = calcSizeDisplaylist(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX | LV_VC, 3, treetricount);
		treeDispList = memalign(32, treeDispListSize);
		memset(treeDispList, 0, treeDispListSize);
		DCInvalidateRange(treeDispList, treeDispListSize);

		GX_BeginDispList(treeDispList, treeDispListSize);
			drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX | LV_VC, treetricount, 0, 
					NULL, NULL, treevertposidx,
					NULL, NULL, treevertnoridx,
					NULL, NULL, treetexcordidx,
					treemtxarr,
					treevertcol);
		treeDispListSize = GX_EndDispList();

		treeleavesDispListSize = calcSizeDisplaylist(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX, 3, treeleavestricount);
		treeleavesDispList = memalign(32, treeleavesDispListSize);
		memset(treeleavesDispList, 0, treeleavesDispListSize);
		DCInvalidateRange(treeleavesDispList, treeleavesDispListSize);

		GX_BeginDispList(treeleavesDispList, treeleavesDispListSize);
			drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16 | LV_MX, treeleavestricount, 0, 
					NULL, NULL, treeleavesvertposidx,
					NULL, NULL, treeleavesvertnoridx,
					NULL, NULL, treeleavestexcordidx,
					treeleavesmtxarr,
					NULL);
		treeleavesDispListSize = GX_EndDispList();

	// Grass clump

		grassclumpDispListSize = calcSizeDisplaylist(LV_VPIDX16 | LV_TCIDX16 | LV_MX, 3, grassplaneclumptricount);
		grassclumpDispList = memalign(32, grassclumpDispListSize);
		memset(grassclumpDispList, 0, grassclumpDispListSize);
		DCInvalidateRange(grassclumpDispList, grassclumpDispListSize);

		GX_BeginDispList(grassclumpDispList, grassclumpDispListSize);
			drawArrConfig(LV_VPIDX16 | LV_TCIDX16 | LV_MX, grassplaneclumptricount, 0,
					NULL, NULL, grassplaneclumpvertposidx,
					NULL, NULL, NULL,
					NULL, NULL, grassplaneclumptexcordidx,
					grassplaneclumpmtxidx,
					NULL);
		grassclumpDispListSize = GX_EndDispList();

	//Gill hair is pretty big so displist of that

		gillHairDispListSize = calcSizeDisplaylist(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16, 3, gillHairtricount);
		gillHairDispList = memalign(32, gillHairDispListSize);
		memset(gillHairDispList, 0, gillHairDispListSize);
		DCInvalidateRange(gillHairDispList, gillHairDispListSize);
		GX_BeginDispList(gillHairDispList, gillHairDispListSize);
		drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_TCIDX16, gillHairtricount, 0, 
				NULL, NULL, gillHairvertposidx,
				NULL, NULL, gillHairvertnoridx,
				NULL, NULL, gillHairtexcordidx,
				NULL,
				NULL);
		gillHairDispListSize = GX_EndDispList();

	//Gill head 🤤

		gillHeadDispListSize = calcSizeDisplaylist(LV_VPIDX16 | LV_VNIDX16 | LV_VC, 3, gillHeadtricount);
		gillHeadDispList = memalign(32, gillHeadDispListSize);
		memset(gillHeadDispList, 0, gillHeadDispListSize);
		DCInvalidateRange(gillHeadDispList, gillHeadDispListSize);
		GX_BeginDispList(gillHeadDispList, gillHeadDispListSize);
		drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC, gillHeadtricount, 0, 
				NULL, NULL, gillHeadvertposidx,
				NULL, NULL, gillHeadvertnoridx,
				NULL, NULL, NULL,
				NULL,
				gillHeadvertcol);
		gillHeadDispListSize = GX_EndDispList();

	//Lake head 😒

		lakeHeadDispListSize = calcSizeDisplaylist(LV_VPIDX16 | LV_VNIDX16 | LV_VC, 3, lakeHeadtricount);
		lakeHeadDispList = memalign(32, lakeHeadDispListSize);
		memset(lakeHeadDispList, 0, lakeHeadDispListSize);
		DCInvalidateRange(lakeHeadDispList, lakeHeadDispListSize);
		GX_BeginDispList(lakeHeadDispList, lakeHeadDispListSize);
		drawArrConfig(LV_VPIDX16 | LV_VNIDX16 | LV_VC, lakeHeadtricount, 0, 
				NULL, NULL, lakeHeadvertposidx,
				NULL, NULL, lakeHeadvertnoridx,
				NULL, NULL, NULL,
				NULL,
		lakeHeadvertcol);
		lakeHeadDispListSize = GX_EndDispList();

	//Locker

		lockerDispListSize = calcSizeDisplaylist(LV_VPIDX16 | LV_VC, 3, locker_fulltricount);
		lockerDispList = memalign(32, lockerDispListSize);
		memset(lockerDispList, 0, lockerDispListSize);
		DCInvalidateRange(lockerDispList, lockerDispListSize);
		GX_BeginDispList(lockerDispList, lockerDispListSize);
		ag_draw_locker_full(0);
		lockerDispListSize = GX_EndDispList();
}