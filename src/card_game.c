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
int gbLogLevel = 1;

STRUCT_PLAYER gstPlayer;

void vFreeProgramName(){ 
  if (gkpszProgramName != NULL)
    free(gkpszProgramName);
}

int main(int argc, char *argv[]) {
  STRUCT_MONSTER aMonsters[MAX_MONSTERS];
  STRUCT_DECK stDeck;
  char szPath[_MAX_PATH];
  char szName[_MAX_PATH];
  char szExt[_MAX_PATH];
  int bLog = gbLogLevel;
  int bRunning;
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

  if ( argc > 1 ) vTraceVarArgsFn("excess of cmdline prms argc=%d", argc);

  vTraceVarArgsFn("Init OK -- Starting main loop...");

  vInitBasicDeck(&stDeck);
  iDrawMultipleCard(INIT_HAND_CARDS, &stDeck);
  vInitPlayer(&stDeck);
  vInitMonstersForLevel(aMonsters, iLevel, &iMonsterCount);
  
  while (bRunning) {
    vClearTerminal();

    vSortHandByName(&stDeck);
    vShowPlayer(bLog);
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
        vShowPlayer(bLog);
        vPrintLine("\t=== Sua mao (ordenada) ===", INSERT_NEW_LINE);
        vShowDeck(&stDeck);
        vPrintLine("\t== Monstros ==", INSERT_NEW_LINE);
        vShowMonsters(aMonsters, iMonsterCount);
        vLogDeck(&stDeck, TRACE_DECK_ALL);
      }
    }

    /* fim de turno do jogador */
    vDiscardHand(&stDeck);
    vDoEnemyActions(aMonsters, iMonsterCount);

    vLogDeck(&stDeck, TRACE_DECK_ALL);
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
      iDrawMultipleCard(INIT_HAND_CARDS, &stDeck);
      gstPlayer.iEnergy = PLAYER_ENERGY_MAX;
      gstPlayer.iBlock = 0;
      vFlushInput();
      continue;
    }

    /* próximo turno no mesmo nível */
    iDrawMultipleCard(INIT_HAND_CARDS, &stDeck);
    gstPlayer.iEnergy = PLAYER_ENERGY_MAX;
    
    vLogDeck(&stDeck, TRACE_DECK_ALL);
  }

  vFreeProgramName();
  return 0;
}