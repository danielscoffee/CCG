#include <debuff.h>
#include <deck.h>
#include <monster.h>
#include <player.h>
#include <hud.h>
#include <dialog.h>
#include <input.h>
#include <trace.h>
#include <console_api.h>
#ifdef USE_SDL2
  #include <sdl_api.h>
#endif
#include <card_game.h>

/** === Globals === */
char *gkpszProgramName;
int gbLogLevel = 1;
int giLevel;
int gbSDL_Mode;
STRUCT_PLAYER gstPlayer;
#ifdef USE_SDL2
  STRUCT_HUD_LIST gstHudList;
#endif
/** === Procedures === */
void vFreeProgramName(){
  if (gkpszProgramName != NULL)
    free(gkpszProgramName);
}
void vSetProgramName(char *argv[]){
  char szPath[_MAX_PATH];
  char szName[_MAX_PATH];
  char szExt[_MAX_PATH];

  iDIR_SplitFilename(argv[0], szPath, szName, szExt);

  if (!bStrIsEmpty(szName)) {
    gkpszProgramName = (char *)malloc(strlen(szName) + 1);
    memset(gkpszProgramName, 0, strlen(szName) + 1);
    strcpy(gkpszProgramName, szName);
  }
}
void vInitGlobals(){
  giLevel = 1;
  giDebugLevel = DEBUG_LVL_DETAILS;
  gbSDL_Mode = FALSE;
  gkpszProgramName = NULL;
}
void vParseCmdlineArgs(char *argv[]){
  char *pTok;

  vSetProgramName(argv);

  if (bStrIsEmpty(argv[1]) || strstr(argv[1], "--") == NULL)
    return ;

  pTok = strtok(argv[1], "--");

  if (!memcmp(pTok, "sdl", 3))
    gbSDL_Mode = TRUE;

  if (bStrIsEmpty(argv[2]) || strstr(argv[2], "--") == NULL)
    return ;
  
  pTok = strtok(argv[2], "--");

  if (!memcmp(pTok, "debug=", 6)){
    pTok += 6;
    pTok = strtok(argv[2]," \n\r\t");
    if ( bStrIsNumeric(pTok) )
      giDebugLevel = atoi(pTok);
  }
  
}

/** ===  Main  === **/
int CCG_Main(int argc, char *argv[]){
#ifdef USE_SDL2
  SDL_Window *pSDL_Wndw = NULL;
  SDL_Renderer *pSDL_Rnd = NULL;
  SDL_Event SDL_Ev;
#endif
  STRUCT_MONSTER astMonsters[MAX_MONSTERS];
  STRUCT_DECK stDeck;
  int bRunning = TRUE;
  int iMonsterCount;
  
  vInitGlobals();

  if (argc > 1)
    vParseCmdlineArgs(argv);

  vInitLogs();
  
     
  #ifdef USE_SDL2
    if ( gbSDL_Mode ){
      vSDL_MainInit();
        // Create a window
      pSDL_Wndw = SDL_CreateWindow(
        "CCG",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        600,
        SDL_WINDOW_SHOWN
      );
      pSDL_Rnd = SDL_CreateRenderer(pSDL_Wndw, -1, SDL_RENDERER_ACCELERATED );
      SDL_SetRenderDrawBlendMode(pSDL_Rnd, SDL_BLENDMODE_BLEND);
    }
    else
      vShowInitDialog();
  #else
    vShowInitDialog();
  #endif

  vInitBasicDeck(&stDeck);
  iDrawMultipleCard(INIT_HAND_CARDS, &stDeck);
  vInitPlayer(&stDeck, !gbSDL_Mode);
  vInitMonstersForLevel(astMonsters, giLevel, &iMonsterCount);
  vInitDialog();
 
#ifdef FAKE
  vFakeOpenShopEarly(&stDeck);
#endif

  vTraceMainLoopInit();

  #ifdef USE_SDL2
    if ( gbSDL_Mode )
      vSDL_MainLoop(&bRunning, &SDL_Ev, pSDL_Rnd, &stDeck, astMonsters, iMonsterCount);
    else 
      vCNSL_MainLoop(&bRunning, &stDeck, astMonsters, iMonsterCount);
  #else
    vCNSL_MainLoop(&bRunning, &stDeck, astMonsters, iMonsterCount);
  #endif

  vTraceMainLoopEnd();
  vFreeDialog();
  vFreeProgramName();
  #ifdef USE_SDL2
    if ( gbSDL_Mode ) vSDL_MainQuit();
  #endif

  return 0;
}