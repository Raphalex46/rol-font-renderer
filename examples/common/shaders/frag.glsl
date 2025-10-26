#version 330 core

out vec4 fragColor;
in vec2 texCoord;

uniform sampler2D ourTexture;

void main() {
  float col = texture(ourTexture, texCoord).r;
  fragColor = vec4(0.f, 0.f, 0.f, col);
}
