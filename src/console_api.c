#include <debuff.h>
#include <deck.h>
#include <trace.h>
#include <terminal_utils.h>
#include <monster.h>
#include <player.h>
#include <battle.h>
#include <input.h>
#include <shop.h>
#include <dialog.h>
#include <console_api.h>

void vCNSL_MainLoop(int *pbRunning, PSTRUCT_DECK pstDeck, PSTRUCT_MONSTER pastMonsters, int iMonsterCt){
  while (*pbRunning) {
    vClearTerminal();
    vSortHandByName(pstDeck);
    vShowTable(pstDeck, pastMonsters, iMonsterCt);
    vTraceDialog(FALSE);

    while (gstPlayer.iEnergy > 0 && iAnyMonsterAlive(pastMonsters, iMonsterCt)) {
      if (iDoPlayerTurn(pbRunning, pstDeck, pastMonsters, iMonsterCt))
        break;
    }

    /* fim de turno do jogador */
    vDiscardHand(pstDeck);
    vDoEnemyActions(pastMonsters, iMonsterCt);

    /* checa derrota */
    if (gstPlayer.iHP <= 0) {
      vPrintLine("\n***  Derrota!! Você morreu! ***", INSERT_NEW_LINE);
      vSleepSeconds(3);
      break;
    }

    /* checa vitória do nível */
    if (!iAnyMonsterAlive(pastMonsters, iMonsterCt)) {
      char szMsg[128];
      snprintf(szMsg, sizeof(szMsg), "\n*** Nivel %d completo! ***", giLevel);
      vPrintHighlitedLine(szMsg, INSERT_NEW_LINE);

      vFlushInput();
      vAddPlayerReward(&gstPlayer);
      vOpenShop(pstDeck);

      vFlushInput();
      giLevel++;
      vInitMonstersForLevel(pastMonsters, giLevel, &iMonsterCt);

      /* reset de mão/energia para novo nível */
      iDrawMultipleCard(INIT_HAND_CARDS, pstDeck);
      vSleepSeconds(3);
      gstPlayer.iEnergy = PLAYER_ENERGY_MAX;
      gstPlayer.iBlock = 0;
      continue;
    }

    /* próximo turno no mesmo nível */
    iDrawMultipleCard(INIT_HAND_CARDS, pstDeck);
    vSleepSeconds(3);
    gstPlayer.iEnergy = PLAYER_ENERGY_MAX;

    vTraceDeck(pstDeck, TRACE_DECK_ALL);
  }
}