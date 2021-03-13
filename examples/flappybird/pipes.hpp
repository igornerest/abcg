#ifndef PIPES_HPP_
#define PIPES_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "bird.hpp"

class OpenGLWindow;

class Pipes {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void update(const Bird &bird, const GameData &gameData, float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};

  struct Pipe {
    GLuint m_vao{};
    GLuint m_ebo{};
    GLuint m_vbo{};

    float m_upperPipeTop{+1.0f};
    float m_upperPipeBottom;
    
    float m_lowerPipeTop;
    float m_lowerPipeBottom{-1.0f};

    float m_width{0.4f};

    bool m_dead{false};
    bool m_behindBird{false};

    glm::vec4 m_color{1};
    glm::vec2 m_translation{glm::vec2{+1.0f + m_width, +0.0f}};
  };

  std::list<Pipe> m_pipes;

  std::default_random_engine m_randomEngine;

  abcg::ElapsedTimer m_pipeCooldownTimer;

  Pipes::Pipe createPipe(const Bird &bird);
};

#endif