#include <stdio.h>
#include <sys_interface.h>
#include <trace.h>
#include <string.h>
#include <stdlib.h>
#include <terminal_utils.h>
#include <input.h>
#include <deck.h>
#include <monster.h>
#include <player.h>
#include <battle.h>
#include <shop.h>

char *gkpszProgramName;

STRUCT_PLAYER gstPlayer;

void vFreeProgramName(){ 
  if (gkpszProgramName != NULL)
    free(gkpszProgramName);
}

int main(int argc, char *argv[]) {
  STRUCT_DECK stDeck;
  int bRunning;
  char szPath[_MAX_PATH];
  char szName[_MAX_PATH];
  char szExt[_MAX_PATH];
  STRUCT_MONSTER aMonsters[MAX_MONSTERS];
  int iLevel;
  int iMonsterCount;

  bRunning = TRUE;
  iLevel = 1;

  gkpszProgramName = NULL;
  iDIR_SplitFilename(argv[0], szPath, szName, szExt);
  if (!bStrIsEmpty(szName)) {
    gkpszProgramName = (char *)malloc(strlen(szName)+1);
    memset(gkpszProgramName, 0, strlen(szName)+1);
    strcpy(gkpszProgramName, szName);
  }

  vInitLogs();
  vShowInitDialog();

  vTraceVarArgsFn("Init OK -- Starting main loop...");

  vInitBasicDeck(&stDeck);
  iDrawMultipleCard(MAX_HAND, &stDeck);
  vInitPlayer(&stDeck);
  vInitMonstersForLevel(aMonsters, iLevel, &iMonsterCount);
  
  while (bRunning) {
    vClearTerminal();

    vSortHandByName(&stDeck);
    vShowPlayer();
    vPrintLine("\t=== Sua mao (ordenada) ===", INSERT_NEW_LINE);
    vShowDeck(&stDeck);
    vPrintLine("\t== Monstros ==", INSERT_NEW_LINE);
    vShowMonsters(aMonsters, iMonsterCount);

    while (gstPlayer.iEnergy > 0 && iAnyMonsterAlive(aMonsters, iMonsterCount)) {
      int iCh, iIdx;
      vPrintLine("\nEscolha carta (1..9), 'e' encerra turno, 'q' sai:", INSERT_NEW_LINE);
      iCh = iPortableGetchar();
      if (iCh == 'q') { bRunning = FALSE; break; }
      if (iCh == 'e') { break; }
      if (iCh >= '0' && iCh <= '9') {
        iIdx = iCh - '0';
        vPlayCard(iIdx, &stDeck, aMonsters, iMonsterCount);
        vClearTerminal();
        vSortHandByName(&stDeck);
        vShowPlayer();
        vPrintLine("\t=== Sua mao (ordenada) ===", INSERT_NEW_LINE);
        vShowDeck(&stDeck);
        vPrintLine("\t== Monstros ==", INSERT_NEW_LINE);
        vShowMonsters(aMonsters, iMonsterCount);
      }
    }

    /* fim de turno do jogador */
    vDiscardHand(&stDeck);
    vDoEnemyActions(aMonsters, iMonsterCount);

    /* checa derrota */
    if (gstPlayer.iHP <= 0) {
      vPrintLine("\n*** Derrota! ***", INSERT_NEW_LINE);
      break;
    }

    /* checa vitória do nível */
    if (!iAnyMonsterAlive(aMonsters, iMonsterCount)) {
      char szMsg[128];
      snprintf(szMsg, sizeof(szMsg), "\n*** Nivel %d completo! ***", iLevel);
      vPrintHighlitedLine(szMsg, INSERT_NEW_LINE);
      
      vOpenShop(&stDeck);

      iLevel++;
      vInitMonstersForLevel(aMonsters, iLevel, &iMonsterCount);

      /* reset de mão/energia para novo nível */
      iDrawMultipleCard(MAX_HAND, &stDeck);
      gstPlayer.iEnergy = PLAYER_ENERGY_MAX;
      vFlushInput();
      continue; /* redesenha no próximo loop */
    }

    /* próximo turno no mesmo nível */
    iDrawMultipleCard(MAX_HAND, &stDeck);
    gstPlayer.iEnergy = PLAYER_ENERGY_MAX;
  }

  vFreeProgramName();
  return 0;
}