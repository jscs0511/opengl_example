#include "context.h"

ContextUPtr Context::Create(){
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init(){

    float vertices[] = {
        0.5f, 0.5f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, // top left
    };
    uint32_t indices[] = { // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);
    //만드는 순서가 중요
    //VertexArray object가 먼저 만들어지면, VertexArray object가 사용할
    //Buffer로 자동으로 setting이 됌 (그래서 만들어지는 순서 중요)

    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float)*12);
    /*
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*12, vertices, GL_STATIC_DRAW);
    */

    glEnableVertexAttribArray(0);//0th attribute--0th location
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,0);

    //in case of index buffer, no need to set Attribute Array because index must be integer value
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t)*6);
    /*
    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6, indices, GL_STATIC_DRAW);
    */

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER); // must check the location of exe file.
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get()); 
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(0.0f, 0.1f, 0.2f, 0.0f); // set the clear color

    return true;
}

void Context::Render(){
    glClear(GL_COLOR_BUFFER_BIT);
    
    m_program->Use();
    //glUseProgram(m_program->Get());
    
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//the number of indexes, index type(uint32_t:GL_UNSIGNED_INT, uint16_t:GL_UNSIGNED_SHORT, ...)
} 