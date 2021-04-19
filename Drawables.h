#pragma once

#include <vector>
#include <string>

struct Orb {
    float x;
    float y;
    bool done;
    float rotation;
};

struct TargaHeader;

class Texture {

public:

    GLuint mTexId;
    int mWidth, mHeight;
    int mBytesPerPixel;
    char* mData;

    Texture();
	Texture(const std::string& fname, GLint wrapmode = GL_REPEAT, GLint filteringMode = GL_LINEAR);
	~Texture();

	GLuint id() const { return mTexId; }
    void Deallocate();
	bool isValid() const { return mTexId > 0; }

private:

    void LoadTargaUncompressed(const TargaHeader* hdr, const char* imgData);
    void LoadTargaRLE(const TargaHeader* hdr, const char* imgData);

};

void DrawSans();
void initTextures();
void drawWall();
void drawFloor();
void drawCieling();
void drawInnerWall(float start_x, float start_z, float door_distance, float end_x, float end_z, float door_width, bool rotated);
void drawOrbs(Orb* orb);