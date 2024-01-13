#include "ThreadedAI.h"
#include "../Global.h"

#include <iostream>

ThreadedAI::ThreadedAI( bool botIsWhite, int moveCount )
	: BaseAI( botIsWhite, moveCount ),
	m_AiIsWhite( botIsWhite )
{
  m_ThreadCount = std::thread::hardware_concurrency();

  // Limit threads to 19 since I'm doing calculations per row to make it easier to split
  // Should change to split by index
  m_ThreadCount = std::min( m_ThreadCount, (unsigned int)19 );

	for ( int ndx = 0; ndx < m_ThreadCount - 1; ndx++ )
	{
		m_Boards[ndx] = new AIBoard( botIsWhite, moveCount );
	}
}

ThreadedAI::~ThreadedAI()
{
	for ( int ndx = 0; ndx < m_ThreadCount - 1; ndx++ )
	{
		delete m_Boards[ndx];
	}
}

int ThreadedAI::Move( int moveCount )
{
	aiGameBoard->SetMoveCount( moveCount );

	AlphaBeta();

	return m_MoveSelected;
}

void ThreadedAI::PlayMove( int row, int column )
{
	aiGameBoard->PlayTurn( row, column, m_AiIsWhite );

	for ( int ndx = 0; ndx < m_ThreadCount - 1; ndx++ )
	{
		m_Boards[ndx]->PlayTurn( row, column, m_AiIsWhite );
	}
}

void ThreadedAI::AlphaBeta()
{
	int v;
	m_BestV = INT_MIN;

	int start = 0;
	int end;
	for ( int ndx = 0; ndx < m_ThreadCount - 1; ndx++ )
	{
		end = 19 * ndx / m_ThreadCount;
		m_Threads[ndx] = std::thread( &ThreadedAI::MaxValueBlock, this, start, end, INT_MIN, INT_MAX, 1, m_Boards[ndx] );
		start = end;
	}

	v = MaxValueBlock( start, 19, INT_MIN, INT_MAX, 1, aiGameBoard );

	for ( int ndx = 0; ndx < m_ThreadCount - 1; ndx++ )
	{
		m_Threads[ndx].join();
	}

	std::cout << "The bot has made move " << m_BestMove << std::endl;
}

int ThreadedAI::MaxValueBlock( int start, int end, int alpha, int beta, int depth, AIBoard* board )
{
	int v = INT_MIN;

	for ( int y = start; y < end; y++ )
	{
		for ( int x = 0; x < 19; x++ )
		{
			if ( !board->FindMove( y, x, m_AiIsWhite, 1 ) ) continue;

			// Set highest evaluation value
			if ( board->IsGameOver() )
			{
				v = std::max( v, 20000 );
			}
			else if ( depth == MAX_DEPTH )
			{
				v = std::max( v, board->Evaluate() );
			}
			else
			{
				v = std::max( v, MinValue( alpha, beta, depth + 1, board ) );
			}

			if ( v >= beta )
			{
				board->UndoMove( y, x, depth );
				return v;
			}
			alpha = std::max( alpha, v );

			if ( depth == 1 && v > m_BestV )
			{
				UpdateBestScore( x, y, v );
			}

			board->UndoMove( y, x, depth );
		}
	}

	return v;
}

int ThreadedAI::MaxValue( int alpha, int beta, int depth, AIBoard* board )
{
	int v = INT_MIN;

	for ( int y = 0; y < 19; y++ )
	{
		for ( int x = 0; x < 19; x++ )
		{
			if ( !board->FindMove( y, x, m_AiIsWhite, 1 ) ) continue;

			// Set highest evaluation value
			if ( board->IsGameOver() )
			{
				v = std::max( v, 20000 );
			}
			else if ( depth == MAX_DEPTH )
			{
				v = std::max( v, board->Evaluate() );
			}
			else
			{
				v = std::max( v, MinValue( alpha, beta, depth + 1, board ) );
			}

			if ( v >= beta )
			{
				board->UndoMove( y, x, depth );
				return v;
			}
			alpha = std::max( alpha, v );

			if ( depth == 1 && v > m_BestV )
			{
				UpdateBestScore( x, y, v );
			}

			board->UndoMove( y, x, depth );
		}
	}

	return v;
}

int ThreadedAI::MinValue( int alpha, int beta, int depth, AIBoard* board )
{
	int v = INT_MAX;

	for ( int y = 0; y < 19; y++ )
	{
		for ( int x = 0; x < 19; x++ )
		{
			if ( !board->FindMove( y, x, !m_AiIsWhite, depth ) ) continue;

			// Set lowest evaluation number
			if ( board->IsGameOver() )
			{
				v = std::min( v, -20000 );
			}
			else if ( depth == MAX_DEPTH )
			{
				v = std::min( v, board->Evaluate() );
			}
			else
			{
				v = std::min( v, MaxValue( alpha, beta, depth + 1, board ) );
			}

			if ( v <= alpha )
			{
				board->UndoMove( y, x, depth );
				return v;
			}
			beta = std::min( beta, v );

			board->UndoMove( y, x, depth );
		}
	}

	return v;
}

void ThreadedAI::UpdateBestScore( int x, int y, int score )
{
	m_ScoreLock.lock();

	// Verify no new update
	if ( score <= m_BestV )
	{
		m_ScoreLock.unlock();
		return;
	}

	std::string move;

	int temp = x > 7 ? x + 1 : x;
	move += (char)( temp + INT_TO_COLUMN );

	temp = 19 - y;

	if ( temp > 9 )
	{
		move += '1';
		move += (char)( temp - 10 + INT_TO_ROW );
	}
	else
	{
		move += (char)( temp + INT_TO_ROW );
	}

	m_BestMove = move;
	m_MoveSelected = 19 * x + y;
	m_BestV = score;

	m_ScoreLock.unlock();
}