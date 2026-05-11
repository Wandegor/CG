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

vec2 intersectAABB(vec3 rayOrigin, vec3 rayDir, vec3 boxMin, vec3 boxMax)
{
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

float random(vec3 seed) {
    return fract(sin(dot(seed, vec3(12.9898, 78.233, 45.164))) * 43758.5453123);
}

float getShadowFactor(vec3 point, vec3 lightDir, vec3 norm, vec3 b1Min, vec3 b1Max, vec3 b2Min, vec3 b2Max)
{
    float lightRadius = 0.04;
    int samples = 50;
    int hits = 0;

    for (int i = 0; i < samples; i++) {
        // небольшое случайное смещение для каждой итерации
        vec3 offset = vec3(
            random(point + float(i) * 0.1),
            random(point + float(i) * 0.2),
            random(point + float(i) * 0.3)
        ) * 2.0 - 1.0;

        // Направление луча со смещением
        vec3 sampleLightPos = lightPos + offset * lightRadius;
        vec3 lightDir = normalize(sampleLightPos - point);

        // Сдвиг вдоль нормали(чтобы не врезаться в тот же объект)
        vec3 shadowRayOrigin = point + norm * 0.001;
        vec2 t1 = intersectAABB(shadowRayOrigin, lightDir, b1Min, b1Max);
        vec2 t2 = intersectAABB(shadowRayOrigin, lightDir, b2Min, b2Max);

        bool shadowed = (t1.x < t1.y && t1.x > 0.0) ||
                        (t2.x < t2.y && t2.x > 0.0);

        if (!shadowed) {
            hits++;
        }
    }
    // соотношение дошедших ко всем
    return float(hits) / float(samples);
}

vec3 getCubeNormal(vec3 p, vec3 bMin, vec3 bMax) {
    vec3 center = (bMin + bMax) * 0.5;
    vec3 size = (bMax - bMin) * 0.5;
    vec3 pc = (p - center) / size;
    vec3 v = abs(pc);
    if (v.x > v.y && v.x > v.z) return vec3(sign(pc.x), 0, 0);
    if (v.y > v.z) return vec3(0, sign(pc.y), 0);
    return vec3(0, 0, sign(pc.z));
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

    // Кубы
    vec3 b1Min = vec3(-0.5, -0.5, -0.5);
    vec3 b1Max = vec3(0.5, 0.5, 0.5);
    vec3 b2Min = vec3(0.5, 0.6, 1.0);
    vec3 b2Max = vec3(1.0, 1.1, 1.5);

    vec2 tHit1 = intersectAABB(rayOrigin, rayDir, b1Min, b1Max);
    vec2 tHit2 = intersectAABB(rayOrigin, rayDir, b2Min, b2Max);

    float tResult = -1.0;
    vec3 currentMin, currentMax;

    // x - Near, y - Far, луч вошел в куб если Near < Far
    if (tHit1.x < tHit1.y && tHit1.x > 0.0) {
        tResult = tHit1.x;
        currentMin = b1Min; currentMax = b1Max;
    }

    if (tHit2.x < tHit2.y && tHit2.x > 0.0) {
        if (tResult < 0.0 || tHit2.x < tResult) {
            tResult = tHit2.x;
            currentMin = b2Min; currentMax = b2Max;
        }
    }

    // Near <= 0.0 значит куб за спиной
    if (tResult > 0.0) {

        vec3 hitPos = rayOrigin + tResult * rayDir;

        vec3 norm = getCubeNormal(hitPos, currentMin, currentMax);

        // Ambient
        vec3 ambientResult = lightAmbient * (ambient * matAmbient) * objectColor;

        vec3 lightDir = normalize(lightPos - hitPos);

        vec3 diffuseResult = vec3(0.0);
        vec3 specularResult = vec3(0.0);
        // Тень
        float shadowFactor = getShadowFactor(hitPos, lightDir, norm, b1Min, b1Max, b2Min, b2Max);

        // Диффузное освещение
        float diff = max(dot(norm, lightDir), 0.0);
        diffuseResult = diff * lightColor * objectColor * shadowFactor;

        // Specular
        // направление в камеру от объекта
        vec3 viewDir = normalize(rayOrigin - hitPos);
        // Blinn-Phong, вектор между lightDir и viewDir
        vec3 halfwayDir = normalize(lightDir + viewDir);
        // значение spec тем ближе к 1 чем меньше угол между norm и halfwayDir
        float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);

        specularResult = spec * specularColor * shadowFactor;

        vec3 result = ambientResult + diffuseResult + specularResult;
        FragColor = vec4(result, 1.0);
    } else {
        // Фон
        FragColor = vec4(0.2, 0.2, 0.25, 1.0);
    }
}