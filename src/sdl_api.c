#include <deck.h>
#include <debuff.h>
#include <monster.h>
#include <player.h>
#include <trace.h>
#include <sdl_api.h>

#ifdef USE_SDL2

  static TTF_Font *gFont = NULL;

  // Helper simples para texto
  static void vSDL_DrawText(SDL_Renderer *r, const char *txt, int x, int y, SDL_Color col)
  {
      if (!gFont || !txt || !*txt) return;
      SDL_Surface *surf = TTF_RenderUTF8_Blended(gFont, txt, col);
      if (!surf) return;
      SDL_Texture *tex = SDL_CreateTextureFromSurface(r, surf);
      if (tex) {
          SDL_Rect dst = { x, y, surf->w, surf->h };
          SDL_RenderCopy(r, tex, NULL, &dst);
          SDL_DestroyTexture(tex);
      }
      SDL_FreeSurface(surf);
  }

void vSDL_DrawTable(SDL_Renderer *pSDL_Renderer, 
                    PSTRUCT_DECK pstDeck, 
                    PSTRUCT_MONSTER pastMonsters, 
                    int iMonsterCt) 
{
  SDL_Rect stRectMesa;
  SDL_Rect stRectCard;
  int iCardCt;
  int i;

  vTraceVarArgsFn("vSDL_DrawTable");

  /* Mesa principal */
  stRectMesa.x = 50;
  stRectMesa.y = 50;
  stRectMesa.w = 700;
  stRectMesa.h = 500;

  /* fundo da mesa */
  SDL_SetRenderDrawColor(pSDL_Renderer, 0, 100, 0, 255);
  SDL_RenderFillRect(pSDL_Renderer, &stRectMesa);

  /* borda da mesa */
  SDL_SetRenderDrawColor(pSDL_Renderer, 139, 69, 19, 255);
  SDL_RenderDrawRect(pSDL_Renderer, &stRectMesa);

  /* --- MONSTROS (topo da mesa) --- */
  {
    int pad = 16;
    int usableW = stRectMesa.w - pad*2;
    int slotW = (iMonsterCt > 0) ? usableW / iMonsterCt : usableW;
    int slotH = 80;
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color black = { 0, 0, 0, 255 };

    for (i = 0; i < iMonsterCt; ++i) {
      SDL_Rect mrect;
      mrect.x = stRectMesa.x + pad + i*slotW + 8;
      mrect.y = stRectMesa.y + pad;
      mrect.w = slotW - 16;
      mrect.h = slotH;

      /* base do monstro */
      SDL_SetRenderDrawColor(pSDL_Renderer, 180, 40, 40, 255);
      SDL_RenderFillRect(pSDL_Renderer, &mrect);
      SDL_SetRenderDrawColor(pSDL_Renderer, 0, 0, 0, 255);
      SDL_RenderDrawRect(pSDL_Renderer, &mrect);

      /* textos do monstro */
      char line1[128], line2[128];
      /* ajuste os campos conforme seu struct */
      const char *name = pastMonsters[i].szName;     // EX.: char szName[...];
      int hp  = pastMonsters[i].iHP;                 // EX.: vida atual
      int atk = pastMonsters[i].iAttack;                // EX.: ataque
      snprintf(line1, sizeof(line1), "%s", name ? name : "Monster");
      snprintf(line2, sizeof(line2), "HP:%d  ATK:%d", hp, atk);

      /* sombreia o texto pra legibilidade */
      vSDL_DrawText(pSDL_Renderer, line1, mrect.x+9,  mrect.y+9,  black);
      vSDL_DrawText(pSDL_Renderer, line1, mrect.x+8,  mrect.y+8,  white);
      vSDL_DrawText(pSDL_Renderer, line2, mrect.x+9,  mrect.y+32, black);
      vSDL_DrawText(pSDL_Renderer, line2, mrect.x+8,  mrect.y+31, white);
    }
  }

  /* --- MÃO DO JOGADOR (base da mesa) --- */
  iCardCt = pstDeck->iHandCount; /* ajuste se o campo tiver outro nome */
  if (iCardCt > 0) {
    int padX = 24;
    int usableW = stRectMesa.w - padX*2;
    int slotW = usableW / iCardCt;
    SDL_Color black = { 0, 0, 0, 255 };
    SDL_Color white = { 255, 255, 255, 255 };

    for (i = 0; i < iCardCt; i++) {
      stRectCard.w = 80;
      stRectCard.h = 112;
      stRectCard.x = stRectMesa.x + padX + i*slotW + (slotW - stRectCard.w)/2;
      stRectCard.y = stRectMesa.y + stRectMesa.h - stRectCard.h - 20;

      /* carta */
      SDL_SetRenderDrawColor(pSDL_Renderer, 200, 200, 200, 255);
      SDL_RenderFillRect(pSDL_Renderer, &stRectCard);
      SDL_SetRenderDrawColor(pSDL_Renderer, 30, 30, 30, 255);
      SDL_RenderDrawRect(pSDL_Renderer, &stRectCard);

      /* texto da carta (nome + custo) */
      /* AJUSTE estes acessos conforme seu struct de carta na mão */
      const char *cardName = pstDeck->astHand[i].szName;   // EX.: array de cartas na mão
      int energy = pstDeck->astHand[i].iCost;        // EX.: custo de energia
      int value  = pstDeck->astHand[i].iValue;             // opcional, se quiser mostrar

      char line1[64], line2[64];
      snprintf(line1, sizeof(line1), "%s", cardName ? cardName : "Card");
      snprintf(line2, sizeof(line2), "E:%d%s", energy, value ? " " : "");
      /* se quiser incluir o valor: snprintf(line2, sizeof(line2), "E:%d  V:%d", energy, value); */

      int tx = stRectCard.x + 8;
      int ty = stRectCard.y + 8;

      vSDL_DrawText(pSDL_Renderer, line1, tx+1, ty+1, black);
      vSDL_DrawText(pSDL_Renderer, line1, tx,   ty,   white);

      vSDL_DrawText(pSDL_Renderer, line2, tx+1, ty+22, black);
      vSDL_DrawText(pSDL_Renderer, line2, tx,   ty+21, white);
    }
  }

  vTraceVarArgsFn("vSDL_DrawTable END OK");
}


  void vSDL_TraceEv(SDL_Event *pSDL_Event) {
    u_int ui32Type = pSDL_Event->type; // Uint32
    size_t ui64_Ct;
    char szDbg[128];
    if (ui32Type >= SDL_USEREVENT && ui32Type <= SDL_LASTEVENT) {
      if (ui32Type == SDL_USEREVENT){
        snprintf(szDbg, sizeof(szDbg), "%s", "SDL_USEREVENT");
        vTraceVarArgsFn("Event=[%s] (0x%04X)", szDbg, (unsigned)ui32Type);
        return ;
      }
      snprintf(szDbg, sizeof(szDbg), "SDL_USEREVENT+%u", (unsigned)(ui32Type - SDL_USEREVENT));
      vTraceVarArgsFn("Event=[%s] (0x%04X)", szDbg, (unsigned)ui32Type);
      return ;
    }
    for ( ui64_Ct = 0; ui64_Ct < sizeof(gEventNames) / sizeof(gEventNames[0]); ui64_Ct++) {
      if (gEventNames[ui64_Ct].type == ui32Type){
        vTraceVarArgsFn("Event=[%s] (0x%04X)", gEventNames[ui64_Ct].name, (unsigned)ui32Type);
        return ;
      }
    }
    vTraceVarArgsFn("Event=[%s] (0x%04X)", "SDL_UNKNOWN_EVENT", (unsigned)ui32Type);
    return ;
  }
  void vSDL_MainInit() {
    vTraceVarArgsFn("MainInit Begin");
    SDL_SetMainReady();

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      if (DEBUG_MSGS)
      {
        char szMsg[256];
        memset(szMsg, 0x00, sizeof(szMsg));
        sprintf(szMsg,
                "Couldn't initialize SDL: %s\n",
                SDL_GetError());
        vTraceVarArgsFn(szMsg);
      }
      return;
    }
    gFont = ttfSDL_InitFont();
    if (!gFont) {
      vTraceVarArgsFn("Erro ao carregar fonte: %s", TTF_GetError());
    }
    vTraceVarArgsFn("MainInit End OK");
  }

  void vSDL_MainLoop(int *pbRunning,
                     SDL_Event *pSDL_Event, 
                     SDL_Renderer *pSDL_Renderer,
                     PSTRUCT_DECK pstDeck,
                     PSTRUCT_MONSTER pastMonsters,
                     int iMonsterCt){
    uint64_t ui64ElapsedTime;
    
    vTraceVarArgsFn("vSDL_MainLoop ");

    while (*pbRunning) {
      vSortHandByName(pstDeck);

      SDL_RenderClear(pSDL_Renderer);  // limpa a tela

      vSDL_DrawTable(pSDL_Renderer, pstDeck, pastMonsters, iMonsterCt); // redesenha tudo
      
      SDL_RenderPresent(pSDL_Renderer); // mostra na janela
      
      while (SDL_PollEvent(pSDL_Event) && *pbRunning) {
        vSDL_TraceEv(pSDL_Event);
        switch (pSDL_Event->type) {
        case SDL_QUIT:
          *pbRunning = FALSE;
          break;
        }
      }

      ui64ElapsedTime = SDL_GetTicks64();
      // Vamos sincronizar o refreshrate
      ui64ElapsedTime = SDL_GetTicks64() - ui64ElapsedTime;
      if (ui64ElapsedTime <= VSYNC_TIME)
        SDL_Delay(VSYNC_TIME - ui64ElapsedTime);
  }

    
    vTraceVarArgsFn("vSDL_MainLoop END OK");
  }
  void vSDL_MainQuit(void) {
    vTraceVarArgsFn("MainQuit Begin");

    if (gFont) {
      TTF_CloseFont(gFont);
      gFont = NULL;
    }

    TTF_Quit();
    SDL_Quit();

    vTraceVarArgsFn("MainQuit End OK");
  }

#endif
