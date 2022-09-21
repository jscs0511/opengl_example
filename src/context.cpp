#include "context.h"
#include "image.h"
#include <imgui.h>
ContextUPtr Context::Create(){
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init(){
    
    // float vertices[] = {//[x, y, z, r, g, b, s(x-axis), t(y-axis)]
    //     0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    //     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    //     -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    // };
    // uint32_t indices[] = { // note that we start from 0!
    //     0, 1, 3, // first triangle
    //     1, 2, 3, // second triangle
    // };

    float vertices[] = {// x, y, z, s, t
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
    };

    uint32_t indices[] = {
        0,  2,  1,  2,  0,  3,// first triangle indices(0,2,1), second triangle indices(2,0,3)
        4,  5,  6,  6,  7,  4,// third, fourth, ..
        8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };


    m_vertexLayout = VertexLayout::Create();
    //glGenVertexArrays(1, &m_vertexArrayObject);
    //glBindVertexArray(m_vertexArrayObject);
    
    
    //만드는 순서가 중요
    //Vertexlayout object가 먼저 만들어지고 이후에 buffer가 생성되면, Vertexlayout object가 사용할
    //Buffer로 자동으로 setting이 됌 (그래서 만들어지는 순서 중요)
    //Buffer 먼저 생성시 에러 발생
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float)*120);
    /*
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*12, vertices, GL_STATIC_DRAW);
    */

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, 0);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, sizeof(float)*3);
    //glEnableVertexAttribArray(0);//0th attribute--0th location
    //glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,0);

    //in case of index buffer, no need to set Attribute Array because index must be integer value
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t)*36);
    /*
    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6, indices, GL_STATIC_DRAW);
    */

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/lighting.vs", GL_VERTEX_SHADER); // must check the location of exe file.
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/lighting.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get()); 
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    /*
    auto loc = glGetUniformLocation(m_program->Get(), "color"); // "color" : variable name in simple.fx
    m_program->Use(); // declare that m_program will be used
    glUniform4f(loc,1.0f, 1.0f, 0.0f, 1.0f); // 4 floating points, RGB and 
    */
    glClearColor(0.0f, 0.1f, 0.2f, 0.0f); // set the clear color

    
    auto image = Image::Load("./image/container.jpg");
    if (!image){
        return false;
    }
    SPDLOG_INFO("image: {}x{}, {}channels",image->GetWidth(), image->GetHeight(), image->GetChannelCount());
    
    //auto image = Image::Create(512, 512);
    //image->SetCheckImage(16,16);

    m_texture = Texture::CreateFromImage(image.get()); //.get func: get a raw pointer from unique pointer

    auto image2 = Image::Load("./image/awesomeface.png");
    m_texture2 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);//texture index 0 slot
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());//2D texture's id : m_texture
    glActiveTexture(GL_TEXTURE1);//texture index 1 slot
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());//2D texture's id : m_texture2
    //As a result, GL_TEXTURE0 loaded m_texture, GL_TEXTURE1 loaded m_texture2


    m_program->Use();
    //shader can understand that m_texture is binded with 0th slot from the code below
    m_program->SetUniform("tex",0);
    //glUniform1i(glGetUniformLocation(m_program->Get(),"tex"), 0);// 0: GL_TEXTURE0
    
    //shader can understand that m_texture2 is binded with 1st slot from the code below
    m_program->SetUniform("tex2",1);
    //glUniform1i(glGetUniformLocation(m_program->Get(), "tex2"), 1);// 1: GL_TEXTURE1

    /*
    //glGen~glTex : GPU setting
    glGenTextures(1, &m_texture);// 1 image & texture id
    glBindTexture(GL_TEXTURE_2D, m_texture);//GL_TEXTURE_2D will target m_texture(binding)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //if either x or y is grater than 1 or less than 0, then this wrap option 
    //will be applied.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //S : x-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //T : y-axis

    //copy image data into GPU from CPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        image->GetWidth(), image->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image->GetData());
    /* 
    target: GL_TEXTURE_2D which is binded with m_texture
    0: image level (size of image)
    GL_RGB: will use RGB channel in GPU
    GetWidth, GetHeight: image size in GPU
    0: image border
    GL_RGB: data RGB channel information in CPU
    GL_UNSIGNED_BYTE: each channel data consists of unsigned char
    image->GetData(): send image data pointer to GPU from CPU
    */

    // // x축으로 -55도 회전
    // auto model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // // 카메라는 원점으로부터 z축 방향으로 -3만큼 떨어짐
    // auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    // // 종횡비 4:3, 세로화각 45도의 원근 투영
    // auto projection = glm::perspective(glm::radians(45.0f),
    // (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);
    // auto transform = projection * view * model;

    // //translate code
    // //auto transform = glm::translate( glm::mat4(1.0f), glm::vec3(0.2f,0.2f,0.0));
    // auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");
    // //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    // m_program->SetUniform("transform",transform);
    return true;
}

void Context::Render(){
	if (ImGui::Begin("ui window")) {
        if(ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))){
            glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        }
        ImGui::Separator();
        ImGui::DragFloat3("camera pos",glm::value_ptr(m_cameraPos),0.05f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")){
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }
         if (ImGui::CollapsingHeader("light")) {
            ImGui::ColorEdit3("light color", glm::value_ptr(m_lightColor));
            ImGui::ColorEdit3("object color", glm::value_ptr(m_objectColor));
            ImGui::SliderFloat("ambient strength", &m_ambientStrength, 0.0f, 1.0f);
        }
    }
    ImGui::End();

    m_program->Use();
    m_program->SetUniform("lightColor", m_lightColor);
    m_program->SetUniform("objectColor", m_objectColor);
    m_program->SetUniform("ambientStrength", m_ambientStrength);


    std::vector<glm::vec3> cubePositions = {
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f), 
    };
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    m_cameraFront =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * 
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    // vec4 last value 0.0f: vector(mean direction), 1.0f: vertex(mean a certain location)
    //glUseProgram(m_program->Get());
    auto projection = glm::perspective(glm::radians(45.0f),(float)m_width/(float)m_height, 0.01f, 100.0f);
   
    auto view = glm::lookAt(
        m_cameraPos, 
        m_cameraPos + m_cameraFront, 
        m_cameraUp);
   
    for (size_t i = 0; i < cubePositions.size(); i++){
        auto& pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f),pos);
        model = glm::rotate(model, glm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i),glm::vec3(1.0f,0.5f,0.0f));
        auto transform = projection * view * model;
        m_program->SetUniform("transform",transform);
        glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);

    }
    

    /*
    static float time = 0.0f;
    float t = sinf(time) * 0.5f + 0.5f;
    auto loc = glGetUniformLocation(m_program->Get(), "color");
    m_program->Use();
    glUniform4f(loc, t*t, 2.0f*t*(1.0f-t), (1.0f-t)*(1.0f-t), 1.0f);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//the number of indexes, index type(uint32_t:GL_UNSIGNED_INT, uint16_t:GL_UNSIGNED_SHORT, ...)

    time += 0.0007f;
    */

} 

void Context::ProcessInput(GLFWwindow* window) {

    if(!m_cameraControl)
        return;
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;    

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::Reshape(int width, int height){
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y) {

    if(!m_cameraControl)
        return; 
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.8f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    m_prevMousePos = pos;    
}

void Context::MouseButton(int button, int action, double x, double y) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
      m_prevMousePos = glm::vec2((float)x, (float)y);
      m_cameraControl = true;
    }
    else if (action == GLFW_RELEASE) {
      m_cameraControl = false;
    }
  }
}