uniform sampler2D texture;
uniform sampler2D shadowTexture;
uniform vec2 lightPositions[16];
uniform float lightBrightnesses[16];
uniform int numberLightSources;
uniform float distanceRatio;
uniform vec2 screenSize;

void main()
{
    vec4 ambiantLight = vec4(0.4, 0.4, 0.4, 1.0);
    vec2 unitScreenCoord = gl_FragCoord.xy / screenSize;
    float localLight = 0.;
    for (int i = 0; i < numberLightSources; i++) {
        float dx = (gl_FragCoord.x - lightPositions[i].x) * distanceRatio;
        float dy = (gl_FragCoord.y - lightPositions[i].y) * distanceRatio;
        localLight += lightBrightnesses[i] / (dx * dx + dy * dy);
    }
    localLight = clamp(localLight, 0., 10.);
    vec4 lightColor = texture2D(shadowTexture, unitScreenCoord) * localLight * (1. - ambiantLight)  + ambiantLight;
    gl_FragColor = texture2D(texture, gl_TexCoord[0].st) * gl_Color * lightColor;
}
