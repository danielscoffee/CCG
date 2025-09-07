# Makefile for Card Game
# Written by:
#   Renato Fermi   <repiazza@gmail.com>
# in September 2025

CC            = gcc
GCC_TARGET    := $(shell $(CC) -dumpmachine)

ifeq ($(findstring mingw32, $(GCC_TARGET)), mingw32)
  _WIN32 = 1
endif
ifeq ($(findstring cygwin, $(GCC_TARGET)), cygwin)
  _WIN32 = 1
endif

PREFIX = 
ifdef _WIN32
# 	PREFIX = D:/cardgame/
# 	CC = D:/msys64/mingw64/bin/gcc.exe
endif

LIBS          = 
SRC_PATH      = ${PREFIX}src
INCLUDE_PATH  = ${PREFIX}include
OBJ_DIR       = ${PREFIX}obj
BIN_DIR       = ${PREFIX}bin
HTML_DIR      = ${PREFIX}html
LATEX_DIR     = ${PREFIX}latex
LOG_DIR       = ${PREFIX}log
INC_DIR       = -I$(INCLUDE_PATH)
CCOPT         = -Wall -Wextra


ifdef _WIN32
  CCOPT += -D_WIN32
endif

ifdef LINUX
  CCOPT += -Wl,-rpath,/usr/lib64 -Wl,--enable-new-dtags -DLINUX
  LIBS  += -Wl,-rpath,/usr/lib64 -Wl,--enable-new-dtags -DLINUX 
endif


DEBUG_ADD_FLAGS = -O2
ifdef DEBUG
  DEBUG_ADD_FLAGS = -g -ggdb
endif

ifdef FAKE
  CCOPT += -DFAKE
endif

CARD_GAME_EXEC = card_game

OBJS = \
	$(OBJ_DIR)/card_game.o \
	$(OBJ_DIR)/sys_interface.o \
	$(OBJ_DIR)/input.o \
	$(OBJ_DIR)/terminal_utils.o \
	$(OBJ_DIR)/deck.o \
	$(OBJ_DIR)/monster.o \
	$(OBJ_DIR)/player.o \
	$(OBJ_DIR)/battle.o \
	$(OBJ_DIR)/shop.o \
	$(OBJ_DIR)/trace.o 

RM_CMD = rm
MKDIR_CMD = mkdir -p

ifdef _WIN32
	MKDIR_CMD = D:/msys64/usr/bin/mkdir.exe
	RM_CMD = D:/msys64/usr/bin/rm.exe

endif
all: clean directories $(BIN_DIR)/$(CARD_GAME_EXEC)

clean:
	@${RM_CMD} -rf $(OBJ_DIR) $(BIN_DIR) $(LOG_DIR)

directories:
	@${MKDIR_CMD} $(OBJ_DIR) $(BIN_DIR)

$(BIN_DIR)/$(CARD_GAME_EXEC): directories $(OBJS)
	$(CC) $(LDOPT) $(INC_DIR) -o $@ $(OBJS) $(LIBS) $(LDOPT)

$(OBJ_DIR)/%.o: $(SRC_PATH)/%.c
	$(CC) -c $(CCOPT) $(DEBUG_ADD_FLAGS) $(INC_DIR) $< -o $@


distclean: clean

.PHONY: all clean distclean directories 

# DOCKER TARGETS
docker-build:
	docker build -t ccg:latest .

docker-run:
	docker run -it --rm ccg:latest

docker-run-interactive:
	docker run -it --rm --entrypoint /bin/sh ccg:latest

docker-test: docker-build docker-run

.PHONY: docker-build docker-run docker-run-interactive docker-test
