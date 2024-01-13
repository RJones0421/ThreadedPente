#include "AITemplate.h"

BaseAI::BaseAI( bool botIsWhite, int moveCount )
{
  aiGameBoard = new AIBoard( botIsWhite, moveCount );
}

BaseAI::~BaseAI()
{
  if ( aiGameBoard )
  {
    delete aiGameBoard;
  }
}