#include <stdio.h>  
#include <terminal_utils.h>
#include <input.h>
#include <dialog.h>
#include <string.h>

char *pszTerminalColors[] ={
  "\033",
  "\034",
  "\035",
  "\036",
  "\037",
  "\090",
  "\091",
  "\092",
  "\093",
  "\094",
  "\095",
  "\096",
  "\097",
  NULL
};

void vPrintLine(char *pszLine, int bNewLine) {
  iAddMsgToDialog(pszLine, strlen(pszLine));
  printf("%s", pszLine);
  if ( bNewLine )
    printf("\n");
}

void vPrintHighlitedLine(char *pszLine, int bNewLine) {
  iAddMsgToDialog(pszLine, strlen(pszLine));
  printf("\x1b[7m%s\x1b[0m", pszLine);
  if ( bNewLine )
    printf("\n");
}

void vSetCursorPosition(int iRow, int iCol) {
  printf("\033[%d;%dH", iRow, iCol);
  fflush(stdout);
}

void vClearScreen() {
  vClearTerminal();
    printf("\033[2J\033[1;1H");  /** Limpa a tela e volta para (1,1) */
  fflush(stdout);
}


void vGotoInputPosition() {
  vSetCursorPosition(11, 1);  /** Input sempre na linha 11 */
}

void vGotoFeedbackPosition() {
  vSetCursorPosition(13, 1);  /** Feedback sempre na linha 23 */
}
void vGotoFeedbackErrorPosition() {
  vSetCursorPosition(13, 1);  /** Feedback de erro sempre na linha 13 */
}

void vGotoInputTextPosition() {
  vSetCursorPosition(11, 21); /** Linha 11, coluna após "Digite sua palavra: " */
}

void vPrintColored(const char *pszText, int iColor) {
  iAddMsgToDialog((char*)pszText, strlen((char*)pszText));
  printf("\033[%dm%s\033[0m", iColor, pszText);
}

void vClearLineFromCursor() {
  printf("\033[K");
  fflush(stdout);
}

