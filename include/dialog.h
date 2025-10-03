#ifndef DIALOG_H
  #define DIALOG_H
  #include <card_game.h> 
  typedef struct STRUCT_DIALOG{
    int  iLevel;
    char *pszMsg;
    char szDT[64];
    struct STRUCT_DIALOG *pstNext;
  }STRUCT_DIALOG, *PSTRUCT_DIALOG;
  
  void vInitDialog(void);
  int iAddMsgToDialog(char *pszMsg, int iMsgLen);
  void vFreeDialog(void);
  void vTraceDialog(int bLogDT);
  extern STRUCT_DIALOG gstDialog;
#endif