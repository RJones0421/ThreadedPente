#include "AIBoard.h"
#include <string>

AIBoard::AIBoard( bool botIsWhite, int moveCount )
  : m_BotIsWhite( botIsWhite ),
  m_MoveCount( moveCount )
{
  Board::Board();
}

bool AIBoard::FindMove( int row, int column, bool isWhite, int depth )
{
  if ( board[row][column] != EMPTY_PIECE )
  {
    return false;
  }
  // Skip center for second white move
  if ( m_MoveCount == 3 )
  {
    if ( row >= 7 && row <= 11 && column >= 7 && column <= 11 )
    {
      return false;
    }
  }
  board[row][column] = isWhite ? WHITE_PIECE : BLACK_PIECE;
  m_LastColumn = column;
  m_LastRow = row;
  CapturePieces( depth );
  return true;
}

void AIBoard::CapturePieces( int depth )
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
      // Encode the capture to allow for reverting
      captured[capturedIndex * 4 + 0] = depth;
      captured[capturedIndex * 4 + 1] = m_LastColumn + offsetX;
      captured[capturedIndex * 4 + 2] = m_LastRow + offsetY;
      captured[capturedIndex * 4 + 3] = piece == WHITE_PIECE ? 0 : 1;

      capturedIndex++;

      captured[capturedIndex * 4 + 0] = depth;
      captured[capturedIndex * 4 + 1] = m_LastColumn + offsetX * 2;
      captured[capturedIndex * 4 + 2] = m_LastRow + offsetY * 2;
      captured[capturedIndex * 4 + 3] = piece == WHITE_PIECE ? 0 : 1;

      capturedIndex++;

      // Remove the pieces
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

void AIBoard::ReturnPieces( int depth )
{
  while ( capturedIndex > 0 && captured[capturedIndex * 4 - 4] == depth )
  {
    capturedIndex--;

    char piece;
    if ( captured[capturedIndex * 4 + 3] == 1 )
    {
      m_CapturedBlack -= 2;
      piece = WHITE_PIECE;
    }
    else
    {
      m_CapturedWhite -= 2;
      piece = BLACK_PIECE;
    }

    int x = captured[capturedIndex * 4 + 1], y = captured[capturedIndex * 4 + 2];

    board[y][x] = piece;

    capturedIndex--;
    x = captured[capturedIndex * 4 + 1];
    y = captured[capturedIndex * 4 + 2];

    board[captured[capturedIndex * 4 + 2]][captured[capturedIndex * 4 + 1]] = piece;
  }
}

// Will return piece back to empty, could add check to make sure piece is there
// but algoirthm should be designed to never let this happen
void AIBoard::UndoMove( int row, int column, int depth )
{
  board[row][column] = EMPTY_PIECE;
  ReturnPieces( depth );
}

// Function to determine how good current position is
// Used one from my previous work
// Could use improvement and is possible future project
int AIBoard::Evaluate()
{
  if ( m_CapturedWhite >= 10 )
  {
    return m_BotIsWhite ? 20000 : -20000;
  }
  else if ( m_CapturedBlack >= 10 )
  {
    return m_BotIsWhite ? -20000 : 20000;
  }

  memset( piecesAway, 0, 6 * sizeof( int ) );

  for ( int x = 0; x < 19; x++ )
  {
    for ( int y = 0; y < 19; y++ )
    {
      if ( board[y][x] == EMPTY_PIECE )
      {
        continue;
      }

      Adjacent( x, y, board[y][x] );
      Diagonal( x, y, board[y][x] );
    }
  }

  if ( piecesAway[4] != 0 )
  {
    return m_BotIsWhite ? WEIGHTS[4] * piecesAway[4] : -WEIGHTS[4] * piecesAway[4];
  }
  if ( piecesAway[5] != 0 )
  {
    return m_BotIsWhite ? WEIGHTS[4] * piecesAway[5] : -WEIGHTS[4] * piecesAway[5];
  }

  int sum = WEIGHTS[0] * piecesAway[0] +
            WEIGHTS[1] * piecesAway[1] +
            WEIGHTS[2] * piecesAway[2] +
            WEIGHTS[3] * piecesAway[3];
  sum +=    WEIGHTS[5] * ( m_CapturedWhite - m_CapturedBlack );

  return m_BotIsWhite ? sum : -sum;
}

// Adjacent and Diagonal work the same way
// Make sure piece is the first piece in the chain to prevent duplicates
// Add to array to sum to evaluation score later
void AIBoard::Adjacent( int x, int y, char piece )
{
  int dx;
  int dy;
  int length;
  int num;
  char temp;

  if ( piece == EMPTY_PIECE )
  {
    return;
  }

  // Horizontal
  // Left
  if ( x >= 4 )
  {
    if ( x == 18 || board[y][x + 1] != piece )
    {
      dx = x;
      dy = y;
      length = 1;
      num = 1;

      while ( length < 5 )
      {
        temp = board[dy][--dx];
        if ( temp == piece )
        {
          num++;
        }
        else if ( temp == EMPTY_PIECE )
        {
          break;
        }

        length++;
      }

      if ( length == 5 && num == 4 && x != 18 && board[y][x + 1] == EMPTY_PIECE && board[y][x - 4] == EMPTY_PIECE )
      {
        piecesAway[5] += piece == WHITE_PIECE ? 1 : -1;
      }
      else if ( length == 5 )
      {
        piecesAway[num - 1] += piece == WHITE_PIECE ? 1 : -1;
      }
    }
  }

  // Right
  if ( x <= 14 )
  {
    if ( x == 0 || board[y][x - 1] != piece )
    {
      dx = x;
      dy = y;
      length = 1;
      num = 1;

      while ( length < 5 )
      {
        temp = board[dy][++dx];
        if ( temp == piece )
        {
          num++;
        }
        else if ( temp != EMPTY_PIECE )
        {
          break;
        }

        length++;
      }

      if ( length == 5 && num == 4 && x != 0 && board[y][x - 1] == EMPTY_PIECE && board[y][x + 4] == EMPTY_PIECE )
      {
        piecesAway[5] += piece == WHITE_PIECE ? 1 : -1;
      }
      else if ( length == 5 )
      {
        piecesAway[num - 1] += piece == WHITE_PIECE ? 1 : -1;
      }
    }
  }

  // Vertical
  // Down
  if ( y >= 4 )
  {
    if ( y == 18 || board[y + 1][x] != piece )
    {
      dx = x;
      dy = y;
      length = 1;
      num = 1;

      while ( length < 5 )
      {
        temp = board[--dy][dx];
        if ( temp == piece )
        {
          num++;
        }
        else if ( temp != EMPTY_PIECE )
        {
          break;
        }

        length++;
      }

      if ( length == 5 && num == 4 && y != 18 && board[y + 1][x] == EMPTY_PIECE && board[y - 4][x] == EMPTY_PIECE )
      {
        piecesAway[5] += piece == WHITE_PIECE ? 1 : -1;
      }
      else if ( length == 5 )
      {
        piecesAway[num - 1] += piece == WHITE_PIECE ? 1 : -1;
      }
    }
  }

  // Up
  if ( y <= 14 )
  {
    if ( y == 0 || board[y - 1][x] != piece )
    {
      dx = x;
      dy = y;
      length = 1;
      num = 1;

      while ( length < 5 )
      {
        temp = board[++dy][dx];
        if ( temp == piece )
        {
          num++;
        }
        else if ( temp != EMPTY_PIECE )
        {
          break;
        }

        length++;
      }

      if ( length == 5 && num == 4 && y != 0 && board[y - 1][x] == EMPTY_PIECE && board[y + 4][x] == EMPTY_PIECE )
      {
        piecesAway[5] += piece == WHITE_PIECE ? 1 : -1;
      }
      else if ( length == 5 )
      {
        piecesAway[num - 1] += piece == WHITE_PIECE ? 1 : -1;
      }
    }
  }
}

void AIBoard::Diagonal( int x, int y, char piece )
{
  int dx;
  int dy;
  int length;
  int num;
  char temp;

  if ( piece == EMPTY_PIECE )
  {
    return;
  }

  // NE
  if ( x <= 14 && y >= 4 )
  {
    if ( x == 0 || y == 18 || board[y + 1][x - 1] != piece )
    {
      dx = x;
      dy = y;
      length = 1;
      num = 1;

      while ( length < 5 )
      {
        temp = board[--dy][++dx];
        if ( temp == piece )
        {
          num++;
        }
        else if ( temp != EMPTY_PIECE )
        {
          break;
        }

        length++;
      }

      if ( length == 5 && num == 4 && x != 0 && y != 18 &&
        board[y + 1][x - 1] == EMPTY_PIECE && board[y - 4][x + 4] == EMPTY_PIECE )
      {
        piecesAway[5] += piece == WHITE_PIECE ? 1 : -1;
      }
      else if ( length == 5 )
      {
        piecesAway[num - 1] += piece == WHITE_PIECE ? 1 : -1;
      }
    }
  }

  // SE
  if ( x <= 14 && y <= 14 )
  {
    if ( x == 0 || y == 0 || board[y - 1][x - 1] != piece )
    {
      dx = x;
      dy = y;
      length = 1;
      num = 1;

      while ( length < 5 )
      {
        temp = board[++dy][++dx];
        if ( temp == piece )
        {
          num++;
        }
        else if ( temp != EMPTY_PIECE )
        {
          break;
        }

        length++;
      }

      if ( length == 5 && num == 4 && x != 0 && y != 0 &&
        board[y - 1][x - 1] == EMPTY_PIECE && board[y + 4][x + 4] == EMPTY_PIECE )
      {
        piecesAway[5] += piece == WHITE_PIECE ? 1 : -1;
      }
      else if ( length == 5 )
      {
        piecesAway[num - 1] += piece == WHITE_PIECE ? 1 : -1;
      }
    }
  }

  // SW
  if ( x >= 4 && y <= 14 )
  {
    if ( x == 18 || y == 0 || board[y - 1][x + 1] != piece )
    {
      dx = x;
      dy = y;
      length = 1;
      num = 1;

      while ( length < 5 )
      {
        temp = board[++dy][--dx];
        if ( temp == piece )
        {
          num++;
        }
        else if ( temp != EMPTY_PIECE )
        {
          break;
        }

        length++;
      }

      if ( length == 5 && num == 4 && x != 18 && y != 0 &&
        board[y - 1][x + 1] == EMPTY_PIECE && board[y + 4][x - 4] == EMPTY_PIECE )
      {
        piecesAway[5] += piece == WHITE_PIECE ? 1 : -1;
      }
      else if ( length == 5 )
      {
        piecesAway[num - 1] += piece == WHITE_PIECE ? 1 : -1;
      }
    }
  }

  // NW
  if ( x >= 4 && y >= 4 )
  {
    if ( x == 18 || y == 18 || board[y + 1][x + 1] != piece )
    {
      dx = x;
      dy = y;
      length = 1;
      num = 1;

      while ( length < 5 )
      {
        temp = board[--dy][--dx];
        if ( temp == piece )
        {
          num++;
        }
        else if ( temp != EMPTY_PIECE )
        {
          break;
        }

        length++;
      }

      if ( length == 5 && num == 4 && x != 18 && y != 18 &&
        board[y + 1][x + 1] == EMPTY_PIECE && board[y - 4][x - 4] == EMPTY_PIECE )
      {
        piecesAway[5] += piece == WHITE_PIECE ? 1 : -1;
      }
      else if ( length == 5 )
      {
        piecesAway[num - 1] += piece == WHITE_PIECE ? 1 : -1;
      }
    }
  }
}