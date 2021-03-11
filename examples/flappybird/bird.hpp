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
  void paintGL();
  void terminateGL();

  void update(const GameData &gameData, float deltaTime);

 private:
  friend OpenGLWindow;
  friend Pipes;

  GLuint m_program{};
  GLint m_pointSizeLoc{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};

  glm::vec4 m_color{1};
  glm::vec2 m_translation{0, 0};
  glm::vec2 m_birdLocation{0.0f, 0.0f};

  float m_acceleration{0.0f};
  float m_birdRadius{0.1f};
  
  int m_defaultPolygonSides{40};

  std::default_random_engine m_randomEngine;

  abcg::ElapsedTimer m_jumpCooldownTimer;
  abcg::ElapsedTimer m_moveCooldownTimer;

  void setupModel(std::vector<glm::vec2> positions);
};

#endif