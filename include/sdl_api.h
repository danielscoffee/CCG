#ifdef USE_SDL2
    #ifndef SDL_API_H
        #define SDL_API_H
        #include <card_game.h>
        void vSDL_MainInit(SDL_Window *pSDL_window, SDL_Renderer *pSDL_Renderer );
        void vSDL_MainLoop(int *pbRunning, SDL_Event *pSDL_Event, SDL_Renderer *pSDL_Renderer, PSTRUCT_DECK pstDeck, PSTRUCT_MONSTER pastMonsters, int iMonsterCt);
    #endif
#endif