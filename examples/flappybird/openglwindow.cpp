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
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render pipes
  m_pipeProgram = createProgramFromFile(getAssetsPath() + "pipe.vert",
                                        getAssetsPath() + "pipe.frag");
  // Create program to render the bird
  m_birdProgram = createProgramFromFile(getAssetsPath() + "bird.vert",
                                        getAssetsPath() + "bird.frag");

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
  m_pipes.initializeGL(m_pipeProgram);
  m_bird.initializeGL(m_birdProgram);

  m_gameData.m_state = State::Playing;
  m_gameData.m_score = 0;
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

  glClearColor(0.3, 0.5, 0.8, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_pipes.paintGL();
  m_bird.paintGL(m_gameData);
}

void OpenGLWindow::paintUI() {
  // display score
  {
    std::string scoreStr = std::to_string(m_gameData.m_score);

    auto size{ImVec2(scoreStr.size() * 40, 85)};
    auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                         (m_viewportHeight - size.y) / 4.0f)};

    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                            ImGuiWindowFlags_NoTitleBar |
                            ImGuiWindowFlags_NoInputs};

    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);
    ImGui::Text(scoreStr.c_str());
    ImGui::PopFont();
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_birdProgram);
  glDeleteProgram(m_pipeProgram);
  m_pipes.terminateGL();
  m_bird.terminateGL();
}

void OpenGLWindow::checkCollisions() {
  bool hasCollided{false};
  bool hasFallen{false};

  for (auto &pipe : m_pipes.m_pipes) {
    if (std::abs(pipe.m_translation.x) < pipe.m_width/2 + m_bird.m_radius) {
      // check collision between bird and pipes
      if (m_bird.m_translation.y + m_bird.m_radius > pipe.m_upperPipeBottom || 
          m_bird.m_translation.y - m_bird.m_radius < pipe.m_lowerPipeTop) {
            hasCollided = true;
      }

      // check if bird passed a pair of pipes
      if (pipe.m_translation.x < -(m_bird.m_radius * 2) && !pipe.m_behindBird) {
        pipe.m_behindBird = true;
        m_gameData.m_score++;
      }
    }
  }

  // check if bird has fallen
  if (m_bird.m_translation.y - m_bird.m_radius < -1.0f) {
    hasFallen = true;
  }

  if (hasCollided || hasFallen) {
    m_gameData.m_state = State::GameOver;
    m_restartWaitTimer.restart();
  }
}
