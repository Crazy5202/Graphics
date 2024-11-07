#include <SFML/Window.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <array>

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

    const size_t size = 1;
    std::array<GLuint, size> VBO, VAO, EBO;
    glGenVertexArrays(size, &VAO[0]);
    glGenBuffers(size, &VBO[0]);
    glGenBuffers(size, &EBO[0]);

    VAO_setup(VAO[0], VBO[0], EBO[0], cube_vertices, sizeof(cube_vertices), cube_indices, sizeof(cube_indices));

    GLuint shaderProgram = createShaderProgram();
    glUseProgram(shaderProgram);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::vec3 color = glm::vec3(0.5f, 0.0f, 0.0f);

    GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "color");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    bool running = true;

    std::vector<glm::mat4> models(size, glm::mat4(1.0f));
    sf::Clock clock;

    int radx = 10;
    int rady = 10;
    int coeff = 50;
    while (running) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
            }
            if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) coeff+=10;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) coeff-=10;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) radx+=5;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) radx-=5;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) rady+=5;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) rady-=5;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i=0; i<size; ++i) {
            float time = coeff*clock.getElapsedTime().asSeconds();
            float new_x = radx*cos(glm::radians(time));
            float new_y = rady*sin(glm::radians(time));
            glm::mat4 tempMat = glm::translate(models[i], glm::vec3(new_x, new_y, 0.0f));
            glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(new_x, new_y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tempMat));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            glBindVertexArray(VAO[i]);
            int num_elem = 36;

            glUniform3f(vertexColorLocation, color[0],  color[1],  color[2]);
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            glDrawElements(GL_TRIANGLES, num_elem, GL_UNSIGNED_INT, 0);

            glUniform3f(vertexColorLocation, 1.0f, 1.0f, 1.0f);
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