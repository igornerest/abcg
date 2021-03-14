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

    float m_width{0.3f};

    bool m_dead{false};
    bool m_behindBird{false};

    glm::vec4 m_color{0.0, 0.75, 0.4, 1}; // green
    glm::vec2 m_translation{+1.0f + m_width, +0.0f};
  };

  std::list<Pipe> m_pipes;

  std::default_random_engine m_randomEngine;

  float m_lastPipeDistance{0};

  Pipes::Pipe createPipe(const Bird &bird);
};

#endif