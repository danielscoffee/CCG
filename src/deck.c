#include <deck.h>
#include <string.h>  
#include <stdlib.h>  
#include <terminal_utils.h>

char *pszCardTypeDesc[] ={
  "Strike",
  "Block",
  "Heal",
  "Spell",
  "Veneno",
  NULL
};

void vLogCardList(STRUCT_CARD astCardList[], int iListCt){
  int ii;

  if ( iListCt <= 0 )
    return ;

  for (ii = 0; ii < iListCt; ii++){
    vTraceVarArgsFn(
  "\t|-> Card=%s EnergyCost=%d Value=%d",
      pszCardTypeDesc[astCardList[ii].iType],
      astCardList[ii].iCost,
      astCardList[ii].iValue
    );
  }

}

void vLogDeck(PSTRUCT_DECK pstDeck, int iTraceLevel){

  vTraceVarArgsFn("%d cartas na pilha de compra:", pstDeck->iDrawCount);
  if ( iTraceLevel & TRACE_DRAW_PILE )
    vLogCardList(pstDeck->astDraw, pstDeck->iDrawCount);

  vTraceVarArgsFn("%d cartas na mão do jogador:", pstDeck->iHandCount);
  if ( iTraceLevel & TRACE_HAND )
    vLogCardList(pstDeck->astHand, pstDeck->iHandCount);

  vTraceVarArgsFn("%d cartas na pilha de descarte:", pstDeck->iDiscardCount);
  if ( iTraceLevel & TRACE_DISCARD_PILE )
    vLogCardList(pstDeck->astDiscard, pstDeck->iDiscardCount);

}

void vInitBasicDeck(PSTRUCT_DECK pstDeck)
{
  int ii;
  pstDeck->iDrawCount = 0;
  pstDeck->iDiscardCount = 0;
  pstDeck->iHandCount = 0;

  
  memset(pstDeck->astDiscard, 0, MAX_DECK*sizeof(STRUCT_CARD));
  memset(pstDeck->astDraw,    0, MAX_DECK*sizeof(STRUCT_CARD));
  memset(pstDeck->astHand,    0, MAX_HAND*sizeof(STRUCT_CARD));

  /* 20 cartas: 8 Strike, 6 Defend, 4 Heal, 2 Fireball */
  for (ii = 0; ii < 8; ii++)
    pstDeck->astDraw[pstDeck->iDrawCount++] = stMakeCard(CARD_STRIKE, "Strike", 1, 6, CARD_TARGET_SINGLE);
  for (ii = 0; ii < 6; ii++)
    pstDeck->astDraw[pstDeck->iDrawCount++] = stMakeCard(CARD_DEFEND, "Defend", 2, 5, CARD_TARGET_SINGLE);
  for (ii = 0; ii < 4; ii++)
    pstDeck->astDraw[pstDeck->iDrawCount++] = stMakeCard(CARD_HEAL, "Heal", 1, 4, CARD_TARGET_SINGLE);
  for (ii = 0; ii < 2; ii++)
    pstDeck->astDraw[pstDeck->iDrawCount++] = stMakeCard(CARD_FIREBALL, "Fireball", 1, 4, CARD_TARGET_MULTIPLE);
  
  vShuffle(pstDeck->astDraw, pstDeck->iDrawCount);
  vTraceVarArgsFn("Deck inicial embaralhado com %d cartas.", pstDeck->iDrawCount);
  vLogDeck(pstDeck, TRACE_DECK_ALL);
}

/* descartar uma carta específica da mão (índice 0..iHandCount-1) */
void vDiscardCard(PSTRUCT_DECK pstDeck, int iIndex)
{
  int i;
  if (iIndex < 0 || iIndex >= pstDeck->iHandCount) return;

  pstDeck->astDiscard[pstDeck->iDiscardCount++] = pstDeck->astHand[iIndex];
  memset(&pstDeck->astHand[iIndex], 0, sizeof(STRUCT_CARD));
  for (i = iIndex; i < pstDeck->iHandCount - 1; i++)
    pstDeck->astHand[i] = pstDeck->astHand[i + 1];

  pstDeck->iHandCount--;
}

void vAddCardToDiscard(PSTRUCT_DECK pstDeck, STRUCT_CARD stCard)
{
  if (pstDeck->iDiscardCount < MAX_DECK)
    pstDeck->astDiscard[pstDeck->iDiscardCount++] = stCard;
}

/* procura por nome nas zonas (mao -> draw -> discard) e aplica upgrade */
int iUpgradeFirstCardByName(PSTRUCT_DECK pstDeck, const char *kpszName, int iDeltaValue, int iDeltaCost)
{
  int i;
  /* mao */
  for (i = 0; i < pstDeck->iHandCount; i++) {
    if (strcmp(pstDeck->astHand[i].szName, kpszName) == 0) {
      pstDeck->astHand[i].iValue += iDeltaValue;
      pstDeck->astHand[i].iCost += iDeltaCost;
      if (pstDeck->astHand[i].iCost < 0) pstDeck->astHand[i].iCost = 0;
      if (strchr(pstDeck->astHand[i].szName, '+') == NULL) strcat(pstDeck->astHand[i].szName, "+");
      return 1;
    }
  }
  /* draw */
  for (i = 0; i < pstDeck->iDrawCount; i++) {
    if (strcmp(pstDeck->astDraw[i].szName, kpszName) == 0) {
      pstDeck->astDraw[i].iValue += iDeltaValue;
      pstDeck->astDraw[i].iCost += iDeltaCost;
      if (pstDeck->astDraw[i].iCost < 0) pstDeck->astDraw[i].iCost = 0;
      if (strchr(pstDeck->astDraw[i].szName, '+') == NULL) strcat(pstDeck->astDraw[i].szName, "+");
      return 1;
    }
  }
  /* discard */
  for (i = 0; i < pstDeck->iDiscardCount; i++) {
    if (strcmp(pstDeck->astDiscard[i].szName, kpszName) == 0) {
      pstDeck->astDiscard[i].iValue += iDeltaValue;
      pstDeck->astDiscard[i].iCost += iDeltaCost;
      if (pstDeck->astDiscard[i].iCost < 0) pstDeck->astDiscard[i].iCost = 0;
      if (strchr(pstDeck->astDiscard[i].szName, '+') == NULL) strcat(pstDeck->astDiscard[i].szName, "+");
      return 1;
    }
  }
  return 0;
}

/* ---------------- Funções utilitárias ---------------- */
void vSwapCards(PSTRUCT_CARD pstA, PSTRUCT_CARD pstB)
{
  STRUCT_CARD stTmp;
  stTmp = *pstA;
  *pstA = *pstB;
  *pstB = stTmp;
}

/**
 * @brief Ordena as cartas na mão por nome (ordem alfabética)
 */
void vSortHandByName(PSTRUCT_DECK pstDeck) {
  int ii, j;
  for (ii = 0; ii < pstDeck->iHandCount - 1; ii++) {
    for (j = ii + 1; j < pstDeck->iHandCount; j++) {
      if (strcmp(pstDeck->astHand[ii].szName, pstDeck->astHand[j].szName) > 0) {
        vSwapCards(&pstDeck->astHand[ii], &pstDeck->astHand[j]);
      }
    }
  }
  vTraceVarArgsFn("Mao ordenada alfabeticamente (%d cartas).", pstDeck->iHandCount);
}

void vShuffle(PSTRUCT_CARD paCards, int iCount) {
  int ii;
  int jj;

  for (ii = iCount - 1; ii > 0; ii--) {
    jj = rand() % (ii + 1);
    vSwapCards(&paCards[ii], &paCards[jj]);
  }
}

int iDrawMultipleCard(int iCardCt, PSTRUCT_DECK pstDeck)
{
  int ii = 0;
  for (ii = 0; ii < iCardCt; ii++){
    iDrawCard(pstDeck);
  }

  return ii;
}

int iDrawCard(PSTRUCT_DECK pstDeck)
{
  int ii;
  STRUCT_CARD stCard;
  if (pstDeck->iHandCount >= MAX_HAND) {
    vTraceVarArgsFn("Mao cheia, nao pode comprar.");
    return 0;
  }
  if (pstDeck->iDrawCount <= 0) {
    if (pstDeck->iDiscardCount <= 0) {
      vTraceVarArgsFn("Nao ha cartas para comprar.");
      return 0;
    }
    pstDeck->iDrawCount    = 0;
    for (ii = 0; ii < pstDeck->iDiscardCount; ii++) {
      vShuffle(pstDeck->astDiscard, pstDeck->iDiscardCount);
      pstDeck->astDraw[ii] = pstDeck->astDiscard[ii];
      pstDeck->iDrawCount++;
    }
    pstDeck->iDiscardCount = 0;
    memset(pstDeck->astDiscard, 0, MAX_DECK*sizeof(STRUCT_CARD));
    vShuffle(pstDeck->astDraw, pstDeck->iDrawCount);
    vTraceVarArgsFn("Recycle: descarte movido para draw (%d cartas).", pstDeck->iDrawCount);
  }
  stCard = pstDeck->astDraw[pstDeck->iDrawCount - 1];
  pstDeck->iDrawCount--;
  pstDeck->astHand[pstDeck->iHandCount++] = stCard;
  vTraceVarArgsFn("Comprou: %s", stCard.szName);
  return 1;
}
void vAddDiscardPile2Deck(PSTRUCT_DECK pstDeck){
  int ii;
  for (ii = pstDeck->iDiscardCount; ii >= 0; ii--)  {
    pstDeck->astDraw[ii] = pstDeck->astDiscard[pstDeck->iDiscardCount--];
  }
  memset(pstDeck->astDiscard, 0, MAX_DECK*sizeof(STRUCT_CARD));
  pstDeck->iDiscardCount = 0;
}
void vDiscardHand(PSTRUCT_DECK pstDeck)
{
  int ii;
  for (ii = 0; ii < pstDeck->iHandCount; ii++)
    pstDeck->astDiscard[pstDeck->iDiscardCount++] = pstDeck->astHand[ii];
  
  memset(pstDeck->astHand, 0, MAX_HAND*sizeof(STRUCT_CARD));
  pstDeck->iHandCount = 0;
}

void vShowDeck(PSTRUCT_DECK pstDeck) {
  int ii;
  char szLine[1024];

  for (ii = 0; ii < pstDeck->iHandCount; ii++)
  {
    int iLineCt = ii+1;
    int iPadding = (iLineCt % MAX_HAND) ?  1: 0; 
    memset(szLine,0, sizeof(szLine));
    snprintf(szLine, sizeof(szLine), "  [%d]%*.*s%-*.*s (custo:%d, %s:%d)",
             iLineCt, iPadding, iPadding, " ", 
             MAX_HAND, MAX_HAND, pstDeck->astHand[ii].szName,
             pstDeck->astHand[ii].iCost,
             pszCardTypeDesc[pstDeck->astHand[ii].iType],
             pstDeck->astHand[ii].iValue
    );
    vPrintLine(szLine, INSERT_NEW_LINE);
  }
} 

STRUCT_CARD stMakeCard(int iType, const char *pszName, int iCost, int iValue, int iTarget) {
  STRUCT_CARD st;
  st.iType = iType;
  st.iCost = iCost;
  st.iValue = iValue;
  st.iTarget = iTarget;
  strncpy(st.szName, pszName, sizeof(st.szName) - 1);
  st.szName[sizeof(st.szName) - 1] = '\0';
  return st;
}