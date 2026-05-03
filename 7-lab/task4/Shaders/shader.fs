#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // UV координаты прямоугольника (0.0 - 1.0)

uniform sampler2D texture1; // Исходная картинка
uniform sampler2D texture2; // Конечная картинка

uniform float uProgress;    // Время анимации от 0.0 до 1.0
uniform float uAspect;      // Отношение сторон экрана (Width / Height)

void main() {
    // 1. Настройки эффекта (можно покрутить для красоты)
    vec2 center = vec2(0.5, 0.5); // Откуда расходится волна
    float waveFreq = 40.0;        // Частота волн (сколько колец)
    float waveAmp = 0.03;         // Сила искажения
    float waveWidth = 0.2;        // Толщина "кольца" волн на фронте

    // Максимальный радиус должен покрывать углы (диагональ квадрата 1х1 это ~1.41)
    float maxRadius = 1.5;
    float currentRadius = uProgress * maxRadius;

    // 2. Коррекция соотношения сторон (чтобы круги не были овалами)
    vec2 uv = TexCoords;
    uv.x *= uAspect;
    vec2 c = center;
    c.x *= uAspect;

    // 3. Вычисление расстояния до фронта волны
    float dist = distance(uv, c);
    // diff > 0: пиксель впереди волны. diff < 0: волна уже прошла пиксель
    float diff = dist - currentRadius;

    // 4. Гауссова функция для ограничения волны (создаем кольцо ряби)
    // Она равна 1.0 прямо на фронте и плавно падает до 0.0 по краям
    float envelope = exp(-pow(diff / (waveWidth * 0.5), 2.0));

    // 5. Искажение UV координат
    vec2 dir = normalize(TexCoords - center); // Направление сдвига
    float wavePhase = diff * waveFreq;        // Фаза синусоиды

    // Смещение. Умножаем на envelope, чтобы искажался только фронт
    float distortion = sin(wavePhase) * waveAmp * envelope;
    vec2 displacedUV = TexCoords + dir * distortion;

    // 6. Плавное смешивание двух картинок
    // smoothstep создаст градиент перехода:
    // Впереди волны (diff > 0) -> 0.0 (texture1)
    // Позади волны (diff < -waveWidth) -> 1.0 (texture2)
    float mixFactor = smoothstep(waveWidth * 0.5, -waveWidth * 0.5, diff);

    // Считываем обе текстуры по ИСКАЖЕННЫМ координатам
    vec4 color1 = texture(texture1, displacedUV);
    vec4 color2 = texture(texture2, displacedUV);
    vec4 finalColor = mix(color1, color2, mixFactor);

    // 7. Бонус: Имитация освещения (Яркость)
    // Используем косинус (производную синуса), чтобы найти "склоны" волны
    float highlight = cos(wavePhase) * envelope * 0.15; // 0.15 - сила блика
    finalColor.rgb += highlight; // Добавляем свет (и тени, так как cos может быть < 0)

    FragColor = finalColor;
}