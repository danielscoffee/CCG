#include <deck.h>
#include <debuff.h>
#include <monster.h>
#include <player.h>
#include <trace.h>
#include <battle.h>
#include <shop.h>
#include <dialog.h>
#include <sdl_api.h>

#ifdef USE_SDL2

static TTF_Font *gFont = NULL;
/* Retângulos para hit-test */
static SDL_Rect gCardRects[64];
static int gCardCount = 0;
static SDL_Rect gMonsterRects[64];
static int gMonsterCount = 0;

/* Estado de seleção de alvo */
static int gSelectingTarget = 0;
static int gPendingCard = -1;

/* Comunicação com o motor (battle.c) */
int gSDL_SelectedMonster = -1; /* usado pelo vPlayCard em modo SDL */

void vSDL_DrawDialog(SDL_Renderer *pSDL_Renderer, int iX, int iY, int iW, int iH) {
  SDL_Rect stRectDialog = { iX, iY, iW, iH };
  SDL_Color stColorWhite = (SDL_Color){ 255, 255, 255, 255 };
  PSTRUCT_DIALOG pstWrk = &gstDialog;
  int i = 0;
  int iLineH = 18;
  int iBaseY = iY + 8;

  SDL_SetRenderDrawColor(pSDL_Renderer, 40, 40, 40, 220);   /* fundo escuro */
  SDL_RenderFillRect(pSDL_Renderer, &stRectDialog);
  SDL_SetRenderDrawColor(pSDL_Renderer, 200, 200, 200, 255);/* borda clara */
  SDL_RenderDrawRect(pSDL_Renderer, &stRectDialog);

  /* mostra as últimas N linhas (cabe na altura da HUD) */
  int iMaxLines = iH / iLineH -1;
  int iTotal = 0;
  for (pstWrk = &gstDialog; pstWrk; pstWrk = pstWrk->pstNext) iTotal++;
  int iSkip = (iTotal > iMaxLines) ? (iTotal - iMaxLines) : 0;

  int iIdx = 0;
  for (pstWrk = &gstDialog; pstWrk; pstWrk = pstWrk->pstNext) {
    char szLine[1024];
    if (iIdx++ < iSkip || bStrIsEmpty(pstWrk->pszMsg) ) continue;

    snprintf(szLine, sizeof(szLine), "%s - %s", pstWrk->szDT, pstWrk->pszMsg);

    vSDL_DrawText(pSDL_Renderer, szLine, iX + 8, iBaseY + (i++) * iLineH, stColorWhite);
  }
}


// Helper simples para texto
static void vSDL_DrawText(SDL_Renderer *pSDL_Renderer, const char *szTxt, int iX, int iY, SDL_Color stCol)
{
  SDL_Surface *pSurf;
  SDL_Texture *pTex;
  SDL_Rect stDst;

  if (!gFont || !szTxt || !*szTxt)
    return;

  pSurf = TTF_RenderUTF8_Blended(gFont, szTxt, stCol);
  if (!pSurf)
    return;

  pTex = SDL_CreateTextureFromSurface(pSDL_Renderer, pSurf);
  if (pTex)
  {
    stDst.x = iX;
    stDst.y = iY;
    stDst.w = pSurf->w;
    stDst.h = pSurf->h;
    SDL_RenderCopy(pSDL_Renderer, pTex, NULL, &stDst);
    SDL_DestroyTexture(pTex);
  }
  SDL_FreeSurface(pSurf);
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
    int iPad = 16;
    int iUsableW = stRectMesa.w - iPad * 2;
    int iSlotW = (iMonsterCt > 0) ? iUsableW / iMonsterCt : iUsableW;
    int iSlotH = 80;
    SDL_Color stWhite = {255, 255, 255, 255};
    SDL_Color stBlack = {0, 0, 0, 255};

    for (i = 0; i < iMonsterCt; ++i)
    {
      SDL_Rect stRectMonster;
      char szLine1[128], szLine2[128];
      const char *szName;
      int iHP;
      int iAtk;

      stRectMonster.x = stRectMesa.x + iPad + i * iSlotW + 8;
      stRectMonster.y = stRectMesa.y + iPad;
      stRectMonster.w = iSlotW - 16;
      stRectMonster.h = iSlotH;

      /* base do monstro */
      SDL_SetRenderDrawColor(pSDL_Renderer, 180, 40, 40, 255);
      SDL_RenderFillRect(pSDL_Renderer, &stRectMonster);
      SDL_SetRenderDrawColor(pSDL_Renderer, 0, 0, 0, 255);
      SDL_RenderDrawRect(pSDL_Renderer, &stRectMonster);

      szName = pastMonsters[i].szName;
      iHP = pastMonsters[i].iHP;
      iAtk = pastMonsters[i].iAttack;

      snprintf(szLine1, sizeof(szLine1), "%s", szName ? szName : "Monster");
      snprintf(szLine2, sizeof(szLine2), "HP:%d  ATK:%d", iHP, iAtk);

      vSDL_DrawText(pSDL_Renderer, szLine1, stRectMonster.x + 9, stRectMonster.y + 9, stBlack);
      vSDL_DrawText(pSDL_Renderer, szLine1, stRectMonster.x + 8, stRectMonster.y + 8, stWhite);
      vSDL_DrawText(pSDL_Renderer, szLine2, stRectMonster.x + 9, stRectMonster.y + 32, stBlack);
      vSDL_DrawText(pSDL_Renderer, szLine2, stRectMonster.x + 8, stRectMonster.y + 31, stWhite);

      if (i < (int)(sizeof(gMonsterRects) / sizeof(gMonsterRects[0])))
        gMonsterRects[i] = stRectMonster;
    }
    gMonsterCount = iMonsterCt;
  }

  /* --- MÃO DO JOGADOR (base da mesa) --- */
  iCardCt = pstDeck->iHandCount;
  gCardCount = iCardCt;

  if (gCardCount > 0)
  {
    int iPadX = 24;
    int iUsableW = stRectMesa.w - iPadX * 2;
    int iSlotW = iUsableW / gCardCount;
    SDL_Color stBlack = {0, 0, 0, 255};
    SDL_Color stWhite = {255, 255, 255, 255};

    for (i = 0; i < gCardCount; i++)
    {
      const char *szCardName;
      int iEnergy;
      int iValue;
      char szLine1[64], szLine2[64];
      int iTx, iTy;

      stRectCard.w = 80;
      stRectCard.h = 112;
      stRectCard.x = stRectMesa.x + iPadX + i * iSlotW + (iSlotW - stRectCard.w) / 2;
      stRectCard.y = stRectMesa.y + stRectMesa.h - stRectCard.h - 20;

      gCardRects[i] = stRectCard;

      SDL_SetRenderDrawColor(pSDL_Renderer, 200, 200, 200, 255);
      SDL_RenderFillRect(pSDL_Renderer, &stRectCard);
      SDL_SetRenderDrawColor(pSDL_Renderer, 30, 30, 30, 255);
      SDL_RenderDrawRect(pSDL_Renderer, &stRectCard);

      szCardName = pstDeck->astHand[i].szName;
      iEnergy = pstDeck->astHand[i].iCost;
      iValue = pstDeck->astHand[i].iValue;

      snprintf(szLine1, sizeof(szLine1), "%s", szCardName ? szCardName : "Card");
      snprintf(szLine2, sizeof(szLine2), "E:%d V:%d", iEnergy, iValue);

      iTx = stRectCard.x + 8;
      iTy = stRectCard.y + 8;

      vSDL_DrawText(pSDL_Renderer, szLine1, iTx + 1, iTy + 1, stBlack);
      vSDL_DrawText(pSDL_Renderer, szLine1, iTx, iTy, stWhite);

      vSDL_DrawText(pSDL_Renderer, szLine2, iTx + 1, iTy + 22, stBlack);
      vSDL_DrawText(pSDL_Renderer, szLine2, iTx, iTy + 21, stWhite);
    }
  }

  vTraceVarArgsFn("vSDL_DrawTable END OK");
}

void vSDL_TraceEv(SDL_Event *pSDL_Event)
{
  u_int ui32Type = pSDL_Event->type; // Uint32
  size_t ui64_Ct;
  char szDbg[128];
  if (ui32Type >= SDL_USEREVENT && ui32Type <= SDL_LASTEVENT)
  {
    if (ui32Type == SDL_USEREVENT)
    {
      snprintf(szDbg, sizeof(szDbg), "%s", "SDL_USEREVENT");
      vTraceVarArgsFn("Event=[%s] (0x%04X)", szDbg, (unsigned)ui32Type);
      return;
    }
    snprintf(szDbg, sizeof(szDbg), "SDL_USEREVENT+%u", (unsigned)(ui32Type - SDL_USEREVENT));
    vTraceVarArgsFn("Event=[%s] (0x%04X)", szDbg, (unsigned)ui32Type);
    return;
  }
  for (ui64_Ct = 0; ui64_Ct < sizeof(gEventNames) / sizeof(gEventNames[0]); ui64_Ct++)
  {
    if (gEventNames[ui64_Ct].type == ui32Type)
    {
      vTraceVarArgsFn("Event=[%s] (0x%04X)", gEventNames[ui64_Ct].name, (unsigned)ui32Type);
      return;
    }
  }
  vTraceVarArgsFn("Event=[%s] (0x%04X)", "SDL_UNKNOWN_EVENT", (unsigned)ui32Type);
  return;
}

void vSDL_DrawHUD(SDL_Renderer *pSDL_Renderer, PSTRUCT_PLAYER pstPlayer)
{
  SDL_Color white = {255, 255, 255, 255};
  char szHUD[128];
  sprintf(szHUD, "HP: %d  Energia: %d  Escudo: %d",
          pstPlayer->iHP, pstPlayer->iEnergy, pstPlayer->iBlock);

  vSDL_DrawText(pSDL_Renderer, szHUD, 60, 20, white);
}

void vSDL_MainInit()
{
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
  if (!gFont)
  {
    vTraceVarArgsFn("Erro ao carregar fonte: %s", TTF_GetError());
  }
  vTraceVarArgsFn("MainInit End OK");
}

void vSDL_MainLoop(int *pbRunning,
                   SDL_Event *pSDL_Event,
                   SDL_Renderer *pSDL_Renderer,
                   PSTRUCT_DECK pstDeck,
                   PSTRUCT_MONSTER pastMonsters,
                   int iMonsterCt)
{
  uint64_t ui64ElapsedTime;

  vTraceVarArgsFn("vSDL_MainLoop BEGIN");

  /* setup inicial igual console */
  gstPlayer.iEnergy = PLAYER_ENERGY_MAX;
  gstPlayer.iBlock  = 0;
  iDrawMultipleCard(INIT_HAND_CARDS, pstDeck);
  pstDeck->iHandCount = INIT_HAND_CARDS;

  while (*pbRunning)
  {
    /* === TURNO DO JOGADOR === */
    vSortHandByName(pstDeck);

    /* redesenha HUD + mesa */
    SDL_RenderClear(pSDL_Renderer);
    vSDL_DrawTable(pSDL_Renderer, pstDeck, pastMonsters, iMonsterCt);
    vSDL_DrawHUD(pSDL_Renderer, &gstPlayer);
    vSDL_DrawDialog(pSDL_Renderer, 60, 600, 640, 190 );

    SDL_RenderPresent(pSDL_Renderer);

    /* espera jogadas do jogador até acabar energia ou monstros */
    while (gstPlayer.iEnergy > 0 && iAnyMonsterAlive(pastMonsters, iMonsterCt) && *pbRunning)
    {
      while (SDL_PollEvent(pSDL_Event) && *pbRunning)
      {
        vSDL_TraceEv(pSDL_Event);
        switch (pSDL_Event->type)
        {
        case SDL_QUIT:
          *pbRunning = FALSE;
          break;
        case SDL_MOUSEBUTTONDOWN:
          if (pSDL_Event->button.button == SDL_BUTTON_LEFT)
          {
            int mx = pSDL_Event->button.x;
            int my = pSDL_Event->button.y;
            int j, m;

            if (!gSelectingTarget)
            {
              for (j = 0; j < gCardCount; j++)
              {
                if (mx >= gCardRects[j].x && mx <= gCardRects[j].x + gCardRects[j].w &&
                    my >= gCardRects[j].y && my <= gCardRects[j].y + gCardRects[j].h)
                {
                  gPendingCard = j;
                  /* checa se há mais de um monstro vivo */
                  int vivos = 0, ultimo = -1;
                  for (m = 0; m < iMonsterCt; m++)
                    if (pastMonsters[m].iHP > 0) { vivos++; ultimo = m; }

                  if (vivos <= 1)
                  {
                    gSDL_SelectedMonster = (ultimo >= 0) ? ultimo : 0;
                    vPlayCard(gPendingCard, pstDeck, pastMonsters, iMonsterCt);
                    gSDL_SelectedMonster = -1;
                    gPendingCard = -1;
                  }
                  else
                  {
                    gSelectingTarget = 1;
                    vTraceVarArgsFn("Selecione um monstro com o mouse...");
                  }
                  break;
                }
              }
            }
            else
            {
              for (m = 0; m < gMonsterCount; m++)
              {
                if (mx >= gMonsterRects[m].x && mx <= gMonsterRects[m].x + gMonsterRects[m].w &&
                    my >= gMonsterRects[m].y && my <= gMonsterRects[m].y + gMonsterRects[m].h)
                {
                  if (gPendingCard >= 0)
                  {
                    gSDL_SelectedMonster = m;
                    vPlayCard(gPendingCard, pstDeck, pastMonsters, iMonsterCt);
                    gSDL_SelectedMonster = -1;
                    gPendingCard = -1;
                    gSelectingTarget = 0;
                  }
                  break;
                }
              }
            }
          }
          break;
        }
      }

      /* redesenha continuamente */
      SDL_RenderClear(pSDL_Renderer);
      vSDL_DrawTable(pSDL_Renderer, pstDeck, pastMonsters, iMonsterCt);
      vSDL_DrawHUD(pSDL_Renderer, &gstPlayer);
      vSDL_DrawDialog(pSDL_Renderer, 60, 600, 640, 190);
      SDL_RenderPresent(pSDL_Renderer);
    }

    /* fim de turno do jogador */
    vDiscardHand(pstDeck);
    vDoEnemyActions(pastMonsters, iMonsterCt);

    /* checa derrota */
    if (gstPlayer.iHP <= 0)
    {
      vTraceVarArgsFn("*** Derrota! Jogador morreu ***");
      *pbRunning = FALSE;
      break;
    }

    /* checa vitória */
    if (!iAnyMonsterAlive(pastMonsters, iMonsterCt))
    {
      vTraceVarArgsFn("*** Nivel %d completo! ***", giLevel);

      vAddPlayerReward(&gstPlayer);
      vOpenShop(pstDeck);

      giLevel++;
      vInitMonstersForLevel(pastMonsters, giLevel, &iMonsterCt);

      /* reset para novo nível */
      iDrawMultipleCard(INIT_HAND_CARDS, pstDeck);
      gstPlayer.iEnergy = PLAYER_ENERGY_MAX;
      gstPlayer.iBlock = 0;
      continue;
    }

    /* próximo turno no mesmo nível */
    iDrawMultipleCard(INIT_HAND_CARDS, pstDeck);
    gstPlayer.iEnergy = PLAYER_ENERGY_MAX;

    /* VSync */
    ui64ElapsedTime = SDL_GetTicks64();
    ui64ElapsedTime = SDL_GetTicks64() - ui64ElapsedTime;
    if (ui64ElapsedTime <= VSYNC_TIME)
      SDL_Delay(VSYNC_TIME - ui64ElapsedTime);
  }

  vTraceVarArgsFn("vSDL_MainLoop END OK");
}


void vSDL_MainQuit(void)
{
  vTraceVarArgsFn("MainQuit Begin");

  if (gFont)
  {
    TTF_CloseFont(gFont);
    gFont = NULL;
  }

  TTF_Quit();
  SDL_Quit();

  vTraceVarArgsFn("MainQuit End OK");
}

#endif
