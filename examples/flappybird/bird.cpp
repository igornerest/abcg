#include "bird.hpp"

#include <cppitertools/itertools.hpp>
#include <algorithm> 

void Bird::initializeGL(GLuint program) {
  terminateGL();
  resetBird();

  m_program = program;
  m_translationLoc = glGetUniformLocation(m_program, "translation");
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_rotationLoc = glGetUniformLocation(m_program, "rotation");
  
  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);

  double mouthAngle{M_PI / 6};
  double step{2 * M_PI / m_closedMouthPolygonSides};

  for (auto angle : iter::range(mouthAngle,  2 * M_PI + mouthAngle, step)) {
    positions.emplace_back(m_radius * std::cos(angle), m_radius * std::sin(angle));
    m_openMouthPolygonSides += (angle > mouthAngle && angle < 2 * M_PI - mouthAngle);
  }
  positions.push_back(positions.at(1));

  setupModel(positions);
}

void Bird::setupModel(std::vector<glm::vec2> positions) {
  // ** Generate VBO
  // Generate a new VBO and get the associated ID
  glGenBuffers(1, &m_vbo);
  // Bind VBO in order to use it
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  // Upload data to VBO
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2), positions.data(), GL_STATIC_DRAW);
  // Unbinding the VBO is allowed (data can be released now)
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute = glGetAttribLocation(m_program, "inPosition");

  // ** Create VAO
  glGenVertexArrays(1, &m_vao);
  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);
  // Bind vertex attributes to current VAO
  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Bird::paintGL(const GameData &gameData) {
  glUseProgram(m_program);
  // Start using buffers created in createBuffers()
  glBindVertexArray(m_vao);

  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glUniform2f(m_translationLoc, m_translation.x, m_translation.y);
  glUniform1f(m_rotationLoc, m_rotation);

  // Each mouth movement takes 500ms
  if (m_mouthBlinkTimer.elapsed() > 1000.0 / 1000.0) {
    m_mouthBlinkTimer.restart();
  }
  int polygonSides = m_mouthBlinkTimer.elapsed() < 500.0 / 1000.0 && gameData.m_state == State::Playing ?
                     m_closedMouthPolygonSides + 2:
                     m_openMouthPolygonSides;

  // Draw a single point
  glDrawArrays(GL_TRIANGLE_FAN, 0, polygonSides);
  // End using VAO
  glBindVertexArray(0);
  // End using the shader program
  glUseProgram(0);
}

void Bird::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void Bird::update(const GameData &gameData, float deltaTime) {
  if (m_translation.y - m_radius > -1.0f) {
    if (m_moveCooldownTimer.elapsed() > 10.0 / 1000.0) {
        m_moveCooldownTimer.restart();

        if (gameData.m_state == State::Playing) {
          m_velocity.x += 0.0001f;
          m_velocity.y = gameData.m_shouldJump ? std::max(m_velocity.y, 0.01f) : m_velocity.y - 0.001f;
        }

        m_translation.y += m_velocity.y;
        m_rotation = -std::atan2(-m_velocity.y, m_radius/4);
    }
  }
}

void Bird::resetBird() {
  m_translation = glm::vec2(0.0f, 0.0f);
  m_velocity = glm::vec2(0.3f, 0.0f);
  m_rotation = 0.0f;
}