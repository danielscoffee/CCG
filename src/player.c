/**
 * player.c 
 * 
 * Handles player data structures and actions
 * 
 * Written by Renato Fermi<repiazza@gmail.com> in September 2025
 * Maintained by Gustavo <> since September 2025
 * 
 */
#include <player.h>
#include <deck.h>
#include <battle.h>
#include <input.h>
#include <string.h>
#include <monster.h>
#include <terminal_utils.h>

void vInitPlayer(PSTRUCT_DECK pstGameDeck)
{
  gstPlayer.iHP = PLAYER_HP_MAX;
  gstPlayer.iEnergy = PLAYER_ENERGY_MAX;
  gstPlayer.iBlock = 0;
  gstPlayer.iGold = 30;
  vReadPlayerName(gstPlayer.szPlayerName, PLAYER_NAME_SIZE);
  gstPlayer.pstPlayerCards = pstGameDeck;
}

void vShowPlayer(int bTrace) {
  int ii;
  char szLine[1024];
  char szDbg[5096];

  memset(szDbg, 0,sizeof(szDbg));
  memset(szLine, 0,sizeof(szLine));
  snprintf(szLine, sizeof(szLine),
"   Jogador=%s",
    gstPlayer.szPlayerName
  );
  vPrintColored(szLine, TERMINAL_COLOR_BLUE);
  strcat(szDbg, szLine);

  snprintf(szLine, sizeof(szLine),
" Vida=[%d/%d]",
    gstPlayer.iHP, 
    PLAYER_HP_MAX
  );
  vPrintColored(szLine, TERMINAL_COLOR_RED);
  strcat(szDbg, szLine);
  
  snprintf(szLine, sizeof(szLine),
" Defesa=[%d]",
    gstPlayer.iBlock
  );
  vPrintColored(szLine, TERMINAL_COLOR_CYAN);
  strcat(szDbg, szLine);

  snprintf(szLine, sizeof(szLine),
" Energia=[%d/%d]",
    gstPlayer.iEnergy, PLAYER_ENERGY_MAX
  );
  vPrintColored(szLine, TERMINAL_COLOR_YELLOW);
  strcat(szDbg, szLine);

  snprintf(szLine, sizeof(szLine),
" Gold=[%d]\n\n",
    gstPlayer.iGold
  );
  vPrintColored(szLine, TERMINAL_COLOR_BYELLOW);
  strcat(szDbg, szLine);
  if (bTrace){
    strtok(szDbg, "\n");
    vTraceVarArgsFn("%s", szDbg);
  }  
  for (ii = 0; ii < gstPlayer.iDebuffCt; ii++) {
    if (gstPlayer.stDebuff[ii].iType == DEBUFF_TYPE_POISON) {
      snprintf(szLine, sizeof(szLine),
    " Dano de Veneno(por turno)=%d  Turnos restantes=%d",
        gstPlayer.stDebuff[ii].iDamage,
        gstPlayer.stDebuff[ii].iRounds
      );
      vPrintColored(szLine, TERMINAL_COLOR_GREEN);
    }
  }
}

int iDoPlayerTurn(int *bRunning, PSTRUCT_DECK pstDeck, PSTRUCT_MONSTER pastMonster, int iMonsterCt){
  int iCh;
  int iIdx;

  if ( !bRunning ) return 1;
  
  vPrintLine("\nEscolha carta (1..9), 'e' encerra turno, 'q' sai:", INSERT_NEW_LINE);
  iCh = iPortableGetchar();
  if (iCh == 'q') { bRunning = FALSE; return 1; }
  if (iCh == 'e') { return 1; }
  if (iCh >= '0' && iCh <= '9') {
    iIdx = iCh - '0';
    vPlayCard(iIdx, pstDeck, pastMonster, iMonsterCt);
    vClearTerminal();
    vSortHandByName(pstDeck);
    vShowPlayer(TRUE);
    vPrintLine("\t=== Sua mao (ordenada) ===", INSERT_NEW_LINE);
    vShowDeck(pstDeck);
    vPrintLine("\t== Monstros ==", INSERT_NEW_LINE);
    vShowMonsters(pastMonster, iMonsterCt);
    vLogDeck(pstDeck, TRACE_DECK_ALL);
  }
  return 0;
}