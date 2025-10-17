#include <dialog.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* estado global */
STRUCT_DIALOG_LIST gstDlgList;

/* ---------- Helpers internos ---------- */

static PSTRUCT_DIALOG pstDlgNodeCreate(const char *pszMsgIn, int iMsgLen, int iLevel) {
  PSTRUCT_DIALOG pstNode;
  int iAllocLen;
  time_t lNow;
  struct tm *pstTM;

  pstNode = (PSTRUCT_DIALOG)malloc(sizeof(STRUCT_DIALOG));
  if (pstNode == NULL)
    return NULL;

  pstNode->pstPrev = NULL;
  pstNode->pstNext = NULL;
  pstNode->iLevel = iLevel;

  if (iMsgLen < 0)
    iMsgLen = (int)strlen(pszMsgIn);
  iAllocLen = iMsgLen + 1;
  pstNode->pszMsg = (char*)malloc((size_t)iAllocLen);
  if (pstNode->pszMsg == NULL) {
    free(pstNode);
    return NULL;
  }
  memcpy(pstNode->pszMsg, pszMsgIn, (size_t)iMsgLen);
  pstNode->pszMsg[iMsgLen] = '\0';

  lNow = time(NULL);
  pstTM = localtime(&lNow);
  if (pstTM != NULL) {
    /* exemplo: 2025-10-17 13:04:22 */
    snprintf(pstNode->szDT, sizeof(pstNode->szDT),
             "%04d-%02d-%02d %02d:%02d:%02d",
             pstTM->tm_year + 1900,
             pstTM->tm_mon + 1,
             pstTM->tm_mday,
             pstTM->tm_hour,
             pstTM->tm_min,
             pstTM->tm_sec);
  } else {
    strcpy(pstNode->szDT, "0000-00-00 00:00:00");
  }

  return pstNode;
}

static void vDlgNodeDestroy(PSTRUCT_DIALOG pstNode) {
  if (pstNode == NULL)
    return;
  if (pstNode->pszMsg != NULL)
    free(pstNode->pszMsg);
  free(pstNode);
}

static int iClamp(int iVal, int iMin, int iMax) {
  if (iVal < iMin) return iMin;
  if (iVal > iMax) return iMax;
  return iVal;
}

/* ---------- API pública ---------- */

void vInitDialog(void) {
  gstDlgList.pstHead = NULL;
  gstDlgList.pstTail = NULL;
  gstDlgList.iCount = 0;
  gstDlgList.iTopIndex = 0;
}

int iAddMsgToDialog(char *pszMsg, int iMsgLen) {
  PSTRUCT_DIALOG pstNode;

  if (pszMsg == NULL)
    return -1;

  pstNode = pstDlgNodeCreate(pszMsg, iMsgLen, 0);
  if (pstNode == NULL)
    return -2;

  if (gstDlgList.pstTail == NULL) {
    gstDlgList.pstHead = pstNode;
    gstDlgList.pstTail = pstNode;
  } else {
    pstNode->pstPrev = gstDlgList.pstTail;
    gstDlgList.pstTail->pstNext = pstNode;
    gstDlgList.pstTail = pstNode;
  }

  gstDlgList.iCount++;

  return 0;
}

void vFreeDialog(void) {
  PSTRUCT_DIALOG pstCur;
  PSTRUCT_DIALOG pstNext;

  pstCur = gstDlgList.pstHead;
  while (pstCur != NULL) {
    pstNext = pstCur->pstNext;
    vDlgNodeDestroy(pstCur);
    pstCur = pstNext;
  }

  gstDlgList.pstHead = NULL;
  gstDlgList.pstTail = NULL;
  gstDlgList.iCount = 0;
  gstDlgList.iTopIndex = 0;
}

void vTraceDialog(int bLogDT) {
  PSTRUCT_DIALOG pstCur;
  int iIdx;

  pstCur = gstDlgList.pstHead;
  iIdx = 0;

  while (pstCur != NULL) {
    if (bLogDT != 0)
      printf("[%03d][%s] %s\n", iIdx, pstCur->szDT, pstCur->pszMsg);
    else
      printf("[%03d] %s\n", iIdx, pstCur->pszMsg);

    pstCur = pstCur->pstNext;
    iIdx++;
  }
}

/* ---------- Navegação/scroll ---------- */

PSTRUCT_DIALOG pstDlgGetAt(int iIndex) {
  PSTRUCT_DIALOG pstCur;
  int i;

  if (iIndex < 0 || iIndex >= gstDlgList.iCount)
    return NULL;

  if (iIndex <= (gstDlgList.iCount / 2)) {
    pstCur = gstDlgList.pstHead;
    i = 0;
    while (pstCur != NULL && i < iIndex) {
      pstCur = pstCur->pstNext;
      i++;
    }
    return pstCur;
  } else {
    pstCur = gstDlgList.pstTail;
    i = gstDlgList.iCount - 1;
    while (pstCur != NULL && i > iIndex) {
      pstCur = pstCur->pstPrev;
      i--;
    }
    return pstCur;
  }
}

int iDlgGetTopIndex(void) {
  return gstDlgList.iTopIndex;
}

int iDlgGetCount(void) {
  return gstDlgList.iCount;
}

void vDlgScrollLines(int iDelta, int iVisibleCount) {
  int iMaxTop;

  if (iVisibleCount < 1)
    iVisibleCount = 1;

  iMaxTop = gstDlgList.iCount - iVisibleCount;
  if (iMaxTop < 0)
    iMaxTop = 0;

  gstDlgList.iTopIndex = iClamp(gstDlgList.iTopIndex + iDelta, 0, iMaxTop);
}

void vDlgPageUp(int iVisibleCount) {
  vDlgScrollLines(-iVisibleCount, iVisibleCount);
}

void vDlgPageDown(int iVisibleCount) {
  vDlgScrollLines(iVisibleCount, iVisibleCount);
}

void vDlgScrollToBottom(int iVisibleCount) {
  int iMaxTop;

  if (iVisibleCount < 1)
    iVisibleCount = 1;

  iMaxTop = gstDlgList.iCount - iVisibleCount;
  if (iMaxTop < 0)
    iMaxTop = 0;

  gstDlgList.iTopIndex = iMaxTop;
}
