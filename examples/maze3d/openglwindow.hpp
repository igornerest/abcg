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

  int m_viewportWidth{};
  int m_viewportHeight{};

  Model m_grassModel;
  Model m_wallModel;
  Model m_skyModel;
  
  Maze m_maze;

  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};

  bool m_screenFocus{false};
  bool m_isFlashlightOn{true};
  abcg::ElapsedTimer m_mouseTimer{};

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

  void renderMaze();
  void renderSkybox();
  void update();
  glm::vec2 getRotationSpeedFromMouse();
};

#endif