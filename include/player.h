#include <deck.h>

#ifndef PLAYER_H
  #define PLAYER_H

  #define PLAYER_HP_MAX    100
  #define PLAYER_NAME_SIZE  64
  #define PLAYER_ENERGY_MAX  3
  #define PLAYER_INIT_GOLD  30


  typedef struct STRUCT_PLAYER{
    int iHP;
    int iBlock;
    int iEnergy;
    int iGold;  
    int iDebuff; 
    int iDebuffCycCt;  
    char szPlayerName[PLAYER_NAME_SIZE];
    PSTRUCT_DECK pstPlayerCards;
  }STRUCT_PLAYER, *PSTRUCT_PLAYER;
  
  extern STRUCT_PLAYER gstPlayer;

  void vInitPlayer(PSTRUCT_DECK pstGameDeck);
  void vShowPlayer();
#endif