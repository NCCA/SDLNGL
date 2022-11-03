#include "NGLDraw.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLInit.h>
#include <ngl/Transformation.h>
const static float INCREMENT = 0.01f;
const static float ZOOM = 0.05f;
constexpr auto PBR = "PBR";

NGLDraw::NGLDraw()
{
  m_rotate = false;
  // mouse rotation values set to 0
  m_spinXFace = 0;
  m_spinYFace = 0;

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // now to load the shader and set the values
  // we are creating a shader called PBR to save typos
  // in the code create some constexpr
  constexpr auto vertexShader = "PBRVertex";
  constexpr auto fragShader = "PBRFragment";
  // create the shader program
  ngl::ShaderLib::createShaderProgram(PBR);
  // now we are going to create empty shaders for Frag and Vert
  ngl::ShaderLib::attachShader(vertexShader, ngl::ShaderType::VERTEX);
  ngl::ShaderLib::attachShader(fragShader, ngl::ShaderType::FRAGMENT);
  // attach the source
  ngl::ShaderLib::loadShaderSource(vertexShader, "shaders/PBRVertex.glsl");
  ngl::ShaderLib::loadShaderSource(fragShader, "shaders/PBRFragment.glsl");
  // compile the shaders
  ngl::ShaderLib::compileShader(vertexShader);
  ngl::ShaderLib::compileShader(fragShader);
  // add them to the program
  ngl::ShaderLib::attachShaderToProgram(PBR, vertexShader);
  ngl::ShaderLib::attachShaderToProgram(PBR, fragShader);
  // now we have associated that data we can link the shader
  ngl::ShaderLib::linkProgramObject(PBR);
  // and make it active ready to load values
  ngl::ShaderLib::use(PBR);
  // We now create our view matrix for a static camera
  ngl::Vec3 from(0.0f, 2.0f, 2.0f);
  ngl::Vec3 to(0.0f, 0.0f, 0.0f);
  ngl::Vec3 up(0.0f, 1.0f, 0.0f);
  // now load to our new camera
  m_view = ngl::lookAt(from, to, up);
  ngl::ShaderLib::setUniform("camPos", from);
  // setup the default shader material and light porerties
  // these are "uniform" so will retain their values
  ngl::ShaderLib::setUniform("lightPosition", 0.0f, 2.0f, 2.0f);
  ngl::ShaderLib::setUniform("lightColor", 400.0f, 400.0f, 400.0f);
  ngl::ShaderLib::setUniform("exposure", 2.2f);
  ngl::ShaderLib::setUniform("albedo", 0.950f, 0.71f, 0.29f);

  ngl::ShaderLib::setUniform("metallic", 1.02f);
  ngl::ShaderLib::setUniform("roughness", 0.38f);
  ngl::ShaderLib::setUniform("ao", 0.2f);
  ngl::VAOPrimitives::createTrianglePlane("floor", 20, 20, 1, 1, ngl::Vec3::up());

  ngl::ShaderLib::use(ngl::nglCheckerShader);
  ngl::ShaderLib::setUniform("lightDiffuse", 1.0f, 1.0f, 1.0f, 1.0f);
  ngl::ShaderLib::setUniform("checkOn", true);
  ngl::ShaderLib::setUniform("lightPos", 0.0f, 2.0f, 2.0f);
  ngl::ShaderLib::setUniform("colour1", 0.9f, 0.9f, 0.9f, 1.0f);
  ngl::ShaderLib::setUniform("colour2", 0.6f, 0.6f, 0.6f, 1.0f);
  ngl::ShaderLib::setUniform("checkSize", 60.0f);
}

NGLDraw::~NGLDraw()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLDraw::resize(int _w, int _h)
{
  glViewport(0, 0, _w, _h);
  m_project = ngl::perspective(45.0f, static_cast<float>(_w) / _h, 0.1f, 200.0f);
  m_width = _w;
  m_height = _h;
}

void NGLDraw::draw()
{
  glViewport(0, 0, m_width, m_height);
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // grab an instance of the shader manager
  ngl::ShaderLib::use(PBR);

  // Rotation based on the mouse position for our global transform
  auto rotX = ngl::Mat4::rotateX(m_spinXFace);
  auto rotY = ngl::Mat4::rotateY(m_spinYFace);
  // create the rotation matrices
  m_mouseGlobalTX = rotX * rotY;
  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;
  // get the VBO instance and draw the built in teapot
  // draw
  loadMatricesToShader();
  ngl::VAOPrimitives::draw("teapot");
  ngl::ShaderLib::use(ngl::nglCheckerShader);
  auto tx = ngl::Mat4::translate(0.0f, -0.45f, 0.0f);
  ngl::Mat4 MVP = m_project * m_view * m_mouseGlobalTX * tx;
  ngl::Mat3 normalMatrix = m_view * m_mouseGlobalTX;
  normalMatrix.inverse().transpose();
  ngl::ShaderLib::setUniform("MVP", MVP);
  ngl::ShaderLib::setUniform("normalMatrix", normalMatrix);
  ngl::VAOPrimitives::draw("floor");
}

void NGLDraw::loadMatricesToShader()
{
  ngl::ShaderLib::use("PBR");
  struct transform
  {
    ngl::Mat4 MVP;
    ngl::Mat4 normalMatrix;
    ngl::Mat4 M;
  };

  transform t;
  t.M = m_view * m_mouseGlobalTX;

  t.MVP = m_project * t.M;
  t.normalMatrix = t.M;
  t.normalMatrix.inverse().transpose();
  ngl::ShaderLib::setUniformBuffer("TransformUBO", sizeof(transform), &t.MVP.m_00);
}
//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::mouseMoveEvent(const SDL_MouseMotionEvent &_event)
{
  if (m_rotate && _event.state & SDL_BUTTON_LMASK)
  {
    int diffx = _event.x - m_origX;
    int diffy = _event.y - m_origY;
    m_spinXFace += (float)0.5f * diffy;
    m_spinYFace += (float)0.5f * diffx;
    m_origX = _event.x;
    m_origY = _event.y;
    this->draw();
  }
  // right mouse translate code
  else if (m_translate && _event.state & SDL_BUTTON_RMASK)
  {
    int diffX = (int)(_event.x - m_origXPos);
    int diffY = (int)(_event.y - m_origYPos);
    m_origXPos = _event.x;
    m_origYPos = _event.y;
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    this->draw();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::mousePressEvent(const SDL_MouseButtonEvent &_event)
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if (_event.button == SDL_BUTTON_LEFT)
  {
    m_origX = _event.x;
    m_origY = _event.y;
    m_rotate = true;
  }
  // right mouse translate mode
  else if (_event.button == SDL_BUTTON_RIGHT)
  {
    m_origXPos = _event.x;
    m_origYPos = _event.y;
    m_translate = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::mouseReleaseEvent(const SDL_MouseButtonEvent &_event)
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event.button == SDL_BUTTON_LEFT)
  {
    m_rotate = false;
  }
  // right mouse translate mode
  if (_event.button == SDL_BUTTON_RIGHT)
  {
    m_translate = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLDraw::wheelEvent(const SDL_MouseWheelEvent &_event)
{

  // check the diff of the wheel position (0 means no change)
  if (_event.y > 0)
  {
    m_modelPos.m_z += ZOOM;
    this->draw();
  }
  else if (_event.y < 0)
  {
    m_modelPos.m_z -= ZOOM;
    this->draw();
  }

  // check the diff of the wheel position (0 means no change)
  if (_event.x > 0)
  {
    m_modelPos.m_x -= ZOOM;
    this->draw();
  }
  else if (_event.x < 0)
  {
    m_modelPos.m_x += ZOOM;
    this->draw();
  }
}
//----------------------------------------------------------------------------------------------------------------------
