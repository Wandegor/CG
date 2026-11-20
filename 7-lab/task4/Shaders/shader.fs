#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float uProgress;
uniform float uAspect;

void main() {

    vec2 center = vec2(0.5, 0.5);
    float waveFreq = 40.0;
    float waveAmp = 0.04;
    float waveWidth = 0.2;

    float maxRadius = 1.0;
    float currentRadius = uProgress * maxRadius;

    vec2 uv = TexCoords;
    uv.x *= uAspect;
    vec2 c = center;
    c.x *= uAspect;

    float dist = distance(uv, c);
    // diff > 0: пиксель впереди волны. diff < 0: волна уже прошла пиксель
    float diff = dist - currentRadius;

    // 1.0 прямо на фронте и плавно падает до 0.0 по краям
    float envelope = exp(-pow(diff / (waveWidth * 0.5), 2.0));

    // Искажение UV координат
    vec2 dir = normalize(TexCoords - center); // Направление сдвига
    float wavePhase = diff * waveFreq;        // Фаза синусоиды

    // Смещение. Умножаем на envelope, чтобы искажался только фронт
    float distortion = sin(wavePhase) * waveAmp * envelope;
    vec2 displacedUV = TexCoords + dir * distortion;

    // Смешивание картинок
    // smoothstep создает градиент перехода:
    // Впереди волны (diff > 0) -> 0.0 (texture1)
    // Позади волны (diff < -waveWidth) -> 1.0 (texture2)
    float mixFactor = smoothstep(waveWidth * 0.5, -waveWidth * 0.5, diff);

    vec4 color1 = texture(texture1, displacedUV);
    vec4 color2 = texture(texture2, displacedUV);
    vec4 finalColor = mix(color1, color2, mixFactor);

    // Освещение
    float highlight = cos(wavePhase) * envelope * 0.15; // 0.15 - сила блика
    finalColor.rgb += highlight;

    FragColor = finalColor;
}