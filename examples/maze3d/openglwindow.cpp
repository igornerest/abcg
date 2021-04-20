#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>

const auto epsilon{std::numeric_limits<float>::epsilon()};

void OpenGLWindow::handleEvent(SDL_Event& ev) {
  if (ev.type == SDL_KEYDOWN) {
    if (ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;

    if (ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    
    if (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a)
      m_truckSpeed = -1.0f;
    
    if (ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d)
      m_truckSpeed = 1.0f;

    if (ev.key.keysym.sym == SDLK_f)
      m_isFlashlightOn = !m_isFlashlightOn;
    
    if (ev.key.keysym.sym == SDLK_ESCAPE)
      m_screenFocus = false;
  }

  if (ev.type == SDL_KEYUP) {
    if ((ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w) && m_dollySpeed > 0)
      m_dollySpeed = 0.0f;

    if ((ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s) && m_dollySpeed < 0)
      m_dollySpeed = 0.0f;

    if ((ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a) && m_truckSpeed < 0)
      m_truckSpeed = 0.0f;

    if ((ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d) && m_truckSpeed > 0)
      m_truckSpeed = 0.0f;
  }

  if (ev.type == SDL_MOUSEBUTTONDOWN) {
    m_screenFocus = true;
  }
}

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  glEnable(GL_DEPTH_TEST);

  // Create programs
  m_program = createProgramFromFile(getAssetsPath() + "shaders/normalmapping.vert",
                                    getAssetsPath() + "shaders/normalmapping.frag");

  m_skyProgram = createProgramFromFile(getAssetsPath() + "shaders/skybox.vert",
                                       getAssetsPath() + "shaders/skybox.frag");

  // Load models
  m_grassModel.loadFromFile(getAssetsPath() + "grass.obj", false);
  m_grassModel.setupVAO(m_program);

  m_wallModel.loadFromFile(getAssetsPath() + "wall.obj", false);
  m_wallModel.setupVAO(m_program);

  // Load cubemap
  m_skyModel.loadFromFile(getAssetsPath() + "skybox.obj", false);
  m_skyModel.loadCubeTexture(getAssetsPath() + "maps/cube/");
  m_skyModel.setupVAO(m_skyProgram);

  // Use material properties from the loaded model (they are the same)
  m_Ka = m_wallModel.getKa();
  m_Kd = m_wallModel.getKd();
  m_Ks = m_wallModel.getKs();
  m_shininess = m_wallModel.getShininess();
  m_mappingMode = 3;  // "From mesh" option

  m_maze.initializeMaze(getAssetsPath() + "levels/level1.txt");
  
  m_camera.initializeCamera(m_maze);
}

void OpenGLWindow::paintGL() {
  update();

  // Clear color buffer and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  renderMaze();
  renderSkybox();
}

void OpenGLWindow::paintUI() { 
  abcg::OpenGLWindow::paintUI(); 
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() { 
  glDeleteProgram(m_program); 
  glDeleteProgram(m_skyProgram);
}

void OpenGLWindow::renderMaze() {
  glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  GLint modelMatrixLoc{glGetUniformLocation(m_program, "modelMatrix")};
  GLint viewMatrixLoc{glGetUniformLocation(m_program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_program, "projMatrix")};
  GLint normalMatrixLoc{glGetUniformLocation(m_program, "normalMatrix")};
  
  GLint lightDirLoc{glGetUniformLocation(m_program, "lightDirWorldSpace")};
  GLint lightPosLoc{glGetUniformLocation(m_program, "lightPosWorldSpace")};
  GLint lightCutOffLoc{glGetUniformLocation(m_program, "lightCutOff")};
  GLint lightOuterCutOffLoc{glGetUniformLocation(m_program, "lightOuterCutOff")};

  GLint IaLoc{glGetUniformLocation(m_program, "Ia")};
  GLint IdLoc{glGetUniformLocation(m_program, "Id")};
  GLint IsLoc{glGetUniformLocation(m_program, "Is")};
  GLint KaLoc{glGetUniformLocation(m_program, "Ka")};
  GLint KdLoc{glGetUniformLocation(m_program, "Kd")};
  GLint KsLoc{glGetUniformLocation(m_program, "Ks")};
  GLint shininessLoc{glGetUniformLocation(m_program, "shininess")};

  GLint diffuseTexLoc{glGetUniformLocation(m_program, "diffuseTex")};
  GLint normalTexLoc{glGetUniformLocation(m_program, "normalTex")};
  GLint mappingModeLoc{glGetUniformLocation(m_program, "mappingMode")};

  // Set uniform variables that will be used for every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_camera.m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_camera.m_projMatrix[0][0]);
  glUniform1i(diffuseTexLoc, 0);
  glUniform1i(normalTexLoc, 1);
  glUniform1i(mappingModeLoc, m_mappingMode);

  glm::vec4 lightDir(m_camera.m_at - m_camera.m_eye, 0.0f);
  glm::vec4 lightPos(m_camera.m_eye, 1.0f);
  glUniform4fv(lightDirLoc, 1, &lightDir.x);
  glUniform4fv(lightPosLoc, 1, &lightPos.x);
  glUniform1f(lightCutOffLoc, m_isFlashlightOn ? m_lightCutOff : m_lightOff);
  glUniform1f(lightOuterCutOffLoc, m_isFlashlightOn ? m_lightOuterCutOff : m_lightOff);

  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);
  glUniform1f(shininessLoc, m_shininess);

  // Draw all wall boxes by setting uniform variables of the current object
  for (size_t i = 0; i < m_maze.m_mazeMatrix.size(); i++) {
    for (size_t j = 0; j < m_maze.m_mazeMatrix[i].size(); j++) {
      float xPos =  static_cast<float>(i);
      float yPos =  static_cast<float>(j);

      glm::mat4 modelMatrix{1.0f};
      modelMatrix = glm::translate(modelMatrix, glm::vec3(xPos, 0.0f, yPos));
      glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

      auto modelViewMatrix{glm::mat3(m_camera.m_viewMatrix * modelMatrix)};
      glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
      glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);
        
      if (m_maze.isBox(i, j)) {
        m_wallModel.render();
      }
      else {
        m_grassModel.render();
      }
    }
  }

  glUseProgram(0);
}

void OpenGLWindow::renderSkybox() {
  glUseProgram(m_skyProgram);

  // Get location of uniform variables
  GLint modelMatrixLoc{glGetUniformLocation(m_program, "modelMatrix")};
  GLint viewMatrixLoc{glGetUniformLocation(m_skyProgram, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_skyProgram, "projMatrix")};
  GLint skyTexLoc{glGetUniformLocation(m_skyProgram, "skyTex")};

  // Set uniform variables
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_camera.m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_camera.m_projMatrix[0][0]);
  glUniform1i(skyTexLoc, 0);

  float xTranslation = m_maze.m_mazeMatrix.size() / 2 ;
  float yTranslation = m_maze.m_mazeMatrix[0].size() / 2;
  float skyboxScale = std::max(m_maze.m_mazeMatrix.size(), m_maze.m_mazeMatrix[0].size()) * 50;

  glm::mat4 modelMatrix{1.0f};
  modelMatrix = glm::translate(modelMatrix, glm::vec3(xTranslation, 0.0f, yTranslation));
  modelMatrix = glm::scale(modelMatrix, glm::vec3(skyboxScale, skyboxScale, skyboxScale));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  m_skyModel.render();

  glUseProgram(0);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  glm::vec2 rotationSpeed = getRotationSpeedFromMouse();

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(rotationSpeed.x * deltaTime);
  m_camera.tilt(rotationSpeed.y * deltaTime);
}

glm::vec2 OpenGLWindow::getRotationSpeedFromMouse() {
  if (!m_screenFocus)
    return glm::vec2{0, 0};
  
  if (m_mouseTimer.elapsed() > 0.10) {
    SDL_WarpMouseInWindow(nullptr, m_viewportWidth/2, m_viewportHeight/2);
    m_mouseTimer.restart();
  }

  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);  

  float maxMovement{0.9f};
  float speedScale{50.0f};
  glm::vec2 movement{2.0f * mousePosition.x / m_viewportWidth - 1.0f,
                     1.0f - 2.0f * mousePosition.y / m_viewportHeight};
  
  // Avoid infinite tilt/pan movement when cursor gets out of the window
  if (std::abs(movement.x) > maxMovement || std::abs(movement.y) > maxMovement)
    return glm::vec2{0, 0};

  return glm::vec2{movement.x * speedScale, movement.y * speedScale};

}