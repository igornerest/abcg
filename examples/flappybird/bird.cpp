#include "bird.hpp"

#include <cppitertools/itertools.hpp>

void Bird::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_pointSizeLoc = glGetUniformLocation(m_program, "pointSize");
  m_translationLoc = glGetUniformLocation(m_program, "translation");
  m_colorLoc = glGetUniformLocation(m_program, "color");

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);

  float fullAngle{2*M_PI};
  float mounthAngle{M_PI/6};
  float step{(fullAngle - mounthAngle) / m_defaultPolygonSides};
  for (auto angle : iter::range(mounthAngle, fullAngle - mounthAngle, step)) {
    positions.emplace_back(m_birdRadius * std::cos(angle), m_birdRadius * std::sin(angle));
  }

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

void Bird::paintGL() {
  glUseProgram(m_program);
  // Start using buffers created in createBuffers()
  glBindVertexArray(m_vao);

  glUniform4fv(m_colorLoc, 1, &m_color.r);
  // Draw a single point
  glDrawArrays(GL_TRIANGLE_FAN, 0, m_defaultPolygonSides);
  // End using VAO
  glBindVertexArray(0);
  // End using the shader program
  glUseProgram(0);
}

void Bird::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void Bird::update(float deltaTime) {
}