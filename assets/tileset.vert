#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in uint tileIndex;
layout (location = 3) in vec2 aOffset;

out vec2 vTexCoord;

uniform mat4 uProjection;
uniform ivec2 tilemapSizePixels;
uniform ivec2 tileSizePixels;

void main() {
    gl_Position = uProjection * vec4(aPos + aOffset, 0.0, 1.0);

    uint tilesPerRow = uint(tilemapSizePixels.x) / uint(tileSizePixels.x);
    uint tilesPerCol = uint(tilemapSizePixels.y) / uint(tileSizePixels.y);

    uint col = tileIndex % tilesPerRow;
    uint row = tileIndex / tilesPerRow;

    vec2 singleTile = vec2(1.0) / vec2(tilesPerRow, tilesPerCol);
    vTexCoord = (vec2(col, row) + texCoord) * singleTile;
}
