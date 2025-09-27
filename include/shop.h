#ifndef SHOP_H
  #define SHOP_H
  #include <card_game.h>

  #define SHOP_PRICE_POISON            15
  #define SHOP_PRICE_PARALIZE          20
  
  #define SHOP_PRICE_UPGRADE           20
  
  #define SHOP_STD_GOLD_AMOUNT_REWARD  15
  #define SHOP_GOLD_MULTIPLIYER        1.2

  void vOpenShop(PSTRUCT_DECK pstDeck);
  void vAddPlayerReward(PSTRUCT_PLAYER pstPlayer);
#endif
