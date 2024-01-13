#include "Board.h"

#include <iostream>

Board::Board()
{
  ClearBoard();
}

void Board::ClearBoard()
{
  memset( board, EMPTY_PIECE, sizeof( board ) );
}

void Board::DisplayBoard() const
{
  std::cout << "   ";
  for ( int i = 0; i < SIDE_LENGTH; i++ )
  {
    std::cout << " " << (char)( 'A' + ( i < 8 ? i : i + 1 ) );
  }
  std::cout << std::endl;

  for ( int y = 0; y < SIDE_LENGTH; y++ )
  {
    if ( y > 9 )
    {
      printf( " " );
    }
    printf( " %d", SIDE_LENGTH - y );
    for ( int x = 0; x < SIDE_LENGTH; x++ )
    {
      std::cout << " " << board[y][x];
    }
    std::cout << std::endl;
  }

  std::cout <<    "Captured White Pieces: " << m_CapturedWhite;
  std::cout << "   Captured Black Pieces: " << m_CapturedBlack << std::endl;
}

bool Board::PlayTurn( int row, int column, bool isWhite )
{
  if ( board[row][column] != EMPTY_PIECE ) return false;
  board[row][column] = isWhite ? WHITE_PIECE : BLACK_PIECE;
  m_LastColumn = column;
  m_LastRow = row;
  CapturePieces();
  return true;
}

void Board::CapturePieces()
{
  char piece = board[m_LastRow][m_LastColumn];
  char oppPiece = piece == WHITE_PIECE ? BLACK_PIECE : WHITE_PIECE;

  for ( int i = 0; i < 8; i++ )
  {
    int offsetX = DIRECTIONS[i][0];
    int offsetY = DIRECTIONS[i][1];

    int dx = m_LastColumn + offsetX * 3;
    int dy = m_LastRow + offsetY * 3;

    if ( dx >= 19 || dx < 0 || dy >= 19 || dy < 0 )
    {
      continue;
    }

    if ( board[m_LastRow + offsetY][m_LastColumn + offsetX] == oppPiece &&
      board[m_LastRow + offsetY * 2][m_LastColumn + offsetX * 2] == oppPiece &&
      board[m_LastRow + offsetY * 3][m_LastColumn + offsetX * 3] == piece )
    {
      board[m_LastRow + offsetY][m_LastColumn + offsetX] = EMPTY_PIECE;
      board[m_LastRow + offsetY * 2][m_LastColumn + offsetX * 2] = EMPTY_PIECE;

      if ( piece == BLACK_PIECE )
      {
        m_CapturedWhite += 2;
      }
      else
      {
        m_CapturedBlack += 2;
      }

      if ( m_CapturedBlack >= 10 || m_CapturedWhite >= 10 )
      {
        return;
      }
    }
  }

}

bool Board::IsGameOver()
{
  // Check victory by captures
  if ( m_CapturedWhite >= 10 || m_CapturedBlack >= 10 )
  {
    return true;
  }

  char piece = board[m_LastRow][m_LastColumn];
  // Check victory by 5-in-a-row
  for ( int ndx = 0; ndx < 4; ndx++ )
  {
    int rightX = DIRECTIONS[ndx * 2][0];
    int rightY = DIRECTIONS[ndx * 2][1];
    int leftX = DIRECTIONS[ndx * 2 + 1][0];
    int leftY = DIRECTIONS[ndx * 2 + 1][1];

    int counter = 1;
    int dx = m_LastColumn;
    int dy = m_LastRow;

    bool countingLeft = true;
    bool countingRight = true;

    // Check going one direction from placed piece
    while ( countingLeft )
    {
      dx += leftX;
      dy += leftY;
      // Verify position on board and matching piece
      if ( dx < 0 || dx >= 19 || dy < 0 || dy >= 19 || board[dy][dx] != piece )
      {
        break;
      }
      // Increment length and check if long enough
      else
      {
        counter++;
        if ( counter >= 5 )
        {
          return true;
        }
      }
    }

    dx = m_LastColumn;
    dy = m_LastRow;
    // Checking going other direction from placed piece
    while ( countingRight )
    {
      dx += rightX;
      dy += rightY;
      // Verify position on board and matching piece
      if ( dx < 0 || dx >= 19 || dy < 0 || dy >= 19 || board[dy][dx] != piece )
      {
        break;
      }
      // Increment length and check if long enough
      else
      {
        counter++;
        if ( counter >= 5 )
        {
          return true;
        }
      }
    }
  }

  return false;
}