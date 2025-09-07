#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <battle.h>
#include <deck.h>
#include <monster.h>
#include <input.h>
#include <player.h>
#include <terminal_utils.h>

void vPlayCard(int iCardIndex, PSTRUCT_DECK pstDeck, PSTRUCT_MONSTER paMonsters, int iMonsterCount)
{
  PSTRUCT_CARD pstCard;
  int i;

  if (iCardIndex < 0 || iCardIndex > pstDeck->iHandCount)
    return;

  pstCard = &pstDeck->aHand[iCardIndex - 1];

  if (gstPlayer.iEnergy <= 0 || pstCard->iCost > gstPlayer.iEnergy ) {
    vTraceVarArgsFn("Not enough energy to play card[%s]", pstCard->szName);
    return;
  }
  
  if (strcmp(pstCard->szName, "Strike") == 0) {
    int iTarget = 0;    
    vTraceVarArgsFn("Player %s utilizou Strike dmg=[%d]", gstPlayer.szPlayerName);
    
    while (iTarget < iMonsterCount && paMonsters[iTarget].iHP <= 0) iTarget++;

    if (iTarget < iMonsterCount) {
      if (paMonsters[iTarget].iBlock > 0) {
        paMonsters[iTarget].iBlock -= pstCard->iValue;
      }
      if (paMonsters[iTarget].iBlock < 0) {
        paMonsters[iTarget].iHP -= (-paMonsters[iTarget].iBlock);
        paMonsters[iTarget].iBlock = 0;
      } else {
        paMonsters[iTarget].iHP -= pstCard->iValue;
      }
      vPrintLine("Voce usou Strike!", INSERT_NEW_LINE);
    }
  } else if (strcmp(pstCard->szName, "Defend") == 0) {
    gstPlayer.iBlock += pstCard->iValue;
    vPrintLine("Voce se defendeu.", INSERT_NEW_LINE);
  } else if (strcmp(pstCard->szName, "Heal") == 0) {
    gstPlayer.iHP += pstCard->iValue;
    if (gstPlayer.iHP > PLAYER_HP_MAX) gstPlayer.iHP = PLAYER_HP_MAX;
    vPrintLine("Voce se curou!", INSERT_NEW_LINE);
  } else if (strcmp(pstCard->szName, "Fireball") == 0) { /* <<< NOVO */
    vTraceVarArgsFn("Player %s conjurou Fireball (AOE)", gstPlayer.szPlayerName);
    for (i = 0; i < iMonsterCount; i++) {
      int iDmg;
      if (paMonsters[i].iHP <= 0) continue;
      iDmg = pstCard->iValue;
      if (paMonsters[i].iBlock > 0) {
        paMonsters[i].iBlock -= iDmg;
        if (paMonsters[i].iBlock < 0) {
          paMonsters[i].iHP -= (-paMonsters[i].iBlock);
          paMonsters[i].iBlock = 0;
        }
      } else {
        paMonsters[i].iHP -= iDmg;
      }
    }
    vPrintLine("Bola de Fogo atinge todos os monstros!", INSERT_NEW_LINE);
  } else if (strcmp(pstCard->szName, "Poison") == 0 || pstCard->iType == CARD_POISON) {
      int iTarget;
      iTarget = 0;
      while (iTarget < iMonsterCount && paMonsters[iTarget].iHP <= 0) iTarget++;
      if (iTarget < iMonsterCount) {
        //Aplicar veneno
        int iPoisonHitDamage = 5;
        int iPoisonRounds =    5;

        paMonsters[iTarget].stDebuff->iType = DEBUFF_TYPE_POISON;
        paMonsters[iTarget].stDebuff->iDamage = iPoisonHitDamage;
        paMonsters[iTarget].stDebuff->iRounds = iPoisonRounds;
        vPrintLine("Voce aplicou Veneno!", INSERT_NEW_LINE);
        vSleepSeconds(1);
      }
  }

  vSleepSeconds(1);
  vDiscardCard(pstDeck, (iCardIndex - 1));

  gstPlayer.iEnergy -= pstCard->iCost;
  vTraceVarArgsFn("Energia restante=%d (comprou 1 carta apos jogar)", gstPlayer.iEnergy);
}
