//required globals for draws
Mtx view,mv,mr,mvi; // view and perspective matrices
Mtx viewreflec;
Mtx model, modelview;

Mtx fullQuadMv;
Mtx44 fullQuadPersp;

guVector cam = {-.5F, -1.0F, 0.25F},
	up = {0.0F, 0.0F, 1.0F},
	  look = {0.0F, -0.15F, 1.3F};
guVector camrot = {1.54f, 0.0f, 0.0f};

u16 fbwidth, fbheight;

float camFov = 70;

Mtx *gillSkeletonMv, *lakeSkeletonMv, *riverSkeletonMv, *gillSkeletonMvReflect;
Mtx *gillSkeletonMvReflectionmap, *lakeSkeletonMvReflectionmap, *riverSkeletonMvReflectionmap, *gillSkeletonMvReflectReflectionmap;
char gillReflect, doGillReflect = 0;
GXTexObj gilltexTexObj, laketexTexObj, rivershirtTexObj, riverhairTexObj;
GXTexObj hallway0TexObj, hallway1TexObj, hallwaytilesTexObj;
GXTexObj skyboxTexObj;
GXTexObj foliage1TexObj, buildingTexObj, chainlinkTexObj, barkTexObj;
GXTexObj gillnotebooktexTexObj, firematteTexObj, flutterbytexTexObj;
GXTexObj leatherMatcapTexObj;
GXTexObj lakerivercombineTexObj;

GXTexObj matcap1TexObj, rimlightTexObj;
unsigned char * rimlighttex;

GXTexObj gillDoodleTexATexObj, gilldoodlescrambleTexObj,
riverturn1TexObj, riverturn2TexObj, riverturn3TexObj,
riverturn4TexObj, riverturn5TexObj, riverturn6TexObj,
riverturn7TexObj, 
laketiltdown1TexObj, laketiltdown2TexObj, laketiltdown3TexObj, laketiltdown4TexObj,
laketiltdown5TexObj, 
lakeusquat2TexObj, lakeusquat3TexObj, lakeusquat4TexObj, lakeusquat5TexObj, 
lakeusquat5ear0TexObj, lakeusquat5ear1TexObj, lakeusquat5ear2TexObj, 
lakeusquat5ear3TexObj, lakeusquat5eye0TexObj, lakeusquat5eye1TexObj, 
lakeusquat5eye2TexObj, 
lakevstand2TexObj, lakevstand3TexObj, lakevstand4TexObj, lakevstand5TexObj, lakevstand45TexObj, 
lakewalk1TexObj, lakewalk2TexObj, lakewalk3TexObj, lakewalk4TexObj,
lakeoutstretch1TexObj, lakeoutstretch2TexObj, lakeoutstretch3TexObj, lakeoutstretch4TexObj, lakeoutstretch5TexObj, 
lakeoutstretch6TexObj, lakeoutstretch7TexObj, lakeoutstretch8TexObj, lakeoutstretch9TexObj, lakeoutstretch10TexObj, 
lakeoutstretch11TexObj, 
hand1TexObj, hand2TexObj, hand3TexObj, hand4TexObj, hand5TexObj, hand6TexObj, hand7TexObj, hand8TexObj, hand9TexObj, hand10TexObj, 
hand11TexObj, hand12TexObj, hand13TexObj, hand14TexObj, hand15TexObj, hand16TexObj, hand17TexObj, 
guycarry1TexObj, guycarry2TexObj, guycarry3TexObj, guycarry4TexObj, guycarry5TexObj, guycarry6TexObj, guycarry7TexObj, 
guycarry8TexObj, guycarry9TexObj, guycarry10TexObj, guycarry11TexObj, guycarry12TexObj, guycarry13TexObj, guycarry14TexObj, 
guycarry15TexObj, guycarry16TexObj, guycarry17TexObj, guycarry18TexObj, guycarry19TexObj, guycarry20TexObj, guycarry21TexObj,
lakeusquat5ear0TexObj, lakeusquat5ear1TexObj, lakeusquat5ear2TexObj, lakeusquat5ear3TexObj, 
riveroutstretch1TexObj, riveroutstretch2TexObj, riveroutstretch3TexObj, riveroutstretch4TexObj, riveroutstretch5TexObj,
lakeusquat5eye0TexObj, lakeusquat5eye1TexObj, lakeusquat5eye2TexObj
;

// I changed the textures for these objects but i didnt have the 
// models to change their coordinates so i have some matrices
// that i'll put some values into and then load into the texture
// matrix memory instead of the identity
Mtx dandelionUnfertilizedTexmtx, daisyPatchTexmtx, idkflowerTexmtx;

#define DEFAULT_FIFO_SIZE	(256*1024)

static GXRModeObj *rmode = NULL;
static void *frameBuffer[2] = {NULL, NULL};

u32 fb = 0;

int widescreen = 0;

guVector overlayCam = {0.0F, 0.0F, 1.0F},
	 overlayUp = {0.0F, 1.0F, 0.0F},
	 overlaylook = {0.0F, 0.0F, 0.0F};
Mtx overlayView;
Mtx44 overlayPersp;

FILE *infile;
FILE *mouthfile;

GXColor background = {138, 189, 255, 255};

void *gpfifo = NULL;
f32 aspect;

#define YES 1
#define SURE 1
#define YEAH 1
#define FINE 1
#define WHY_NOT 1
#define PROBABLY rand()&3
#define MAYBE rand()&1
#define NOPE 0
#define NUHUH 0
#define NAH 0
#define NO 0

u8 riverhappyornot = NAH;
u8 riverjacketed = SURE;
u8 gilljacketed = NO;
int scene, scenelastframe;

s8 pleaseexit = 0;

float accumulatedbehindtime = 0.0f;

u8 dolphin = 0;
u8 european = 0;

//Super huge models get 2 have display lists
//Like heads, the tree, and gills hair
int treeDispListSize, treeleavesDispListSize, grassclumpDispListSize, gillHairDispListSize, gillHeadDispListSize,
lakeHeadDispListSize, lockerDispListSize;
void *treeDispList, *treeleavesDispList, *grassclumpDispList, *gillHairDispList, *gillHeadDispList, 
*lakeHeadDispList, *lockerDispList;

int comic = 0;
int comiclear = 0;

int lyric = 0;
int istypist = 0;

int whatever = 0;

int eye;
int gillheadspecial = 1;

float *LotsOfRandoms; //floats in some reasonable range	