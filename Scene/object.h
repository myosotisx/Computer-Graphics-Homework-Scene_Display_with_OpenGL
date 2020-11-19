#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Object {
public:
    glm::vec3 Position;
    glm::vec3 Offset;
    glm::vec3 Scale;
    glm::vec3 Min, Max;
    bool Collision;
    int verticesNum, bufLength;
    float* vertices;
    Object(int _verticesNum, int _bufLength, float _vertices[], glm::vec3 position=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 offset=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale=glm::vec3(1.0f, 1.0f, 1.0f), bool collision=false)
        : verticesNum(_verticesNum), bufLength(_bufLength), vertices(_vertices), Min(0.0f, 0.0f, 0.0f), Max(0.0f, 0.0f, 0.0f), Collision(collision) {
        Position = position;
        Offset = offset;
        Scale = scale;
        if(collision) {
            calcAABB(verticesNum, bufLength, vertices, Min, Max, model((float)glfwGetTime()));
        }
    }
    glm::mat4 model(float time) {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, Position);
        trans = glm::scale(trans, Scale);
        trans = glm::translate(trans, Offset); // 将枢轴变换到Object的中心
        return trans;
    }
    bool collide(const Object& object) {
        bool xCollide = (object.Min.x >= Min.x && object.Min.x <= Max.x) || (Min.x >= object.Min.x && Min.x <= object.Max.x);
        bool yCollide = (object.Min.y >= Min.y && object.Min.y <= Max.y) || (Min.y >= object.Min.y && Min.y <= object.Max.y);
        bool zCollide = (object.Min.z >= Min.z && object.Min.z <= Max.z) || (Min.z >= object.Min.z && Min.z <= object.Max.z);
        return xCollide && yCollide && zCollide;
    }
    static void calcAABB(int verticesNum, int bufLength, float vertices[], glm::vec3& min, glm::vec3& max, const glm::mat4& trans, float bound=1000.0f) {
        min = glm::vec3(bound, bound, bound);
        max = glm::vec3(-bound, -bound, -bound);
        int n = verticesNum*bufLength;
        for(int i=0; i<n; i+=bufLength) {
            glm::vec3 v = trans*glm::vec4(vertices[i], vertices[i+1], vertices[i+2], 1.0f);
            
            if(v.x < min.x) min.x = v.x;
            if(v.y < min.y) min.y = v.y;
            if(v.z < min.z) min.z = v.z;

            if(v.x > max.x) max.x = v.x;
            if(v.y > max.y) max.y = v.y;
            if(v.z > max.z) max.z = v.z;
        }
    }
};


class Cube: public Object {
public:
    float radius;
    float initTime;
    Cube(float _radius, float _initTime, int verticesNum, int bufLength, float vertices[], glm::vec3 position=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 offset=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale=glm::vec3(1.0f, 1.0f, 1.0f))
        : Object(verticesNum, bufLength, vertices, position, offset, scale, false), radius(_radius), initTime(_initTime) {
        calcAABB(verticesNum, bufLength, vertices, Min, Max, model(initTime));
    }
    glm::mat4 model(float time) {
        updatePosition(time);
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, Position);
        trans = glm::rotate(trans, time-initTime, glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::scale(trans, Scale);
        trans = glm::translate(trans, Offset);
        calcAABB(verticesNum, bufLength, vertices, Min, Max, trans);
        return trans;
    }
    void updatePosition(float time) {
        Position.x = radius*cos(time-initTime);
        Position.y = radius*sin(time-initTime);
    }
};


class Pyramid: public Object {
public:
    Pyramid(int verticesNum, int bufLength, float vertices[], glm::vec3 position=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 offset=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale=glm::vec3(1.0f, 1.0f, 1.0f))
        : Object(verticesNum, bufLength, vertices, position, offset, scale, true) {}
    glm::mat4 model(float) {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, Position);
        trans = glm::scale(trans, Scale);
        trans = glm::translate(trans, Offset);
        return trans;
    }
};


class Diamond: public Object {
public:
    Diamond(int verticesNum, int bufLength, float vertices[], glm::vec3 position=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 offset=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale=glm::vec3(1.0f, 1.0f, 1.0f))
        : Object(verticesNum, bufLength, vertices, position, offset, scale, true) {}
    glm::mat4 model(float) {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, Position);
        trans = glm::scale(trans, Scale);
        trans = glm::translate(trans, Offset);
        return trans;
    }
};


enum Plane_Face {
    FRONT_FACE,
    BACK_FACE,
    LEFT_FACE,
    RIGHT_FACE,
    TOP_FACE,
    DOWN_FACE
};

class Plane: public Object {
public:
    Plane_Face face;
    Plane(Plane_Face _face, int verticesNum, int bufLength, float vertices[], glm::vec3 position=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 offset=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale=glm::vec3(1.0f, 1.0f, 1.0f))
        : Object(verticesNum, bufLength, vertices, position, offset, scale, true), face(_face) {}
    glm::mat4 model(float) {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, Position);
        if(face == FRONT_FACE) {
            trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else if(face == LEFT_FACE) {
            trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else if(face == RIGHT_FACE) {
            trans = glm::rotate(trans, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else if(face == TOP_FACE) {
            trans = glm::rotate(trans, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        else if(face == DOWN_FACE) {
            trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        trans = glm::scale(trans, Scale);
        trans = glm::translate(trans, Offset);
        return trans;
    }
};
#endif