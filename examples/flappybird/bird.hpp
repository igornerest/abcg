#ifndef BIRD_HPP_
#define BIRD_HPP_

#include <array>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;
class Pipes;

class Bird {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  void update(const GameData &gameData);

 private:
  friend OpenGLWindow;
  friend Pipes;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_rotationLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};

  glm::vec4 m_color{0.75, 0.75, 0.0, 1.0}; // yellow
  glm::vec2 m_translation{0.0f, 0.0f};
  glm::vec2 m_velocity{0.3f, 0.0f};
  
  float m_rotation{0.0f};
  float m_radius{0.1f};
  
  int m_closedMouthPolygonSides{40};
  int m_openMouthPolygonSides{0};

  std::default_random_engine m_randomEngine;

  abcg::ElapsedTimer m_jumpCooldownTimer;
  abcg::ElapsedTimer m_moveCooldownTimer;
  abcg::ElapsedTimer m_mouthBlinkTimer;

  void setupModel(std::vector<glm::vec2> positions);
  void resetBird();
};

#endif