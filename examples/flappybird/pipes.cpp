#include "pipes.hpp"

#include <cppitertools/itertools.hpp>
#include <iostream>
void Pipes::initializeGL(GLuint program) {
  terminateGL();

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  // Create asteroids
  m_pipes.clear();
  m_pipes.resize(1);

  for (auto &pipe : m_pipes) {
    pipe = createPipe();
  }
}

void Pipes::paintGL() {
  glUseProgram(m_program);

  for (auto &pipe : m_pipes) {
    glBindVertexArray(pipe.m_vao);

    glUniform4fv(m_colorLoc, 1, &pipe.m_color.r);
    glUniform2f(m_translationLoc, pipe.m_translation.x, pipe.m_translation.y);

    glDrawElements(GL_TRIANGLES, 4 * 3, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Pipes::terminateGL() {
  for (auto pipe : m_pipes) {
    glDeleteBuffers(1, &pipe.m_vbo);
    glDeleteVertexArrays(1, &pipe.m_vao);
  }
}

void Pipes::update(float deltaTime) {
  // At least 250 ms must have passed to create new pipe
  if (m_pipeCooldownTimer.elapsed() > 4000.0 / 1000.0) {
    m_pipeCooldownTimer.restart();
    m_pipes.push_back(createPipe());
  }

  float birdVelocity{0.3f};
  for (auto &pipe : m_pipes) {
    pipe.m_translation.x -= birdVelocity * deltaTime;
    
    if (pipe.m_translation.x < -1.5f) {
      pipe.m_dead = true;
    }
  }

  // Remove dead pipes
  m_pipes.remove_if([](const Pipe &p) { return p.m_dead; });
}

Pipes::Pipe Pipes::createPipe() {
  Pipe pipe;
  
  float middleDistance{0.15f};

  auto &re{m_randomEngine};  
  std::uniform_real_distribution<float> randomMiddle{-0.9f + middleDistance, 0.9f - middleDistance};
  
  float middlePosition{randomMiddle(re)};
  pipe.m_upperPipeBottom = middlePosition + middleDistance;
  pipe.m_lowerPipeTop = middlePosition - middleDistance;

  // Create geometry
  std::array<glm::vec2, 8> positions{
    // upper pipe
    glm::vec2{pipe.m_pipeLeft, pipe.m_upperPipeTop}, 
    glm::vec2{pipe.m_pipeRight, pipe.m_upperPipeTop},
    glm::vec2{pipe.m_pipeLeft, pipe.m_upperPipeBottom}, 
    glm::vec2{pipe.m_pipeRight, pipe.m_upperPipeBottom},
    // lower pipe
    glm::vec2{pipe.m_pipeLeft, pipe.m_lowerPipeTop}, 
    glm::vec2{pipe.m_pipeRight, pipe.m_lowerPipeTop},
    glm::vec2{pipe.m_pipeLeft, pipe.m_lowerPipeBottom}, 
    glm::vec2{pipe.m_pipeRight, pipe.m_lowerPipeBottom}
  };

  std::array indices{0, 1, 2,
                     1, 2, 3,
                     4, 5, 6,
                     5, 6, 7};
  
  // Generate VBO
  glGenBuffers(1, &pipe.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, pipe.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2), positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &pipe.m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pipe.m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &pipe.m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(pipe.m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, pipe.m_vbo);
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pipe.m_ebo);

  // End of binding to current VAO
  glBindVertexArray(0);

  return pipe;
}