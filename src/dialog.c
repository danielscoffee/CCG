#include <trace.h>
#include <dialog.h>

#ifdef LINUX
  #ifdef __has_include
    #if __has_include(<time.h>)
      #include <time.h>
    #elif __has_include(<sys/time.h>)
      #include <sys/time.h>
    #endif
  #endif
  #include <sys/types.h>
#else
  #include <windows.h>
  #include <process.h>
  #include <time.h>
#endif

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
    (int)st_tm_Now->tm_mday, (int)st_tm_Now->tm_mon + 1, (int)st_tm_Now->tm_year -100 ,
    (int)st_tm_Now->tm_hour, (int)st_tm_Now->tm_min,     (int)st_tm_Now->tm_sec
  );
  
  pszWrkMsg = (char *) malloc(iMsgLen + 1);
  memset(pszWrkMsg, 0, iMsgLen + 1);
  strncpy(pszWrkMsg, pszMsg, iMsgLen);
  pszWrkMsg[iMsgLen-1] = 0;
  
  if ( bStrIsEmpty(gstDialog.pszMsg) ){
    pstDialog = &gstDialog;
  }
  else{
    PSTRUCT_DIALOG pWrk;
    for (pWrk = &gstDialog; pWrk->pstNext != NULL; pWrk = pWrk->pstNext);
    pstDialog = (PSTRUCT_DIALOG) malloc(sizeof(STRUCT_DIALOG));
    (*pWrk).pstNext = pstDialog;
  }
  memset(&(*pstDialog), 0, sizeof(STRUCT_DIALOG));
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

void vTraceDialog(int bLogDT){
  char szDbg[1024];
  PSTRUCT_DIALOG pWrk;

  if ( bStrIsEmpty(gstDialog.pszMsg) )
    return ;

  for (pWrk = &gstDialog; pWrk!= NULL; pWrk = pWrk->pstNext){
    if ( bLogDT )
      sprintf(szDbg, "[%s] %s", pWrk->szDT, pWrk->pszMsg);
    else
      sprintf(szDbg, "%s", pWrk->pszMsg);

    vTraceMsgDialog(szDbg);
  }
}
