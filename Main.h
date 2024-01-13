#pragma once
#include "AI/AITemplate.h"

class GameManager
{
public:
  ~GameManager();

  void GameSetup();
  void ChooseColor();
  void ChooseAIType();
  void RunStep();
  void HandlePlayerMove();

private:
  bool    m_IsPlayerWhite;
  int     m_MoveCount = 2;
  BaseAI* m_AIPlayer;
};