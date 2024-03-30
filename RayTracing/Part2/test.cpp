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
//std::vector<glm::vec3> points;      // 顶点坐标
//std::vector<glm::vec2> texcoords;   // 顶点纹理坐标
//std::vector<glm::vec3> normals;     // 顶点法线
//
//GLuint program; // 着色器程序对象  
//
//// 读取文件并且返回一个长字符串表示文件内容
//std::string readShaderFile(std::string filepath)
//{
//    std::string res, line;
//    std::ifstream fin(filepath);
//    if (!fin.is_open())
//    {
//        std::cout << "文件 " << filepath << " 打开失败" << std::endl;
//        exit(-1);
//    }
//    while (std::getline(fin, line))
//    {
//        res += line + '\n';
//    }
//    fin.close();
//    return res;
//}
//// 获取着色器对象
//GLuint getShaderProgram(std::string fshader, std::string vshader)
//{
//    // 读取shader源文件
//    std::string vSource = readShaderFile(vshader);
//    std::string fSource = readShaderFile(fshader);
//    const char* vpointer = vSource.c_str();
//    const char* fpointer = fSource.c_str();
//
//    // 容错
//    GLint success;
//    GLchar infoLog[512];
//
//    // 创建并编译顶点着色器
//    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    //这个最后一个参数learnopengl里写的是null，我之前学的课程里是传入了length，这里又是null，以后就默认null好了
//    glShaderSource(vertexShader, 1, (const GLchar**)(&vpointer), NULL);
//    glCompileShader(vertexShader);
//    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);   // 错误检测
//    if (!success)
//    {
//        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
//        std::cout << "顶点着色器编译错误\n" << infoLog << std::endl;
//        exit(-1);
//    }
//
//    // 创建并且编译片段着色器
//    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader, 1, (const GLchar**)(&fpointer), NULL);
//    glCompileShader(fragmentShader);
//    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);   // 错误检测
//    if (!success)
//    {
//        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
//        std::cout << "片段着色器编译错误\n" << infoLog << std::endl;
//        exit(-1);
//    }
//
//    // 链接两个着色器到program对象
//    GLuint shaderProgram = glCreateProgram();
//    glAttachShader(shaderProgram, vertexShader);
//    glAttachShader(shaderProgram, fragmentShader);
//    glLinkProgram(shaderProgram);
//
//    // 删除着色器对象
//    glDeleteShader(vertexShader);
//    glDeleteShader(fragmentShader);
//
//    return shaderProgram;
//}
//// 读取obj文件并且生成最终传递给顶点着色器的 顶点位置 / 顶点纹理坐标 / 顶点法线
//void readObj(
//    std::string filepath,
//    std::vector<glm::vec3>& points,
//    std::vector<glm::vec2>& texcoords,
//    std::vector<glm::vec3>& normals
//)
//{
//    // 顶点属性
//    std::vector<glm::vec3> vectexPosition;//v
//    std::vector<glm::vec2> vertexTexcoord;//vt
//    std::vector<glm::vec3> vectexNormal;//vn
//
//    // 面片索引信息,f  x/x/x
//    std::vector<glm::ivec3> positionIndex;
//    std::vector<glm::ivec3> texcoordIndex;
//    std::vector<glm::ivec3> normalIndex;
//
//    // 打开文件流
//    std::ifstream fin(filepath);
//    std::string line;
//    if (!fin.is_open())
//    {
//        std::cout << "文件 " << filepath << " 打开失败" << std::endl;
//        exit(-1);
//    }
//
//    // 按行读取
//    while (std::getline(fin, line))
//    {
//        std::istringstream sin(line);   // 以一行的数据作为 string stream 解析并且读取
//        std::string type;
//        GLfloat x, y, z;
//        int v0, vt0, vn0;   // 面片第 1 个顶点的【位置，纹理坐标，法线】索引
//        int v1, vt1, vn1;   // 2
//        int v2, vt2, vn2;   // 3
//        char slash;
//
//        // 读取obj文件
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
//    // 根据面片信息生成最终传入顶点着色器的顶点数据
//    for (int i = 0; i < positionIndex.size(); i++)
//    {
//        // 顶点位置
//        points.push_back(vectexPosition[positionIndex[i].x]);
//        points.push_back(vectexPosition[positionIndex[i].y]);
//        points.push_back(vectexPosition[positionIndex[i].z]);
//
//        // 顶点纹理坐标
//        texcoords.push_back(vertexTexcoord[texcoordIndex[i].x]);
//        texcoords.push_back(vertexTexcoord[texcoordIndex[i].y]);
//        texcoords.push_back(vertexTexcoord[texcoordIndex[i].z]);
//
//        // 顶点法线
//        normals.push_back(vectexNormal[normalIndex[i].x]);
//        normals.push_back(vectexNormal[normalIndex[i].y]);
//        normals.push_back(vectexNormal[normalIndex[i].z]);
//    }
//}
//// 初始化
//void init()
//{
//    
//    //// 手动指定正方形的 4 个顶点位置和其纹理坐标
//    //std::vector<glm::vec3> vectexPosition = {
//    //    glm::vec3(-1,-0.2,-1), glm::vec3(-1,-0.2,1), glm::vec3(1,-0.2,-1),glm::vec3(1,-0.2,1)
//    //};
//    //std::vector<glm::vec2> vertexTexcoord = {
//    //    glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(1, 1)
//    //};
//    //// 根据顶点属性生成两个三角面片顶点位置 -- 共6个顶点
//    //points.push_back(vectexPosition[0]);
//    //points.push_back(vectexPosition[2]);
//    //points.push_back(vectexPosition[1]);
//    //points.push_back(vectexPosition[2]);
//    //points.push_back(vectexPosition[3]);
//    //points.push_back(vectexPosition[1]);
//    //// 根据顶点属性生成三角面片的纹理坐标 -- 共6个顶点
//    //texcoords.push_back(vertexTexcoord[0]);
//    //texcoords.push_back(vertexTexcoord[2]);
//    //texcoords.push_back(vertexTexcoord[1]);
//    //texcoords.push_back(vertexTexcoord[2]);
//    //texcoords.push_back(vertexTexcoord[3]);
//    //texcoords.push_back(vertexTexcoord[1]);
//    
//
//    // 读取 obj 文件
//    readObj("models/obj/table.obj", points, texcoords, normals);
//
//    // ---------------------------------------------------------------------//
//
//    // 生成vbo对象并且绑定vbo
//    GLuint vbo;
//    glGenBuffers(1, &vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    // 先确定vbo的总数据大小 -- 传NULL指针表示我们暂时不传数据
//    GLuint dataSize = sizeof(glm::vec3) * points.size() + sizeof(glm::vec2) * texcoords.size();
//    glBufferData(GL_ARRAY_BUFFER, dataSize, NULL, GL_STATIC_DRAW);
//
//    // 传送数据到vbo 分别传递 顶点位置 和 顶点纹理坐标
//    GLuint pointDataOffset = 0;
//    GLuint texcoordDataOffset = sizeof(glm::vec3) * points.size();
//    glBufferSubData(GL_ARRAY_BUFFER, pointDataOffset, sizeof(glm::vec3) * points.size(), &points[0]);
//    glBufferSubData(GL_ARRAY_BUFFER, texcoordDataOffset, sizeof(glm::vec2) * texcoords.size(), &texcoords[0]);
//
//    // 生成vao对象并且绑定vao
//    GLuint vao;
//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
//
//    // 生成着色器程序对象
//    std::string fshaderPath = "shaders/fshader.fsh";
//    std::string vshaderPath = "shaders/vshader.vsh";
//    program = getShaderProgram(fshaderPath, vshaderPath);
//    glUseProgram(program);  // 使用着色器
//
//    // 建立顶点变量vPosition在着色器中的索引 同时指定vPosition变量的数据解析格式
//    GLuint vlocation = glGetAttribLocation(program, "vPosition");    // vPosition变量的位置索引
//    glEnableVertexAttribArray(vlocation);
//    glVertexAttribPointer(vlocation, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);  // vao指定vPosition变量的数据解析格式
//
//    // 建立颜色变量vTexcoord在着色器中的索引 同时指定vTexcoord变量的数据解析格式
//    GLuint tlocation = glGetAttribLocation(program, "vTexcoord");    // vTexcoord变量的位置索引
//    glEnableVertexAttribArray(tlocation);
//    glVertexAttribPointer(tlocation, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3) * points.size()));  // 注意指定offset参数
//
//    // 生成纹理
//    GLuint texture;
//    glGenTextures(1, &texture);
//    glBindTexture(GL_TEXTURE_2D, texture);
//    // 参数设置 -- 过滤方式与越界规则
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
//    // 读取图片纹理
//    int textureWidth, textureHeight;
//    unsigned char* image = SOIL_load_image("models/obj/table.png", &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);   // 生成纹理
//
//    glEnable(GL_DEPTH_TEST);  // 开启深度测试
//
//    glClearColor(0.0, 0.0, 0.0, 1.0);   // 背景颜色 -- 黑
//}