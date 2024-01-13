#pragma once

#include "../Board.h"

const int WEIGHTS[6]{ 1, 3, 12, 75, 10000, 30 };

class AIBoard : public Board
{
public:
  AIBoard( bool botIsWhite, int moveCount );

  inline void SetMoveCount( int moveCount )
  {
    moveCount = m_MoveCount;
  }
  bool FindMove( int row, int column, bool isWhite, int depth );
  void CapturePieces( int depth );
  void ReturnPieces( int depth );
  void UndoMove( int row, int column, int depth );
  int Evaluate();

private:
  void Adjacent( int x, int y, char piece );
  void Diagonal( int x, int y, char piece );

  int  m_MoveCount;
  bool m_BotIsWhite;
  int capturedIndex = 0;
  int captured[80]; // At most 18 captured pieces, 20 to be safe, encoded [depth, x, y, piece]
  int piecesAway[6]{ 0 };
};