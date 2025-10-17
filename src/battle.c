#include <debuff.h>
#include <deck.h>
#include <monster.h>
#include <player.h>
#include <terminal_utils.h>
#include <trace.h>
#include <input.h>
#include <battle.h>

#ifdef USE_SDL2
/* Recebe índice escolhido pela UI SDL; -1 = não definido */
  extern int gSDL_SelectedMonster;
#endif

int bHasAnyPlayableCard(PSTRUCT_DECK pstDeck){
  PSTRUCT_CARD pstCard;
  int iWrkCardIx;
  int ii;
  
  /* No SDL já recebemos índice zero-based, no console era 1-based */
  iWrkCardIx = pstDeck->iHandCount-1;

  for ( ii = 0 ; ii < iWrkCardIx ; ii++ ){
    pstCard = &pstDeck->astHand[ii];
    if ( pstCard->iCost <= gstPlayer.iEnergy )
      return TRUE;
  }
  return FALSE;
}

int iSelectMonsterFromList(int iMonsterCt){
#ifndef USE_SDL2
  int iChoice;
  char szLine[1024];
  
  sprintf(szLine, "\nEscolha um monstro (1..%d). 'q' para sair.", iMonsterCt);
  vPrintLine(szLine, INSERT_NEW_LINE);
  iChoice = iPortableGetchar();
  return (iChoice - 48); /* converte de char p/ int */
#else
/* No SDL, o alvo é definido pelo clique */
  if (gSDL_SelectedMonster >= 0 && gSDL_SelectedMonster < iMonsterCt) {
    return gSDL_SelectedMonster;
  }
  return 0; /* fallback: primeiro monstro vivo */
  
#endif
}

/**
 * Imprime a mesa de jogo (somente no console).
 */
void vShowTable(PSTRUCT_DECK pstDeck, PSTRUCT_MONSTER pastMonsters, int iMonsterCount){
#ifndef USE_SDL2
  vShowPlayer();
  vShowDeck(pstDeck);
  vShowMonsters(pastMonsters, iMonsterCount);
#else
  /* no SDL a mesa é desenhada em sdl_api.c */
  (void)pstDeck;
  (void)pastMonsters;
  (void)iMonsterCount;
#endif
}

int iHandlePlayerActionByCard(PSTRUCT_CARD pstCard, PSTRUCT_MONSTER pastMonsters, int iMonsterCt){
  int iTarget = 0;
  int ii = 0;
  int bAlreadyApplied = FALSE;
  int bUsed = FALSE;

  vTraceVarArgsFn("Card Type=%d", pstCard->iType);

  switch ( pstCard->iType ){
    case CARD_STRIKE:
    case CARD_FIREBALL:
    case CARD_POISON:
    case CARD_PARALIZE:
    {
      int iRoundsParalized = 0;
      char* pszRoundStr = "rodadas";
      time_t lTime;

      if ( iAliveMonsterQty(pastMonsters, iMonsterCt) == 1 ){
        iTarget = iGetFirstAliveMonster(pastMonsters, iMonsterCt);
      }
      else if ( pstCard->iTarget != CARD_TARGET_MULTIPLE && iMonsterCt > 1 ){
        #ifdef USE_SDL2
          /* SDL: usa o monstro clicado pelo jogador */
          if (gbSDL_Mode) {
            iTarget = gSDL_SelectedMonster;
            vTraceVarArgsFn("SDL Target [%d] %s", iTarget, pastMonsters[iTarget].szName);
          }
          else{
            iTarget = iSelectMonsterFromList(iMonsterCt);    
            iTarget--; /* console: ajusta para base 0 */
          }
        #else
          /* modo console: pedir ao jogador */
          iTarget = iSelectMonsterFromList(iMonsterCt);
          iTarget--; /* console: ajusta para base 0 */
        #endif  

        if ( iTarget < 0 || iTarget >= iMonsterCt )
          return -1;
      }
      if ( pstCard->iTarget == CARD_TARGET_MULTIPLE ){
        bAlreadyApplied = FALSE;
        iTarget = iMonsterCt;
      }

      for ( ; ii <= iTarget; ii++ ) {
        char szLine[1024];
        if ( iTarget < iMonsterCt ) ii = iTarget;
        if ( pstCard->iTarget == CARD_TARGET_MULTIPLE && ii == iTarget ) break;
        if ( pastMonsters[ii].iHP <= 0 ) continue;

        if ( pstCard->iType == CARD_PARALIZE ){
          time(&lTime);
          srand(lTime);
          iRoundsParalized = (rand() % 2) + 2;
          pastMonsters[ii].astDebuff[pastMonsters[ii].iDebuffCt].iType   = DEBUFF_TYPE_PARALYZE;
          pastMonsters[ii].astDebuff[pastMonsters[ii].iDebuffCt].iDamage = pstCard->iValue;
          pastMonsters[ii].astDebuff[pastMonsters[ii].iDebuffCt].iRounds = iRoundsParalized;
          pastMonsters[ii].iDebuffCt++;
          snprintf(szLine, sizeof(szLine),
            "Voce aplicou paralisia ao monstro %s por %d %s",
            pastMonsters[ii].szName,
            iRoundsParalized,
            pszRoundStr
          );
          vPrintLine(szLine, INSERT_NEW_LINE);
          bUsed = TRUE;
        }
        else if ( pstCard->iType == CARD_POISON ) {
          pastMonsters[ii].astDebuff[pastMonsters[ii].iDebuffCt].iType   = DEBUFF_TYPE_POISON;
          pastMonsters[ii].astDebuff[pastMonsters[ii].iDebuffCt].iDamage = pstCard->iValue;
          pastMonsters[ii].astDebuff[pastMonsters[ii].iDebuffCt].iRounds = DEBUFF_POISON_CYCS;
          pastMonsters[ii].iDebuffCt++;
          snprintf(szLine, sizeof(szLine),
            "Voce aplicou veneno ao monstro %s por %d %s",
            pastMonsters[ii].szName,
            DEBUFF_POISON_CYCS,
            pszRoundStr
          );
          vPrintLine(szLine, INSERT_NEW_LINE);
          bUsed = TRUE;
        }
        else {
          pastMonsters[ii].iBlock  -= pstCard->iValue;
          if (pastMonsters[ii].iBlock < 0) {
            pastMonsters[ii].iHP   -= (pastMonsters[ii].iBlock)*-1;
            pastMonsters[ii].iBlock = 0;
          }
          else {
            pastMonsters[ii].iHP   -= pstCard->iValue;
          }

          sprintf(szLine, "Voce utilizou %s:", pstCard->szName);
          if ( !bAlreadyApplied )
            vPrintLine(szLine, INSERT_NEW_LINE);
          if ( pstCard->iTarget == CARD_TARGET_MULTIPLE  ){
            bAlreadyApplied = TRUE;
          }
          sprintf(szLine, " Causou %d dano a %s", pstCard->iValue, pastMonsters[ii].szName);
          vPrintLine(szLine, INSERT_NEW_LINE);
          bUsed = TRUE;
          if ( iTarget < iMonsterCt ) ii++;
        }
      }
    }
    break;
    case CARD_DEFEND:
      gstPlayer.iBlock += pstCard->iValue;
      vPrintLine("Voce se defendeu.", INSERT_NEW_LINE);
      bUsed = TRUE;
      break;
    case CARD_HEAL:
      gstPlayer.iHP    += pstCard->iValue;
      if ( gstPlayer.iHP >= PLAYER_HP_MAX )
        gstPlayer.iHP = PLAYER_HP_MAX;
      vPrintLine("Voce se curou.", INSERT_NEW_LINE);
      bUsed = TRUE;
      break;
    default:
      return CARD_NONE;
  }

  if ( !bUsed ) return CARD_NONE; 
  return pstCard->iType;
}

void vPlayCard(int iCardIndex, PSTRUCT_DECK pstDeck, PSTRUCT_MONSTER pastMonsters, int iMonsterCount){
  PSTRUCT_CARD pstCard;
  int iWrkCardIx;
  
  /* No SDL já recebemos índice zero-based, no console era 1-based */
  
  iWrkCardIx = iCardIndex - 1;
  #ifdef USE_SDL2
    if ( gbSDL_Mode )
      iWrkCardIx = iCardIndex;
  #endif
 
  if (iWrkCardIx < 0 || iWrkCardIx >= pstDeck->iHandCount)
    return;

  pstCard = &pstDeck->astHand[iWrkCardIx];

  if (gstPlayer.iEnergy <= 0 || pstCard->iCost > gstPlayer.iEnergy ) {
    char szMsg[128];
    sprintf(szMsg, "Energia insuficiente [%d/%d]", pstCard->iCost, gstPlayer.iEnergy);
    vPrintHighlitedLine(szMsg, INSERT_NEW_LINE);
    return;
  }

  vPrintHighlitedLine("Carta Escolhida: ", NO_NEW_LINE);
  vPrintHighlitedLine(pstCard->szName, INSERT_NEW_LINE);

  if ( iHandlePlayerActionByCard(pstCard, pastMonsters, iMonsterCount) == CARD_NONE ) 
    return;

  gstPlayer.iEnergy -= pstCard->iCost;
  vDiscardCard(pstDeck, iWrkCardIx);

  vTraceVarArgsFn("Energia restante=%d", gstPlayer.iEnergy);
}