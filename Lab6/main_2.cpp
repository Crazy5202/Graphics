#include <SFML/Window.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <array>
#include "parse_obj_smart.cpp"

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 position;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    uniform vec3 color;

    out vec3 fragColor;

    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0);
        fragColor = color;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 fragColor;
    out vec4 color;

    void main()
    {
        color = vec4(fragColor, 1.0);
    }
)";

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void VAO_setup(GLuint VAO, GLuint VBO, GLuint EBO, float vertices[], int ver_size, unsigned int indices[], int ind_size) {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, ver_size, vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_size, indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

int main() {
    sf::Window window(sf::VideoMode(800, 600), "3D Cube with Two-Point Perspective", sf::Style::Default, sf::ContextSettings(24, 8, 8));
    window.setVerticalSyncEnabled(true);

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    float cube_vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
    };
    unsigned int cube_indices[] = {
        0, 1, 2, 2, 3, 0,  
        4, 5, 6, 6, 7, 4,  
        4, 5, 1, 1, 0, 4,  
        7, 6, 2, 2, 3, 7,  
        4, 0, 3, 3, 7, 4,  
        5, 1, 2, 2, 6, 5  
    };
    float pyramid_vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
    unsigned int pyramid_indices[] = {
        0, 1, 2, 
        2, 3, 0,
        0, 1, 4,
        4, 1, 2,
        2, 4, 3,
        3, 4, 0
    };

    float radius = 1.0f;
    float height = 2.0f;
    int slices = 36;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float halfHeight = height / 2.0f;

    vertices.push_back(0.0f);     
    vertices.push_back(halfHeight);
    vertices.push_back(0.0f); 

    vertices.push_back(0.0f);    
    vertices.push_back(-halfHeight); 
    vertices.push_back(0.0f);   

    for (int i = 0; i <= slices; ++i) {
        float angle = (2.0f * M_PI * i) / slices;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.push_back(x);
        vertices.push_back(halfHeight);
        vertices.push_back(z);

        vertices.push_back(x);
        vertices.push_back(-halfHeight);
        vertices.push_back(z);
    }

    for (int i = 0; i < slices; ++i) {
        indices.push_back(0);        
        indices.push_back(2 + i * 2);    
        indices.push_back(2 + ((i + 1) % slices) * 2);  
    }

    for (int i = 0; i < slices; ++i) {
        indices.push_back(1);           
        indices.push_back(3 + i * 2);  
        indices.push_back(3 + ((i + 1) % slices) * 2); 
    }

    for (int i = 0; i < slices; ++i) {
        int topCurrent = 2 + i * 2;
        int topNext = 2 + ((i + 1) % slices) * 2;
        int bottomCurrent = 3 + i * 2;
        int bottomNext = 3 + ((i + 1) % slices) * 2;

        indices.push_back(topCurrent);
        indices.push_back(bottomCurrent);
        indices.push_back(topNext);

        indices.push_back(topNext);
        indices.push_back(bottomCurrent);
        indices.push_back(bottomNext);
    }

    const size_t size = 3;
    std::array<GLuint, size> VBO, VAO, EBO;
    glGenVertexArrays(size, &VAO[0]);
    glGenBuffers(size, &VBO[0]);
    glGenBuffers(size, &EBO[0]);

    auto result = parseOBJ("../untitled.obj");

    VAO_setup(VAO[0], VBO[0], EBO[0], result.interleavedData.data(), result.interleavedData.size() * sizeof(float), result.indices.data(), result.indices.size()*sizeof(unsigned int));
    VAO_setup(VAO[1], VBO[1], EBO[1], pyramid_vertices, sizeof(pyramid_vertices), pyramid_indices, sizeof(pyramid_indices));
    VAO_setup(VAO[2], VBO[2], EBO[2], vertices.data(), vertices.size() * sizeof(float), indices.data(), indices.size() * sizeof(float));

    GLuint shaderProgram = createShaderProgram();
    glUseProgram(shaderProgram);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "color");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    bool running = true;

    std::vector<glm::mat4> models(size, glm::mat4(1.0f));
    int current = 0;
    sf::Clock clock;
    models[0] = glm::translate(models[0], glm::vec3(-2.0f, -4.0f, 0.0f));
    models[1] = glm::translate(models[0], glm::vec3(-1.0f, 1.1f, -0.5f));
    models[2] = glm::translate(models[1], glm::vec3(-1.0f, 1.0f, 0.5f));

    std::vector<glm::vec3> colors;
    colors.push_back(glm::vec3(0.5f, 0.0f, 0.0f));
    colors.push_back(glm::vec3(0.0f, 0.5f, 0.0f));
    colors.push_back(glm::vec3(0.0f, 0.0f, 0.5f));

    while (running) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
            }
            if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) current = 0;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) current = 1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) current = 2;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) models[current] = glm::translate(models[current], glm::vec3(-0.1, 0.0, 0.0));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) models[current] = glm::translate(models[current], glm::vec3(0.1, 0.0, 0.0));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) models[current] = glm::translate(models[current], glm::vec3(0.0, -0.1, 0.0));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) models[current] = glm::translate(models[current], glm::vec3(0.0, 0.1, 0.0));
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i=0; i<size; ++i) {
            glm::mat4 tempMat;
            if (i==2)  {
                tempMat = glm::rotate(models[2], glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
                tempMat = glm::scale(tempMat, glm::vec3(0.5, 0.5, 0.5));
            } else tempMat = models[i];
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tempMat));
            glUniform3f(vertexColorLocation, colors[i][0],  colors[i][1],  colors[i][2]);

            glBindVertexArray(VAO[i]);
            int num_elem;
            if (i==0) num_elem = 36;
            else if (i==1) num_elem = 18;
            else if (i==2) num_elem = indices.size();
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            glDrawElements(GL_TRIANGLES, num_elem, GL_UNSIGNED_INT, 0);

            glUniform3f(vertexColorLocation, 0.0f, 0.0f, 0.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1.f,-1.f);

            glDrawElements(GL_TRIANGLES, num_elem, GL_UNSIGNED_INT, 0);
        }

        window.display();
    }

    glDeleteVertexArrays(size, &VAO[0]);
    glDeleteBuffers(size, &VBO[0]);
    glDeleteBuffers(size, &EBO[0]);
    glDeleteProgram(shaderProgram);

    return 0;
}