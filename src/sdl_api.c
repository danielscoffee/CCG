#ifdef USE_SDL2

#include <card_game.h>

/* Quarteto base */
#include <deck.h>
#include <debuff.h>
#include <monster.h>
#include <player.h>

/* Restante do motor */
#include <trace.h>
#include <battle.h>
#include <shop.h>
#include <dialog.h>
#include <input.h>
#include <terminal_utils.h>
#include <event.h>
#include <sdl_api.h>

/* SDL */
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/* -------------------------------------------------------------- */
/* Variáveis globais locais de renderização */
static TTF_Font *gFont = NULL;
static SDL_Rect gCardRects[64];
static SDL_Rect gMonsterRects[64];
static int gCardCount = 0;
static int gMonsterCount = 0;

/* Estado de seleção para jogar carta em monstro */
static int gSelectingTarget = 0;
static int gPendingCard = -1;

/* Comunicação com o motor */
int gSDL_SelectedMonster = -1;

/* -------------------------------------------------------------- */
/* Utilidades visuais */
void vSDL_DrawText(SDL_Renderer *pSDL_Renderer, const char *szTxt, int iX, int iY, SDL_Color stCol) {
  SDL_Surface *pSurf;
  SDL_Texture *pTex;
  SDL_Rect stDst;

  if (!gFont || !szTxt || !*szTxt)
    return;

  pSurf = TTF_RenderUTF8_Blended(gFont, szTxt, stCol);
  if (!pSurf)
    return;

  pTex = SDL_CreateTextureFromSurface(pSDL_Renderer, pSurf);
  if (pTex) {
    stDst.x = iX;
    stDst.y = iY;
    stDst.w = pSurf->w;
    stDst.h = pSurf->h;
    SDL_RenderCopy(pSDL_Renderer, pTex, NULL, &stDst);
    SDL_DestroyTexture(pTex);
  }
  SDL_FreeSurface(pSurf);
}

int bAreCoordsInSDL_Rect(SDL_Rect *pSDL_RECT, int iX, int iY) {
  if (pSDL_RECT == NULL)
    return FALSE;

  if (iX < 0 || iY < 0 || iX > INT_WINDOW_WIDTH || iY > INT_WINDOW_HEIGHT)
    return FALSE;

  if (iX >= pSDL_RECT->x &&
      iY >= pSDL_RECT->y &&
      iX <= pSDL_RECT->x + pSDL_RECT->w &&
      iY <= pSDL_RECT->y + pSDL_RECT->h) {
    return TRUE;
  }
  return FALSE;
}

/* -------------------------------------------------------------- */
/* HUD / Dialog / Mesa */

void vSDL_DrawDialog(SDL_Renderer *pSDL_Renderer, int iX, int iY, int iW, int iH) {
  int iLineH;
  int iBaseY;
  int ii;
  SDL_Rect stRectDialog;
  SDL_Color stColorWhite;
  PSTRUCT_DIALOG pstWrk;

  iLineH = 18;
  iBaseY = iY + 8;
  ii = 0;
  stRectDialog.x = iX;
  stRectDialog.y = iY;
  stRectDialog.w = iW;
  stRectDialog.h = iH;
  stColorWhite = (SDL_Color){255, 255, 255, 255};
  pstWrk = &gstDialog;

  SDL_SetRenderDrawColor(pSDL_Renderer, 40, 40, 40, 220);
  SDL_RenderFillRect(pSDL_Renderer, &stRectDialog);
  SDL_SetRenderDrawColor(pSDL_Renderer, 200, 200, 200, 255);
  SDL_RenderDrawRect(pSDL_Renderer, &stRectDialog);

  {
    int iMaxLines;
    int iTotal;
    int iSkip;
    int iIdx;

    iMaxLines = iH / iLineH - 1;
    iTotal = 0;
    for (pstWrk = &gstDialog; pstWrk; pstWrk = pstWrk->pstNext)
      iTotal++;
    iSkip = (iTotal > iMaxLines) ? (iTotal - iMaxLines) : 0;

    iIdx = 0;
    for (pstWrk = &gstDialog; pstWrk; pstWrk = pstWrk->pstNext) {
      char szLine[1024];
      if (iIdx++ < iSkip || bStrIsEmpty(pstWrk->pszMsg))
        continue;

      snprintf(szLine, sizeof(szLine), "%s - %s", pstWrk->szDT, pstWrk->pszMsg);
      vSDL_DrawText(pSDL_Renderer, szLine, iX + 8, iBaseY + (ii++) * iLineH, stColorWhite);
    }
  }
}

void vSDL_DrawHUD(SDL_Renderer *pSDL_Renderer, PSTRUCT_PLAYER pstPlayer) {
  SDL_Color white;
  char szHUD[128];
  white = (SDL_Color){255, 255, 255, 255};
  sprintf(szHUD, "HP: %d  Energia: %d  Escudo: %d",
          pstPlayer->iHP, pstPlayer->iEnergy, pstPlayer->iBlock);
  vSDL_DrawText(pSDL_Renderer, szHUD, 60, 20, white);
}

void vSDL_DrawTable(SDL_Renderer *pSDL_Renderer,
                    PSTRUCT_DECK pstDeck,
                    PSTRUCT_MONSTER pastMonsters,
                    int iMonsterCt) {
  SDL_Rect stRectMesa;
  SDL_Rect stRectCard;
  int ii;

  if (DEBUG_DIALOG)
    vTraceVarArgsFn("vSDL_DrawTable");

  stRectMesa.x = 50;
  stRectMesa.y = 50;
  stRectMesa.w = 700;
  stRectMesa.h = 500;

  SDL_SetRenderDrawColor(pSDL_Renderer, 0, 100, 0, 255);
  SDL_RenderFillRect(pSDL_Renderer, &stRectMesa);
  SDL_SetRenderDrawColor(pSDL_Renderer, 139, 69, 19, 255);
  SDL_RenderDrawRect(pSDL_Renderer, &stRectMesa);

  /* --- Monstros --- */
  {
    int iPad;
    int iUsableW;
    int iSlotW;
    int iSlotH;
    SDL_Color stWhite;

    iPad = 16;
    iUsableW = stRectMesa.w - iPad * 2;
    iSlotW = (iMonsterCt > 0) ? iUsableW / iMonsterCt : iUsableW;
    iSlotH = 80;
    stWhite = (SDL_Color){255, 255, 255, 255};

    for (ii = 0; ii < iMonsterCt; ++ii) {
      SDL_Rect stRectMonster;
      char szLine1[128];
      char szLine2[128];
      const char *szName;
      int iHP;
      int iAtk;

      if ( pastMonsters[ii].iHP <= 0) continue;
 
      stRectMonster.x = stRectMesa.x + iPad + ii * iSlotW + 8;
      stRectMonster.y = stRectMesa.y + iPad;
      stRectMonster.w = iSlotW - 16;
      stRectMonster.h = iSlotH;

      SDL_SetRenderDrawColor(pSDL_Renderer, 180, 40, 40, 255);
      SDL_RenderFillRect(pSDL_Renderer, &stRectMonster);
      SDL_SetRenderDrawColor(pSDL_Renderer, 0, 0, 0, 255);
      SDL_RenderDrawRect(pSDL_Renderer, &stRectMonster);

      szName = pastMonsters[ii].szName;
      iHP = pastMonsters[ii].iHP;
      iAtk = pastMonsters[ii].iAttack;

      snprintf(szLine1, sizeof(szLine1), "%s", szName ? szName : "Monster");
      snprintf(szLine2, sizeof(szLine2), "HP:%d  ATK:%d", iHP, iAtk);

      vSDL_DrawText(pSDL_Renderer, szLine1, stRectMonster.x + 8, stRectMonster.y + 8, stWhite);
      vSDL_DrawText(pSDL_Renderer, szLine2, stRectMonster.x + 8, stRectMonster.y + 31, stWhite);

      if (ii < (int)(sizeof(gMonsterRects) / sizeof(gMonsterRects[0])))
        gMonsterRects[ii] = stRectMonster;
    }
    gMonsterCount = iMonsterCt;
  }

  /* --- Cartas do jogador --- */
  gCardCount = 0;
  if (pstDeck->iHandCount > 0) {
    int iPadX;
    int iUsableW;
    int iSlotW;
    SDL_Color stWhite;

    iPadX = 24;
    iUsableW = stRectMesa.w - iPadX * 2;
    iSlotW = iUsableW / pstDeck->iHandCount;
    stWhite = (SDL_Color){255, 255, 255, 255};

    for (ii = 0; ii < pstDeck->iHandCount; ii++) {
      char szLine1[64];
      char szLine2[64];
      int iTx;
      int iTy;

      stRectCard.w = 80;
      stRectCard.h = 112;
      stRectCard.x = stRectMesa.x + iPadX + ii * iSlotW + (iSlotW - stRectCard.w) / 2;
      stRectCard.y = stRectMesa.y + stRectMesa.h - stRectCard.h - 20;

      if (ii < (int)(sizeof(gCardRects) / sizeof(gCardRects[0])))
        gCardRects[ii] = stRectCard;

      SDL_SetRenderDrawColor(pSDL_Renderer, 200, 200, 200, 255);
      SDL_RenderFillRect(pSDL_Renderer, &stRectCard);
      SDL_SetRenderDrawColor(pSDL_Renderer, 30, 30, 30, 255);
      SDL_RenderDrawRect(pSDL_Renderer, &stRectCard);

      snprintf(szLine1, sizeof(szLine1), "%s", pstDeck->astHand[ii].szName);
      snprintf(szLine2, sizeof(szLine2), "E:%d V:%d", pstDeck->astHand[ii].iCost, pstDeck->astHand[ii].iValue);

      iTx = stRectCard.x + 8;
      iTy = stRectCard.y + 8;

      vSDL_DrawText(pSDL_Renderer, szLine1, iTx, iTy, stWhite);
      vSDL_DrawText(pSDL_Renderer, szLine2, iTx, iTy + 21, stWhite);

      gCardCount++;
    }
  }

  if (DEBUG_DIALOG)
    vTraceVarArgsFn("vSDL_DrawTable END OK");
}

/* -------------------------------------------------------------- */
/* Inicialização e loop principal */

void vSDL_MainInit(void) {
  vTraceVarArgsFn("MainInit Begin");
  SDL_SetMainReady();

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    if (DEBUG_MSGS)
      vTraceVarArgsFn("Couldn't initialize SDL: %s", SDL_GetError());
    return;
  }

  /* Garante TTF inicializado antes de carregar fonte, caso ttfSDL_InitFont não o faça */
  if (TTF_WasInit() == 0) {
    if (TTF_Init() != 0)
      vTraceVarArgsFn("Erro TTF_Init: %s", TTF_GetError());
  }

  gFont = ttfSDL_InitFont();
  if (!gFont)
    vTraceVarArgsFn("Erro ao carregar fonte: %s", TTF_GetError());

  vTraceVarArgsFn("MainInit End OK");
}
int iHitTestMonster(int iX, int iY) {
  int ii;
  for (ii = 0; ii < gMonsterCount; ii++) {
    if (bAreCoordsInSDL_Rect(&gMonsterRects[ii], iX, iY)) {
      return ii;
    }
  }
  return -1;
}
/* Alterna fullscreen/windowed */
void vSDL_ToggleFullscreen(void) {
  SDL_Window *pWindow;
  Uint32 uFlags;

  pWindow = SDL_GetWindowFromID(1);
  if (pWindow == NULL)
    return;

  uFlags = SDL_GetWindowFlags(pWindow);
  if ((uFlags & SDL_WINDOW_FULLSCREEN) != 0)
    SDL_SetWindowFullscreen(pWindow, 0);
  else
    SDL_SetWindowFullscreen(pWindow, SDL_WINDOW_FULLSCREEN);
}

void vSDL_MainLoop(int *pbRunning, SDL_Event *pSDL_Event, SDL_Renderer *pSDL_Renderer,
                   PSTRUCT_DECK pstDeck, PSTRUCT_MONSTER pastMonsters, int iMonsterCt) {
  uint64_t ui64ElapsedTime;
  int iRedrawAction;

  vTraceVarArgsFn("vSDL_MainLoop BEGIN");

  vInitPlayer(pstDeck, PLAYER_GET_NAME_NONE);
  iDrawMultipleCard(INIT_HAND_CARDS, pstDeck);
  vSortHandByName(pstDeck);
  iRedrawAction = REDRAW_IMAGE;

  while (*pbRunning) {
    while (SDL_PollEvent(pSDL_Event)) {
      /* Handler genérico: marca redraw quando apropriado */
      iRedrawAction = iEVENT_HandlePollEv(pSDL_Event, iRedrawAction);

      if (pSDL_Event->type == SDL_QUIT ||
          (pSDL_Event->type == SDL_KEYDOWN &&
           pSDL_Event->key.keysym.sym == SDLK_ESCAPE)) {
        *pbRunning = FALSE;
        break;
      }

      /* Alt+Enter → fullscreen */
      if (pSDL_Event->type == SDL_KEYDOWN &&
          (pSDL_Event->key.keysym.mod & KMOD_ALT) &&
          pSDL_Event->key.keysym.sym == SDLK_RETURN) {
        vSDL_ToggleFullscreen();
        iRedrawAction = REDRAW_IMAGE;
      }

      /* ---- LÓGICA DE CLIQUE PARA JOGAR CARTA ---- */
      if (pSDL_Event->type == SDL_MOUSEBUTTONDOWN &&
          pSDL_Event->button.button == SDL_BUTTON_LEFT) {
        int iX;
        int iY;
        int jj;

        iX = pSDL_Event->button.x;
        iY = pSDL_Event->button.y;

        if (!gSelectingTarget) {
          for (jj = 0; jj < gCardCount; jj++) {
            if (bAreCoordsInSDL_Rect(&gCardRects[jj], iX, iY)) {
              PSTRUCT_CARD pstCard;
              int iAlive = 0;
              int iLastM = -1;
              int mm;

              pstCard = &pstDeck->astHand[jj];
              gPendingCard = jj;

              /* --- Caso 1: Carta não requer alvo (Heal, Defend, etc.) --- */
              if (pstCard->iTarget == CARD_TARGET_SELF) {
                vTraceVarArgsFn("SDL: alvo da carta eh o player [%s]", pstCard->szName);
                vPlayCard(gPendingCard, pstDeck, pastMonsters, gMonsterCount);
                gPendingCard = -1;
                iRedrawAction = REDRAW_IMAGE;
                break;
              }

              /* --- Caso 2: carta de múltiplos alvos (Fireball, Poison AOE) --- */
              if (pstCard->iTarget == CARD_TARGET_MULTIPLE) {
                vTraceVarArgsFn("SDL: carta multi-alvo [%s]", pstCard->szName);
                vPlayCard(gPendingCard, pstDeck, pastMonsters, gMonsterCount);
                gPendingCard = -1;
                iRedrawAction = REDRAW_IMAGE;
                break;
              }

              /* --- Caso 3: carta com alvo único --- */
              for (mm = 0; mm < gMonsterCount; mm++) {
                if (pastMonsters[mm].iHP > 0) {
                  iAlive++;
                  iLastM = mm;
                }
              }

              if (iAlive <= 1) {
                if (bHasAnyPlayableCard(pstDeck)) {
                  gSDL_SelectedMonster = (iLastM >= 0) ? iLastM : 0;
                  vTraceVarArgsFn("SDL: auto-alvo [%s] -> monstro %d",
                                  pstCard->szName, gSDL_SelectedMonster);
                  vPlayCard(gPendingCard, pstDeck, pastMonsters, gMonsterCount);
                  gSDL_SelectedMonster = -1;
                  gPendingCard = -1;
                  iRedrawAction = REDRAW_IMAGE;
                }
              } else {
                gSelectingTarget = 1;
                vPrintLine("Selecione um monstro com o mouse...", NO_NEW_LINE);
              }

              break; /* já processou a carta */
            }
          }
        } else {
          /* 2) já escolheu carta -> clique no monstro alvo */
          int iTarget;
          iTarget = iHitTestMonster(iX, iY);

          if (iTarget >= 0 && gPendingCard >= 0) {
            gSDL_SelectedMonster = iTarget;
            vTraceVarArgsFn("Click: alvo [%d] X[%d] Y[%d]", iTarget, iX, iY);
            vPlayCard(gPendingCard, pstDeck, pastMonsters, gMonsterCount);
            gSDL_SelectedMonster = -1;
            gPendingCard = -1;
            gSelectingTarget = 0;
            iRedrawAction = REDRAW_IMAGE;
          }
        }
      }
    }

    if (!*pbRunning)
      break;

    /* Fim de turno / ações dos inimigos */
    if (gstPlayer.iEnergy <= 0 || !iAnyMonsterAlive(pastMonsters, iMonsterCt)) {
      SDL_RenderClear(pSDL_Renderer);
      vSDL_DrawTable(pSDL_Renderer, pstDeck, pastMonsters, iMonsterCt);
      vSDL_DrawHUD(pSDL_Renderer, &gstPlayer);

      {
        int iDlgY;
        int iDlgH;
        int iWinW;
        int iWinH;
        int iMarginInferior = 2;
        int iMarginSuperior = 2;

        SDL_GetRendererOutputSize(pSDL_Renderer, &iWinW, &iWinH);
        iDlgY = 50 + 500 + iMarginSuperior;
        iDlgH = iWinH - iDlgY - iMarginInferior;
        if (iDlgH < 0)
          iDlgH = 0;

        vSDL_DrawDialog(pSDL_Renderer, 50, iDlgY, 700, iDlgH);
      }

      SDL_RenderPresent(pSDL_Renderer);
      vSleepSeconds(1);
      vDoEnemyActions(pastMonsters, iMonsterCt);
      vDiscardHand(pstDeck);
      iDrawMultipleCard(INIT_HAND_CARDS, pstDeck);
      gstPlayer.iEnergy = PLAYER_ENERGY_MAX;
      iRedrawAction = REDRAW_IMAGE;
    }

    if (iRedrawAction != REDRAW_IMAGE)
      continue;

    ui64ElapsedTime = SDL_GetTicks64();

    SDL_RenderClear(pSDL_Renderer);
    vSDL_DrawTable(pSDL_Renderer, pstDeck, pastMonsters, iMonsterCt);
    vSDL_DrawHUD(pSDL_Renderer, &gstPlayer);

    /* Dialog dentro da janela (evita ficar fora em 800x600) */
    {
      int iDlgY;
      int iDlgH;
      int iWinW;
      int iWinH;
      int iMarginInferior;
      int iMarginSuperior;

      /* Margens configuráveis */
      iMarginSuperior = 2;   /* espaço entre mesa e diálogo */
      iMarginInferior = 2;   /* espaço inferior antes da borda da janela */

      /* Obtém dimensões reais do renderer */
      SDL_GetRendererOutputSize(pSDL_Renderer, &iWinW, &iWinH);

      /* Posição Y inicial: onde termina a mesa + margem superior */
      iDlgY = 50 + 500 + iMarginSuperior;

      /* Altura: altura real da janela - posição inicial - margem inferior */
      iDlgH = iWinH - iDlgY - iMarginInferior;

      if (iDlgH < 0)
        iDlgH = 0;

      vTraceVarArgsFn("DlgH[%d] WinH[%d] DlgY[%d]", iDlgH, iWinH, iDlgY);

      vSDL_DrawDialog(pSDL_Renderer, 50, iDlgY, 700, iDlgH);
    }



    SDL_RenderPresent(pSDL_Renderer);

    ui64ElapsedTime = SDL_GetTicks64() - ui64ElapsedTime;
    if (ui64ElapsedTime <= VSYNC_TIME)
      SDL_Delay(VSYNC_TIME - ui64ElapsedTime);

    iRedrawAction = REDRAW_NONE;
  }

  vTraceVarArgsFn("vSDL_MainLoop END OK");
}

/* -------------------------------------------------------------- */
/* Quit */

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

#endif /* USE_SDL2 */
