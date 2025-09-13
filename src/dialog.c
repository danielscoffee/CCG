#include <card_game.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys_interface.h>

#ifdef LINUX
  #include <sys/time.h>
  #include <sys/types.h>
#else
  #include <windows.h>
  #include <process.h>
  #include <time.h>
  #ifndef timeval
    struct timeval {
      long tv_sec;
      long tv_usec;
    };
  #endif
#endif

typedef struct STRUCT_DIALOG{
  int  iLevel;
  char *pszMsg;
  char szDT[64];
  struct STRUCT_DIALOG *pstNext;
}STRUCT_DIALOG, *PSTRUCT_DIALOG;

STRUCT_DIALOG gstDialog;

void vInitDialog(){
  memset(&gstDialog, 0, sizeof(STRUCT_DIALOG));
  gstDialog.pstNext = NULL;
}

int iAddMsgToDialog(char *pszMsg, int iMsgLen){
  time_t lTime;
  struct tm *st_tm_Now;
  char szNow[64];
  char *pszWrkMsg;
  PSTRUCT_DIALOG pstDialog;

  if ( iMsgLen <= 0 )
    return -1;
    
  time(&lTime);
  st_tm_Now = localtime(&lTime);

  memset(szNow, 0, sizeof(szNow));
  sprintf(szNow,
"%02d/%02d/%02d %02d:%02d:%02d",
    (int)st_tm_Now->tm_mday, (int)st_tm_Now->tm_mon + 1, (int)st_tm_Now->tm_year + 1900,
    (int)st_tm_Now->tm_hour, (int)st_tm_Now->tm_min,     (int)st_tm_Now->tm_sec
  );
  
  pszWrkMsg = (char *) malloc(iMsgLen + 2);
  
  memset(pszWrkMsg, 0, iMsgLen);
  strcpy(pszWrkMsg, pszMsg);
  
  if ( bStrIsEmpty(gstDialog.pszMsg) ){
    pstDialog = &gstDialog;
  }
  else{
    PSTRUCT_DIALOG pWrk;
    for (pWrk = &gstDialog; pWrk->pstNext != NULL; pWrk = pWrk->pstNext);
    pstDialog = (PSTRUCT_DIALOG) malloc(sizeof(STRUCT_DIALOG));
    (*pWrk).pstNext = pstDialog;
  }
  
  (*pstDialog).iLevel = giLevel;
  (*pstDialog).pszMsg = pszWrkMsg;
  strcpy((*pstDialog).szDT, szNow);
  (*pstDialog).pstNext = NULL;

  return 0;
}

void vFreeDialog(){ 
  PSTRUCT_DIALOG pWrk;

  for (pWrk = gstDialog.pstNext; pWrk != NULL; ){
    PSTRUCT_DIALOG pLast = pWrk;
    pWrk = pWrk->pstNext;
    free(pLast->pszMsg);
    free(pLast);
  }
}
