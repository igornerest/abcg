#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class State { Playing, GameOver };

struct GameData {
  State m_state{State::Playing};
  bool m_shouldJump{false};
};

#endif