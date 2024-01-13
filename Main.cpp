// This file will serve as the game manager possibly

#include "Main.h"
#include "Board.h"
#include "AI/SimpleAI.h"
#include "AI/ThreadedAI.h"

#include <iostream>
#include <thread>

const std::string SEPERATOR = "\n=======================================\n";

Board       gameBoard;
GameManager gameManager;

int StrToInt( std::string input )
{
  int num = 0;
  for ( char c : input )
  {
    if ( c < '0' || c > '9' )
    {
      return -1;
    }

    num *= 10;
    num += (int)( c - '0' );
  }

  return num;
}

int main()
{
  gameManager.GameSetup();

  // White has to start in center
  // This counts as the first move of the game
  gameBoard.PlayTurn( 9, 9, true );
  gameBoard.DisplayBoard();
  std::cout << SEPERATOR << std::endl;

  while ( true )
  {
    gameManager.RunStep();
    gameBoard.DisplayBoard();
    std::cout << SEPERATOR << std::endl;

    if ( gameBoard.IsGameOver() )
    {
      std::cout << "Game is over!\n" << SEPERATOR;
      return 0;
    }
  }
}

GameManager::~GameManager()
{
  if ( m_AIPlayer )
  {
    delete m_AIPlayer;
  }
}

void GameManager::GameSetup()
{
  ChooseColor();
  ChooseAIType();

  std::cout << SEPERATOR << std::endl;
  std::cout << "To mitigate first-player-advanage, white must start in the center.\n" \
                "For white's second move, they must avoid the center 5x5 square.\n" \
                "Black has no restrictions on placement.\n";
  std::cout << SEPERATOR << std::endl;
}

void GameManager::ChooseColor()
{
  while ( true )
  {
    std::cout << "Would you like to play as WHITE or BLACK: ";
    std::string playerChoice;
    std::cin >> playerChoice;

    for ( char& c : playerChoice )
    {
      c = std::toupper( c );
    }

    if ( playerChoice == "W" || playerChoice == "WHITE" )
    {
      std::cout << "The player will play as white, the AI will play as black.\n\n";
      m_IsPlayerWhite = true;
      break;
    }

    if ( playerChoice == "B" || playerChoice == "BLACK" )
    {
      std::cout << "The player will play as black, the AI will play as white.\n\n";
      m_IsPlayerWhite = false;
      break;
    }

    // Bad input, reloop
    std::cout << "Input does not match, please put WHITE or BLACK.\n\n";
  }
}

void GameManager::ChooseAIType()
{
  while ( true )
  {
    std::cout << "Will the AI be SINGLE or MULTI threaded: ";
    std::string playerChoice;
    std::cin >> playerChoice;

    for ( char& c : playerChoice )
    {
      c = std::toupper( c );
    }

    if ( playerChoice == "S" || playerChoice == "SINGLE" )
    {
      std::cout << "The AI will be single threaded.\n\n";
      m_AIPlayer = new SimpleAI( !m_IsPlayerWhite, m_MoveCount );
      break;
    }

    if ( playerChoice == "M" || playerChoice == "MULTI" )
    {
      std::cout << "The AI will be multi threaded.\n\n";
      m_AIPlayer = new ThreadedAI( !m_IsPlayerWhite, m_MoveCount );
      break;
    }

    // Bad input, reloop
    std::cout << "Input does not match, please put SINGLE or MULTI.\n\n";
  }
}

void GameManager::RunStep()
{
  bool isWhiteTurn = m_MoveCount % 2 == 1;
  bool isPlayerTurn = isWhiteTurn == m_IsPlayerWhite;


  if ( isPlayerTurn )
  {
    HandlePlayerMove();
  }
  else
  {
    int move = m_AIPlayer->Move( m_MoveCount );
    int column = move / 19;
    int row = move % 19;
    gameBoard.PlayTurn( row, column, !m_IsPlayerWhite );
    m_AIPlayer->PlayMove( row, column );
  }

  m_MoveCount++;
}

void GameManager::HandlePlayerMove()
{
  int column;
  int row;
  while ( true )
  {
    std::cout << "Enter your move (e.g. K10): ";
    std::string playerMove;
    std::cin >> playerMove;
    playerMove[0] = std::toupper( playerMove[0] );

    // All input will be 2 or 3 characters
    // 1 letter, 1-2 numbers
    if ( playerMove.size() < 2 || playerMove.size() > 3 )
    {
      std::cout << "Bad input: Make sure input is a letter and number.\n\n";
      continue;
    }

    // Check the column
    if ( playerMove[0] < 'A' || playerMove[0] == 'I' || playerMove[0] > 'T' )
    {
      std::cout << "Bad input: The column (letter) is not a valid column.\n\n";
      continue;
    }
    // Set the column
    column = (int)( playerMove[0] - 'A' );
    if ( column > 7 )
    {
      column--;
    }

    // Check the row
    row = StrToInt( playerMove.substr( 1, 2 ) );
    if ( row < 1 || row > 19 )
    {
      std::cout << "Bad input: The row (number) is not a valid row.\n\n";
      continue;
    }

    // Validate if second white move
    if ( m_MoveCount == 3 )
    {
      if ( row >= 8 && row <= 12 && column >= 8 && column <= 12 )
      {
        std::cout << "White's second move cannot be inside the center 5x5.\n\n";
        continue;
      }
    }

    if ( gameBoard.PlayTurn( 19 - row, column, m_IsPlayerWhite ) )
    {
      break;
    }
    else
    {
      std::cout << "That position is already taken, please make a different move.\n\n";
      gameBoard.DisplayBoard();
    }
  }
}