#pragma once
#include "AIBoard.h"

const int MAX_DEPTH = 3;

class BaseAI
{
public:
  BaseAI( bool botIsWhite, int moveCount );
  ~BaseAI();

  virtual int Move( int moveCount ) = 0;
  virtual void PlayMove( int row, int column ) = 0;

protected:
  AIBoard* aiGameBoard;
};