#ifndef SYSINTERFACE_H
  #define SYSINTERFACE_H
/****************************************************************************** 
 *                                                                            *
 *               Compatibilidade Windows (MSVC vs MinGW/GCC)                  *
 *                                                                            *
 ******************************************************************************/

  #ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>

    /* Definições ausentes no MinGW */
  #endif

  #ifndef _MAX_DRIVE
    #define _MAX_DRIVE 3
  #endif
  #ifndef _MAX_DIR
    #define _MAX_DIR 256
  #endif
  #ifndef _MAX_FNAME
    #define _MAX_FNAME 256
  #endif
  #ifndef _MAX_EXT
    #define _MAX_EXT 256
  #endif
  #ifndef _MAX_PATH
    #define _MAX_PATH 256
  #endif

  #define TRUE  1
  #define FALSE 0

/******************************************************************************
 *                                                                            *
 *                          Protótipos de funções                             *
 *                                                                            *
 ******************************************************************************/
  int bStrIsEmpty(const char *kpszStr);
  int iDIR_MkDir(char *szDir);
  int iDIR_SplitFilename(char *szFilename, char *szPath, char *szName,
                         char *szExt);
  int iDIR_IsDir(char *szDir);

  int bOpenFile(FILE **fppFile, const char *kpszFileName, const char *kpszMode);
  int bCloseFile(FILE **fppFile);
  int bFileExist(const char *kpszFileName);

#endif /* SYSINTERFACE_H */
