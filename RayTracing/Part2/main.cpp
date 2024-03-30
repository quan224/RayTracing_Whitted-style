#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define INF 114514.0

using namespace glm;
// ----------------------------------------------------------------------------- //

GLuint program;                 // ��ɫ���������
std::vector<vec3> vertices;     // ��������
std::vector<GLuint> indices;    // ��������
std::vector<vec3> lines;        // �߶ζ˵�����
vec3 rotateControl(0, 0, 0);    // ��ת����
vec3 scaleControl(1, 1, 1);     // ���Ų���

// ----------------------------------------------------------------------------- //
//������
typedef struct Triangle {
    vec3 p1, p2, p3;   // ����
    vec3 center;       // ����
    Triangle(vec3 a, vec3 b, vec3 c) {
        p1 = a, p2 = b, p3 = c;
        center = (p1 + p2 + p3) / vec3(3, 3, 3);
    }
} Triangle;
// ����
typedef struct Ray {
    vec3 startPoint = vec3(0, 0, 0);    // ���
    vec3 direction = vec3(0, 0, 0);     // ����
} Ray;

std::vector<Triangle> triangles;
// ������������������ -- �ȽϺ���
bool cmpx(const Triangle& t1, const Triangle& t2) {
    return t1.center.x < t2.center.x;
}
bool cmpy(const Triangle& t1, const Triangle& t2) {
    return t1.center.y < t2.center.y;
}
bool cmpz(const Triangle& t1, const Triangle& t2) {
    return t1.center.z < t2.center.z;
}
// �󽻽��
struct HitResult {
    Triangle* triangle = NULL;
    float distance = INF;
};
// BVH ���ڵ�
struct BVHNode {
    BVHNode* left = NULL;       // ������������
    BVHNode* right = NULL;
    int n, index;               // Ҷ�ӽڵ���Ϣ     n��Ϊ0����Ҷ�ӽڵ㣬n�Ǵ���������θ�����ֻ��Ҷ�ӽڵ㴢�������������ڵ㴢���Χ�� index��ʾ�ڼ���������          
    vec3 AA, BB;                // ��ײ��
};

// SAH �Ż����� BVH
BVHNode* buildBVHwithSAH(std::vector<Triangle>& triangles, int l, int r, int n) {
    if (l > r) return 0;

    BVHNode* node = new BVHNode();
    node->AA = vec3(1145141919, 1145141919, 1145141919);
    node->BB = vec3(-1145141919, -1145141919, -1145141919);

    // ���� AABB�������ò����е������ν���AABB
    for (int i = l; i <= r; i++) {
        // ��С�� AA
        float minx = min(triangles[i].p1.x, min(triangles[i].p2.x, triangles[i].p3.x));
        float miny = min(triangles[i].p1.y, min(triangles[i].p2.y, triangles[i].p3.y));
        float minz = min(triangles[i].p1.z, min(triangles[i].p2.z, triangles[i].p3.z));
        node->AA.x = min(node->AA.x, minx);
        node->AA.y = min(node->AA.y, miny);
        node->AA.z = min(node->AA.z, minz);
        // ���� BB
        float maxx = max(triangles[i].p1.x, max(triangles[i].p2.x, triangles[i].p3.x));
        float maxy = max(triangles[i].p1.y, max(triangles[i].p2.y, triangles[i].p3.y));
        float maxz = max(triangles[i].p1.z, max(triangles[i].p2.z, triangles[i].p3.z));
        node->BB.x = max(node->BB.x, maxx);
        node->BB.y = max(node->BB.y, maxy);
        node->BB.z = max(node->BB.z, maxz);
    }

    // ������ n �������� ����Ҷ�ӽڵ�
    if ((r - l + 1) <= n) {
        node->n = r - l + 1;
        node->index = l;
        return node;
    }

    // ����ݹ齨��
    float Cost = INF;
    int Axis = 0;
    int Split = (l + r) / 2;
    for (int axis = 0; axis < 3; axis++) {
        // �ֱ� x��y��z ������
        if (axis == 0) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpx);
        if (axis == 1) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpy);
        if (axis == 2) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpz);

        // leftMax[i]: [l, i] ������ xyz ֵ
        // leftMin[i]: [l, i] ����С�� xyz ֵ    ���ҵ����Χ�е�AABB
        std::vector<vec3> leftMax(r - l + 1, vec3(-INF, -INF, -INF));
        std::vector<vec3> leftMin(r - l + 1, vec3(INF, INF, INF));
        // ����ǰ׺ ע�� i-l �Զ��뵽�±� 0
        for (int i = l; i <= r; i++) {
            Triangle& t = triangles[i];
            int bias = (i == l) ? 0 : 1;  // ��һ��Ԫ�����⴦��

            leftMax[i - l].x = max(leftMax[i - l - bias].x, max(t.p1.x, max(t.p2.x, t.p3.x)));
            leftMax[i - l].y = max(leftMax[i - l - bias].y, max(t.p1.y, max(t.p2.y, t.p3.y)));
            leftMax[i - l].z = max(leftMax[i - l - bias].z, max(t.p1.z, max(t.p2.z, t.p3.z)));

            leftMin[i - l].x = min(leftMin[i - l - bias].x, min(t.p1.x, min(t.p2.x, t.p3.x)));
            leftMin[i - l].y = min(leftMin[i - l - bias].y, min(t.p1.y, min(t.p2.y, t.p3.y)));
            leftMin[i - l].z = min(leftMin[i - l - bias].z, min(t.p1.z, min(t.p2.z, t.p3.z)));
        }

        // rightMax[i]: [i, r] ������ xyz ֵ
        // rightMin[i]: [i, r] ����С�� xyz ֵ���������ҵ��Ұ�Χ�е�AABB
        std::vector<vec3> rightMax(r - l + 1, vec3(-INF, -INF, -INF));
        std::vector<vec3> rightMin(r - l + 1, vec3(INF, INF, INF));
        // �����׺ ע�� i-l �Զ��뵽�±� 0
        for (int i = r; i >= l; i--) {
            Triangle& t = triangles[i];
            int bias = (i == r) ? 0 : 1;  // ��һ��Ԫ�����⴦��

            rightMax[i - l].x = max(rightMax[i - l + bias].x, max(t.p1.x, max(t.p2.x, t.p3.x)));
            rightMax[i - l].y = max(rightMax[i - l + bias].y, max(t.p1.y, max(t.p2.y, t.p3.y)));
            rightMax[i - l].z = max(rightMax[i - l + bias].z, max(t.p1.z, max(t.p2.z, t.p3.z)));

            rightMin[i - l].x = min(rightMin[i - l + bias].x, min(t.p1.x, min(t.p2.x, t.p3.x)));
            rightMin[i - l].y = min(rightMin[i - l + bias].y, min(t.p1.y, min(t.p2.y, t.p3.y)));
            rightMin[i - l].z = min(rightMin[i - l + bias].z, min(t.p1.z, min(t.p2.z, t.p3.z)));
        }

        // ����Ѱ�ҷָ�
        float cost = INF;
        int split = l;
        for (int i = l; i <= r - 1; i++) {
            float lenx, leny, lenz;
            // ��� [l, i]
            vec3 leftAA = leftMin[i - l];
            vec3 leftBB = leftMax[i - l];
            lenx = leftBB.x - leftAA.x;
            leny = leftBB.y - leftAA.y;
            lenz = leftBB.z - leftAA.z;
            float leftS = 2.0 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
            float leftCost = leftS * (i - l + 1);//���Ϊ�����ľ�������Χ�еı����������Χ�е�����������

            // �Ҳ� [i+1, r]
            vec3 rightAA = rightMin[i + 1 - l];
            vec3 rightBB = rightMax[i + 1 - l];
            lenx = rightBB.x - rightAA.x;
            leny = rightBB.y - rightAA.y;
            lenz = rightBB.z - rightAA.z;
            float rightS = 2.0 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
            float rightCost = rightS * (r - i);//���Ϊ�����ľ�������Χ�еı����������Χ�е�����������

            // ��¼ÿ���ָ����С��
            float totalCost = leftCost + rightCost;
            if (totalCost < cost) {
                cost = totalCost;
                split = i;
            }
        }
        // ��¼ÿ�������Ѵ�
        if (cost < Cost) {
            Cost = cost;
            Axis = axis;
            Split = split;
        }
    }

    // �������ָ�
    if (Axis == 0) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpx);
    if (Axis == 1) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpy);
    if (Axis == 2) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpz);

    // �ݹ飬���������ҵ���ѵ����Լ���ѵķָ�λ�ö�����ֱ�Ӷ԰��
    node->left = buildBVHwithSAH(triangles, l, Split, n);
    node->right = buildBVHwithSAH(triangles, Split + 1, r, n);

    return node;
}
// �� aabb �����󽻣�û�н����򷵻� -1,,,,û̫�㶮
float hitAABB(Ray r, vec3 AA, vec3 BB) {
    // 1.0 / direction
    vec3 invdir = vec3(1.0 / r.direction.x, 1.0 / r.direction.y, 1.0 / r.direction.z);

    vec3 in = (BB - r.startPoint) * invdir;
    vec3 out = (AA - r.startPoint) * invdir;

    vec3 tmax = max(in, out);
    vec3 tmin = min(in, out);

    float t1 = min(tmax.x, min(tmax.y, tmax.z));
    float t0 = max(tmin.x, max(tmin.y, tmin.z));

    return (t1 >= t0) ? ((t0 > 0.0) ? (t0) : (t1)) : (-1);
}

// ���ߺ��������� 
float hitTriangle(Triangle* triangle, Ray ray) {

    vec3 p1 = triangle->p1, p2 = triangle->p2, p3 = triangle->p3;
    vec3 S = ray.startPoint;        // �������
    vec3 d = ray.direction;         // ���߷���
    vec3 N = normalize(cross(p2 - p1, p3 - p1));    // ������
    if (dot(N, d) > 0.0f) N = -N;   // ��ȡ��ȷ�ķ�����

    // ������ߺ�������ƽ��
    if (fabs(dot(N, d)) < 0.00001f) return INF;

    // ����
    float t = (dot(N, p1) - dot(S, N)) / dot(d, N);
    if (t < 0.0005f) return INF;    // ����������ڹ��߱���

    // �������
    vec3 P = S + d * t;

    // �жϽ����Ƿ�����������
    vec3 c1 = cross(p2 - p1, P - p1);
    vec3 c2 = cross(p3 - p2, P - p2);
    vec3 c3 = cross(p1 - p3, P - p3);
    if (dot(c1, N) > 0 && dot(c2, N) > 0 && dot(c3, N) > 0) return t;
    if (dot(c1, N) < 0 && dot(c2, N) < 0 && dot(c3, N) < 0) return t;

    return INF;
}

// ����������
HitResult hitTriangleArray(Ray ray, std::vector<Triangle>& triangles, int l, int r) {
    HitResult res;
    for (int i = l; i <= r; i++) {
        float d = hitTriangle(&triangles[i], ray);
        //���ⷶΧ������������ұ�֮ǰ�ľ���Ҫ���ͽ��и���
        if (d < INF && d < res.distance) {
            res.distance = d;
            res.triangle = &triangles[i];
        }
    }
    return res;
}

// �� BVH �ϱ�����
HitResult hitBVH(Ray ray, std::vector<Triangle>& triangles, BVHNode* root) {
    if (root == NULL) return HitResult();

    // ��Ҷ�� ������
    if (root->n > 0) {
        return hitTriangleArray(ray, triangles, root->n, root->n + root->index - 1);
    }

    // ���������� AABB ��,�������������Χ���ཻ�����Ұ�Χ���ཻ,��Ȼһ�����߿��������Һ��Ӷ��ཻ�ˣ���Ϊ����ʱ�����
    float d1 = INF, d2 = INF;
    if (root->left) d1 = hitAABB(ray, root->left->AA, root->left->BB);
    if (root->right) d2 = hitAABB(ray, root->right->AA, root->right->BB);

    // �ݹ���
    HitResult r1, r2;
    if (d1 > 0) r1 = hitBVH(ray, triangles, root->left);
    if (d2 > 0) r2 = hitBVH(ray, triangles, root->right);
    //BVH�Ľ�����������������ģ������󽻲��ǣ���ֻ�����ҵ����Ľ����Ȼ��һ·���ؾͺ���
    return r1.distance < r2.distance ? r1 : r2;
}

// ----------------------------------------------------------------------------- //

// ��ȡ�ļ����ҷ���һ�����ַ�����ʾ�ļ�����
std::string readShaderFile(std::string filepath) {
    std::string res, line;
    std::ifstream fin(filepath);
    if (!fin.is_open())
    {
        std::cout << "�ļ� " << filepath << " ��ʧ��" << std::endl;
        exit(-1);
    }
    while (std::getline(fin, line))
    {
        res += line + '\n';
    }
    fin.close();
    return res;
}
// ��ȡ��ɫ������
GLuint getShaderProgram(std::string fshader, std::string vshader) {
    // ��ȡshaderԴ�ļ�
    std::string vSource = readShaderFile(vshader);
    std::string fSource = readShaderFile(fshader);
    const char* vpointer = vSource.c_str();
    const char* fpointer = fSource.c_str();

    // �ݴ�
    GLint success;
    GLchar infoLog[512];

    // ���������붥����ɫ��
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)(&vpointer), NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);   // ������
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "������ɫ���������\n" << infoLog << std::endl;
        exit(-1);
    }

    // �������ұ���Ƭ����ɫ��
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)(&fpointer), NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);   // ������
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Ƭ����ɫ���������\n" << infoLog << std::endl;
        exit(-1);
    }

    // ����������ɫ����program����
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // ɾ����ɫ������
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
// ��ȡ obj
void readObj(std::string filepath, std::vector<vec3>& vertices, std::vector<GLuint>& indices) {
    // ���ļ���
    std::ifstream fin(filepath);
    std::string line;
    if (!fin.is_open()) {
        std::cout << "�ļ� " << filepath << " ��ʧ��" << std::endl;
        exit(-1);
    }

    // ������ȡ
    int offset = vertices.size();

    // ���ж�ȡ
    while (std::getline(fin, line)) {
        std::istringstream sin(line);   // ��һ�е�������Ϊ string stream �������Ҷ�ȡ
        std::string type;
        GLfloat x, y, z;
        int v0, v1, v2;

        // ��ȡobj�ļ�
        sin >> type;
        if (type == "v") {
            sin >> x >> y >> z;
            vertices.push_back(vec3(x, y, z));
        }
        if (type == "f") {
            sin >> v0 >> v1 >> v2;
            indices.push_back(v0 - 1 + offset);
            indices.push_back(v1 - 1 + offset);
            indices.push_back(v2 - 1 + offset);
        }
    }
}

void addLine(vec3 p1, vec3 p2) {
    lines.push_back(p1);
    lines.push_back(p2);
}
void addBox(BVHNode* root) {
    float x1 = root->AA.x, y1 = root->AA.y, z1 = root->AA.z;
    float x2 = root->BB.x, y2 = root->BB.y, z2 = root->BB.z;
    lines.push_back(vec3(x1, y1, z1)), lines.push_back(vec3(x2, y1, z1));
    lines.push_back(vec3(x1, y1, z1)), lines.push_back(vec3(x1, y1, z2));
    lines.push_back(vec3(x1, y1, z1)), lines.push_back(vec3(x1, y2, z1));
    lines.push_back(vec3(x2, y1, z1)), lines.push_back(vec3(x2, y1, z2));
    lines.push_back(vec3(x2, y1, z1)), lines.push_back(vec3(x2, y2, z1));
    lines.push_back(vec3(x1, y2, z1)), lines.push_back(vec3(x2, y2, z1));
    lines.push_back(vec3(x1, y1, z2)), lines.push_back(vec3(x1, y2, z2));
    lines.push_back(vec3(x1, y2, z1)), lines.push_back(vec3(x1, y2, z2));
    lines.push_back(vec3(x1, y2, z2)), lines.push_back(vec3(x2, y2, z2));
    lines.push_back(vec3(x1, y1, z2)), lines.push_back(vec3(x2, y1, z2));
    lines.push_back(vec3(x2, y2, z1)), lines.push_back(vec3(x2, y2, z2));
    lines.push_back(vec3(x2, y1, z2)), lines.push_back(vec3(x2, y2, z2));
}

void addTriangle(Triangle* tri) {
    if (tri) {
        lines.push_back(tri->p1 - vec3(0.0005, 0.0005, 0.0005));
        lines.push_back(tri->p2 - vec3(0.0005, 0.0005, 0.0005));
        lines.push_back(tri->p2 - vec3(0.0005, 0.0005, 0.0005));
        lines.push_back(tri->p3 - vec3(0.0005, 0.0005, 0.0005));
        lines.push_back(tri->p3 - vec3(0.0005, 0.0005, 0.0005));
        lines.push_back(tri->p1 - vec3(0.0005, 0.0005, 0.0005));
        lines.push_back(tri->p1 + vec3(0.0005, 0.0005, 0.0005));
        lines.push_back(tri->p2 + vec3(0.0005, 0.0005, 0.0005));
        lines.push_back(tri->p2 + vec3(0.0005, 0.0005, 0.0005));
        lines.push_back(tri->p3 + vec3(0.0005, 0.0005, 0.0005));
        lines.push_back(tri->p3 + vec3(0.0005, 0.0005, 0.0005));
        lines.push_back(tri->p1 + vec3(0.0005, 0.0005, 0.0005));
    }
}

// ��ʾ�ص�����
void display() {

    // ����ģ�ͱ任����
    mat4 unit(    // ��λ����
        vec4(1, 0, 0, 0),
        vec4(0, 1, 0, 0),
        vec4(0, 0, 1, 0),
        vec4(0, 0, 0, 1)
    );
    mat4 scaleMat = scale(unit, scaleControl);   // xyz����0.6��
    mat4 rotateMat = unit;    // ��ת
    rotateMat = rotate(rotateMat, radians(rotateControl.x), vec3(1, 0, 0)); // ��x��ת
    rotateMat = rotate(rotateMat, radians(rotateControl.y), vec3(0, 1, 0)); // ��y��ת
    rotateMat = rotate(rotateMat, radians(rotateControl.z), vec3(0, 0, 1)); // ��z��ת
    mat4 modelMat = rotateMat * scaleMat;   // �任���� -- ����ģ�ͱ任����

    GLuint mlocation = glGetUniformLocation(program, "model");  // ��Ϊmodel��uniform������λ������
    glUniformMatrix4fv(mlocation, 1, GL_FALSE, value_ptr(modelMat));

    GLuint clocation = glGetUniformLocation(program, "color");

    // ����
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // ��մ�����ɫ����
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUniform3fv(clocation, 1, value_ptr(vec3(1, 0, 0)));
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // ���� AABB ����  
    glUniform3fv(clocation, 1, value_ptr(vec3(1, 1, 1)));
    glDrawArrays(GL_LINES, vertices.size(), lines.size());
    glutSwapBuffers();  // ����������
}

// ����˶�����
double lastX = 0.0, lastY = 0.0;
void mouse(int x, int y)
{
    // ������ת
    rotateControl.y += -200 * (x - lastX) / 512;
    rotateControl.x += -200 * (y - lastY) / 512;
    lastX = x, lastY = y;
    glutPostRedisplay();    // �ػ�
}

// ��갴��
void mouseDown(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        lastX = x, lastY = y;
    }
}

// �����ֺ���
void mouseWheel(int wheel, int direction, int x, int y) {
    scaleControl.x += 1 * direction * 0.1;
    scaleControl.y += 1 * direction * 0.1;
    scaleControl.z += 1 * direction * 0.1;
    glutPostRedisplay();    // �ػ�
}