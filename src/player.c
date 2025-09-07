#include <player.h>
#include <deck.h>
#include <input.h>
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

void vShowPlayer() {
  char szLine[1024];
  // gstPlayer.iGold
  // gstPlayer.iDebuff
  // gstPlayer.iDebuffCycCt
  snprintf(szLine, sizeof(szLine),
           "   Jogador=%s",
           gstPlayer.szPlayerName);
  vPrintColored(szLine, TERMINAL_COLOR_BLUE);
  snprintf(szLine, sizeof(szLine),
           " Vida=[%d/%d]",
           gstPlayer.iHP, PLAYER_HP_MAX);
  vPrintColored(szLine, TERMINAL_COLOR_RED);
  snprintf(szLine, sizeof(szLine),
           " Defesa=[%d]",
           gstPlayer.iBlock);
  vPrintColored(szLine, TERMINAL_COLOR_CYAN);
  snprintf(szLine, sizeof(szLine),
           " Energia=[%d/%d]",
           gstPlayer.iEnergy, PLAYER_ENERGY_MAX);
  vPrintColored(szLine, TERMINAL_COLOR_YELLOW);
 
  if ( gstPlayer.iDebuffCycCt > 0 ){
    snprintf(szLine, sizeof(szLine),
            " Dano de Veneno(por turno)=%d  Turnos restantes=%d",
            gstPlayer.iDebuff, gstPlayer.iDebuffCycCt);
    vPrintColored(szLine, TERMINAL_COLOR_GREEN);
  }
  snprintf(szLine, sizeof(szLine),
           " Gold=[%d]\n\n",
           gstPlayer.iGold);
  vPrintColored(szLine, TERMINAL_COLOR_BYELLOW);
}