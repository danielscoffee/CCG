#ifndef DIALOG_H
  #define DIALOG_H
  void vInitDialog(void);
  int iAddMsgToDialog(char *pszMsg, int iMsgLen);
  void vFreeDialog(void);
  void vTraceDialog(int bLogDT);
#endif