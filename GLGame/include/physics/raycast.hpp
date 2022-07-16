#ifndef GLGAME_RAYCAST_HPP
#define GLGAME_RAYCAST_HPP

class Raycast
{
public:
    Raycast() = default;

    Raycast(glm::vec2 mouseCoords, glm::vec2 screenDimensions, const glm::mat4& projection, const glm::mat4& view) {
        float x = (2.0f * mouseCoords.x) / screenDimensions.x - 1.0f;
        float y = 1.0f - (2.0f * mouseCoords.y) / screenDimensions.y;
        float z = 1.0f;
        glm::vec3 ray_nds = glm::vec3(x, y, z);

        glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

        glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

        auto temp = (glm::inverse(view) * ray_eye);
        ray = glm::vec3(temp.x, temp.y, temp.z);
        ray = glm::normalize(ray);
    }

    glm::vec3 checkPlaneIntersection(glm::vec3 rayOrigin, glm::vec3 planeNormal, float distanceFromOrigin){
        float t = -1 * (glm::dot(rayOrigin, planeNormal) + distanceFromOrigin) / glm::dot(ray, planeNormal);
        return rayOrigin + t * ray;
    }

    glm::vec3 getRay(){
        return ray;
    }

private:
    glm::vec3 ray = glm::vec3(0, 0, 1);
};
#endif
