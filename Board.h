#pragma once
#include "Global.h"

class Board
{
public:
  Board();

  void ClearBoard();
  void DisplayBoard() const;
  bool PlayTurn( int row, int column, bool isWhite );
  void CapturePieces();
  bool IsGameOver();

protected:
  char board[SIDE_LENGTH][SIDE_LENGTH];
  int  m_CapturedWhite = 0;
  int  m_CapturedBlack = 0;
  int  m_LastRow;
  int  m_LastColumn;
};