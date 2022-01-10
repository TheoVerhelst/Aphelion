uniform sampler2D texture;

void main()
{
    vec4 ambiant = vec4(0.6, 0.6, 0.6, 1.0);
    gl_FragColor = texture2D(texture, gl_TexCoord[0].xy) * gl_Color * ambiant;
}
