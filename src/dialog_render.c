#ifdef USE_SDL2
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_ttf.h>
  #include <dialog.h>
  #include <card_game.h>

  /* helper declarado no seu sdl_api.c */
  extern void vSDL_DrawText(SDL_Renderer *pSDL_Renderer, const char *pszText, int iX, int iY, SDL_Color stCol);

  void vSDL_DrawDialogBox(SDL_Renderer *pSDL_Renderer,
                          TTF_Font *pFont,
                          int iX, int iY, int iW, int iH,
                          SDL_Color stColBG,
                          SDL_Color stColText,
                          SDL_Color stColShadow,
                          int iPad, int iLineH,
                          int iDrawScrollbar) {
  SDL_Rect stRectBox;
  SDL_Rect stRectClip;
  SDL_Rect stRectSB;
  SDL_Color stColSBBack;
  SDL_Color stColSBGrip;
  int iLinesFit;
  int iIdx;
  int iCurY;
  int iMaxTop;
  int iCount;
  PSTRUCT_DIALOG pstNode;

  stRectBox.x = iX;
  stRectBox.y = iY;
  stRectBox.w = iW;
  stRectBox.h = iH;

  SDL_SetRenderDrawColor(pSDL_Renderer, stColBG.r, stColBG.g, stColBG.b, stColBG.a);
  SDL_RenderFillRect(pSDL_Renderer, &stRectBox);

  if (iLineH <= 0)
      iLineH = 18;

  iLinesFit = (iH - 2 * iPad) / iLineH;
  if (iLinesFit < 1)
      iLinesFit = 1;

  stRectClip.x = iX + iPad;
  stRectClip.y = iY + iPad;
  stRectClip.w = iW - 2 * iPad;
  stRectClip.h = iH - 2 * iPad;
  SDL_RenderSetClipRect(pSDL_Renderer, &stRectClip);

  iCurY = iY + iPad;
  iCount = iDlgGetCount();

  for (iIdx = 0; iIdx < iLinesFit; iIdx++) {
      int iListIndex;
      int iTextX;

      iListIndex = iDlgGetTopIndex() + iIdx;
      if (iListIndex >= iCount)
      break;

      pstNode = pstDlgGetAt(iListIndex);
      if (pstNode == NULL)
      break;

      iTextX = iX + iPad;
      vSDL_DrawText(pSDL_Renderer, pstNode->pszMsg, iTextX + 1, iCurY + 1, stColShadow);
      vSDL_DrawText(pSDL_Renderer, pstNode->pszMsg, iTextX,     iCurY,     stColText);

      iCurY += iLineH;
  }

  SDL_RenderSetClipRect(pSDL_Renderer, NULL);

  /* scrollbar */
  if (iDrawScrollbar != 0) {
      int iSBW;
      int iTrackX;
      int iTrackY;
      int iTrackH;
      int iGripH;
      double dFrac;
      int iGripY;

      stColSBBack.r = 0; stColSBBack.g = 0; stColSBBack.b = 0; stColSBBack.a = 80;
      stColSBGrip.r = 255; stColSBGrip.g = 255; stColSBGrip.b = 255; stColSBGrip.a = 160;

      iSBW = 6;
      iTrackX = iX + iW - iSBW - 2;
      iTrackY = iY + 2;
      iTrackH = iH - 4;

      SDL_SetRenderDrawColor(pSDL_Renderer, stColSBBack.r, stColSBBack.g, stColSBBack.b, stColSBBack.a);
      stRectSB.x = iTrackX;
      stRectSB.y = iTrackY;
      stRectSB.w = iSBW;
      stRectSB.h = iTrackH;
      SDL_RenderFillRect(pSDL_Renderer, &stRectSB);

      iMaxTop = iCount - iLinesFit;
      if (iMaxTop < 0)
      iMaxTop = 0;

      if (iCount > 0) {
      iGripH = (int)((double)iTrackH * ((double)iLinesFit / (double)iCount));
      if (iGripH < 12)
          iGripH = 12;

      if (iMaxTop == 0)
          dFrac = 0.0;
      else
          dFrac = (double)iDlgGetTopIndex() / (double)iMaxTop;

      iGripY = iTrackY + (int)((double)(iTrackH - iGripH) * dFrac);

      SDL_SetRenderDrawColor(pSDL_Renderer, stColSBGrip.r, stColSBGrip.g, stColSBGrip.b, stColSBGrip.a);
      stRectSB.x = iTrackX;
      stRectSB.y = iGripY;
      stRectSB.w = iSBW;
      stRectSB.h = iGripH;
      SDL_RenderFillRect(pSDL_Renderer, &stRectSB);
      }
  }

  SDL_SetRenderDrawColor(pSDL_Renderer, 255, 255, 255, 180);
  SDL_RenderDrawRect(pSDL_Renderer, &stRectBox);
  }
#endif