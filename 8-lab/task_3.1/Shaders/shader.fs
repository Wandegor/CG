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
uniform float heights[5]; // Позиции Z
uniform float rads[5]; // Радиусы крышек
uniform float sizesH[5]; // Высоты

struct Hit {
    float t;
    vec3 norm;
    bool hit;
};

Hit intersectParaboloid(vec3 ro, vec3 rd, float R, float H)
{
    Hit hit;
    hit.hit = false;
    hit.t = 1e20;

    float k = H / (R * R); // коэф крутизны

    // z = x^2 + y^2
    // P = Origin + t*D =>
    // (Oz + t*Dz) = (Ox + t*Dx)^2 + (Oy + t*Dy)^2 =>
    // At^2 + Bt + C = 0:
    float A = k * (rd.x * rd.x + rd.y * rd.y);
    float B = 2.0 * k * (ro.x * rd.x + ro.y * rd.y) - rd.z;
    float C = k * (ro.x * ro.x + ro.y * ro.y) - ro.z;

    float det = B * B - 4.0 * A * C;

    if (det >= 0.0)
    {
        float sqrtDet = sqrt(det);
        float t1 = (-B - sqrtDet) / (2.0 * A);
        float t2 = (-B + sqrtDet) / (2.0 * A);

        float ts[2] = float[](t1, t2);
        // Проверка корней
        for (int i = 0; i < 2; i++) {
            float t = ts[i];
            if (t > 0.001 && t < hit.t) {
                vec3 p = ro + t * rd;
                // Ограничение по H
                if (p.z >= 0.0 && p.z <= H) {
                    hit.t = t;
                    // Нормаль: градиент f(x,y,z) = x^2 + y^2 - z -> (2x, 2y, -1) с учетом k
                    hit.norm = normalize(vec3(2.0 * k * p.x, 2.0 * k * p.y, -1.0));
                    hit.hit = true;
                }
            }
        }
    }

    // Крышка (z = 1)
    // 0*x + 0*y + 1*z = 1
    // при rd.z = 0 луч параллелен, пересечений нет
    if (abs(rd.z) > 0.00001) {
       // 1 = Oz + t*Dz =>
       float tCap = (H - ro.z) / rd.z;
       // если tCap<0 - крышка сзади,
       // tCap < hit.t - крышка ближе стенки
       if (tCap > 0.001 && tCap < hit.t) {
           vec3 p = ro + tCap * rd;
           // Условие x^2 + y^2 <= R^2
           if (p.x * p.x + p.y * p.y <= R*R + 0.001) {
               hit.t = tCap;
               hit.norm = vec3(0.0, 0.0, 1.0);
               hit.hit = true;
           }
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
    int samples = 50;
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
            vec3 localRo = shadowRayOrigin - vec3(0.0, 0.0, heights[j]);
            Hit hPara = intersectParaboloid(localRo, lightDir, rads[j], sizesH[j]);
            if (hPara.hit && hPara.t < distToLight) {
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

    for (int i = 0; i < numObjects; i++) {
        // Сдвиг луча вниз, что сдвигает объект ВВЕРХ
        vec3 localRo = rayOrigin - vec3(0.0, 0.0, heights[i]);

        Hit h = intersectParaboloid(localRo, rayDir, rads[i], sizesH[i]);

        // Попадаение и объект ближе, чем предыдущие найденные
        if (h.hit && h.t < sceneHit.t) {
            sceneHit = h;
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
        vec3 ambientResult = lightAmbient * (ambient * matAmbient) * objectColor;

        vec3 lightDir = normalize(lightPos - hitPos);

        vec3 diffuseResult = vec3(0.0);
        vec3 specularResult = vec3(0.0);
        // Тень
        float shadowFactor = getShadowFactor(hitPos, norm);

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