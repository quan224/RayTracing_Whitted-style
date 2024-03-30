//#include <iostream>
//#include <string>
//#include <fstream>
//#include <vector>
//#include <sstream>
//#include <iostream>
//
//#include <GL/glew.h>
//#include <GL/freeglut.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <SOIL2/SOIL2.h>
//
//std::vector<glm::vec3> points;      // ��������
//std::vector<glm::vec2> texcoords;   // ������������
//std::vector<glm::vec3> normals;     // ���㷨��
//
//GLuint program; // ��ɫ���������  
//
//// ��ȡ�ļ����ҷ���һ�����ַ�����ʾ�ļ�����
//std::string readShaderFile(std::string filepath)
//{
//    std::string res, line;
//    std::ifstream fin(filepath);
//    if (!fin.is_open())
//    {
//        std::cout << "�ļ� " << filepath << " ��ʧ��" << std::endl;
//        exit(-1);
//    }
//    while (std::getline(fin, line))
//    {
//        res += line + '\n';
//    }
//    fin.close();
//    return res;
//}
//// ��ȡ��ɫ������
//GLuint getShaderProgram(std::string fshader, std::string vshader)
//{
//    // ��ȡshaderԴ�ļ�
//    std::string vSource = readShaderFile(vshader);
//    std::string fSource = readShaderFile(fshader);
//    const char* vpointer = vSource.c_str();
//    const char* fpointer = fSource.c_str();
//
//    // �ݴ�
//    GLint success;
//    GLchar infoLog[512];
//
//    // ���������붥����ɫ��
//    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    //������һ������learnopengl��д����null����֮ǰѧ�Ŀγ����Ǵ�����length����������null���Ժ��Ĭ��null����
//    glShaderSource(vertexShader, 1, (const GLchar**)(&vpointer), NULL);
//    glCompileShader(vertexShader);
//    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);   // ������
//    if (!success)
//    {
//        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
//        std::cout << "������ɫ���������\n" << infoLog << std::endl;
//        exit(-1);
//    }
//
//    // �������ұ���Ƭ����ɫ��
//    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader, 1, (const GLchar**)(&fpointer), NULL);
//    glCompileShader(fragmentShader);
//    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);   // ������
//    if (!success)
//    {
//        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
//        std::cout << "Ƭ����ɫ���������\n" << infoLog << std::endl;
//        exit(-1);
//    }
//
//    // ����������ɫ����program����
//    GLuint shaderProgram = glCreateProgram();
//    glAttachShader(shaderProgram, vertexShader);
//    glAttachShader(shaderProgram, fragmentShader);
//    glLinkProgram(shaderProgram);
//
//    // ɾ����ɫ������
//    glDeleteShader(vertexShader);
//    glDeleteShader(fragmentShader);
//
//    return shaderProgram;
//}
//// ��ȡobj�ļ������������մ��ݸ�������ɫ���� ����λ�� / ������������ / ���㷨��
//void readObj(
//    std::string filepath,
//    std::vector<glm::vec3>& points,
//    std::vector<glm::vec2>& texcoords,
//    std::vector<glm::vec3>& normals
//)
//{
//    // ��������
//    std::vector<glm::vec3> vectexPosition;//v
//    std::vector<glm::vec2> vertexTexcoord;//vt
//    std::vector<glm::vec3> vectexNormal;//vn
//
//    // ��Ƭ������Ϣ,f  x/x/x
//    std::vector<glm::ivec3> positionIndex;
//    std::vector<glm::ivec3> texcoordIndex;
//    std::vector<glm::ivec3> normalIndex;
//
//    // ���ļ���
//    std::ifstream fin(filepath);
//    std::string line;
//    if (!fin.is_open())
//    {
//        std::cout << "�ļ� " << filepath << " ��ʧ��" << std::endl;
//        exit(-1);
//    }
//
//    // ���ж�ȡ
//    while (std::getline(fin, line))
//    {
//        std::istringstream sin(line);   // ��һ�е�������Ϊ string stream �������Ҷ�ȡ
//        std::string type;
//        GLfloat x, y, z;
//        int v0, vt0, vn0;   // ��Ƭ�� 1 ������ġ�λ�ã��������꣬���ߡ�����
//        int v1, vt1, vn1;   // 2
//        int v2, vt2, vn2;   // 3
//        char slash;
//
//        // ��ȡobj�ļ�
//        sin >> type;
//        if (type == "v") {
//            sin >> x >> y >> z;
//            vectexPosition.push_back(glm::vec3(x, y, z));
//        }
//        if (type == "vt") {
//            sin >> x >> y;
//            vertexTexcoord.push_back(glm::vec2(x, y));
//        }
//        if (type == "vn") {
//            sin >> x >> y >> z;
//            vectexNormal.push_back(glm::vec3(x, y, z));
//        }
//        if (type == "f") {
//            sin >> v0 >> slash >> vt0 >> slash >> vn0;
//            sin >> v1 >> slash >> vt1 >> slash >> vn1;
//            sin >> v2 >> slash >> vt2 >> slash >> vn2;
//            positionIndex.push_back(glm::ivec3(v0 - 1, v1 - 1, v2 - 1));
//            texcoordIndex.push_back(glm::ivec3(vt0 - 1, vt1 - 1, vt2 - 1));
//            normalIndex.push_back(glm::ivec3(vn0 - 1, vn1 - 1, vn2 - 1));
//        }
//    }
//
//    // ������Ƭ��Ϣ�������մ��붥����ɫ���Ķ�������
//    for (int i = 0; i < positionIndex.size(); i++)
//    {
//        // ����λ��
//        points.push_back(vectexPosition[positionIndex[i].x]);
//        points.push_back(vectexPosition[positionIndex[i].y]);
//        points.push_back(vectexPosition[positionIndex[i].z]);
//
//        // ������������
//        texcoords.push_back(vertexTexcoord[texcoordIndex[i].x]);
//        texcoords.push_back(vertexTexcoord[texcoordIndex[i].y]);
//        texcoords.push_back(vertexTexcoord[texcoordIndex[i].z]);
//
//        // ���㷨��
//        normals.push_back(vectexNormal[normalIndex[i].x]);
//        normals.push_back(vectexNormal[normalIndex[i].y]);
//        normals.push_back(vectexNormal[normalIndex[i].z]);
//    }
//}
//// ��ʼ��
//void init()
//{
//    
//    //// �ֶ�ָ�������ε� 4 ������λ�ú�����������
//    //std::vector<glm::vec3> vectexPosition = {
//    //    glm::vec3(-1,-0.2,-1), glm::vec3(-1,-0.2,1), glm::vec3(1,-0.2,-1),glm::vec3(1,-0.2,1)
//    //};
//    //std::vector<glm::vec2> vertexTexcoord = {
//    //    glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(1, 1)
//    //};
//    //// ���ݶ���������������������Ƭ����λ�� -- ��6������
//    //points.push_back(vectexPosition[0]);
//    //points.push_back(vectexPosition[2]);
//    //points.push_back(vectexPosition[1]);
//    //points.push_back(vectexPosition[2]);
//    //points.push_back(vectexPosition[3]);
//    //points.push_back(vectexPosition[1]);
//    //// ���ݶ�����������������Ƭ���������� -- ��6������
//    //texcoords.push_back(vertexTexcoord[0]);
//    //texcoords.push_back(vertexTexcoord[2]);
//    //texcoords.push_back(vertexTexcoord[1]);
//    //texcoords.push_back(vertexTexcoord[2]);
//    //texcoords.push_back(vertexTexcoord[3]);
//    //texcoords.push_back(vertexTexcoord[1]);
//    
//
//    // ��ȡ obj �ļ�
//    readObj("models/obj/table.obj", points, texcoords, normals);
//
//    // ---------------------------------------------------------------------//
//
//    // ����vbo�����Ұ�vbo
//    GLuint vbo;
//    glGenBuffers(1, &vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    // ��ȷ��vbo�������ݴ�С -- ��NULLָ���ʾ������ʱ��������
//    GLuint dataSize = sizeof(glm::vec3) * points.size() + sizeof(glm::vec2) * texcoords.size();
//    glBufferData(GL_ARRAY_BUFFER, dataSize, NULL, GL_STATIC_DRAW);
//
//    // �������ݵ�vbo �ֱ𴫵� ����λ�� �� ������������
//    GLuint pointDataOffset = 0;
//    GLuint texcoordDataOffset = sizeof(glm::vec3) * points.size();
//    glBufferSubData(GL_ARRAY_BUFFER, pointDataOffset, sizeof(glm::vec3) * points.size(), &points[0]);
//    glBufferSubData(GL_ARRAY_BUFFER, texcoordDataOffset, sizeof(glm::vec2) * texcoords.size(), &texcoords[0]);
//
//    // ����vao�����Ұ�vao
//    GLuint vao;
//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
//
//    // ������ɫ���������
//    std::string fshaderPath = "shaders/fshader.fsh";
//    std::string vshaderPath = "shaders/vshader.vsh";
//    program = getShaderProgram(fshaderPath, vshaderPath);
//    glUseProgram(program);  // ʹ����ɫ��
//
//    // �����������vPosition����ɫ���е����� ͬʱָ��vPosition���������ݽ�����ʽ
//    GLuint vlocation = glGetAttribLocation(program, "vPosition");    // vPosition������λ������
//    glEnableVertexAttribArray(vlocation);
//    glVertexAttribPointer(vlocation, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);  // vaoָ��vPosition���������ݽ�����ʽ
//
//    // ������ɫ����vTexcoord����ɫ���е����� ͬʱָ��vTexcoord���������ݽ�����ʽ
//    GLuint tlocation = glGetAttribLocation(program, "vTexcoord");    // vTexcoord������λ������
//    glEnableVertexAttribArray(tlocation);
//    glVertexAttribPointer(tlocation, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3) * points.size()));  // ע��ָ��offset����
//
//    // ��������
//    GLuint texture;
//    glGenTextures(1, &texture);
//    glBindTexture(GL_TEXTURE_2D, texture);
//    // �������� -- ���˷�ʽ��Խ�����
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
//    // ��ȡͼƬ����
//    int textureWidth, textureHeight;
//    unsigned char* image = SOIL_load_image("models/obj/table.png", &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);   // ��������
//
//    glEnable(GL_DEPTH_TEST);  // ������Ȳ���
//
//    glClearColor(0.0, 0.0, 0.0, 1.0);   // ������ɫ -- ��
//}