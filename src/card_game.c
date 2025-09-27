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
  gkpszProgramName = NULL;
  gbSDL_Mode = FALSE;
}
void vParseCmdlineArgs(char *argv[]){
  char *pTok;
  if (bStrIsEmpty(argv[1]) || strstr(argv[1], "--") == NULL)
    return ;

  pTok = strtok(argv[1], "--");

  if (pTok != NULL && strcasecmp(pTok, "sdl"))
    return ;

  gbSDL_Mode = TRUE;
}

/** ===  Main  === */
#ifdef USE_SDL2
int SDL_main(int argc, char *argv[]){
#else
int main(int argc, char *argv[]){
#endif
#ifdef USE_SDL2
  SDL_Window *pSDL_Wndw = NULL;
  SDL_Renderer* pSDL_Rnd = NULL;
  SDL_Event SDL_Ev;
#endif
  STRUCT_MONSTER astMonsters[MAX_MONSTERS];
  STRUCT_DECK stDeck;
  int bRunning = TRUE;
  int iMonsterCount;
  
  vSetProgramName(argv);
  vInitLogs();
  vInitGlobals();
  
  if (argc > 1)
    vParseCmdlineArgs(argv);
     
  #ifdef USE_SDL2
  if ( gbSDL_Mode )
    vSDL_MainInit(pSDL_Wndw, pSDL_Rnd);
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
    vSDL_MainInit(pSDL_Wndw, pSDL_Rnd);
  else 
    vCNSL_MainLoop(&bRunning, &stDeck, astMonsters, iMonsterCount);
  #else
  vCNSL_MainLoop(&bRunning, &stDeck, astMonsters, iMonsterCount);
  #endif

  vTraceMainLoopEnd();
  vFreeDialog();
  vFreeProgramName();

  return 0;
}