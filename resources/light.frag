uniform sampler2D texture;
uniform sampler2D shadowTexture;
uniform vec2 lightPositions[16]; // World coordinates
uniform float lightBrightnesses[16];
uniform int numberLightSources;
uniform mat3 viewMatrix;
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
    // gl_FragCoord has domain [0, screenSize.x] x [0, screenSize.y]
    vec2 unitScreenCoord = gl_FragCoord.xy / screenSize; // domain [0, 1]
    vec2 homoScreenCoord = 2. * unitScreenCoord - 1.; // domain [-1, 1]
    vec2 worldCoord = (viewMatrix * vec3(homoScreenCoord, 1)).xy;
    vec4 ambiantLight = vec4(0.4, 0.4, 0.4, 1);
    float localLight = 0.;
    for (int i = 0; i < numberLightSources; i++) {
        vec2 gap = worldCoord - lightPositions[i];
        localLight += lightBrightnesses[i] / dot(gap, gap);
    }
    localLight = clamp(localLight, 0., 10.);
    vec4 lightColor = getBlurredPixel(shadowTexture, unitScreenCoord) * localLight * (1. - ambiantLight) + ambiantLight;
    gl_FragColor = texture2D(texture, gl_TexCoord[0].st) * gl_Color * lightColor;
}
