#ifdef USE_SDL2

/* Hub do projeto primeiro */
#include <card_game.h>

/* Quarteto core do jogo */
#include <deck.h>
#include <debuff.h>
#include <monster.h>
#include <player.h>

/* SDL e headers do projeto */
#include <SDL2/SDL.h>
#include <trace.h>
#include <event.h>

/* =========================================================================
   Módulo de eventos — versão enxuta para CCG (SDL-only)
   - Apenas dois estados de redraw: REDRAW_NONE / REDRAW_IMAGE
   - Um único handler: iEVENT_HandlePollEv()
   - Delega efeitos colaterais (fullscreen, sair, etc.) ao chamador
   ========================================================================= */

void vEVENT_Init(void) {
  if (DEBUG_MSGS) vTraceVarArgsFn("EVENT: init");
}

void vEVENT_Quit(void) {
  if (DEBUG_MSGS) vTraceVarArgsFn("EVENT: quit");
}

/* Handler único: processa 1 SDL_Event e retorna REDRAW_* */
int iEVENT_HandlePollEv(SDL_Event *pSDL_EVENT_Ev, int iRedrawCurrentAction) {
  int iRedrawReturnStatus;

  iRedrawReturnStatus = iRedrawCurrentAction;

  if (pSDL_EVENT_Ev == NULL) {
    return iRedrawReturnStatus;
  }

  switch (pSDL_EVENT_Ev->type) {
    case SDL_QUIT: {
      /* Caller decide setar *pbRunning = FALSE; aqui não forçamos redraw */
      break;
    }

    case SDL_MOUSEBUTTONDOWN: {
      /* Clique do mouse normalmente altera seleção/estado → pedir redraw */
      iRedrawReturnStatus = REDRAW_IMAGE;
      break;
    }

    case SDL_MOUSEBUTTONUP: {
      /* Se sua UX exigir, pode manter sem redraw para evitar flicker */
      break;
    }

    case SDL_MOUSEMOTION: {
      /* Ative se tiver hover/tooltip; comentei para evitar redraw excessivo */
      /* iRedrawReturnStatus = REDRAW_IMAGE; */
      break;
    }

    case SDL_KEYDOWN: {
      /* Alt+Enter (fullscreen) e outras teclas que afetam UI → redraw */
      if ((pSDL_EVENT_Ev->key.keysym.mod & KMOD_ALT) != 0 &&
           pSDL_EVENT_Ev->key.keysym.sym == SDLK_RETURN) {
        iRedrawReturnStatus = REDRAW_IMAGE;
      } else if (pSDL_EVENT_Ev->key.keysym.sym == SDLK_ESCAPE) {
        /* Caller decide encerrar; sem redraw obrigatório aqui */
      } else {
        /* Outras teclas que mudem UI também podem pedir redraw */
        /* iRedrawReturnStatus = REDRAW_IMAGE; */
      }
      break;
    }

    case SDL_TEXTINPUT: {
      /* Se houver console/chat, geralmente precisa redesenhar */
      /* iRedrawReturnStatus = REDRAW_IMAGE; */
      break;
    }

    default: {
      break;
    }
  }

  return iRedrawReturnStatus;
}

#endif /* USE_SDL2 */
