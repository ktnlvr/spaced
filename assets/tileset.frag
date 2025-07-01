#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;
uniform sampler2D sSampler;

void main() {
    vec4 tex = texture(sSampler, vTexCoord);
    FragColor = tex;
}
