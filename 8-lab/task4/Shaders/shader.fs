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
uniform float torusR[5];        // Большой радиус
uniform float torusr[5];        // Радиус трубки
uniform vec3 torusColors[5];    // Цвет каждого кольца
uniform mat4 modelMatrices[5];  // Матрицы трансформации
uniform mat4 invModelMatrices[5];


float map(vec3 p, out int hitObjIndex) {
    float minDist = 1e20; // Бесконечность
    hitObjIndex = -1;

    for (int i = 0; i < numObjects; i++) {
        // Переводим точку из мира в локальную систему координат тора
        vec3 localP = vec3(invModelMatrices[i] * vec4(p, 1.0));

        // Магия SDF для тора (2 строчки вместо solveQuartic!)
        vec2 q = vec2(length(localP.xz) - torusR[i], localP.y);
        float d = length(q) - torusr[i];

        // Запоминаем ближайший
        if (d < minDist) {
            minDist = d;
            hitObjIndex = i;
        }
    }
    return minDist;
}

vec3 calcNormal(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    int dummy; // Нам не нужен индекс объекта для нормали
    return normalize(vec3(
        map(p + e.xyy, dummy) - map(p - e.xyy, dummy),
        map(p + e.yxy, dummy) - map(p - e.yxy, dummy),
        map(p + e.yyx, dummy) - map(p - e.yyx, dummy)
    ));
}

float getSoftShadow(vec3 ro, vec3 rd) {
    float res = 1.0;
    float t = 0.02; // Сдвиг от поверхности, чтобы не затенить самих себя
    float maxDist = length(lightPos - ro); // Дистанция до источника света
    vec3 nRd = normalize(rd);

    for(int i = 0; i < 64; i++) {
        int dummy;
        float h = map(ro + nRd * t, dummy);
        if(h < 0.001) return 0.0; // Уперлись в объект — полная тень

        // Чем ближе луч прошел к объекту (h), тем темнее (res)
        res = min(res, 8.0 * h / t);
        t += h; // Шагаем дальше
        if(t > maxDist) break; // Дошли до света
    }
    return clamp(res, 0.0, 1.0);
}

void main() {
    // координаты пикселя (0..1) -> (-1..1)
    vec4 pixCoords = vec4(TexCoords * 2.0 - 1.0, -1.0, 1.0);

    // Инвертированная проекция и вид
    mat4 invProj = inverse(projection);
    mat4 invView = inverse(view);

    // invProj определяет тут степень расхождения лучей, как близко глаз к кваду
    vec4 viewSpacePos = invProj * pixCoords;
    vec3 rayDirView = normalize(viewSpacePos.xyz / viewSpacePos.w);

    // перемножив на invView готовый луч исходит в другую сторону(поворот головы), но не перемещается
    vec3 rayDir = normalize(vec3(invView * vec4(rayDirView, 0.0)));
    vec3 rayOrigin = viewPos;

    float t = 0.0;
    int hitObjIndex = -1;
    bool hit = false;

    for (int i = 0; i < 128; i++) { // Максимум 128 шагов
        vec3 p = rayOrigin + t * rayDir;

        int currentObjIndex;
        float d = map(p, currentObjIndex); // Узнаем дистанцию до мира

        if (d < 0.001) { // Если подошли вплотную - это ХИТ!
            hit = true;
            hitObjIndex = currentObjIndex;
            break;
        }

        t += d; // Шагаем вперед ровно на безопасную дистанцию

        if (t > 100.0) break; // Улетели слишком далеко
    }

    // Near <= 0.0 значит куб за спиной
    if (hit) {
        vec3 hitPos = rayOrigin + t * rayDir;
        vec3 norm = calcNormal(hitPos); // Получаем идеальную нормаль

        // Двустороннее освещение (если залетели внутрь тора)
        if (dot(rayDir, norm) > 0.0) {
            norm = -norm;
        }

        vec3 currentColor = torusColors[hitObjIndex];
        vec3 ambientResult = lightAmbient * (ambient * matAmbient) * currentColor;

        vec3 lightDir = normalize(lightPos - hitPos);

        // Получаем тень (теперь красивая и мягкая без тормозов)
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