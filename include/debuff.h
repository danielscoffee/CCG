#ifndef DEBUFF_H
  #define DEBUFF_H

  #define DEBUFF_TYPE_POISON 0
  #define DEBUFF_TYPE_NONE   -1

  typedef struct STRUCT_DEBUFF{
    int iType;
    int iDamage;
    int iRounds;
  }STRUCT_DEBUFF, *PSTRUCT_DEBUFF;
#endif
