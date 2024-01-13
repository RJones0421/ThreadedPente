#include "SimpleAI.h"
#include <limits>
#include <iostream>

SimpleAI::SimpleAI( bool botIsWhite, int moveCount )
	: BaseAI( botIsWhite, moveCount ),
	m_AiIsWhite( botIsWhite ),
	m_BestV( INT_MIN )
{

}

int SimpleAI::Move( int moveCount )
{
	aiGameBoard->SetMoveCount( moveCount );

	AlphaBeta();

  return m_MoveSelected;
}

void SimpleAI::PlayMove( int row, int column )
{
	aiGameBoard->PlayTurn( row, column, m_AiIsWhite );
}

void SimpleAI::AlphaBeta()
{
	int v;
	m_BestV = INT_MIN;

	v = MaxValue( INT_MIN, INT_MAX, 1 );

	std::cout << "The bot has made move " << m_BestMove << std::endl;
}

int SimpleAI::MaxValue( int alpha, int beta, int depth )
{
	bool firstValid = false;
	bool hasTested = false;

	int v = INT_MIN;

	for ( int y = 0; y < 19; y++ )
	{
		for ( int x = 0; x < 19; x++ )
		{
			if ( !aiGameBoard->FindMove( y, x, m_AiIsWhite, 1 ) ) continue;

			// Set highest evaluation value
			if ( aiGameBoard->IsGameOver() )
			{
				v = std::max( v, 20000 );
			}
			else if ( depth == MAX_DEPTH )
			{
				v = std::max( v, aiGameBoard->Evaluate() );
			}
			else
			{
				v = std::max( v, MinValue( alpha, beta, depth + 1 ) );
			}

			if ( v >= beta )
			{
				aiGameBoard->UndoMove( y, x, depth );
				return v;
			}
			alpha = std::max( alpha, v );

			if ( depth == 1 && v > m_BestV )
			{
				std::string move;

				int temp = 19 - y;

				if ( temp > 9 )
				{
					move += '1';
					move += (char)( temp - 10 + INT_TO_ROW );
				}
				else move += (char)( temp + INT_TO_ROW );
				temp = x > 7 ? x + 1 : x;
				move += (char)( temp + INT_TO_COLUMN );

				m_BestMove = move;
				m_MoveSelected = 19 * x + y;
				m_BestV = v;
			}

			aiGameBoard->UndoMove( y, x, depth );
		}
	}

	return v;
}

int SimpleAI::MinValue( int alpha, int beta, int depth )
{
	int v = INT_MAX;

	for ( int y = 0; y < 19; y++ )
	{
		for ( int x = 0; x < 19; x++ )
		{
			if ( !aiGameBoard->FindMove( y, x, !m_AiIsWhite, depth ) ) continue;

			// Set lowest evaluation number
			if ( aiGameBoard->IsGameOver() )
			{
				v = std::min( v, -20000 );
			}
			else if ( depth == MAX_DEPTH )
			{
				v = std::min( v, aiGameBoard->Evaluate() );
			}
			else
			{
				v = std::min( v, MaxValue( alpha, beta, depth + 1 ) );
			}

			if ( v <= alpha )
			{
				aiGameBoard->UndoMove( y, x, depth );
				return v;
			}
			beta = std::min( beta, v );

			aiGameBoard->UndoMove( y, x, depth );
		}
	}

	return v;
}
