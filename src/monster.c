#include <stdio.h>
#include <stdlib.h>
#include <monster.h>
#include <player.h>
#include <input.h>
#include <terminal_utils.h>

int giMaxMonsterHP = MONSTER_INITAL_HP_MAX;

void vInitMonsters(PSTRUCT_MONSTER paMonsters, int iCount) {
  int i;
  for (i = 0; i < iCount; i++) {
    snprintf(paMonsters[i].szName, sizeof(paMonsters[i].szName),
             "Monstro %d", i+1);
    paMonsters[i].iHP = 20 + (i * 5);
    paMonsters[i].iBlock = 0;
    paMonsters[i].iAttack = 3 + i;
  }
  vTraceVarArgsFn("Iniciados %d monstros.", iCount);
}

void vShowMonsters(PSTRUCT_MONSTER paMonsters, int iCount) {
  int i;
  char szLine[1024];
  for (i = 0; i < iCount; i++) {
    if (paMonsters[i].iHP > 0) {
      memset(szLine, 0, sizeof(szLine));
      snprintf(szLine, sizeof(szLine),
               "  [%d] %s ",
               i+1, paMonsters[i].szName);
      vPrintColored(szLine, TERMINAL_COLOR_BBLUE);
      vPrintLine(" (", NO_NEW_LINE);
      snprintf(szLine, sizeof(szLine),
          "HP: %d/%d",
            paMonsters[i].iHP, giMaxMonsterHP);
      vPrintColored(szLine, TERMINAL_COLOR_BRED);
      
      vPrintLine(" | ", NO_NEW_LINE);
      snprintf(szLine, sizeof(szLine),
          "BLOCK: %d",
            paMonsters[i].iBlock);
      vPrintColored(szLine, TERMINAL_COLOR_BCYAN);
      vPrintLine(" | ", NO_NEW_LINE);
      snprintf(szLine, sizeof(szLine),
          "ATK: %d",
            paMonsters[i].iAttack);
      vPrintColored(szLine, TERMINAL_COLOR_BMAGENTA);
      vPrintLine(")", INSERT_NEW_LINE);
      if ( paMonsters[i].iDebuffCycCt > 0 ){

        vPrintLine(" | ", NO_NEW_LINE);
        snprintf(szLine, sizeof(szLine),
                " PoisonDmg=%d  Remaining=%d",
                paMonsters[i].iDebuff, paMonsters[i].iDebuffCycCt);
        vPrintColored(szLine, TERMINAL_COLOR_GREEN);
        vPrintLine(" | ", NO_NEW_LINE);

      }
    }
  }
}


void vDoEnemyActions(PSTRUCT_MONSTER paMonsters, int iMonsterCount) {
  int ii;
  char szLine[128];
  time_t lTime;
  
  time(&lTime);
  srand(lTime);

  for (ii = 0; ii < iMonsterCount; ii++) {
    int iChoice;
    if (paMonsters[ii].iHP <= 0) continue; /* monstro morto não age */
       if (paMonsters[ii].iDebuffCycCt > 0) {
        paMonsters[ii].iHP -= paMonsters[ii].iDebuff;
        paMonsters[ii].iDebuffCycCt--; /* decai 1 por turno */
        if (paMonsters[ii].iHP < 0) paMonsters[ii].iHP = 0;
        snprintf(szLine, sizeof(szLine),
                "%s sofre %d de veneno (restam %d stacks).",
                paMonsters[ii].szName, paMonsters[ii].iDebuff, paMonsters[ii].iDebuffCycCt);
        vPrintLine(szLine, INSERT_NEW_LINE);
        if (paMonsters[ii].iHP <= 0) continue;
      }

    iChoice = rand() % 3; /* 0 = attack, 1 = defend, 2 = heal */

    if (iChoice == 0) {
      /* Ataque */
      int iDamage = paMonsters[ii].iAttack;
      /* Se player tivesse escudo, poderia reduzir aqui */
      if (gstPlayer.iBlock > 0) {
        gstPlayer.iBlock -= iDamage;
        if (gstPlayer.iBlock < 0) {
          gstPlayer.iHP -= (gstPlayer.iBlock);
          gstPlayer.iBlock = 0;
        }
      } else {
        gstPlayer.iHP -= iDamage;
      }
      snprintf(szLine, sizeof(szLine),
               "%s ataca causando %d de dano!",
               paMonsters[ii].szName, iDamage);
      vPrintLine(szLine, INSERT_NEW_LINE);
    } else if (iChoice == 1) {
      /* Defesa */
      paMonsters[ii].iBlock += 3;
      snprintf(szLine, sizeof(szLine),
               "%s se defende e ganha 3 de bloqueio (total=%d).",
               paMonsters[ii].szName, paMonsters[ii].iBlock);
      vPrintLine(szLine, INSERT_NEW_LINE);
    } else {
      /* Cura */
      paMonsters[ii].iHP += MONSTER_HEAL_VALUE;
      if (paMonsters[ii].iHP > giMaxMonsterHP)
        paMonsters[ii].iHP = giMaxMonsterHP;
      snprintf(szLine, sizeof(szLine),
               "%s se cura em %d (HP=%d).",
               paMonsters[ii].szName, MONSTER_HEAL_VALUE, paMonsters[ii].iHP);
      vPrintLine(szLine, INSERT_NEW_LINE);
    }
    vSleepSeconds(2);
  }
}

int iIsMonsterAlive(PSTRUCT_MONSTER pstMonster) {
  return pstMonster->iHP > 0;
}

int iAnyMonsterAlive(PSTRUCT_MONSTER paMonsters, int iCount) {
  int i;
  for (i = 0; i < iCount; i++) {
    if (iIsMonsterAlive(&paMonsters[i])) return 1;
  }
  return 0;
}
int iMonsterCountForLevel(int iLevel) {
  int iCount;
  iCount = 2 + (iLevel - 1); /* cresce 1 por nível, começa em 2 */
  if (iCount > MAX_MONSTERS) iCount = MAX_MONSTERS;
  return iCount;
}

void vInitMonstersForLevel(PSTRUCT_MONSTER paMonsters, int iLevel, int *piOutCount) {
  int i;
  int iCount;
  int iAtkBase;

  iCount = iMonsterCountForLevel(iLevel);
  *piOutCount = iCount;

  for (i = 0; i < iCount; i++) {
    snprintf(paMonsters[i].szName, sizeof(paMonsters[i].szName), "Monstro %d-%d", iLevel, i + 1);

    giMaxMonsterHP = (iLevel-1) *MONSTER_HP_SCALING_RATE * MONSTER_INITAL_HP_MAX;
    giMaxMonsterHP += MONSTER_INITAL_HP_MAX;
    paMonsters[i].iHP = giMaxMonsterHP;

    iAtkBase = 3 + i + (iLevel - 1);            /* ataque cresce com nível */
    paMonsters[i].iAttack = iAtkBase;
    paMonsters[i].iBlock = 0;
  }
  vTraceVarArgsFn("Nivel %d: iniciados %d monstros.", iLevel, iCount);
}

