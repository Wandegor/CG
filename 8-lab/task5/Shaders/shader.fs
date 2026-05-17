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

float smoothMin(float dstA, float dstB, float k) {
    float h = max(k - abs(dstA - dstB), 0.0) / k;
    return min(dstA, dstB) - h * h * h * k * 1.0 / 6.0;
}

float map(vec3 p, out int hitObjIndex) {
    float minDist = 1e20;
    hitObjIndex = -1;

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
        // Запоминаем ближайший
        if (d < minDist) {
            minDist = d;
            hitObjIndex = i;
        }
    }

    float resDist = 1e20;

    // k = 0.3 — это радиус плавления. Чем больше k, тем сильнее капли сливаются
    float k = 0.9;
    for (int i = 0; i < numObjects; i++)
    {
        resDist = smoothMin(resDist, distances[i], k);
    }

    return resDist;
}

vec3 calcNormal(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    int dummy; // заглушка
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
        int dummy;
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
    int hitObjIndex = -1;
    bool hit = false;

    // RayMarching
    for (int i = 0; i < 128; i++) { // Максимум шагов луча
        vec3 p = rayOrigin + t * rayDir;

        int currentObjIndex;
        // Возвращает дистанцию до ближайшего тора
        float d = map(p, currentObjIndex);

        // hit
        if (d < 0.001) {
            hit = true;
            hitObjIndex = currentObjIndex;
            break;
        }

        t += d; // Шаг на безопасную дистанцию

        if (t > 100.0) break; // Улетели слишком далеко
    }

    if (hit) {
        vec3 hitPos = rayOrigin + t * rayDir;
        vec3 norm = calcNormal(hitPos);

        vec3 currentColor = torusColors[hitObjIndex];
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