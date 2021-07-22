/*
Plugin-SDK (Grand Theft Auto San Andreas) header file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CVector.h"
#include "CRGBA.h"
#include "CVector2D.h"
#include "CSprite2d.h"

// Thanks to Wesser for radar-related things
enum eBlipAppearance {
    BLIP_FLAG_FRIEND, // It selects BLIP_COLOUR_BLUE. If unset together with BLIP_FLAG_THREAT, any color.
    BLIP_FLAG_THREAT  // It selects BLIP_COLOUR_RED. If unset together with BLIP_FLAG_FRIEND, any color.
};

enum eBlipType {
    BLIP_NONE,          // 0
    BLIP_CAR,           // 1
    BLIP_CHAR,          // 2
    BLIP_OBJECT,        // 3
    BLIP_COORD,         // 4 - Checkpoint.
    BLIP_CONTACTPOINT,  // 5 - Sphere.
    BLIP_SPOTLIGHT,     // 6
    BLIP_PICKUP,        // 7
    BLIP_AIRSTRIP       // 8
};

enum eBlipDisplay {
    BLIP_DISPLAY_NEITHER,    // 0
    BLIP_DISPLAY_MARKERONLY, // 1
    BLIP_DISPLAY_BLIPONLY,   // 2
    BLIP_DISPLAY_BOTH        // 3
};

// See <https://www.dropbox.com/s/oi3i4f0qsbe7z10/blip_marker_colors.html> to view these colors. (TODO: dead link, fix it.)
enum eBlipColour : unsigned int {
    BLIP_COLOUR_RED,        // 0
    BLIP_COLOUR_GREEN,      // 1
    BLIP_COLOUR_BLUE,       // 2
    BLIP_COLOUR_WHITE,      // 3
    BLIP_COLOUR_YELLOW,     // 4
    BLIP_COLOUR_REDCOPY,    // 5 - What? It was BLIP_COLOUR_PURPLE.
    BLIP_COLOUR_BLUECOPY,   // 6 - Why? It was BLIP_COLOUR_CYAN.
    BLIP_COLOUR_THREAT,     // 7 - If BLIP_FLAG_FRIENDLY is not set (by default) it is BLIP_COLOUR_RED, else BLIP_COLOUR_BLUE.
    BLIP_COLOUR_DESTINATION // 8 - Default color.
};

// https://wiki.multitheftauto.com/index.php?title=Radar_Blips
enum eRadarSprite : char {
    RADAR_SPRITE_NONE,          // 0
    RADAR_SPRITE_WHITE,         // 1
    RADAR_SPRITE_CENTRE,        // 2
    RADAR_SPRITE_MAP_HERE,      // 3
    RADAR_SPRITE_NORTH,         // 4
    RADAR_SPRITE_AIRYARD,       // 5
    RADAR_SPRITE_AMMUGUN,       // 6
    RADAR_SPRITE_BARBERS,       // 7
    RADAR_SPRITE_BIGSMOKE,      // 8
    RADAR_SPRITE_BOATYARD,      // 9
    RADAR_SPRITE_BURGERSHOT,    // 10
    RADAR_SPRITE_BULLDOZER,     // 11
    RADAR_SPRITE_CATALINAPINK,  // 12
    RADAR_SPRITE_CESARVIAPANDO, // 13 - What? R* mistype?
    RADAR_SPRITE_CHICKEN,       // 14
    RADAR_SPRITE_CJ,            // 15
    RADAR_SPRITE_CRASH1,        // 16
    RADAR_SPRITE_DINER,         // 17 - Yet another one?
    RADAR_SPRITE_EMMETGUN,      // 18
    RADAR_SPRITE_ENEMYATTACK,   // 19
    RADAR_SPRITE_FIRE,          // 20
    RADAR_SPRITE_GIRLFRIEND,    // 21
    RADAR_SPRITE_HOSTPITAL,     // 22 - Again?
    RADAR_SPRITE_LOGOSYNDICATE, // 23
    RADAR_SPRITE_MADDOG,        // 24
    RADAR_SPRITE_MAFIACASINO,   // 25
    RADAR_SPRITE_MCSTRAP,       // 26
    RADAR_SPRITE_MODGARAGE,     // 27
    RADAR_SPRITE_OGLOC,         // 28
    RADAR_SPRITE_PIZZA,         // 29
    RADAR_SPRITE_POLICE,        // 30
    RADAR_SPRITE_PROPERTYG,     // 31
    RADAR_SPRITE_PROPERTYR,     // 32
    RADAR_SPRITE_RACE,          // 33
    RADAR_SPRITE_RYDER,         // 34
    RADAR_SPRITE_SAVEGAME,      // 35
    RADAR_SPRITE_SCHOOL,        // 36
    RADAR_SPRITE_QMARK,         // 37
    RADAR_SPRITE_SWEET,         // 38
    RADAR_SPRITE_TATTOO,        // 39
    RADAR_SPRITE_THETRUTH,      // 40
    RADAR_SPRITE_WAYPOINT,      // 41
    RADAR_SPRITE_TORENORANCH,   // 42
    RADAR_SPRITE_TRIADS,        // 43
    RADAR_SPRITE_TRIADSCASINO,  // 44
    RADAR_SPRITE_TSHIRT,        // 45
    RADAR_SPRITE_WOOZIE,        // 46
    RADAR_SPRITE_ZERO,          // 47
    RADAR_SPRITE_DATEDISCO,     // 48
    RADAR_SPRITE_DATEDRINK,     // 49
    RADAR_SPRITE_DATEFOOD,      // 50
    RADAR_SPRITE_TRUCK,         // 51
    RADAR_SPRITE_CASH,          // 52
    RADAR_SPRITE_FLAG,          // 53
    RADAR_SPRITE_GYM,           // 54
    RADAR_SPRITE_IMPOUND,       // 55
    RADAR_SPRITE_LIGHT,         // 56
    RADAR_SPRITE_RUNWAY,        // 57
    RADAR_SPRITE_GANGB,         // 58
    RADAR_SPRITE_GANGP,         // 59
    RADAR_SPRITE_GANGY,         // 60
    RADAR_SPRITE_GANGN,         // 61
    RADAR_SPRITE_GANGG,         // 62
    RADAR_SPRITE_SPRAY/*,       // 63
    RADAR_SPRITE_TORENO*/       // 64
};

enum eRadarTraceHeight : unsigned char {
    RADAR_TRACE_LOW,   // 0
    RADAR_TRACE_HIGH,  // 1
    RADAR_TRACE_NORMAL // 2
};

struct tBlipHandle {
    unsigned short arrayIndex;
    unsigned short number;
};

VALIDATE_SIZE(tBlipHandle, 4);

struct airstrip_info {
    float x;
    float y;
    float direction; // angle
    float radius; // not sure
};

VALIDATE_SIZE(airstrip_info, 0x10);

class CEntryExit;

struct tRadarTrace {
    eBlipColour    m_nColour;
    unsigned int   m_nEntityHandle;
    CVector        m_vPosition;
    unsigned short m_nCounter;
    float          m_fSphereRadius;
    unsigned short m_nBlipSize;
    CEntryExit*    m_pEntryExit;
    eRadarSprite   m_nBlipSprite;
    unsigned char  m_bBright : 1;       // It makes use of bright colors. Always set.
    unsigned char  m_bTrackingBlip : 1; // It is available.
    unsigned char  m_bShortRange : 1;   // It doesn't show permanently on the radar.
    unsigned char  m_bFriendly : 1;     // It is affected by BLIP_COLOUR_THREAT.
    unsigned char  m_bBlipRemain : 1;   // It has the priority over the entity (it will still appear after the entity's deletion).
    unsigned char  m_bBlipFade : 1;     // Possibly a leftover. Always unset (unused).
    unsigned char  pad : 2;
    unsigned char  m_nCoordBlipAppearance : 2; // see eBlipAppearance
    unsigned char  m_nBlipDisplayFlag : 2;     // see eBlipDisplay
    unsigned char  m_nBlipType : 4;            // see eBlipType
};

VALIDATE_SIZE(tRadarTrace, 0x28);

class CRadar {
public:
    static constexpr unsigned int MAX_RADAR_SPRITES = 64;
    static constexpr unsigned int MAX_RADAR_TRACES = 175;
    static constexpr unsigned int MAX_AIRSTRIP_INFOS = 4;
    static constexpr unsigned int MAX_RADAR_WIDTH_TILES = 12;
    static constexpr unsigned int MAX_RADAR_HEIGHT_TILES = 12;

    static float& m_fRadarOrientation;

    // original name unknown
    static unsigned int& legendTraceTimer;
    static eRadarTraceHeight& legendTraceHeight;

    static unsigned int mapYouAreHereTimer;
    static bool mapYouAreHereDisplay;

    static char* RadarBlipFileNames[][2];

    // 2990.0 by default
    static float& m_radarRange;
    // static unsigned short MapLegendList[175];
    static unsigned short* MapLegendList;
    // num icons in legend
    static unsigned short& MapLegendCounter;
    // static CRGBA ArrowBlipColours[6];
    static CRGBA* ArrowBlipColour;
    // static tRadarTrace ms_RadarTrace[175];
    static tRadarTrace* ms_RadarTrace;
    // static CVector2D vec2DRadarOrigin;
    static CVector2D& vec2DRadarOrigin;
    // static CSprite2d RadarBlipSprites[64];
    static CSprite2d* RadarBlipSprites;
    static CRect& m_radarRect;
    // current airstrip index in airstrip_table
    static unsigned char& airstrip_location;
    // blip handle
    static int& airstrip_blip;

    static float& cachedCos;
    static float& cachedSin;

public:
    static void InjectHooks();

    static void Initialise();
    static void Shutdown();

    static void LoadTextures();

    static int GetNewUniqueBlipIndex(int blipIndex);
    static int GetActualBlipArrayIndex(int blipIndex);

    static void DrawLegend(int x, int y, eRadarSprite blipType);
    static float LimitRadarPoint(CVector2D& point);
    static void LimitToMap(float* pX, float* pY);
    static unsigned char CalculateBlipAlpha(float distance);
    static void TransformRadarPointToScreenSpace(CVector2D& out, const CVector2D& in);
    static void TransformRealWorldPointToRadarSpace(CVector2D& out, const CVector2D& in);
    static void TransformRadarPointToRealWorldSpace(CVector2D& out, const CVector2D& in);
    static void TransformRealWorldToTexCoordSpace(CVector2D& out, const CVector2D& in, int arg2, int arg3);
    static void CalculateCachedSinCos();
    static int SetCoordBlip(eBlipType type, CVector posn, _IGNORED_ unsigned int color, eBlipDisplay blipDisplay, _IGNORED_ char* scriptName);
    static int SetShortRangeCoordBlip(eBlipType type, CVector posn, unsigned int color, eBlipDisplay blipDisplay, char* scriptName);
    static int SetEntityBlip(eBlipType type, int entityHandle, unsigned int arg2, eBlipDisplay blipDisplay);
    static void ChangeBlipColour(int blipIndex, unsigned int color);
    static bool HasThisBlipBeenRevealed(int blipIndex);
    static bool DisplayThisBlip(int spriteId, char priority);
    static void ChangeBlipBrightness(int blipIndex, int brightness);
    static void ChangeBlipScale(int blipIndex, int size);
    static void ChangeBlipDisplay(int blipIndex, eBlipDisplay blipDisplay);
    static void SetBlipSprite(int blipIndex, int spriteId);
    static void SetBlipAlwaysDisplayInZoom(int blipIndex, unsigned char display);
    static void SetBlipFade(int blipIndex, unsigned char fade);
    static void SetCoordBlipAppearance(int blipIndex, unsigned char appearance);
    static void SetBlipFriendly(int blipIndex, bool friendly);
    static void SetBlipEntryExit(int blipIndex, CEntryExit* enex);
    static void ShowRadarTrace(float x, float y, unsigned int size, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
    static void ShowRadarTraceWithHeight(float x, float y, unsigned int size, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, eRadarTraceHeight height);
    static void ShowRadarMarker(CVector posn, unsigned int color, float radius);
    static unsigned int GetRadarTraceColour(eBlipColour color, bool bright, bool friendly);
    static void DrawRotatingRadarSprite(CSprite2d* sprite, float x, float y, float angle, unsigned int width, unsigned int height, const CRGBA& color);
    static void DrawYouAreHereSprite(float x, float y);
    static void SetupRadarRect(int x, int y);
    static void RequestMapSection(int x, int y);
    static void RemoveMapSection(int x, int y);
    static void RemoveRadarSections();
    static void StreamRadarSections(const CVector& worldPosn);
    static void StreamRadarSections(int x, int y);
    static int ClipRadarPoly(CVector2D* out, const CVector2D* in);
    static void DrawAreaOnRadar(const CRect& rect, const CRGBA& color, bool inMenu);
    static void DrawRadarMask();
    static void InitFrontEndMap();
    static void AddBlipToLegendList(unsigned char arg0, int blipIndex);
    static void SetMapCentreToPlayerCoords();
    static void Draw3dMarkers();
    static void SetRadarMarkerState(int arg0, unsigned char arg1);
    static void DrawRadarSprite(unsigned short spriteId, float x, float y, unsigned char alpha);
    static void DrawRadarSection(int x, int y);
    static void DrawRadarSectionMap(int x, int y, CRect rect);
    static void DrawRadarGangOverlay(bool inMenu);
    static void DrawRadarMap();
    static void DrawMap();

    static void DrawCoordBlip(int blipIndex, bool isSprite);
    static void DrawEntityBlip(int blipIndex, unsigned char arg1);
    static void ClearActualBlip(int blipIndex);
    static void ClearBlipForEntity(eBlipType blipType, int entityHandle);
    static void ClearBlip(int blipIndex);
    static void SetupAirstripBlips();
    static void DrawBlips();

    static void Load();
    static void Save();
};

bool ClipRadarTileCoords(int& x, int& y);
bool IsPointInsideRadar(const CVector2D& point);
void GetTextureCorners(int x, int y, CVector2D* corners);
int LineRadarBoxCollision(CVector2D& result, const CVector2D& lineStart, const CVector2D& lineEnd);
