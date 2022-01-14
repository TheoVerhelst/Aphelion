uniform sampler2D texture;
uniform sampler2D shadowTexture;
uniform vec2 lightPositions[16]; // World coordinates
uniform float lightBrightnesses[16];
uniform int numberLightSources;
uniform mat3 viewMatrix;
uniform vec2 screenSize;

vec4 getBlurredPixel(sampler2D texture, vec2 coord) {
    float radius = 0.01;
    vec2 offX = vec2(radius, 0.);
    vec2 offY = vec2(0., radius);
    return (texture2D(texture, coord)               * 4.0 +
            texture2D(texture, coord - offX)        * 2.0 +
            texture2D(texture, coord + offX)        * 2.0 +
            texture2D(texture, coord - offY)        * 2.0 +
            texture2D(texture, coord + offY)        * 2.0 +
            texture2D(texture, coord - offX - offY) * 1.0 +
            texture2D(texture, coord - offX + offY) * 1.0 +
            texture2D(texture, coord + offX - offY) * 1.0 +
            texture2D(texture, coord + offX + offY) * 1.0) / 16.;
}

void main() {
    // gl_FragColor has domain [0, screenSize.x] x [0, screenSize.y]
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
