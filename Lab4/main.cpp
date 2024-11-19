#include <SFML/Window.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

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
            
            // отражения
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

int main() {
    // окно
    
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode(); // создание окна
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
    float vertices[] = {
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

    // куб-объект
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // куб света 
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // создаём соответственные шейдерные программы
    GLuint objectShaderProgram = createShaderProgram(objectVertexShaderSource, objectFragmentShaderSource);
    GLuint lightShaderProgram = createShaderProgram(lightVertexShaderSource, lightFragmentShaderSource);

    // задаём матрицы проекции и камеры
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), float(desktop.width / desktop.height), 0.1f, 100.0f);
    
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // параметры освещения
    glm::vec3 lightPos(glm::vec3(-0.7f, -0.7f, 1.0f));
    glm::mat4 lightMat = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), lightPos), glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0)), glm::vec3(0.2f));
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 objectColor(1.0f, 0.5f, 0.31f);

    // заносим все матрицы отображения и вектор света в световой куб, т.к. он не меняется
    glUseProgram(lightShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(lightMat));

    glUniform3fv(glGetUniformLocation(lightShaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
    
    // закидываем матрицы в куб-объект
    glUseProgram(objectShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(objectShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(objectShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // закидываем параметры освещения в куб-объект
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
            // перемещение куба по плоскости
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) models[0] = glm::translate(models[0], glm::vec3(-0.1, 0.0, 0.0));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) models[0] = glm::translate(models[0], glm::vec3(0.1, 0.0, 0.0));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) models[0] = glm::translate(models[0], glm::vec3(0.0, -0.1, 0.0));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) models[0] = glm::translate(models[0], glm::vec3(0.0, 0.1, 0.0));

            // перемещение куба вниз-вверх
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

        // отрисовка кубика-объекта
        glUseProgram(objectShaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(objectShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::scale(models[0]*rot_hor*rot_vert, glm::vec3(0.5f))));
        glUniform1f(glGetUniformLocation(objectShaderProgram, "linear"), linear);
        glUniform1f(glGetUniformLocation(objectShaderProgram, "quadr"), quadr);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // отрисовка кубика света
        glUseProgram(lightShaderProgram);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        

        window.display();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    window.close();
    return 0;
}