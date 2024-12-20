#include <SFML/Window.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

const char* objectVertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;

    out vec3 FragPos;
    out vec3 Normal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal; 
        
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const char* objectFragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    in vec3 FragPos;  
    in vec3 Normal;  
    
    uniform vec3 lightPos;
    uniform vec3 lightColor;
    uniform vec3 objectColor;
    uniform vec3 viewPos;

    uniform float linear;
    uniform float quadr;

    void main()
    {
        // реализация прожектора
        vec3 zDir = vec3(0.0, 0.0, 1.0);
        vec3 lightDir = normalize(lightPos - FragPos);
        float theta = dot(lightDir, zDir);
        float cutOff = cos(radians(45.0));

        // эмбиент (постоянный цвет)
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;

        vec3 result;
        if (theta > cutOff) 
        {
            // диффузное освещение (зависит от положения куба и нормали)
            vec3 norm = normalize(Normal);
            
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            
            // блики
            float specularStrength = 1.0;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * lightColor;

            // затухание света
            float distance = length(lightPos - FragPos);
            float attenuation = 1.0 / (1.0f + linear * distance + quadr * (distance * distance));    

            diffuse *= attenuation;
            specular *= attenuation; 

            // результат - сложение других составляющих освещения
            result = (ambient + diffuse + specular) * objectColor;
        } 
        else 
        {
            result = ambient * objectColor;
        }
        FragColor = vec4(result, 1.0);
    } 
)";

const char* lightVertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

const char* lightFragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    uniform vec3 lightColor;

    void main()
    {
        FragColor = vec4(lightColor, 1.0);
    }
)";

GLuint compileShader(GLenum type, const char* source) { // компиляция шейдера
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

GLuint createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) { // создание шейдерной программы
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

void generateCylinder(float radius, float height, int slices, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    float halfHeight = height / 2.0f;

    // верхний центр
    vertices.push_back(0.0f);        
    vertices.push_back(0.0f);  
    vertices.push_back(halfHeight);        

    // его нормаль  
    vertices.push_back(0.0f);        
    vertices.push_back(0.0f);  
    vertices.push_back(1.0f);        

    // нижний центр
    vertices.push_back(0.0f);        
    vertices.push_back(0.0f); 
    vertices.push_back(-halfHeight);        

    // его нормаль  
    vertices.push_back(0.0f);        
    vertices.push_back(0.0f);  
    vertices.push_back(-1.0f);        

    // вершины для боковой отрисовки (разные нормали)
    for (int i = 0; i < slices; ++i) {
        float angle = (2.0f * M_PI * i) / slices;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        
        // верхний круг
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(halfHeight);

        // нормаль   
        vertices.push_back(x);        
        vertices.push_back(y);  
        vertices.push_back(0.0f);        
    
        // нижний круг
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(-halfHeight);

        // нормаль  
        vertices.push_back(x);        
        vertices.push_back(y);  
        vertices.push_back(0.0f);        
    }

    for (int i = 0; i < slices; ++i) {
        float zero = 0;
        float angle = (2.0f * M_PI * i) / slices;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        
        // верхний круг
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(halfHeight);

        // нормаль  
        vertices.push_back(zero);        
        vertices.push_back(zero);  
        vertices.push_back(1.0f);        
    
        // нижний круг
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(-halfHeight);

        // нормаль
        vertices.push_back(zero);        
        vertices.push_back(zero);  
        vertices.push_back(-1.0f);        
    }

    int jump = slices*2;
    // индексы для верхнего круга
    for (int i = 0; i < slices; ++i) {
        indices.push_back(0);                // верхний центр
        indices.push_back(jump + 2 + i * 2);        // текущая верхняя вершщина
        indices.push_back(jump + 2 + ((i + 1) % slices) * 2);  // следующая верхняя вершина
    }

    // индексы для нижнего круга
    for (int i = 0; i < slices; ++i) {
        indices.push_back(1);                // нижний центр
        indices.push_back(jump + 3 + i * 2);        // текущая нижняя вершина
        indices.push_back(jump + 3 + ((i + 1) % slices) * 2);  // следующая нижняя вершина
    }

    // индексы для боковой поверхности
    for (int i = 0; i < slices; ++i) {
        int topCurrent = 2 + i * 2;
        int topNext = 2 + ((i + 1) % slices) * 2;
        int bottomCurrent = 3 + i * 2;
        int bottomNext = 3 + ((i + 1) % slices) * 2;

        // первый треугольник
        indices.push_back(topCurrent);
        indices.push_back(bottomCurrent);
        indices.push_back(topNext);

        // второй треугольник
        indices.push_back(topNext);
        indices.push_back(bottomCurrent);
        indices.push_back(bottomNext);
    }
}

int main() {
    // окно
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Window window(sf::VideoMode(desktop.width, desktop.height), "Lightpost", sf::Style::Default, sf::ContextSettings(24, 8, 8));
    window.setVerticalSyncEnabled(true);
    // всякие настройки
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    // вершины куба
    auto cubeVertices1 = parseOBJ("/home/Maxim/Documents/Labs_5_sem/Graphics/Lab6/assets/objects/cone.obj");
    auto cubeVertices = cubeVertices1.data();

    // вершины куба
    float cubeVertices2[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    // вершины пирамиды
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    float radius = 0.5;
    float height = 1.5;
    int slices = 36;
    generateCylinder(radius, height, 36, vertices, indices);
    std::cout << indices.size() << std::endl;

    // объект
    unsigned int cylVAO, cylVBO, cylEBO;
    glGenVertexArrays(1, &cylVAO);
    glBindVertexArray(cylVAO);

    glGenBuffers(1, &cylVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cylVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // куб света 
    unsigned int lightCubeVAO, lightCubeVBO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glGenBuffers(1, &lightCubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices1.size()*sizeof(float), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // создаём соответственные шейдерные программы
    GLuint objectShaderProgram = createShaderProgram(objectVertexShaderSource, objectFragmentShaderSource);
    GLuint lightShaderProgram = createShaderProgram(lightVertexShaderSource, lightFragmentShaderSource);

    // задаём матрицы проекции и камеры
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), float(desktop.width / desktop.height), 0.1f, 100.0f);
    
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // параметры освещения
    glm::vec3 lightPos(glm::vec3(-0.7f, -0.7f, 1.5f));
    glm::mat4 lightMat = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), lightPos), glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0)), glm::vec3(0.2f));
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 objectColor(1.0f, 0.5f, 0.31f);

    // заносим все матрицы отображения и вектор света в световой куб, т.к. он не меняется
    glUseProgram(lightShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(lightMat));

    glUniform3fv(glGetUniformLocation(lightShaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
    
    // закидываем матрицы в объект
    glUseProgram(objectShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(objectShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(objectShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // закидываем параметры освещения в объект
    glUniform3fv(glGetUniformLocation(objectShaderProgram, "objectColor"), 1, glm::value_ptr(objectColor));
    glUniform3fv(glGetUniformLocation(objectShaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(objectShaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(objectShaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));

    int size = 1;
    std::vector<glm::mat4> models(size, glm::mat4(1.0f));
    models[0] = glm::translate(models[0], glm::vec3(-1.5f, -1.5f, 0.0f));
    glm::mat4 rot_hor(1.0f);
    glm::mat4 rot_vert(1.0f);
    bool running = true;

    float linear = 0.35;
    float quadr = 0.34;

    while (running) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
            }
            if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
            // перемещение объекта по плоскости
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) models[0] = glm::translate(models[0], glm::vec3(-0.1, 0.0, 0.0));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) models[0] = glm::translate(models[0], glm::vec3(0.1, 0.0, 0.0));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) models[0] = glm::translate(models[0], glm::vec3(0.0, -0.1, 0.0));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) models[0] = glm::translate(models[0], glm::vec3(0.0, 0.1, 0.0));

            // перемещение объекта вниз-вверх
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) models[0] = glm::translate(models[0], glm::vec3(0.0, 0.0, 0.1));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) models[0] = glm::translate(models[0], glm::vec3(0.0, 0.0, -0.1));

            // изменение мощности освещения
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                linear = std::max(linear-0.05, 0.0);
                quadr = std::max(quadr-0.05, 0.0);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {            
                linear+=0.05;
                quadr+=0.05;
            }

            // вращение куба
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) rot_hor = glm::rotate(rot_hor, glm::radians(-10.0f), glm::vec3(0.0, 0.0, 1.0));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) rot_hor = glm::rotate(rot_hor, glm::radians(10.0f), glm::vec3(0.0, 0.0, 1.0));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) rot_vert = glm::rotate(rot_vert, glm::radians(-10.0f), glm::vec3(1.0, 1.0, 0.0));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) rot_vert = glm::rotate(rot_vert, glm::radians(10.0f), glm::vec3(1.0, 1.0, 0.0));
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // отрисовка объекта
        glUseProgram(objectShaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(objectShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::scale(models[0]*rot_hor*rot_vert, glm::vec3(0.5f))));
        glUniform1f(glGetUniformLocation(objectShaderProgram, "linear"), linear);
        glUniform1f(glGetUniformLocation(objectShaderProgram, "quadr"), quadr);

        //glBindVertexArray(cylVAO);
        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // отрисовка кубика света
        //glUseProgram(lightShaderProgram);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, cubeVertices1.size());
        

        window.display();
    }

    glDeleteVertexArrays(1, &cylVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &cylVBO);
    glDeleteBuffers(1, &lightCubeVBO);
    glDeleteBuffers(1, &cylEBO);
    window.close();
    return 0;
}