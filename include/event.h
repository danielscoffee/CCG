#ifndef _EVENT_H_
  #define _EVENT_H_

  #ifdef USE_SDL2

    /* Hub do projeto sempre primeiro */
    #include <card_game.h>

    /* Flags simples de redraw para o loop SDL do CCG */
    #define REDRAW_NONE   0
    #define REDRAW_IMAGE  1

    /* API pública do módulo de eventos */
    void vEVENT_Init(void);
    void vEVENT_Quit(void);

    /* Handler único: processa 1 evento SDL e retorna REDRAW_NONE ou REDRAW_IMAGE */
    int iEVENT_HandlePollEv(SDL_Event *pSDL_EVENT_Ev, int iRedrawCurrentAction);

  #endif /* USE_SDL2 */

#endif /* _EVENT_H_ */
