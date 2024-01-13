#pragma once

#include "AITemplate.h"

#include <mutex>
#include <thread>

class ThreadedAI : public BaseAI
{
public:

  ThreadedAI( bool botIsWhite, int moveCount );
  ~ThreadedAI();

  int Move( int moveCount );
  void PlayMove( int row, int column );

private:
  void AlphaBeta();
  int MaxValueBlock( int start, int end, int alpha, int beta, int depth, AIBoard* board );
  int MaxValue( int alpha, int beta, int depth, AIBoard* board );
  int MinValue( int alpha, int beta, int depth, AIBoard* board );
  void UpdateBestScore( int x, int y, int score );

  // Max threads is 19 so need up to 18 threads + main
  std::thread m_Threads[18];
  AIBoard* m_Boards[18];

  unsigned int m_ThreadCount;
  std::string m_BestMove;

  std::mutex m_ScoreLock;
  int m_BestV;
  int m_MoveSelected;

  bool m_AiIsWhite;
};