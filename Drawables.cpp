#include <GL/freeglut.h>
#include "Drawables.h"
#include <fstream>
#include <iostream>

std::vector<Texture*> mTextures;

enum TargaFileType {
	TARGA_RGB = 2,
	TARGA_GRAYSCALE = 3,
	TARGA_RLE_RGB = 10,
	TARGA_RLE_GRAYSCALE = 11
};

struct TargaHeader {
	unsigned char idLength;
	unsigned char colorMapType;
	unsigned char imageTypeCode;
	unsigned char colorMapSpec[5];
	unsigned short xOrigin;
	unsigned short yOrigin;
	unsigned short width;
	unsigned short height;
	unsigned char bpp;
	unsigned char imageDesc;
};

Texture::Texture() : mTexId(0) {}
GLuint TextureID[4];

Texture::Texture(const std::string& fname, GLint wrapMode, GLint filteringMode) 
{
	
	std::ifstream file(fname.c_str(), std::ios::binary);

	file.seekg(0, std::ios::end);
	size_t len = (size_t)file.tellg();
	file.seekg(0, std::ios::beg);

	char* buf = new char[len];

	file.read(buf, len);
	if (!file.good()) {
		delete[] buf;
	}

	TargaHeader* hdr = reinterpret_cast<TargaHeader*>(buf);

	switch (hdr->imageTypeCode) {
	case TARGA_RGB:
	case TARGA_GRAYSCALE:
	case TARGA_RLE_RGB:
	case TARGA_RLE_GRAYSCALE:
		break;
	default:
		delete[] buf;
	}

	mData = new char[hdr->width * hdr->height * hdr->bpp / 8];
	mWidth = hdr->width;
	mHeight = hdr->height;
	mBytesPerPixel = hdr->bpp;

	char* imgData = buf + sizeof(TargaHeader) + hdr->idLength;

	switch (hdr->imageTypeCode) {
	case TARGA_RGB:
	case TARGA_GRAYSCALE:
		// load an uncompressed image
		LoadTargaUncompressed(hdr, imgData);
		break;
	case TARGA_RLE_RGB:
	case TARGA_RLE_GRAYSCALE:
		// load RLE-compressed image
		LoadTargaRLE(hdr, imgData);
		break;
	default:
		// we should never get here
		delete[] buf;
		Deallocate();
	}

    delete[] buf;
}

void Texture::LoadTargaUncompressed(const TargaHeader* hdr, const char* imgData)
{
	int rowlen = (hdr->bpp / 8) * hdr->width;  // bytes per row
	int rowstep;
	char* dstRow;
	// check bit 5 of image descriptor to determine row ordering
	if (hdr->imageDesc & 0x20) {
		// bottom-to-top
		rowstep = -rowlen;
		dstRow = mData + rowlen * (hdr->height - 1);
	}
	else {
		// top-to-bottom
		rowstep = rowlen;
		dstRow = mData;
	}

	switch (hdr->bpp) {
	case 24:
		//std::cout << "~~ Loading 24 bpp" << std::endl;
		for (unsigned short j = 0; j < hdr->height; j++) {
			char* p = dstRow;
			for (unsigned short i = 0; i < hdr->width; i++) {
				char b = *imgData++;
				char g = *imgData++;
				char r = *imgData++;
				*p++ = r;
				*p++ = g;
				*p++ = b;
			}
			dstRow += rowstep;
		}
		break;

	case 32:
		//std::cout << "~~ Loading 32 bpp" << std::endl;
		for (unsigned short j = 0; j < hdr->height; j++) {
			char* p = dstRow;
			for (unsigned short i = 0; i < hdr->width; i++) {
				char b = *imgData++;
				char g = *imgData++;
				char r = *imgData++;
				char a = *imgData++;
				*p++ = r;
				*p++ = g;
				*p++ = b;
				*p++ = a;
			}
			dstRow += rowstep;
		}
		break;

	case 8:
		//std::cout << "~~ Loading 8 bpp" << std::endl;
		for (unsigned short j = 0; j < hdr->height; j++) {
			char* p = dstRow;
			for (unsigned short i = 0; i < hdr->width; i++) {
				*p++ = *imgData++;
			}
			dstRow += rowstep;
		}
		break;
	}
}

void Texture::LoadTargaRLE(const TargaHeader* hdr, const char* imgData)
{
    int rowlen = (hdr->bpp / 8) * hdr->width;  // bytes per row
    int rowstep;
    char* dstRow;
    // check bit 5 of image descriptor to determine row ordering
    if (hdr->imageDesc & 0x20) {
        // bottom-to-top
        rowstep = -rowlen;
        dstRow = mData + rowlen * (hdr->height - 1);
    }
    else {
        // top-to-bottom
        rowstep = rowlen;
        dstRow = mData;
    }

    const unsigned numPixels = hdr->width * hdr->height;
    unsigned numPixelsRead = 0;
    unsigned short numPixelsInRow = 0;
    char* p = dstRow;

    switch (hdr->bpp) {
    case 24:
        //std::cout << "~~ Loading 24 bpp, RLE" << std::endl;
        while (numPixelsRead < numPixels) {
            unsigned char count = (unsigned char)*imgData++;
            if (count > 127) {
                // RLE packet
                count -= 127;
                char b = *imgData++;
                char g = *imgData++;
                char r = *imgData++;
                for (unsigned char i = 0; i < count; i++) {
                    *p++ = r;
                    *p++ = g;
                    *p++ = b;
                    if (++numPixelsInRow == hdr->width) {
                        // advance to next row
                        dstRow += rowstep;
                        p = dstRow;
                        numPixelsInRow = 0;
                    }
                }
            }
            else {
                // raw packet
                ++count;
                for (unsigned char i = 0; i < count; i++) {
                    char b = *imgData++;
                    char g = *imgData++;
                    char r = *imgData++;
                    *p++ = r;
                    *p++ = g;
                    *p++ = b;
                    if (++numPixelsInRow == hdr->width) {
                        // advance to next row
                        dstRow += rowstep;
                        p = dstRow;
                        numPixelsInRow = 0;
                    }
                }
            }
            numPixelsRead += count;
        }
        break;

    case 32:
        //std::cout << "~~ Loading 24 bpp, RLE" << std::endl;
        while (numPixelsRead < numPixels) {
            unsigned char count = (unsigned char)*imgData++;
            if (count > 127) {
                // RLE packet
                count -= 127;
                char b = *imgData++;
                char g = *imgData++;
                char r = *imgData++;
                char a = *imgData++;
                for (unsigned char i = 0; i < count; i++) {
                    *p++ = r;
                    *p++ = g;
                    *p++ = b;
                    *p++ = a;
                    if (++numPixelsInRow == hdr->width) {
                        // advance to next row
                        dstRow += rowstep;
                        p = dstRow;
                        numPixelsInRow = 0;
                    }
                }
            }
            else {
                // raw packet
                ++count;
                for (unsigned char i = 0; i < count; i++) {
                    char b = *imgData++;
                    char g = *imgData++;
                    char r = *imgData++;
                    char a = *imgData++;
                    *p++ = r;
                    *p++ = g;
                    *p++ = b;
                    *p++ = a;
                    if (++numPixelsInRow == hdr->width) {
                        // advance to next row
                        dstRow += rowstep;
                        p = dstRow;
                        numPixelsInRow = 0;
                    }
                }
            }
            numPixelsRead += count;
        }
        break;

    case 8:
        //std::cout << "~~ Loading 8 bpp, RLE" << std::endl;
        while (numPixelsRead < numPixels) {
            unsigned char count = (unsigned char)*imgData++;
            if (count > 127) {
                // RLE packet
                count -= 127;
                char val = *imgData++;
                for (unsigned char i = 0; i < count; i++) {
                    *p++ = val;
                    if (++numPixelsInRow == hdr->width) {
                        // advance to next row
                        dstRow += rowstep;
                        p = dstRow;
                        numPixelsInRow = 0;
                    }
                }
            }
            else {
                // raw packet
                ++count;
                for (unsigned char i = 0; i < count; i++) {
                    *p++ = *imgData++;
                    if (++numPixelsInRow == hdr->width) {
                        // advance to next row
                        dstRow += rowstep;
                        p = dstRow;
                        numPixelsInRow = 0;
                    }
                }
            }
            numPixelsRead += count;
        }
        break;
    }
}

void Texture::Deallocate()
{
	delete[] mData;
	mData = NULL;
	mWidth = 0;
	mHeight = 0;
	mBytesPerPixel = 0;
}

Texture::~Texture()
{
    if (mTexId)
        glDeleteTextures(1, &mTexId);
}

void initTextures() {
    mTextures.push_back(new Texture("Textures/Bricks.tga", GL_REPEAT, GL_LINEAR));
    mTextures.push_back(new Texture("Textures/Wood.tga", GL_REPEAT, GL_LINEAR));
    mTextures.push_back(new Texture("Textures/Cieling.tga", GL_REPEAT, GL_LINEAR));
    mTextures.push_back(new Texture("Textures/sans.tga", GL_CLAMP, GL_LINEAR));

    //mTextures.push_back(new Texture("Textures/bricks_overpainted_blue_9291383.tga", GL_REPEAT, GL_LINEAR));
    glGenTextures(3, TextureID);

    for (int i = 0; i < mTextures.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, TextureID[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, TextureID[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTextures[i]->mWidth, mTextures[i]->mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mTextures[i]->mData);
    }
}

void drawWall() {


    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, TextureID[0]);

    glBegin(GL_QUADS);
    glTexCoord2f(-10.0, -1.0); glVertex3f(-1.0, -0.1, 0.0);
    glTexCoord2f(-10.0, 1.0); glVertex3f(-1.0, 0.1, 0.0);
    glTexCoord2f(10.0, 1.0); glVertex3f(1.0, 0.1, 0.0);
    glTexCoord2f(10.0, -1.0); glVertex3f(1.0, -0.1, 0.0);
	glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureID[0]);

}

void drawFloor() {

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, TextureID[1]);

    glBegin(GL_QUADS);
    glTexCoord2f(-3.0, -3.0); glVertex3f(-1.0, -1.0, 0.0);
    glTexCoord2f(-3.0, 3.0); glVertex3f(-1.0, 1.0, 0.0);
    glTexCoord2f(3.0, 3.0); glVertex3f(1.0, 1.0, 0.0);
    glTexCoord2f(3.0, -3.0); glVertex3f(1.0, -1.0, 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureID[1]);

}

void drawCieling() {

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, TextureID[2]);

    glBegin(GL_QUADS);
    glTexCoord2f(-3.0, -3.0); glVertex3f(-1.0, -1.0, 0.0);
    glTexCoord2f(-3.0, 3.0); glVertex3f(-1.0, 1.0, 0.0);
    glTexCoord2f(3.0, 3.0); glVertex3f(1.0, 1.0, 0.0);
    glTexCoord2f(3.0, -3.0); glVertex3f(1.0, -1.0, 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureID[2]);

}

void DrawSans() {

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, TextureID[3]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.1, 0.1, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.1, -0.1, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.1, -0.1, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.1, 0.1, 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureID[3]);
}


void drawInnerWall(float start_x, float start_z, float door_distance, float end_x, float end_z, float door_width, bool rotated = false) {

    float distance = 0.0;

    if (rotated) 
    {
        distance = door_distance;

        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, TextureID[0]);

        glBegin(GL_QUADS);
        glTexCoord2f(-3.0 * door_distance, -1.0); glVertex3f(start_x, -0.1, start_z);
        glTexCoord2f(-3.0 * door_distance, 1.0); glVertex3f(start_x, 0.1, start_z);
        glTexCoord2f(3.0 * door_distance, 1.0); glVertex3f(start_x, 0.1, start_z + door_distance);
        glTexCoord2f(3.0 * door_distance, -1.0); glVertex3f(start_x, -0.1, start_z + door_distance);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, TextureID[0]);

        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, TextureID[0]);

        glBegin(GL_QUADS);
        glTexCoord2f(-3.0 * door_width, -0.2); glVertex3f(start_x, 0.06, start_z + door_distance);
        glTexCoord2f(-3.0 * door_width, 0.2); glVertex3f(start_x, 0.1, start_z + door_distance);
        glTexCoord2f(3.0 * door_width, 0.2); glVertex3f(start_x, 0.1, start_z + door_distance + door_width);
        glTexCoord2f(3.0 * door_width, -0.2); glVertex3f(start_x, 0.06, start_z + door_distance + door_width);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, TextureID[0]);

        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, TextureID[0]);

        glBegin(GL_QUADS);
        glTexCoord2f(-3.0 * (end_z - start_z - door_distance - door_width), -1.0); glVertex3f(start_x, -0.1, start_z + door_distance + door_width);
        glTexCoord2f(-3.0 * (end_z - start_z - door_distance - door_width), 1.0); glVertex3f(start_x, 0.1, start_z + door_distance + door_width);
        glTexCoord2f(3.0 * (end_z - start_z - door_distance - door_width), 1.0); glVertex3f(start_x, 0.1, end_z);
        glTexCoord2f(3.0 * (end_z - start_z - door_distance - door_width), -1.0); glVertex3f(start_x, -0.1, end_z);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, TextureID[0]);
    }
    else
    {
        distance = door_distance;

        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, TextureID[0]);

        glBegin(GL_QUADS);
        glTexCoord2f(-3.0 * door_distance, -1.0); glVertex3f(start_x, -0.1, start_z);
        glTexCoord2f(-3.0 * door_distance, 1.0); glVertex3f(start_x, 0.1, start_z);
        glTexCoord2f(3.0 * door_distance, 1.0); glVertex3f(start_x + door_distance, 0.1, start_z);
        glTexCoord2f(3.0 * door_distance, -1.0); glVertex3f(start_x + door_distance, -0.1, start_z);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, TextureID[0]);

        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, TextureID[0]);

        glBegin(GL_QUADS);
        glTexCoord2f(-3.0 * door_width, -0.2); glVertex3f(start_x + door_distance, 0.06, start_z);
        glTexCoord2f(-3.0 * door_width, 0.2); glVertex3f(start_x + door_distance, 0.1, start_z);
        glTexCoord2f(3.0 * door_width, 0.2); glVertex3f(start_x + door_distance + door_width, 0.1, start_z);
        glTexCoord2f(3.0 * door_width, -0.2); glVertex3f(start_x + door_distance + door_width, 0.06, start_z);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, TextureID[0]);

        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, TextureID[0]);

        glBegin(GL_QUADS);
        glTexCoord2f(-3.0 * (end_x - start_x - door_distance - door_width), -1.0); glVertex3f(start_x + door_distance + door_width, -0.1, start_z);
        glTexCoord2f(-3.0 * (end_x - start_x - door_distance - door_width), 1.0); glVertex3f(start_x + door_distance + door_width, 0.1, start_z);
        glTexCoord2f(3.0 * (end_x - start_x - door_distance - door_width), 1.0); glVertex3f(end_x, 0.1, start_z);
        glTexCoord2f(3.0 * (end_x - start_x - door_distance - door_width), -1.0); glVertex3f(end_x, -0.1, start_z);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, TextureID[0]);
    }
}