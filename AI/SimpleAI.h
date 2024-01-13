#pragma once

#include "AITemplate.h"
#include "../Global.h"
#include <string>

class SimpleAI : public BaseAI
{
public:
  SimpleAI( bool botIsWhite, int moveCount );

  int Move( int moveCount );
  void PlayMove( int row, int column );

private:
  void AlphaBeta();
  int MaxValue( int alpha, int beta, int depth );
  int MinValue( int alpha, int beta, int depth );

  std::string m_BestMove;
  int m_BestV;
  int m_MoveSelected;
  bool m_AiIsWhite;
};
