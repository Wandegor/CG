#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform vec3 lightAmbient; // цвет фона
uniform float ambient;
uniform float matAmbient;

uniform float shininess;
uniform vec3 specularColor;

vec2 intersectAABB(vec3 rayOrigin, vec3 rayDir, vec3 boxMin, vec3 boxMax) {
    // point = origin + t * dir =>
    // t = point - origin / dir
    // tMin и tMax будут хранить расстояния до всех плоскостей куба (xyz)
    vec3 tMin = (boxMin - rayOrigin) / rayDir;
    vec3 tMax = (boxMax - rayOrigin) / rayDir;
    // Для защиты при разворота луча,
    // пример: при tMin = 5 и tMax = 2 чтобы они поменялись местами
    // то есть t1 всегда ближняя стена, а t2 дальняя
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    // чтобы влететь в куб нужно пересечь самую дальнюю из его плоскостей
    float tNear = max(max(t1.x, t1.y), t1.z);
    // чтобы вылететь ближнюю
    float tFar = min(min(t2.x, t2.y), t2.z);
    return vec2(tNear, tFar);
}

bool isShadowed(vec3 point, vec3 lightDir, vec3 norm, vec3 boxMin, vec3 boxMax) {
    // Сдвиг вдоль нормали(чтобы не врезаться в тот же объект)
    vec3 shadowRayOrigin = point + norm * 0.001;

    vec2 tHit = intersectAABB(shadowRayOrigin, lightDir, boxMin, boxMax);

    // Если есть пересечение и оно перед светом (свет обычно далеко)
    return (tHit.x < tHit.y && tHit.x > 0.0);
}
void main() {
    // координаты пикселя (0..1) -> (-1..1)
    vec4 pixCoords = vec4(TexCoords * 2.0 - 1.0, -1.0, 1.0);

    // Инвертированная проекция и вид
    mat4 invProj = inverse(projection);
    mat4 invView = inverse(view);

    // луч от глаза до пикселя на кваде
    // invProj определяет тут степень расхождения лучей, как близко глаз к кваду
    vec4 eyeCoords = invProj * pixCoords;
    // Луч именно вперед z = -1 и он только поворачивается а не двигается w = 0
    eyeCoords = vec4(eyeCoords.xy, -1.0, 0.0);

    // перемножив на invView готовый луч исходит в другую сторону(поворот головы), но не перемещается
    vec3 rayDir = normalize(vec3(invView * eyeCoords));
    // позиция выпуска луча
    vec3 rayOrigin = viewPos;

    // Куб (Точки на диагонали)
    vec3 boxMin = vec3(-0.5, -0.5, -0.5);
    vec3 boxMax = vec3( 0.5,  0.5,  0.5);

    vec2 tHit = intersectAABB(rayOrigin, rayDir, boxMin, boxMax);

    // x - Near, y - Far, луч вошел в куб если Near < Far
    // tHit.y <= 0.0 значит куб за спиной
    if (tHit.x < tHit.y && tHit.y > 0.0) {

        vec3 hitPos = rayOrigin + tHit.x * rayDir;

        // Вычисляем нормаль грани куба
        // (-0.5; 0.5) -> (-1; 1)
        vec3 pc = hitPos * 2.0;
        vec3 norm = vec3(0.0);
        // там где координата больше по модулю (~1), на той стенке точка
        // sing вернет -1 или 1 от знака
        if (abs(pc.x) > abs(pc.y) && abs(pc.x) > abs(pc.z))
        norm = vec3(sign(pc.x), 0, 0);
        else if (abs(pc.y) > abs(pc.z))
        norm = vec3(0, sign(pc.y), 0);
        else
        norm = vec3(0, 0, sign(pc.z));


        // Ambient
        vec3 ambientResult = lightAmbient * (ambient * matAmbient) * objectColor;

        vec3 lightDir = normalize(lightPos - hitPos);

        vec3 diffuseResult = vec3(0.0);
        vec3 specularResult = vec3(0.0);
        // Тень
        bool shadow = isShadowed(hitPos, lightDir, norm, boxMin, boxMax);
        if (!shadow)
        {
            // Диффузное освещение
            float diff = max(dot(norm, lightDir), 0.0);
            diffuseResult = diff * lightColor * objectColor;

            // Specular
            // направление в камеру от объекта
            vec3 viewDir = normalize(rayOrigin - hitPos);
            // Blinn-Phong, вектор между lightDir и viewDir
            vec3 halfwayDir = normalize(lightDir + viewDir);
            // значение spec тем ближе к 1 чем меньше угол между norm и halfwayDir
            float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);

            specularResult = spec * specularColor;
        }

        vec3 result = ambientResult + diffuseResult + specularResult;
        FragColor = vec4(result, 1.0);
    } else {
        // Фон
        FragColor = vec4(0.2, 0.2, 0.25, 1.0);
    }
}