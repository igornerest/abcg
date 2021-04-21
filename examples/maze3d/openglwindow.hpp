#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include "abcg.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "maze.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private: 
  GLuint m_program{};
  GLuint m_skyProgram{};
  GLuint m_finalscreenTexture{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  Model m_grassModel;
  Model m_wallModel;
  Model m_flagModel;
  Model m_skyModel;

  Maze m_maze;

  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_moonAngle{0.0f};

  bool m_screenFocus{false};
  bool m_isFlashlightOn{true};
  bool m_gameOver{false};
  bool m_gameOverSound{false};

  abcg::ElapsedTimer m_mouseTimer{};
  abcg::ElapsedTimer m_moonTimer{};
  abcg::ElapsedTimer m_gameOverTimer{};

  // Mapping mode
  // 0: triplanar; 1: cylindrical; 2: spherical; 3: from mesh
  int m_mappingMode{};

  // Light properties
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  float m_lightCutOff{0.98f};
  float m_lightOuterCutOff{0.92f};
  float m_lightOff{2.00f};

  // Material properties
  glm::vec4 m_Ka;
  glm::vec4 m_Kd;
  glm::vec4 m_Ks;
  float m_shininess{};

  // Audio elements
  SDL_AudioDeviceID m_deviceId;
  Uint8 *m_wavBuffer;

  void renderMaze();
  void renderSkybox();
  void update();
  void initializeSound(std::string path);
  void initializeModels();
  void initializeGameObjects();
  glm::vec2 getRotationSpeedFromMouse();
};

#endif