/**
 * rect.h
 *
 * Written by Renato Fermi <repiazza@gmail.com> in March 2024
 *
 * Rect handling
 *
 */

#ifdef USE_SDL2
  #ifndef _RECT_H_
    #define _RECT_H_
    #include <card_game.h>

    // Definições de proporções como percentuais da tela
    #define HUD_X_FACTOR      0.06  // Porcentagem da largura da tela
    #define HUD_Y_FACTOR      0.04  // Porcentagem da altura da tela
    #define HUD_WIDTH_FACTOR  0.5   // 50% da largura da tela
    #define HUD_HEIGHT_FACTOR 0.05  // 5% da altura da tela

    typedef struct STRUCT_RECT_VALUES
    {
      double dFactor;   // Angular coefficient
      double dAddAmount;
    } STRUCT_RECT_VALUES, *PSTRUCT_RECT_VALUES;

    typedef struct STRUCT_RECT_DIMENSIONS
    {
      STRUCT_RECT_VALUES stX;
      STRUCT_RECT_VALUES stY;
      STRUCT_RECT_VALUES stW;
      STRUCT_RECT_VALUES stH;
    } STRUCT_RECT_DIMENSIONS, PSTRUCT_RECT_DIMENSIONS;

    extern STRUCT_RECT_DIMENSIONS gstButtonHUD_Dimension;
    extern STRUCT_RECT_DIMENSIONS gstCmdHUD_Dimensions;
    extern STRUCT_RECT_DIMENSIONS gstTmpHUD_Dimensions;
    extern STRUCT_RECT_DIMENSIONS gstPlayerHUD_Dimensions;

    /**
     * Set
     */
    int iRECT_SetDimensions(SDL_Rect *pSDL_Rect, STRUCT_RECT_DIMENSIONS *pst_RECT_Dimensions)
    {
      if (pSDL_Rect == NULL)
        return FALSE;

      pSDL_Rect->x = pst_RECT_Dimensions->stX.dFactor + pst_RECT_Dimensions->stX.dAddAmount;
      pSDL_Rect->y = pst_RECT_Dimensions->stY.dFactor + pst_RECT_Dimensions->stY.dAddAmount;
      pSDL_Rect->w = pst_RECT_Dimensions->stW.dFactor + pst_RECT_Dimensions->stW.dAddAmount;
      pSDL_Rect->h = pst_RECT_Dimensions->stH.dFactor + pst_RECT_Dimensions->stH.dAddAmount;
      return TRUE;
    }


    void vRECT_InitGlobalDimensions(void)
    {
      (&gstPlayerHUD_Dimensions)->stX.dFactor = 0;
      (&gstPlayerHUD_Dimensions)->stX.dAddAmount = 0;
      (&gstPlayerHUD_Dimensions)->stY.dFactor = 0;
      (&gstPlayerHUD_Dimensions)->stY.dAddAmount = 0;
      (&gstPlayerHUD_Dimensions)->stW.dFactor = INT_WINDOW_WIDTH;
      (&gstPlayerHUD_Dimensions)->stW.dAddAmount = 0;
      (&gstPlayerHUD_Dimensions)->stH.dFactor = INT_WINDOW_HEIGHT;
      (&gstPlayerHUD_Dimensions)->stH.dAddAmount = 0;
    }

    void vRECT_InitPlayerHud()
    {
      vRECT_InitGlobalDimensions();
      iRECT_SetDimensions(&gSDL_Player_Rect, &gstPlayerHUD_Dimensions);
    }
    void vCalculateHUDRect(SDL_Rect *pSDL_Rect, double xFactor, double yFactor, double widthFactor, double heightFactor)
    {
      if (pSDL_Rect == NULL)
        return;

      pSDL_Rect->x = (int)(xFactor * INT_WINDOW_WIDTH);
      pSDL_Rect->y = (int)(yFactor * INT_WINDOW_HEIGHT);
      pSDL_Rect->w = (int)(widthFactor * INT_WINDOW_WIDTH);
      pSDL_Rect->h = (int)(heightFactor * INT_WINDOW_HEIGHT);
    }

  #endif
#endif