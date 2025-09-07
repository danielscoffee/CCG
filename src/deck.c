#include <deck.h>
#include <stdlib.h> /* rand, srand */
#include <string.h>
#include <terminal_utils.h>

char *pszCardTypeDesc[] = {"Dano",  "Defesa", "Cura",
                           "Magia", "Veneno", /* <<< NOVO */
                           NULL};

void vInitBasicDeck(PSTRUCT_DECK pstDeck) {
  int i;
  pstDeck->iDrawCount = 0;
  pstDeck->iDiscardCount = 0;
  pstDeck->iHandCount = 0;

  /* 20 cartas: 8 Strike, 6 Defend, 4 Heal, 2 Fireball */
  for (i = 0; i < 8; i++)
    pstDeck->aDraw[pstDeck->iDrawCount++] =
        stMakeCard(CARD_STRIKE, "Strike", 1, 6);
  for (i = 0; i < 6; i++)
    pstDeck->aDraw[pstDeck->iDrawCount++] =
        stMakeCard(CARD_DEFEND, "Defend", 2, 5);
  for (i = 0; i < 4; i++)
    pstDeck->aDraw[pstDeck->iDrawCount++] = stMakeCard(CARD_HEAL, "Heal", 1, 4);
  for (i = 0; i < 2; i++)
    pstDeck->aDraw[pstDeck->iDrawCount++] =
        stMakeCard(CARD_FIREBALL, "Fireball", 1, 4);

  vShuffle(pstDeck->aDraw, pstDeck->iDrawCount);
  vTraceVarArgsFn("Deck inicial embaralhado com %d cartas.",
                  pstDeck->iDrawCount);
}

/* descartar uma carta específica da mão (índice 0..iHandCount-1) */
void vDiscardCard(PSTRUCT_DECK pstDeck, int iIndex) {
  int i;
  if (iIndex < 0 || iIndex >= pstDeck->iHandCount)
    return;
  pstDeck->aDiscard[pstDeck->iDiscardCount++] = pstDeck->aHand[iIndex];
  for (i = iIndex; i < pstDeck->iHandCount - 1; i++)
    pstDeck->aHand[i] = pstDeck->aHand[i + 1];
  pstDeck->iHandCount--;
}

void vAddCardToDiscard(PSTRUCT_DECK pstDeck, STRUCT_CARD stCard) {
  if (pstDeck->iDiscardCount < MAX_DECK)
    pstDeck->aDiscard[pstDeck->iDiscardCount++] = stCard;
}

/* procura por nome nas zonas (mao -> draw -> discard) e aplica upgrade */
int iUpgradeFirstCardByName(PSTRUCT_DECK pstDeck, const char *kpszName,
                            int iDeltaValue, int iDeltaCost) {
  int i;
  /* mao */
  for (i = 0; i < pstDeck->iHandCount; i++) {
    if (strcmp(pstDeck->aHand[i].szName, kpszName) == 0) {
      pstDeck->aHand[i].iValue += iDeltaValue;
      pstDeck->aHand[i].iCost += iDeltaCost;
      if (pstDeck->aHand[i].iCost < 0)
        pstDeck->aHand[i].iCost = 0;
      if (strchr(pstDeck->aHand[i].szName, '+') == NULL)
        strcat(pstDeck->aHand[i].szName, "+");
      return 1;
    }
  }
  /* draw */
  for (i = 0; i < pstDeck->iDrawCount; i++) {
    if (strcmp(pstDeck->aDraw[i].szName, kpszName) == 0) {
      pstDeck->aDraw[i].iValue += iDeltaValue;
      pstDeck->aDraw[i].iCost += iDeltaCost;
      if (pstDeck->aDraw[i].iCost < 0)
        pstDeck->aDraw[i].iCost = 0;
      if (strchr(pstDeck->aDraw[i].szName, '+') == NULL)
        strcat(pstDeck->aDraw[i].szName, "+");
      return 1;
    }
  }
  /* discard */
  for (i = 0; i < pstDeck->iDiscardCount; i++) {
    if (strcmp(pstDeck->aDiscard[i].szName, kpszName) == 0) {
      pstDeck->aDiscard[i].iValue += iDeltaValue;
      pstDeck->aDiscard[i].iCost += iDeltaCost;
      if (pstDeck->aDiscard[i].iCost < 0)
        pstDeck->aDiscard[i].iCost = 0;
      if (strchr(pstDeck->aDiscard[i].szName, '+') == NULL)
        strcat(pstDeck->aDiscard[i].szName, "+");
      return 1;
    }
  }
  return 0;
}

/* ---------------- Funções utilitárias ---------------- */
void vSwapCards(PSTRUCT_CARD pstA, PSTRUCT_CARD pstB) {
  STRUCT_CARD stTmp;
  stTmp = *pstA;
  *pstA = *pstB;
  *pstB = stTmp;
}

/**
 * @brief Ordena as cartas na mão por nome (ordem alfabética)
 */
void vSortHandByName(PSTRUCT_DECK pstDeck) {
  int i, j;
  for (i = 0; i < pstDeck->iHandCount - 1; i++) {
    for (j = i + 1; j < pstDeck->iHandCount; j++) {
      if (strcmp(pstDeck->aHand[i].szName, pstDeck->aHand[j].szName) > 0) {
        vSwapCards(&pstDeck->aHand[i], &pstDeck->aHand[j]);
      }
    }
  }
  vTraceVarArgsFn("Mao ordenada alfabeticamente (%d cartas).",
                  pstDeck->iHandCount);
}
void vShuffle(PSTRUCT_CARD paCards, int iCount) {
  int i, j;
  for (i = iCount - 1; i > 0; i--) {
    j = rand() % (i + 1);
    vSwapCards(&paCards[i], &paCards[j]);
  }
}

int iDrawMultipleCard(int iCardCt, PSTRUCT_DECK pstDeck) {
  int ii = 0;
  for (ii = 0; ii < iCardCt; ii++) {
    iDrawCard(pstDeck);
  }

  return ii;
}

int iDrawCard(PSTRUCT_DECK pstDeck) {
  int i;
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
    /* Recicla descarte */
    for (i = 0; i < pstDeck->iDiscardCount; i++) {
      pstDeck->aDraw[i] = pstDeck->aDiscard[i];
    }
    pstDeck->iDrawCount = pstDeck->iDiscardCount;
    pstDeck->iDiscardCount = 0;
    vShuffle(pstDeck->aDraw, pstDeck->iDrawCount);
    vTraceVarArgsFn("Recycle: descarte movido para draw (%d cartas).",
                    pstDeck->iDrawCount);
  }
  /* Compra a carta do topo */
  stCard = pstDeck->aDraw[pstDeck->iDrawCount - 1];
  pstDeck->iDrawCount--;
  pstDeck->aHand[pstDeck->iHandCount++] = stCard;
  vTraceVarArgsFn("Comprou: %s", stCard.szName);
  return 1;
}
void vAddDiscardPile2Deck(PSTRUCT_DECK pstDeck) {
  int ii;
  for (ii = pstDeck->iDiscardCount; ii >= 0; ii--) {
    pstDeck->aDraw[ii] = pstDeck->aDiscard[pstDeck->iDiscardCount--];
  }
  memset(pstDeck->aDiscard, 0, MAX_DECK * sizeof(int));
  pstDeck->iDiscardCount = 0;
}
void vDiscardHand(PSTRUCT_DECK pstDeck) {
  int i;
  for (i = 0; i < pstDeck->iHandCount; i++) {
    pstDeck->aDiscard[pstDeck->iDiscardCount++] = pstDeck->aHand[i];
  }
  pstDeck->iHandCount = 0;
}
// void vDiscardCard(PSTRUCT_DECK pstDeck, int iCardIx)
// {
//   int ii;
//   int jj;
//   STRUCT_DECK stWrkDeck;

//   if ( iCardIx < 0 || pstDeck->aHand[iCardIx].iType < CARD_STRIKE )
//     return;

//   memset(&stWrkDeck, 0, sizeof(stWrkDeck));
//   for ( ii = 0, jj = 0; jj < pstDeck->iHandCount; ii++, jj++){
//     if ( ii == iCardIx ) jj++;

//     stWrkDeck.aHand[ii] = pstDeck->aHand[jj];
//   }
//   pstDeck->aDiscard[pstDeck->iDiscardCount++] = pstDeck->aHand[iCardIx];
//   memset(pstDeck->aHand, 0, pstDeck->iHandCount*(sizeof(STRUCT_CARD)));
//   memcpy(pstDeck->aHand, &stWrkDeck.aHand,
//   pstDeck->iHandCount*(sizeof(STRUCT_CARD))); pstDeck->iHandCount--;
// }
void vShowDeck(PSTRUCT_DECK pstDeck) {
  int i;
  char szLine[256];

  for (i = 0; i < pstDeck->iHandCount; i++) {
    int iLineCt = i + 1;
    int iPadding = (iLineCt % MAX_HAND) ? 1 : 0;
    memset(szLine, 0, sizeof(szLine));
    snprintf(szLine, sizeof(szLine), "  [%d]%*.*s%-*.*s (custo:%d, %s:%d)",
             iLineCt, iPadding, iPadding, " ", MAX_HAND, MAX_HAND,
             pstDeck->aHand[i].szName, pstDeck->aHand[i].iCost,
             pszCardTypeDesc[pstDeck->aHand[i].iType],
             pstDeck->aHand[i].iValue);
    vPrintLine(szLine, INSERT_NEW_LINE);
  }
}

STRUCT_CARD stMakeCard(int iType, const char *pszName, int iCost, int iValue) {
  STRUCT_CARD st;
  st.iType = iType;
  st.iCost = iCost;
  st.iValue = iValue;
  strncpy(st.szName, pszName, sizeof(st.szName) - 1);
  st.szName[sizeof(st.szName) - 1] = '\0';
  return st;
}
