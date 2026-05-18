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

uniform int numObjects;
uniform float torusR[5];
uniform float torusr[5];
uniform vec3 torusColors[5];
uniform mat4 invModelMatrices[5];

vec2 intersectAABB(vec3 rayOrigin, vec3 rayDir, vec3 boxMin, vec3 boxMax) {
    vec3 tMin = (boxMin - rayOrigin) / rayDir;
    vec3 tMax = (boxMax - rayOrigin) / rayDir;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
    return vec2(tNear, tFar); // x - точка входа, y - точка выхода
}

float smoothMin(float dstA, float dstB, vec3 colorA, vec3 colorB,
                float k, out vec3 mixedColor) {
    // при h > 0 объекты сливаются, чем они ближе тем h ближе к 1.0
    float h = max(k - abs(dstA - dstB), 0.0) / k;

    // Коэф сжатый в [0, 1]
    float mixFactor = clamp(0.5 + 0.5 * (dstA - dstB) / k, 0.0, 1.0);
    mixedColor = mix(colorA, colorB, mixFactor);

    return min(dstA, dstB) - h * h * h * k * 1.0 / 6.0;
}

float map(vec3 p, out vec3 resColor) {

    float distances[5];

    for (int i = 0; i < numObjects; i++) {
        // Перенос точки луча из мира в локальную систему координат тора
        // тор будет в (0, 0, 0)
        // а луч в координатах тора
        vec3 localP = vec3(invModelMatrices[i] * vec4(p, 1.0));

        // SDF
        vec2 q = vec2(length(localP.xz) - torusR[i], localP.y);
        float d = length(q) - torusr[i];

        distances[i] = d;
    }

    float resDist = 1e20;
    resColor = torusColors[0];
    // k — это радиус плавления. Чем больше k, тем сильнее капли сливаются
    float k = 0.9;
    for (int i = 0; i < numObjects; i++)
    {
        vec3 nextColor;
        resDist = smoothMin(resDist, distances[i], resColor, torusColors[i], k, nextColor);
        resColor = nextColor;
    }

    return resDist;
}

vec3 calcNormal(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    vec3 dummy; // заглушка
    // поиск расстояний после микро смещения по +- X Y Z - вектор нормали
    return normalize(vec3(
        map(p + e.xyy, dummy) - map(p - e.xyy, dummy),
        map(p + e.yxy, dummy) - map(p - e.yxy, dummy),
        map(p + e.yyx, dummy) - map(p - e.yyx, dummy)
    ));
}

float getSoftShadow(vec3 ro, vec3 rd) {
    float res = 1.0;
    float t = 0.02; // Сдвиг от поверхности, чтобы не упереться в самих себя
    float maxDist = length(lightPos - ro); // Дистанция до источника света
    vec3 nRd = normalize(rd);

    for(int i = 0; i < 64; i++) {
        vec3 dummy;
        float h = map(ro + nRd * t, dummy);
        if(h < 0.001) return 0.0; // Уперлись в объект — полная тень

        // Чем ближе луч прошел к объекту (h), тем темнее (res) 1.5 - мягкость тени
        res = min(res, 1.5 * h / t);
        t += h; // Шаг
        if(t > maxDist) break; // Дошли до света
    }
    return clamp(res, 0.0, 1.0); // вернуть res к диапозону
}

void main() {
    // координаты пикселя (0..1) -> (-1..1)
    vec4 pixCoords = vec4(TexCoords * 2.0 - 1.0, -1.0, 1.0);

    // Инвертированная проекция и вид
    mat4 invProj = inverse(projection);
    mat4 invView = inverse(view);

    // invProj определяет тут степень расхождения лучей, как близко глаз к кваду
    vec4 eyeCoords = invProj * pixCoords;
    // Луч именно вперед z = -1 и он только поворачивается а не двигается w = 0
    eyeCoords = vec4(eyeCoords.xy, -1.0, 0.0);

    // перемножив на invView готовый луч исходит в другую сторону(поворот головы), но не перемещается
    vec3 rayDir = normalize(vec3(invView * eyeCoords));
    vec3 rayOrigin = viewPos;

    float t = 0.0;
    vec3 resSurfaceColor;
    bool hit = false;

    // Границы ограничивающей области
    vec3 boxMin = vec3(-0.5, -0.5, -0.5);
    vec3 boxMax = vec3(2.5, 2.5, 2.5);

    vec2 boxHit = intersectAABB(rayOrigin, rayDir, boxMin, boxMax);

    // x - Near, y - Far
    // Far > 0.0 не за спиной
    if (boxHit.x < boxHit.y && boxHit.y > 0.0)
    {
        // Если камера внутри (boxHit.x < 0) - t = 0.0.
        // Иначе начало со стенки
        t = max(0.0, boxHit.x);

        float tMax = boxHit.y;

        // RayMarching
        for (int i = 0; i < 128; i++) { // Максимум шагов луча
            vec3 p = rayOrigin + t * rayDir;

            // Возвращает дистанцию до ближайшего тора
            float d = map(p, resSurfaceColor);

            // hit
            if (d < 0.001) {
                hit = true;
                break;
            }

            t += d; // Шаг на безопасную дистанцию

            if (t > tMax) break; // Улетели слишком далеко
        }
    }

    if (hit) {
        vec3 hitPos = rayOrigin + t * rayDir;
        vec3 norm = calcNormal(hitPos);

        vec3 currentColor = resSurfaceColor;
        vec3 ambientResult = lightAmbient * (ambient * matAmbient) * currentColor;

        vec3 lightDir = normalize(lightPos - hitPos);

        float shadowFactor = getSoftShadow(hitPos, lightDir);

        // Диффуз
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuseResult = diff * lightColor * currentColor * shadowFactor;

        // Блик (Specular)
        vec3 viewDir = normalize(rayOrigin - hitPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
        vec3 specularResult = spec * specularColor * shadowFactor;

        vec3 result = ambientResult + diffuseResult + specularResult;
        FragColor = vec4(result, 1.0);
    } else {
        // Фон
        FragColor = vec4(0.2, 0.2, 0.25, 1.0);
    }
}