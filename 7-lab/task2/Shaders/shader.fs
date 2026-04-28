#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

float sdCircle(vec2 p, float r) {
    return length(p) - r;
}

float sdOrientedBox(vec2 p, vec2 a, vec2 b, float th) {
    float l = length(b-a);
    vec2  d = (b-a)/l;
    vec2  q = p-(a+b)*0.5;
          q = mat2(d.x,-d.y,d.y,d.x)*q;
          q = abs(q)-vec2(l,th)*0.5;
    return length(max(q,0.0)) + min(max(q.x,q.y),0.0);
}

float sdStar(vec2 p, float r)
{
    const vec2 k1 = vec2(0.809016994375, -0.587785252292);
    const vec2 k2 = vec2(-k1.x, k1.y);
    p.x = abs(p.x);
    p -= 2.0*max(dot(k1,p),0.0)*k1;
    p -= 2.0*max(dot(k2,p),0.0)*k2;
    p.x = abs(p.x);

    // Здесь r — это внешний радиус
    // внутренний радиус
    p.y -= r;
    vec2 ba = 0.45 * r * vec2(-k1.y, k1.x) - vec2(0, r);
    float h = clamp( dot(p,ba)/dot(ba,ba), 0.0, 1.0 );
    return length(p-ba*h) * sign(p.y*ba.x-p.x*ba.y);
}

void main()
{
    vec3 red = vec3(0.85, 0.0, 0.0);
    vec3 gold = vec3(1.0, 0.85, 0.0);
    vec3 finalColor = red;

    vec2 symbolUV = (TexCoords - vec2(0.05, 0.60)) * 4.0; // Сдвиг и масштаб
    symbolUV.x = 1.0 - symbolUV.x;

    if (symbolUV.x < -0.2 || symbolUV.x > 1.2 || symbolUV.y < -0.2 || symbolUV.y > 1.2) {
        FragColor = vec4(red, 1.0);
        return;
    }

    vec2 starPos = symbolUV - vec2(0.5, 0.97);
    float starRadius = 0.15;

    float dStarFill = sdStar(starPos, starRadius);

    // 0.02 - это толщина линий контура
    float dStarOutline = abs(dStarFill) - 0.01;

    // Маска (1 - линия, 0 - пустота)
    float starShape = step(dStarOutline, 0.0);

    // Серп
    vec2 sickleCenter = vec2(0.45, 0.45);
    float sickleRadiusOuter = 0.35;
    float sickleRadiusInner = 0.30;

    float dOuter = sdCircle(symbolUV - sickleCenter, sickleRadiusOuter);
    float dInner = sdCircle(symbolUV - (sickleCenter + vec2(0.05, 0.04)), sickleRadiusInner);

    float sickleShape = step(dOuter, 0.0) * step(0.0, dInner);

    sickleShape *= step(symbolUV.x, sickleCenter.x + 0.1);

    // Ручка Серпа
    float dSickleHandle = sdOrientedBox(symbolUV, vec2(0.5, 0.2), vec2(0.68, 0.03), 0.1);
    float sickleHandle = step(dSickleHandle, 0.0);

    // Ручка молота
    float dHandle = sdOrientedBox(symbolUV, vec2(0.2, 0.1), vec2(0.8, 0.7), 0.06);
    float hammerHandle = step(dHandle, 0.0);

    // Головка молота
    vec2 headPos = symbolUV - vec2(0.73, 0.65);
    // Поворот
    float angle = 0.7;
    mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    headPos = rot * headPos;

    float hammerHead = step(abs(headPos.x), 0.08) * step(abs(headPos.y), 0.15);

    // Серп целиком
    float fullSickle = max(sickleShape, sickleHandle);
    // Молот целиком
    float fullHammer = max(hammerHandle, hammerHead);
    // Серп + Молот
    float solidEmblem = max(fullSickle, fullHammer);

    // Если пиксель попадает в серп ИЛИ молот, он должен быть золотым
    float emblem = max(solidEmblem, starShape);

    // Сглаживание краев (Antialiasing) — полезно, чтобы не было "лесенки"
    float border = 0.01;
    finalColor = mix(red, gold, smoothstep(-border, border, emblem - 0.5));

    FragColor = vec4(finalColor, 1.0);
}