#pragma once

const char EMPTY_PIECE = (char)250;
const char BLACK_PIECE = 'O';
const char WHITE_PIECE = (char)254;

const int INT_TO_ROW = (int)'0';
const int INT_TO_COLUMN = (int)'A';

const int SIDE_LENGTH = 19;

const int DIRECTIONS[8][2]{
  {0,1},
  {0,-1},
  {1,0},
  {-1,0},
  {1,1},
  {-1,-1},
  {-1,1},
  {1,-1}
};
