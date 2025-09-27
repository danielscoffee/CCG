#include <deck.h>
#include <debuff.h>
#include <monster.h>
#include <player.h>
#include <trace.h>
#include <sdl_api.h>

#ifdef USE_SDL2  
  static const char *sdl_event_type_name(Uint32 type)
  {
      // user range: 0x8000..0xFFFF
      if (type >= SDL_USEREVENT && type <= SDL_LASTEVENT) {
          static char buf[64];
          if (type == SDL_USEREVENT) return "SDL_USEREVENT";
          snprintf(buf, sizeof(buf), "SDL_USEREVENT+%u", (unsigned)(type - SDL_USEREVENT));
          return buf;
      }
      // linear lookup (tabela pequena)
      for (size_t i = 0; i < sizeof(gEventNames)/sizeof(gEventNames[0]); ++i) {
          if (gEventNames[i].type == type)
              return gEventNames[i].name;
      }
      return "SDL_UNKNOWN_EVENT";
  }
  void vSDL_MainInit(){
    vTraceVarArgsFn("MainInit Begin");
    SDL_SetMainReady();
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      if (DEBUG_MSGS)    {
        char szMsg[256];
        memset(szMsg, 0x00, sizeof(szMsg));
        sprintf(szMsg,
        "Couldn't initialize SDL: %s\n",
          SDL_GetError()
        );
        vTraceVarArgsFn(szMsg);
      }
      return ;
    }
    vTraceVarArgsFn("MainInit End OK");
  }
  
  // void vSDL_DrawTable(SDL_Renderer *pSDL_Renderer, PSTRUCT_DECK pstDeck, PSTRUCT_MONSTER pastMonsters, int iMonsterCt){

  // }
  void vSDL_MainLoop(int *pbRunning, SDL_Event *pSDL_Event, SDL_Renderer *pSDL_Renderer, PSTRUCT_DECK pstDeck, PSTRUCT_MONSTER pastMonsters, int iMonsterCt){
    uint64_t ui64ElapsedTime;

    vTraceVarArgsFn("MainLoop Begin");
    
    while ( *pbRunning ){
      vSortHandByName(pstDeck);
      //vSDL_DrawTable(pSDL_Renderer, pstDeck, pastMonsters, iMonsterCt);

      while ( SDL_PollEvent(pSDL_Event) && *pbRunning ) {
        int iType = pSDL_Event->type; // Uint32
        if (iType > SDL_FIRSTEVENT && iType < SDL_LASTEVENT) {
            const char *name = sdl_event_type_name((Uint32)iType);
            vTraceVarArgsFn("Event=[%s] (0x%04X)", name, (unsigned)iType);
        }
        switch (pSDL_Event->type) {
          case SDL_QUIT:
            *pbRunning = FALSE;
            break;
        }
      }
      ui64ElapsedTime = SDL_GetTicks64();
      SDL_RenderClear( pSDL_Renderer );
      
      // Update the screen
      SDL_RenderPresent( pSDL_Renderer );
      
      // Vamos sincronizar o refreshrate..
      ui64ElapsedTime -= SDL_GetTicks64();
      if ( ui64ElapsedTime <= VSYNC_TIME ) SDL_Delay( VSYNC_TIME - ui64ElapsedTime );
    }
  }
#endif
