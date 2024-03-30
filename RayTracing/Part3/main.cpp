//#include<iostream>
//#include <iomanip>
//#include <string>
//#include <fstream>
//#include <vector>
//#include <sstream>
//#include <iostream>
//#include <algorithm>
//#include <ctime>
//
//#include <GL/glew.h>
//#include <GL/freeglut.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <SOIL2/SOIL2.h>
//
//#define INF 114514.0
//
//using namespace glm;
//
//
//
//// ----------------------------------------------------------------------------- //
//
//
//// 物体表面材质定义
//struct Material {
//    vec3 emissive = vec3(0, 0, 0);  // 作为光源时的发光颜色
//    vec3 baseColor = vec3(1, 1, 1);
//    float subsurface = 0.0;
//    float metallic = 0.0;
//    float specular = 0.0;
//    float specularTint = 0.0;
//    float roughness = 0.0;
//    float anisotropic = 0.0;
//    float sheen = 0.0;
//    float sheenTint = 0.0;
//    float clearcoat = 0.0;
//    float clearcoatGloss = 0.0;
//    float IOR = 1.0;
//    float transmission = 0.0;
//};
//
//// 三角形定义
//struct Triangle {
//    vec3 p1, p2, p3;    // 顶点坐标
//    vec3 n1, n2, n3;    // 顶点法线
//    Material material;  // 材质
//};
//
//// BVH 树节点
//struct BVHNode {
//    int left, right;    // 左右子树索引
//    int n, index;       // 叶子节点信息               
//    vec3 AA, BB;        // 碰撞盒
//};
//
//// ----------------------------------------------------------------------------- //
// struct Triangle_encoded {
//vec3 p1, p2, p3;    // 顶点坐标
//vec3 n1, n2, n3;    // 顶点法线
//vec3 emissive;      // 自发光参数
//vec3 baseColor;     // 颜色
//vec3 param1;        // (subsurface, metallic, specular)
//vec3 param2;        // (specularTint, roughness, anisotropic)
//vec3 param3;        // (sheen, sheenTint, clearcoat)
//vec3 param4;        // (clearcoatGloss, IOR, transmission)
//};
//
//struct BVHNode_encoded {
//    vec3 childs;        // (left, right, 保留)
//    vec3 leafInfo;      // (n, index, 保留)
//    vec3 AA, BB;
//};
//// ----------------------------------------------------------------------------- //
//
//class RenderPass {
//public:
//    GLuint FBO = 0;
//    GLuint vao, vbo;
//    std::vector<GLuint> colorAttachments;
//    GLuint program;
//    int width = 512;
//    int height = 512;
//    void bindData(bool finalPass = false) {
//        if (!finalPass) glGenFramebuffers(1, &FBO);
//        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
//
//        glGenBuffers(1, &vbo);
//        glBindBuffer(GL_ARRAY_BUFFER, vbo);
//        std::vector<vec3> square = { vec3(-1, -1, 0), vec3(1, -1, 0), vec3(-1, 1, 0), vec3(1, 1, 0), vec3(-1, 1, 0), vec3(1, -1, 0) };
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * square.size(), NULL, GL_STATIC_DRAW);
//        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * square.size(), &square[0]);
//
//        glGenVertexArrays(1, &vao);
//        glBindVertexArray(vao);
//        glEnableVertexAttribArray(0);   // layout (location = 0) 
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
//        // 不是 finalPass 则生成帧缓冲的颜色附件
//        if (!finalPass) {
//            std::vector<GLuint> attachments;
//            for (int i = 0; i < colorAttachments.size(); i++) {
//                glBindTexture(GL_TEXTURE_2D, colorAttachments[i]);
//                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorAttachments[i], 0);// 将颜色纹理绑定到 i 号颜色附件
//                attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
//            }
//            glDrawBuffers(attachments.size(), &attachments[0]);
//        }
//
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    }
//    void draw(std::vector<GLuint> texPassArray = {}) {
//        glUseProgram(program);
//        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
//        glBindVertexArray(vao);
//        // 传上一帧的帧缓冲颜色附件
//        for (int i = 0; i < texPassArray.size(); i++) {
//            glActiveTexture(GL_TEXTURE0 + i);
//            glBindTexture(GL_TEXTURE_2D, texPassArray[i]);
//            std::string uName = "texPass" + std::to_string(i);
//            glUniform1i(glGetUniformLocation(program, uName.c_str()), i);
//        }
//        glViewport(0, 0, width, height);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//
//        glBindVertexArray(0);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glUseProgram(0);
//    }
//};
//
//// ----------------------------------------------------------------------------- //
//
//// ----------------------------------------------------------------------------- //
//// 求交结果
//struct HitResult {
//    Triangle* triangle = NULL;
//    float distance = INF;
//};
//// 光线和 aabb 盒子求交，没有交点则返回 -1
//float hitAABB(Ray r, vec3 AA, vec3 BB) {
//    // 1.0 / direction
//    vec3 invdir = vec3(1.0 / r.direction.x, 1.0 / r.direction.y, 1.0 / r.direction.z);
//
//    vec3 in = (BB - r.startPoint) * invdir;
//    vec3 out = (AA - r.startPoint) * invdir;
//
//    vec3 tmax = max(in, out);
//    vec3 tmin = min(in, out);
//
//    float t1 = min(tmax.x, min(tmax.y, tmax.z));
//    float t0 = max(tmin.x, max(tmin.y, tmin.z));
//
//    return (t1 >= t0) ? ((t0 > 0.0) ? (t0) : (t1)) : (-1);
//}
//// 光线和三角形求交 
//float hitTriangle(Triangle* triangle, Ray ray) {
//
//    vec3 p1 = triangle->p1, p2 = triangle->p2, p3 = triangle->p3;
//    vec3 S = ray.startPoint;        // 射线起点
//    vec3 d = ray.direction;         // 射线方向
//    vec3 N = normalize(cross(p2 - p1, p3 - p1));    // 法向量
//    if (dot(N, d) > 0.0f) N = -N;   // 获取正确的法向量
//
//    // 如果视线和三角形平行
//    if (fabs(dot(N, d)) < 0.00001f) return INF;
//
//    // 距离
//    float t = (dot(N, p1) - dot(S, N)) / dot(d, N);
//    if (t < 0.0005f) return INF;    // 如果三角形在光线背面
//
//    // 交点计算
//    vec3 P = S + d * t;
//
//    // 判断交点是否在三角形中
//    vec3 c1 = cross(p2 - p1, P - p1);
//    vec3 c2 = cross(p3 - p2, P - p2);
//    vec3 c3 = cross(p1 - p3, P - p3);
//    if (dot(c1, N) > 0 && dot(c2, N) > 0 && dot(c3, N) > 0) return t;
//    if (dot(c1, N) < 0 && dot(c2, N) < 0 && dot(c3, N) < 0) return t;
//
//    return INF;
//}
//// 暴力查数组
//HitResult hitTriangleArray(Ray ray, std::vector<Triangle>& triangles, int l, int r) {
//    HitResult res;
//    for (int i = l; i <= r; i++) {
//        float d = hitTriangle(&triangles[i], ray);
//        if (d < INF && d < res.distance) {
//            res.distance = d;//虽然t是时间的意思但一定程度上也能代表距离
//            res.triangle = &triangles[i];
//        }
//    }
//    return res;
//}
//// 构建 BVH
//BVHNode* buildBVH(std::vector<Triangle>& triangles, int l, int r, int n) {
//    if (l > r) return 0;
//
//    BVHNode* node = new BVHNode();
//    node->AA = vec3(1145141919, 1145141919, 1145141919);
//    node->BB = vec3(-1145141919, -1145141919, -1145141919);
//
//    // 计算 AABB
//    for (int i = l; i <= r; i++) {
//        // 最小点 AA
//        float minx = min(triangles[i].p1.x, min(triangles[i].p2.x, triangles[i].p3.x));
//        float miny = min(triangles[i].p1.y, min(triangles[i].p2.y, triangles[i].p3.y));
//        float minz = min(triangles[i].p1.z, min(triangles[i].p2.z, triangles[i].p3.z));
//        node->AA.x = min(node->AA.x, minx);
//        node->AA.y = min(node->AA.y, miny);
//        node->AA.z = min(node->AA.z, minz);
//        // 最大点 BB
//        float maxx = max(triangles[i].p1.x, max(triangles[i].p2.x, triangles[i].p3.x));
//        float maxy = max(triangles[i].p1.y, max(triangles[i].p2.y, triangles[i].p3.y));
//        float maxz = max(triangles[i].p1.z, max(triangles[i].p2.z, triangles[i].p3.z));
//        node->BB.x = max(node->BB.x, maxx);
//        node->BB.y = max(node->BB.y, maxy);
//        node->BB.z = max(node->BB.z, maxz);
//    }
//
//    // 不多于 n 个三角形 返回叶子节点
//    if ((r - l + 1) <= n) {
//        node->n = r - l + 1;
//        node->index = l;
//        return node;
//    }
//
//    // 否则递归建树
//    float lenx = node->BB.x - node->AA.x;
//    float leny = node->BB.y - node->AA.y;
//    float lenz = node->BB.z - node->AA.z;
//    // 按 x 划分
//    if (lenx >= leny && lenx >= lenz)
//        std::sort(triangles.begin() + l, triangles.begin() + r + 1, cmpx);
//    // 按 y 划分
//    if (leny >= lenx && leny >= lenz)
//        std::sort(triangles.begin() + l, triangles.begin() + r + 1, cmpy);
//    // 按 z 划分
//    if (lenz >= lenx && lenz >= leny)
//        std::sort(triangles.begin() + l, triangles.begin() + r + 1, cmpz);
//
//    // 递归
//    int mid = (l + r) / 2;
//    node->left = buildBVH(triangles, l, mid, n);
//    node->right = buildBVH(triangles, mid + 1, r, n);
//
//    return node;
//}
//// SAH 优化构建 BVH
//int buildBVHwithSAH(std::vector<Triangle>& triangles, std::vector<BVHNode>& nodes, int l, int r, int n) {
//    if (l > r) return 0;
//
//    nodes.push_back(BVHNode());
//    int id = nodes.size() - 1;
//    nodes[id].left = nodes[id].right = nodes[id].n = nodes[id].index = 0;
//    nodes[id].AA = vec3(1145141919, 1145141919, 1145141919);
//    nodes[id].BB = vec3(-1145141919, -1145141919, -1145141919);
//
//    // 计算 AABB
//    for (int i = l; i <= r; i++) {
//        // 最小点 AA
//        float minx = min(triangles[i].p1.x, min(triangles[i].p2.x, triangles[i].p3.x));
//        float miny = min(triangles[i].p1.y, min(triangles[i].p2.y, triangles[i].p3.y));
//        float minz = min(triangles[i].p1.z, min(triangles[i].p2.z, triangles[i].p3.z));
//        nodes[id].AA.x = min(nodes[id].AA.x, minx);
//        nodes[id].AA.y = min(nodes[id].AA.y, miny);
//        nodes[id].AA.z = min(nodes[id].AA.z, minz);
//        // 最大点 BB
//        float maxx = max(triangles[i].p1.x, max(triangles[i].p2.x, triangles[i].p3.x));
//        float maxy = max(triangles[i].p1.y, max(triangles[i].p2.y, triangles[i].p3.y));
//        float maxz = max(triangles[i].p1.z, max(triangles[i].p2.z, triangles[i].p3.z));
//        nodes[id].BB.x = max(nodes[id].BB.x, maxx);
//        nodes[id].BB.y = max(nodes[id].BB.y, maxy);
//        nodes[id].BB.z = max(nodes[id].BB.z, maxz);
//    }
//
//    // 不多于 n 个三角形 返回叶子节点
//    if ((r - l + 1) <= n) {
//        nodes[id].n = r - l + 1;
//        nodes[id].index = l;
//        return id;
//    }
//
//    // 否则递归建树
//    float Cost = INF;
//    int Axis = 0;
//    int Split = (l + r) / 2;
//    for (int axis = 0; axis < 3; axis++) {
//        // 分别按 x，y，z 轴排序
//        if (axis == 0) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpx);
//        if (axis == 1) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpy);
//        if (axis == 2) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpz);
//
//        // leftMax[i]: [l, i] 中最大的 xyz 值
//        // leftMin[i]: [l, i] 中最小的 xyz 值
//        std::vector<vec3> leftMax(r - l + 1, vec3(-INF, -INF, -INF));
//        std::vector<vec3> leftMin(r - l + 1, vec3(INF, INF, INF));
//        // 计算前缀 注意 i-l 以对齐到下标 0
//        for (int i = l; i <= r; i++) {
//            Triangle& t = triangles[i];
//            int bias = (i == l) ? 0 : 1;  // 第一个元素特殊处理
//
//            leftMax[i - l].x = max(leftMax[i - l - bias].x, max(t.p1.x, max(t.p2.x, t.p3.x)));
//            leftMax[i - l].y = max(leftMax[i - l - bias].y, max(t.p1.y, max(t.p2.y, t.p3.y)));
//            leftMax[i - l].z = max(leftMax[i - l - bias].z, max(t.p1.z, max(t.p2.z, t.p3.z)));
//
//            leftMin[i - l].x = min(leftMin[i - l - bias].x, min(t.p1.x, min(t.p2.x, t.p3.x)));
//            leftMin[i - l].y = min(leftMin[i - l - bias].y, min(t.p1.y, min(t.p2.y, t.p3.y)));
//            leftMin[i - l].z = min(leftMin[i - l - bias].z, min(t.p1.z, min(t.p2.z, t.p3.z)));
//        }
//
//        // rightMax[i]: [i, r] 中最大的 xyz 值
//        // rightMin[i]: [i, r] 中最小的 xyz 值
//        std::vector<vec3> rightMax(r - l + 1, vec3(-INF, -INF, -INF));
//        std::vector<vec3> rightMin(r - l + 1, vec3(INF, INF, INF));
//        // 计算后缀 注意 i-l 以对齐到下标 0
//        for (int i = r; i >= l; i--) {
//            Triangle& t = triangles[i];
//            int bias = (i == r) ? 0 : 1;  // 第一个元素特殊处理
//
//            rightMax[i - l].x = max(rightMax[i - l + bias].x, max(t.p1.x, max(t.p2.x, t.p3.x)));
//            rightMax[i - l].y = max(rightMax[i - l + bias].y, max(t.p1.y, max(t.p2.y, t.p3.y)));
//            rightMax[i - l].z = max(rightMax[i - l + bias].z, max(t.p1.z, max(t.p2.z, t.p3.z)));
//
//            rightMin[i - l].x = min(rightMin[i - l + bias].x, min(t.p1.x, min(t.p2.x, t.p3.x)));
//            rightMin[i - l].y = min(rightMin[i - l + bias].y, min(t.p1.y, min(t.p2.y, t.p3.y)));
//            rightMin[i - l].z = min(rightMin[i - l + bias].z, min(t.p1.z, min(t.p2.z, t.p3.z)));
//        }
//
//        // 遍历寻找分割
//        float cost = INF;
//        int split = l;
//        for (int i = l; i <= r - 1; i++) {
//            float lenx, leny, lenz;
//            // 左侧 [l, i]
//            vec3 leftAA = leftMin[i - l];
//            vec3 leftBB = leftMax[i - l];
//            lenx = leftBB.x - leftAA.x;
//            leny = leftBB.y - leftAA.y;
//            lenz = leftBB.z - leftAA.z;
//            float leftS = 2.0 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
//            float leftCost = leftS * (i - l + 1);
//
//            // 右侧 [i+1, r]
//            vec3 rightAA = rightMin[i + 1 - l];
//            vec3 rightBB = rightMax[i + 1 - l];
//            lenx = rightBB.x - rightAA.x;
//            leny = rightBB.y - rightAA.y;
//            lenz = rightBB.z - rightAA.z;
//            float rightS = 2.0 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
//            float rightCost = rightS * (r - i);
//
//            // 记录每个分割的最小答案
//            float totalCost = leftCost + rightCost;
//            if (totalCost < cost) {
//                cost = totalCost;
//                split = i;
//            }
//        }
//        // 记录每个轴的最佳答案
//        if (cost < Cost) {
//            Cost = cost;
//            Axis = axis;
//            Split = split;
//        }
//    }
//
//    // 按最佳轴分割
//    if (Axis == 0) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpx);
//    if (Axis == 1) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpy);
//    if (Axis == 2) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpz);
//
//    // 递归
//    int left = buildBVHwithSAH(triangles, nodes, l, Split, n);
//    int right = buildBVHwithSAH(triangles, nodes, Split + 1, r, n);
//
//    nodes[id].left = left;
//    nodes[id].right = right;
//
//    return id;
//}
//
//// ----------------------------------------------------------------------------- //
//// 在 BVH 上遍历求交
//HitResult hitBVH(Ray ray, std::vector<Triangle>& triangles, BVHNode* root) {
//    if (root == NULL) return HitResult();
//
//    // 是叶子 暴力查
//    if (root->n > 0) {
//        return hitTriangleArray(ray, triangles, root->n, root->n + root->index - 1);
//    }
//
//    // 和左右子树 AABB 求交
//    float d1 = INF, d2 = INF;
//    if (root->left) d1 = hitAABB(ray, root->left->AA, root->left->BB);
//    if (root->right) d2 = hitAABB(ray, root->right->AA, root->right->BB);
//
//    // 递归结果
//    HitResult r1, r2;
//    if (d1 > 0) r1 = hitBVH(ray, triangles, root->left);
//    if (d2 > 0) r2 = hitBVH(ray, triangles, root->right);
//
//    return r1.distance < r2.distance ? r1 : r2;
//}
//
//// --------------------------------OPENGL等--------------------------------------- //
//GLuint program;
//// 顶点坐标 / 颜色
//std::vector<glm::vec3> points, colors;
//std::vector<glm::vec2> texcoords;
//std::vector<glm::vec3> normals;
//std::vector<GLuint> indices;    // 顶点索引
//std::vector<vec3> lines;        // 线段端点坐标
//glm::vec3 scaleControl(1, 1, 1);  // 缩放控制
//glm::vec3 rotateControl(0, 0, 0);   // 旋转控制
//glm::vec3 translateControl(0, 0, 0);    // 平移控制
//
//int windowWidth = 512;  // 窗口宽
//int windowHeight = 512; // 窗口高
//// 键盘状态数组 keyboardState[x]==true 表示按下x键
//bool keyboardState[1024];
//glm::vec3 cameraPosition(0, 0, 2);      // 相机位置
//glm::vec3 cameraDirection(0, 0, -1);    // 相机视线方向 -- 看向z轴负方向
//glm::vec3 cameraUp(0, 1, 0);            // 世界空间下竖直向上向量
//float pitcha = 0.0f;
//float rolla = 0.0f;
//float yawa = 0.0f;
//// 投影参数
//float fovy = 70.0f;
//float aspect = windowWidth / windowHeight;
//float zNear = 0.1, zFar = 100.0;
//
//
//void addLine(vec3 p1, vec3 p2) {
//    lines.push_back(p1);
//    lines.push_back(p2);
//}
//void addBox(BVHNode* root) {
//    float x1 = root->AA.x, y1 = root->AA.y, z1 = root->AA.z;
//    float x2 = root->BB.x, y2 = root->BB.y, z2 = root->BB.z;
//    lines.push_back(vec3(x1, y1, z1)), lines.push_back(vec3(x2, y1, z1));
//    lines.push_back(vec3(x1, y1, z1)), lines.push_back(vec3(x1, y1, z2));
//    lines.push_back(vec3(x1, y1, z1)), lines.push_back(vec3(x1, y2, z1));
//    lines.push_back(vec3(x2, y1, z1)), lines.push_back(vec3(x2, y1, z2));
//    lines.push_back(vec3(x2, y1, z1)), lines.push_back(vec3(x2, y2, z1));
//    lines.push_back(vec3(x1, y2, z1)), lines.push_back(vec3(x2, y2, z1));
//    lines.push_back(vec3(x1, y1, z2)), lines.push_back(vec3(x1, y2, z2));
//    lines.push_back(vec3(x1, y2, z1)), lines.push_back(vec3(x1, y2, z2));
//    lines.push_back(vec3(x1, y2, z2)), lines.push_back(vec3(x2, y2, z2));
//    lines.push_back(vec3(x1, y1, z2)), lines.push_back(vec3(x2, y1, z2));
//    lines.push_back(vec3(x2, y2, z1)), lines.push_back(vec3(x2, y2, z2));
//    lines.push_back(vec3(x2, y1, z2)), lines.push_back(vec3(x2, y2, z2));
//}
//
//void addTriangle(Triangle* tri) {
//    if (tri) {
//        lines.push_back(tri->p1 - vec3(0.0005, 0.0005, 0.0005));
//        lines.push_back(tri->p2 - vec3(0.0005, 0.0005, 0.0005));
//        lines.push_back(tri->p2 - vec3(0.0005, 0.0005, 0.0005));
//        lines.push_back(tri->p3 - vec3(0.0005, 0.0005, 0.0005));
//        lines.push_back(tri->p3 - vec3(0.0005, 0.0005, 0.0005));
//        lines.push_back(tri->p1 - vec3(0.0005, 0.0005, 0.0005));
//        lines.push_back(tri->p1 + vec3(0.0005, 0.0005, 0.0005));
//        lines.push_back(tri->p2 + vec3(0.0005, 0.0005, 0.0005));
//        lines.push_back(tri->p2 + vec3(0.0005, 0.0005, 0.0005));
//        lines.push_back(tri->p3 + vec3(0.0005, 0.0005, 0.0005));
//        lines.push_back(tri->p3 + vec3(0.0005, 0.0005, 0.0005));
//        lines.push_back(tri->p1 + vec3(0.0005, 0.0005, 0.0005));
//    }
//}
//void dfsNlevel(BVHNode* root, int depth, int targetDepth) {
//    if (root == NULL) return;
//    if (targetDepth == depth) {
//        addBox(root);
//        return;
//    }
//    dfsNlevel(root->left, depth + 1, targetDepth);
//    dfsNlevel(root->right, depth + 1, targetDepth);
//}
//
//// 读取文件并且返回一个长字符串表示文件内容
//std::string readShaderFile(std::string filepath) {
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
//// 读取 obj
//void readObj(std::string filepath, std::vector<Triangle>& triangles, Material material, mat4 trans, bool smoothNormal) {
//
//    // 顶点位置，索引
//    std::vector<vec3> vertices;
//    std::vector<GLuint> indices;
//
//    // 打开文件流
//    std::ifstream fin(filepath);
//    std::string line;
//    if (!fin.is_open()) {
//        std::cout << "文件 " << filepath << " 打开失败" << std::endl;
//        exit(-1);
//    }
//
//    // 计算 AABB 盒，归一化模型大小
//    float maxx = -11451419.19;
//    float maxy = -11451419.19;
//    float maxz = -11451419.19;
//    float minx = 11451419.19;
//    float miny = 11451419.19;
//    float minz = 11451419.19;
//
//    // 按行读取
//    while (std::getline(fin, line)) {
//        std::istringstream sin(line);   // 以一行的数据作为 string stream 解析并且读取
//        std::string type;
//        GLfloat x, y, z;
//        int v0, v1, v2;
//        int vn0, vn1, vn2;
//        int vt0, vt1, vt2;
//        char slash;
//
//        // 统计斜杆数目，用不同格式读取
//        int slashCnt = 0;
//        for (int i = 0; i < line.length(); i++) {
//            if (line[i] == '/') slashCnt++;
//        }
//
//        // 读取obj文件
//        sin >> type;
//        if (type == "v") {
//            sin >> x >> y >> z;
//            vertices.push_back(vec3(x, y, z));
//            maxx = max(maxx, x); maxy = max(maxx, y); maxz = max(maxx, z);
//            minx = min(minx, x); miny = min(minx, y); minz = min(minx, z);
//        }
//        if (type == "f") {
//            if (slashCnt == 6) {
//                sin >> v0 >> slash >> vt0 >> slash >> vn0;
//                sin >> v1 >> slash >> vt1 >> slash >> vn1;
//                sin >> v2 >> slash >> vt2 >> slash >> vn2;
//            }
//            else if (slashCnt == 3) {
//                sin >> v0 >> slash >> vt0;
//                sin >> v1 >> slash >> vt1;
//                sin >> v2 >> slash >> vt2;
//            }
//            else {
//                sin >> v0 >> v1 >> v2;
//            }
//            indices.push_back(v0 - 1);
//            indices.push_back(v1 - 1);
//            indices.push_back(v2 - 1);
//        }
//    }
//
//    // 模型大小归一化
//    float lenx = maxx - minx;
//    float leny = maxy - miny;
//    float lenz = maxz - minz;
//    float maxaxis = max(lenx, max(leny, lenz));
//    for (auto& v : vertices) {
//        v.x /= maxaxis;
//        v.y /= maxaxis;
//        v.z /= maxaxis;
//    }
//
//    // 通过矩阵进行坐标变换
//    for (auto& v : vertices) {
//        vec4 vv = vec4(v.x, v.y, v.z, 1);
//        vv = trans * vv;
//        v = vec3(vv.x, vv.y, vv.z);
//    }
//
//    // 生成法线
//    std::vector<vec3> normals(vertices.size(), vec3(0, 0, 0));
//    for (int i = 0; i < indices.size(); i += 3) {
//        vec3 p1 = vertices[indices[i]];
//        vec3 p2 = vertices[indices[i + 1]];
//        vec3 p3 = vertices[indices[i + 2]];
//        vec3 n = normalize(cross(p2 - p1, p3 - p1));
//        normals[indices[i]] += n;
//        normals[indices[i + 1]] += n;
//        normals[indices[i + 2]] += n;
//    }
//
//    // 构建 Triangle 对象数组
//    int offset = triangles.size();  // 增量更新
//    triangles.resize(offset + indices.size() / 3);
//    for (int i = 0; i < indices.size(); i += 3) {
//        Triangle& t = triangles[offset + i / 3];
//        // 传顶点属性
//        t.p1 = vertices[indices[i]];
//        t.p2 = vertices[indices[i + 1]];
//        t.p3 = vertices[indices[i + 2]];
//        if (!smoothNormal) {
//            vec3 n = normalize(cross(t.p2 - t.p1, t.p3 - t.p1));
//            t.n1 = n; t.n2 = n; t.n3 = n;
//        }
//        else {
//            t.n1 = normalize(normals[indices[i]]);
//            t.n2 = normalize(normals[indices[i + 1]]);
//            t.n3 = normalize(normals[indices[i + 2]]);
//        }
//
//        // 传材质
//        t.material = material;
//    }
//}
//
//
//// 获取着色器对象
//GLuint getShaderProgram(std::string fshader, std::string vshader) {
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
//// 模型变换矩阵
//mat4 getTransformMatrix( vec3 scaleCtrl, vec3 rotateCtrl, vec3 translateCtrl) {
//    glm::mat4 model(    // 单位矩阵
//        glm::vec4(1, 0, 0, 0),
//        glm::vec4(0, 1, 0, 0),
//        glm::vec4(0, 0, 1, 0),
//        glm::vec4(0, 0, 0, 1)
//    );
//    model = glm::translate(model, translateCtrl);
//    model = glm::rotate(model, glm::radians(rotateCtrl.x), glm::vec3(1, 0, 0));
//    model = glm::rotate(model, glm::radians(rotateCtrl.y), glm::vec3(0, 1, 0));
//    model = glm::rotate(model, glm::radians(rotateCtrl.z), glm::vec3(0, 0, 1));
//    model = glm::scale(model, scaleCtrl);
//    return model;
//}
//
//
//void init()
//{
//
//
//    // 生成vbo对象并且绑定vbo
//    GLuint vbo;
//    glGenBuffers(1, &vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    // 先确定vbo的总数据大小（画饼？？？） 传NULL指针表示我们暂时不传数据
//    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * (points.size() + lines.size()), NULL, GL_STATIC_DRAW);
//
//    // 传送数据到vbo 分别传递 顶点位置 和 顶点颜色
//    //GLuint pointDataOffset = 0;
//    //GLuint colorDataOffset = sizeof(glm::vec3) * points.size();
//    //glBufferSubData(GL_ARRAY_BUFFER, pointDataOffset, sizeof(glm::vec3) * points.size(), &points[0]);
//    //glBufferSubData(GL_ARRAY_BUFFER, colorDataOffset, sizeof(glm::vec3) * texcoords.size(), &texcoords[0]);
//
//    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * points.size(), points.data());
//    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec3) * points.size(), sizeof(vec3) * lines.size(), lines.data());
//
//    // 生成vao对象并且绑定vao
//    GLuint vao;
//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
//
//    // 传索引到 ebo
//    GLuint ebo;
//    glGenBuffers(1, &ebo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
//
//    // 生成着色器程序对象
//    std::string fshaderPath = "shaders/shader.frag";
//    std::string vshaderPath = "shaders/shader.vert";
//    program = getShaderProgram(fshaderPath, vshaderPath);
//    glUseProgram(program);  // 使用着色器
//
//    // 建立顶点变量vPosition在着色器中的索引 同时指定vPosition变量的数据解析格式
//    GLuint vlocation = glGetAttribLocation(program, "vPosition");    // vPosition变量的位置索引
//    glEnableVertexAttribArray(vlocation);
//    //上面的VBO放了两种属性，而这里制作了一次描述是因为这两种属性都是位置属性
//    glVertexAttribPointer(vlocation, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);  // vao指定vPosition变量的数据解析格式
//
//    // 建立颜色变量vColor在着色器中的索引 同时指定vColor变量的数据解析格式
//    GLuint clocation = glGetAttribLocation(program, "vColor");    // vColor变量的位置索引
//    glEnableVertexAttribArray(clocation);
//    glVertexAttribPointer(clocation, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3) * points.size()));  // 注意指定offset参数
//    glDisableVertexAttribArray(clocation);
//
//    // 建立颜色变量vTexcoord在着色器中的索引 同时指定vTexcoord变量的数据解析格式
//    GLuint tlocation = glGetAttribLocation(program, "vTexcoord");    // vTexcoord变量的位置索引
//    glEnableVertexAttribArray(tlocation);
//    glVertexAttribPointer(tlocation, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(glm::vec3) * points.size()));  // 注意指定offset参数
//    glDisableVertexAttribArray(tlocation);
//
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
//    unsigned char* image = SOIL_load_image("textures/Cat.png", &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);   // 生成纹理
//
//
//    glClearColor(0.0, 0.0, 0.0, 1.0);   // 背景颜色 -- 黑
//    glEnable(GL_DEPTH_TEST);  // 开启深度测试
//}
//
//// 鼠标运动函数
//void mouse(int x, int y)
//{
//    // 调整旋转
//    yawa += 35 * (x - float(windowWidth) / 2.0) / windowWidth;
//    yawa = glm::mod(yawa + 180.0f, 360.0f) - 180.0f;    // 取模范围 -180 ~ 180
//
//    pitcha += -35 * (y - float(windowHeight) / 2.0) / windowHeight;
//    pitcha = glm::clamp(pitcha, -89.0f, 89.0f);
//
//    glutWarpPointer(windowWidth / 2.0, windowHeight / 2.0);	// 将指针钉死在屏幕正中间
//    glutPostRedisplay();    // 重绘
//}
//// 鼠标滚轮函数
//void mouseWheel(int wheel, int direction, int x, int y)
//{
//    scaleControl += 1 * direction * 0.1;
//    glutPostRedisplay();    // 重绘
//}
//// 根据键盘状态判断移动
//// 根据键盘状态判断移动
//void move()
//{
//    if (keyboardState['w']) cameraPosition += 0.0005f * cameraDirection;
//    if (keyboardState['s']) cameraPosition -= 0.0005f * cameraDirection;
//    if (keyboardState['a']) cameraPosition -= 0.0005f * glm::normalize(glm::cross(cameraDirection, cameraUp));
//    if (keyboardState['d']) cameraPosition += 0.0005f * glm::normalize(glm::cross(cameraDirection, cameraUp));
//    if (keyboardState[GLUT_KEY_CTRL_L]) cameraPosition.y -= 0.0005;
//    if (keyboardState[' ']) cameraPosition.y += 0.0005;
//    glutPostRedisplay();    // 重绘
//}
//// 键盘回调函数
//void keyboardDown(unsigned char key, int x, int y)
//{
//    keyboardState[key] = true;
//}
//void keyboardDownSpecial(int key, int x, int y)
//{
//    keyboardState[key] = true;
//}
//void keyboardUp(unsigned char key, int x, int y)
//{
//    keyboardState[key] = false;
//}
//void keyboardUpSpecial(int key, int x, int y)
//{
//    keyboardState[key] = false;
//}
//// ----------------------------------------------------------------------------- //
//void display()
//{
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // 清空窗口颜色,深度缓存
//    // 传模型矩阵
//    move();
//    mat4 model = getTransformMatrix(scaleControl, rotateControl, translateControl);
//    GLuint mlocation = glGetUniformLocation(program, "model");    // 名为model的uniform变量的位置索引
//    glUniformMatrix4fv(mlocation, 1, GL_FALSE, glm::value_ptr(model));   // 列优先矩阵
//    // 传视图矩阵
//    // 计算欧拉角以确定相机朝向
//    cameraDirection.x = cos(glm::radians(pitcha)) * sin(glm::radians(yawa));
//    cameraDirection.y = sin(glm::radians(pitcha));
//    cameraDirection.z = -cos(glm::radians(pitcha)) * cos(glm::radians(yawa)); // 相机看向z轴负方向
//    glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
//    GLuint vlocation = glGetUniformLocation(program, "view");
//    glUniformMatrix4fv(vlocation, 1, GL_FALSE, glm::value_ptr(view));
//
//    // 传投影矩阵
//    glm::mat4 projection = glm::perspective(glm::radians(fovy), aspect, zNear, zFar);
//    GLuint plocation = glGetUniformLocation(program, "projection");
//    glUniformMatrix4fv(plocation, 1, GL_FALSE, glm::value_ptr(projection));
//
//    GLuint clocation = glGetUniformLocation(program, "color");
//    glUniform3fv(clocation, 1, value_ptr(vec3(1, 0, 0)));
//    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//   // glDrawArrays(GL_TRIANGLES, 0, points.size());   // 绘制三点
//
//        // 绘制 AABB 盒子  
//    glUniform3fv(clocation, 1, value_ptr(vec3(1, 1, 1)));
//    //这里为什么是DrawArray而不是DrawElements是因为indices用完了，obj数据只储存了自己的索引位置信息有没有弄这个
//    glDrawArrays(GL_LINES, points.size(), lines.size());
//    glutSwapBuffers();  // 交换缓冲区
//
//    glutSwapBuffers();                  // 交换缓冲区
//}
//int main(int argc, char** argv) {
//
//    glutInit(&argc, argv);              // glut初始化
//    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
//    glutInitWindowSize(windowWidth, windowHeight);// 窗口大小
//    glutInitWindowPosition(350, 50);
//    glutCreateWindow("Path Tracing GPU"); // 创建OpenGL上下文
//    glewInit();
//    // ----------------------------------------------------------------------------- //
//
// // scene config
//    std::vector<Triangle> triangles;
//
//    Material m;
//    m.baseColor = vec3(1, 1, 1);
//    readObj("models/Stanford Bunny.obj", triangles, m, getTransformMatrix(vec3(0, 0, 0), vec3(0.3, -1.6, 0), vec3(1.5, 1.5, 1.5)), true);
//    //readObj("models/room.obj", triangles, m, getTransformMatrix(vec3(0, 0, 0), vec3(0.0, -2.5, 0), vec3(10, 10, 10)), true);
//
//    m.baseColor = vec3(0.725, 0.71, 0.68);
//    readObj("models/quad.obj", triangles, m, getTransformMatrix(vec3(0, 0, 0), vec3(0, -1.4, 0), vec3(18.83, 0.01, 18.83)), false);
//
//    m.baseColor = vec3(1, 1, 1);
//    m.emissive = vec3(30, 20, 10);
//    //readObj("models/quad.obj", triangles, m, getTransformMatrix(vec3(0, 0, 0), vec3(0.0, 1.38, -0.0), vec3(0.7, 0.01, 0.7)), false);
//    readObj("models/sphere.obj", triangles, m, getTransformMatrix(vec3(0, 0, 0), vec3(0.0, 0.9, -0.0), vec3(1, 1, 1)), false);
//
//    int nTriangles = triangles.size();
//    std::cout << "模型读取完成: 共 " << nTriangles << " 个三角形" << std::endl;
//
//    // 建立 bvh
//    BVHNode testNode;
//    testNode.left = 255;
//    testNode.right = 128;
//    testNode.n = 30;
//    testNode.AA = vec3(1, 1, 0);
//    testNode.BB = vec3(0, 1, 0);
//    std::vector<BVHNode> nodes{ testNode };
//    //buildBVH(triangles, nodes, 0, triangles.size() - 1, 8);
//    buildBVHwithSAH(triangles, nodes, 0, triangles.size() - 1, 8);
//    int nNodes = nodes.size();
//    std::cout << "BVH 建立完成: 共 " << nNodes << " 个节点" << std::endl;
//
//    // 编码 三角形, 材质
//    std::vector<Triangle_encoded> triangles_encoded(nTriangles);
//    for (int i = 0; i < nTriangles; i++) {
//        Triangle& t = triangles[i];
//        Material& m = t.material;
//        // 顶点位置
//        triangles_encoded[i].p1 = t.p1;
//        triangles_encoded[i].p2 = t.p2;
//        triangles_encoded[i].p3 = t.p3;
//        // 顶点法线
//        triangles_encoded[i].n1 = t.n1;
//        triangles_encoded[i].n2 = t.n2;
//        triangles_encoded[i].n3 = t.n3;
//        // 材质
//        triangles_encoded[i].emissive = m.emissive;
//        triangles_encoded[i].baseColor = m.baseColor;
//        triangles_encoded[i].param1 = vec3(m.subsurface, m.metallic, m.specular);
//        triangles_encoded[i].param2 = vec3(m.specularTint, m.roughness, m.anisotropic);
//        triangles_encoded[i].param3 = vec3(m.sheen, m.sheenTint, m.clearcoat);
//        triangles_encoded[i].param4 = vec3(m.clearcoatGloss, m.IOR, m.transmission);
//    }
//
//    // 编码 BVHNode, aabb
//    std::vector<BVHNode_encoded> nodes_encoded(nNodes);
//    for (int i = 0; i < nNodes; i++) {
//        nodes_encoded[i].childs = vec3(nodes[i].left, nodes[i].right, 0);
//        nodes_encoded[i].leafInfo = vec3(nodes[i].n, nodes[i].index, 0);
//        nodes_encoded[i].AA = nodes[i].AA;
//        nodes_encoded[i].BB = nodes[i].BB;
//    }
//
//    // ----------------------------------------------------------------------------- //
//
//    // 生成纹理
//
//    // 三角形数组
//    GLuint trianglesTextureBuffer;
//    GLuint tbo0;
//    glGenBuffers(1, &tbo0);
//    glBindBuffer(GL_TEXTURE_BUFFER, tbo0);
//    glBufferData(GL_TEXTURE_BUFFER, triangles_encoded.size() * sizeof(Triangle_encoded), &triangles_encoded[0], GL_STATIC_DRAW);
//    glGenTextures(1, &trianglesTextureBuffer);
//    glBindTexture(GL_TEXTURE_BUFFER, trianglesTextureBuffer);
//    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo0);
//
//    // BVHNode 数组
//    GLuint tbo1;
//    GLuint nodesTextureBuffer;
//    glGenBuffers(1, &tbo1);
//    glBindBuffer(GL_TEXTURE_BUFFER, tbo1);
//    glBufferData(GL_TEXTURE_BUFFER, nodes_encoded.size() * sizeof(BVHNode_encoded), &nodes_encoded[0], GL_STATIC_DRAW);
//    glGenTextures(1, &nodesTextureBuffer);
//    glBindTexture(GL_TEXTURE_BUFFER, nodesTextureBuffer);
//    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo1);
//
//    // hdr 全景图
//    HDRLoaderResult hdrRes;
//    bool r = HDRLoader::load("./HDR/sunset.hdr", hdrRes);
//    hdrMap = getTextureRGB32F(hdrRes.width, hdrRes.height);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, hdrRes.width, hdrRes.height, 0, GL_RGB, GL_FLOAT, hdrRes.cols);
//
//    // ----------------------------------------------------------------------------- //
//
//    // 管线配置
//
//    pass1.program = getShaderProgram("./shaders/fshader.fsh", "./shaders/vshader.vsh");
//    //pass1.width = pass1.height = 256;
//    pass1.colorAttachments.push_back(getTextureRGB32F(pass1.width, pass1.height));
//    pass1.colorAttachments.push_back(getTextureRGB32F(pass1.width, pass1.height));
//    pass1.colorAttachments.push_back(getTextureRGB32F(pass1.width, pass1.height));
//    pass1.bindData();
//
//    glUseProgram(pass1.program);
//    glUniform1i(glGetUniformLocation(pass1.program, "nTriangles"), triangles.size());
//    glUniform1i(glGetUniformLocation(pass1.program, "nNodes"), nodes.size());
//    glUniform1i(glGetUniformLocation(pass1.program, "width"), pass1.width);
//    glUniform1i(glGetUniformLocation(pass1.program, "height"), pass1.height);
//    glUseProgram(0);
//
//    pass2.program = getShaderProgram("./shaders/pass2.fsh", "./shaders/vshader.vsh");
//    lastFrame = getTextureRGB32F(pass2.width, pass2.height);
//    pass2.colorAttachments.push_back(lastFrame);
//    pass2.bindData();
//
//    pass3.program = getShaderProgram("./shaders/pass3.fsh", "./shaders/vshader.vsh");
//    pass3.bindData(true);
//
//
//
//  // ----------------------------------------------------------------------------- //
//     //init();
//    glEnable(GL_DEPTH_TEST);  // 开启深度测试
//    glClearColor(0.0, 0.0, 0.0, 1.0);   // 背景颜色 -- 黑
//    glutDisplayFunc(display);           // 设置显示回调函数 -- 每帧执行
//    glutPassiveMotionFunc(mouse);   // 鼠标直接移动
//    glutMouseWheelFunc(mouseWheel); // 滚轮缩放
//    // 绑定键盘函数
//    glutKeyboardFunc(keyboardDown);
//    glutSpecialFunc(keyboardDownSpecial);
//    glutKeyboardUpFunc(keyboardUp);
//    glutSpecialUpFunc(keyboardUpSpecial);
//
//
//    glutMainLoop();
//
//    // ----------------------------------------------------------------------------- //
//}