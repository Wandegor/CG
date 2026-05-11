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

struct Hit {
    float t;
    vec3 norm;
    bool hit;
};

int solveQuadratic(float a, float b, float c, float offset, inout float roots[4], inout int count) {
    float disc = b * b - 4.0 * a * c;
    if (disc < 0.0) return 0;

    float sqrtDisc = sqrt(disc);
    float inv2a = 0.5 / a;

    roots[count++] = (-b - sqrtDisc) * inv2a + offset;
    roots[count++] = (-b + sqrtDisc) * inv2a + offset;
    return 2;
}

float solveCubic(float a, float b, float c) {
    float p = b - a * a / 3.0;
    float q = a * (2.0 * a * a - 9.0 * b) / 27.0 + c;
    float p3 = p * p * p;
    float d = q * q + 4.0 * p3 / 27.0;
    if (d >= 0.0) {
        float z = sqrt(d);
        float u = (-q + z) / 2.0;
        float v = (-q - z) / 2.0;
        return (sign(u) * pow(abs(u), 1.0/3.0) + sign(v) * pow(abs(v), 1.0/3.0)) - a / 3.0;
    }
    // Если d < 0, есть 3 вещественных корня, используем тригонометрию
    float r = sqrt(-p * p * p / 27.0);
    float phi = acos(-q / (2.0 * r));
    return 2.0 * pow(r, 1.0/3.0) * cos(phi / 3.0) - a / 3.0;
}

int solveQuartic(float a, float b, float c, float d, float e, out float roots[4]) {
    // 1. Нормализация (приводим к виду t^4 + At^3 + Bt^2 + Ct + D = 0)
    float invA = 1.0 / a;
    float A = b * invA;
    float B = c * invA;
    float C = d * invA;
    float D = e * invA;

    // 2. Переход к неполному уравнению x^4 + px^2 + qx + r = 0
    float ASqr = A * A;
    float p = B - 0.375 * ASqr;
    float q = C + 0.125 * ASqr * A - 0.5 * A * B;
    float r = D - 0.01171875 * ASqr * ASqr + 0.0625 * ASqr * B - 0.25 * A * C;

    float offset = -0.25 * A; // Тот самый сдвиг t = x - A/4
    int count = 0;

    // Спецслучай: если q очень мало, решаем как биквадратное
    if (abs(q) < 1e-7) {
        float disc = p * p - 4.0 * r;
        if (disc >= 0.0) {
            float s = sqrt(disc);
            float x1 = (-p - s) * 0.5;
            float x2 = (-p + s) * 0.5;
            if (x1 >= 0.0) {
                float sx1 = sqrt(x1);
                roots[count++] = -sx1 + offset;
                roots[count++] = sx1 + offset;
            }
            if (x2 >= 0.0) {
                float sx2 = sqrt(x2);
                roots[count++] = -sx2 + offset;
                roots[count++] = sx2 + offset;
            }
        }
    } else {
        // 3. Резольвентное кубическое уравнение: y^3 - p*y^2 - 4*r*y + (4*p*r - q^2) = 0
        float y = solveCubic(-p, -4.0 * r, 4.0 * p * r - q * q);

        // 4. Формируем два квадратных уравнения
        float m = 2.0 * y - p;
        if (m < 0.0) return 0; // Корни только комплексные

        float sqrtM = sqrt(m);

        // Решаем первое: x^2 + sqrtM*x + (y - q/(2*sqrtM)) = 0
        solveQuadratic(1.0, sqrtM, y - q / (2.0 * sqrtM), offset, roots, count);

        // Решаем второе: x^2 - sqrtM*x + (y + q/(2*sqrtM)) = 0
        solveQuadratic(1.0, -sqrtM, y + q / (2.0 * sqrtM), offset, roots, count);
    }

    return count;
}

Hit intersectTorus(vec3 ro, vec3 rd, float R, float r) {
    Hit hit;
    hit.hit = false;
    hit.t = 1e20;

    // a, b, c, d, e
    float G = dot(rd, rd);
    float H = 2.0 * dot(ro, rd);
    float K = dot(ro, ro) + R*R - r*r;

    float a = G * G;
    float b = 2.0 * G * H;
    float c = 2.0 * G * K + H * H - 4.0 * R * R * (rd.x * rd.x + rd.y * rd.y);
    float d = 2.0 * H * K - 8.0 * R * R * (ro.x * rd.x + ro.y * rd.y);
    float e = K * K - 4.0 * R * R * (ro.x * ro.x + ro.y * ro.y);

    float roots[4];
    int numRoots = solveQuartic(a, b, c, d, e, roots);

    // 3. Выбираем лучший корень
    for(int i = 0; i < numRoots; i++) {
        if(roots[i] > 0.001 && roots[i] < hit.t) {
            hit.t = roots[i];
            hit.hit = true;

            // Вычисляем нормаль (градиент функции тора в точке p)
            vec3 p = ro + hit.t * rd;
            float param = p.x*p.x + p.y*p.y + p.z*p.z + R*R - r*r;
            hit.norm = normalize(vec3(
                4.0 * p.x * param - 8.0 * R * R * p.x,
                4.0 * p.y * param - 8.0 * R * R * p.y,
                4.0 * p.z * param
            ));
        }
    }
    return hit;
}

float random(vec3 seed) {
    return fract(sin(dot(seed, vec3(12.9898, 78.233, 45.164))) * 43758.5453123);
}

float getShadowFactor(vec3 point, vec3 norm)
{
    float lightRadius = 0.1;
    int samples = 16;
    int hits = 0;

    for (int i = 0; i < samples; i++)
    {
        // небольшое случайное смещение для каждой итерации
        vec3 offset = vec3(
            random(point + float(i) * 0.1),
            random(point + float(i) * 0.2),
            random(point + float(i) * 0.3)
        ) * 2.0 - 1.0;

        vec3 sampleLightPos = lightPos + offset * lightRadius;
        // Направление луча со смещением
        vec3 lightDir = normalize(sampleLightPos - point);
        float distToLight = distance(sampleLightPos, point);

        // Сдвиг вдоль нормали(чтобы не врезаться в тот же объект)
        vec3 shadowRayOrigin = point + norm * 0.001;

        bool inShadow = false;
        for (int j = 0; j < numObjects; j++)
        {
            mat4 invModel = inverse(modelMatrices[j]);
            vec3 localRo = vec3(invModel * vec4(shadowRayOrigin, 1.0));
            vec3 localRd = vec3(invModel * vec4(lightDir, 0.0));

            Hit hTorus = intersectTorus(localRo, localRd, torusR[j], torusr[j]);
            if (hTorus.hit && hTorus.t < distToLight) {
                inShadow = true;
                break;
            }
        }

        if (!inShadow) hits++;
    }
    // соотношение дошедших ко всем
    return float(hits) / float(samples);
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

    // Параболоиды
    Hit sceneHit;
    sceneHit.hit = false;
    sceneHit.t = 1e20;
    int hitObjIndex = -1;

    for (int i = 0; i < numObjects; i++) {
        // Инвертируем матрицу модели, чтобы перенести луч из мира в локальные координаты тора
        mat4 invModel = inverse(modelMatrices[i]);

        // Точка начала луча (w=1, так как это позиция)
        vec3 localRo = vec3(invModel * vec4(rayOrigin, 1.0));
        // Направление луча (w=0, так как это вектор)
        vec3 localRd = vec3(invModel * vec4(rayDir, 0.0));

        Hit h = intersectTorus(localRo, localRd, torusR[i], torusr[i]);

        if (h.hit && h.t < sceneHit.t) {
            sceneHit = h;
            hitObjIndex = i;

            // нормаль тоже нужно вернуть в мировые координаты
            // Используем обратную транспонированную матрицу для корректного преобразования нормалей
            mat3 normalMatrix = transpose(inverse(mat3(modelMatrices[i])));
            sceneHit.norm = normalize(normalMatrix * h.norm);
        }
    }

    // Near <= 0.0 значит куб за спиной
    if (sceneHit.hit) {

        vec3 hitPos = rayOrigin + sceneHit.t * rayDir;

        vec3 norm = sceneHit.norm;
        if (dot(rayDir, norm) > 0.0) {
            norm = -norm;
        }

        // Ambient
        vec3 currentColor = torusColors[hitObjIndex];
        vec3 ambientResult = lightAmbient * (ambient * matAmbient) * currentColor;

        vec3 lightDir = normalize(lightPos - hitPos);

        vec3 diffuseResult = vec3(0.0);
        vec3 specularResult = vec3(0.0);
        // Тень
        float shadowFactor = getShadowFactor(hitPos, norm);

        // Диффузное освещение
        float diff = max(dot(norm, lightDir), 0.0);
        diffuseResult = diff * lightColor * currentColor * shadowFactor;

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