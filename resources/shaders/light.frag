uniform sampler2D texture;
uniform sampler2D shadowTexture;
uniform vec2 screenSize;

vec4 getBlurredPixel(sampler2D texture, vec2 coord) {
    const float radius = 0.01;
    const float blurWidth = 3.;
    float B = floor(blurWidth / 2.);
    float totalCoeff = 0.;
    vec4 color = vec4(0., 0., 0., 0.);
    for (float i = -B; i <= B; ++i) {
        for (float j = -B; j <= B; ++j) {
            vec2 coeff_ij = B - abs(vec2(i, j)) + 1.;
            float cellCoeff = coeff_ij.x * coeff_ij.y;
            totalCoeff += cellCoeff;
            // Clamp by 1% inside the texture to avoid glitches on the boundaries
            vec2 shiftedCoord = clamp(coord + vec2(i, j) * radius, 0.01, 0.99);
            color += texture2D(texture, shiftedCoord) * cellCoeff;
        }
    }
    return color / totalCoeff;
}

void main() {
    // gl_FragCoord has domain [0, screenSize.x] * [0, screenSize.y]
    vec2 unitScreenCoord = gl_FragCoord.xy / screenSize; // domain [0, 1]
    vec4 ambiantLight = vec4(0.4, 0.4, 0.4, 1);
    // TODO calculate the amount of light at that pixel while taking occlusion
    // (shadows) into account
    float localLight = 1.;
    vec4 lightColor = getBlurredPixel(shadowTexture, unitScreenCoord) * localLight * (1. - ambiantLight) + ambiantLight;
    gl_FragColor = texture2D(texture, gl_TexCoord[0].st) * gl_Color * lightColor;
}
