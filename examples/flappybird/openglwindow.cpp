#include "openglwindow.hpp"

#include <imgui.h>
#include <algorithm> 

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_shouldJump = true;
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_shouldJump = false;
  }

  // Mouse events
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_shouldJump = true;
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_shouldJump = false;
  }
}

void OpenGLWindow::initializeGL() {
  // Create program to render the other objects
  m_program = createProgramFromFile(getAssetsPath() + "objects.vert",
                                    getAssetsPath() + "objects.frag");

  glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  restart();
}

void OpenGLWindow::restart() {
  m_pipes.initializeGL(m_program);
  m_bird.initializeGL(m_program);

  m_gameData.m_state = State::Playing;
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state == State::GameOver && m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  m_bird.update(m_gameData, deltaTime);
  m_pipes.update(m_bird, m_gameData, deltaTime);

  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
  }
}

void OpenGLWindow::paintGL() {
  update();

  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_pipes.paintGL();
  m_bird.paintGL();
}

void OpenGLWindow::paintUI() {
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_program);

  m_pipes.terminateGL();
  m_bird.terminateGL();
}

void OpenGLWindow::checkCollisions() {
  bool hasCollided{false};
  bool hasFallen{false};

  // check collision between bird and pipes
  for (auto &pipe : m_pipes.m_pipes) {
    if (std::abs(pipe.m_translation.x) < pipe.m_width/2 + m_bird.m_radius) {
      if (m_bird.m_translation.y + m_bird.m_radius > pipe.m_upperPipeBottom || 
          m_bird.m_translation.y - m_bird.m_radius < pipe.m_lowerPipeTop) {
            hasCollided = true;
            break;
      }
    }
  }

  // check if bird has fallen
  if (std::abs(m_bird.m_translation.y) + m_bird.m_radius > 1.0f) {
    hasFallen = true;
  }

  if (hasCollided || hasFallen) {
    m_gameData.m_state = State::GameOver;
    m_restartWaitTimer.restart();
  }
}
