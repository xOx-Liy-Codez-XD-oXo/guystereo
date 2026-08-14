void resetViewport() {
	GX_SetViewport(0, 0, fbwidth, fbheight, 0, 1);
	GX_SetScissor(0, 0, fbwidth, fbheight);
}