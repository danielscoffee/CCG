#include <deck.h>
#include <debuff.h>
#include <monster.h>
#include <player.h>
#include <trace.h>
#include <sdl_api.h>

#ifdef USE_SDL2
  void vSDL_MainInit(SDL_Window *pSDL_Window, SDL_Renderer *pSDL_Renderer ){
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

  // Create a window
    pSDL_Window = SDL_CreateWindow(
      "CCG",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      800,
      600,
      SDL_WINDOW_SHOWN
    );

    pSDL_Renderer = SDL_CreateRenderer( pSDL_Window, -1, SDL_RENDERER_ACCELERATED );
    SDL_SetRenderDrawBlendMode( pSDL_Renderer, SDL_BLENDMODE_BLEND );

  }
  // void vSDL_DrawTable(SDL_Renderer *pSDL_Renderer, PSTRUCT_DECK pstDeck, PSTRUCT_MONSTER pastMonsters, int iMonsterCt){

  // }
  void vSDL_MainLoop(int *pbRunning, SDL_Event *pSDL_Event, SDL_Renderer *pSDL_Renderer, PSTRUCT_DECK pstDeck, PSTRUCT_MONSTER pastMonsters, int iMonsterCt){
    uint64_t ui64ElapsedTime;
    while ( *pbRunning ){
      vSortHandByName(pstDeck);
      //vSDL_DrawTable(pSDL_Renderer, pstDeck, pastMonsters, iMonsterCt);
      while ( SDL_PollEvent(pSDL_Event) && *pbRunning ) { 
        switch (pSDL_Event->type) {
          case SDL_QUIT:
          pbRunning = FALSE;
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