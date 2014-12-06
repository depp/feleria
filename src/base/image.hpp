/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_IMAGE_HPP
#define LD_BASE_IMAGE_HPP
#include "sg/opengl.h"
#include "sg/pixbuf.h"
#include <string>
namespace Base {
class Pixbuf;
class Texture;

class Image {
    sg_image *m_image;

public:
    Image();
    Image(const Image &) = delete;
    Image(Image &&other);
    ~Image();
    Image &operator=(const Image &) = delete;
    Image &operator=(Image &&other);
    const sg_image *operator->() const { return m_image; }
    explicit operator bool() const { return m_image != nullptr; }

    bool load(const std::string &path);
    void draw(Pixbuf &buf, int x, int y);
};

class Pixbuf {
    friend class Image;
    friend class Texture;
    sg_pixbuf m_pixbuf;

public:
    Pixbuf();
    Pixbuf(const Pixbuf &) = delete;
    Pixbuf(Pixbuf &&other);
    ~Pixbuf();
    Pixbuf &operator=(const Pixbuf &) = delete;
    Pixbuf &operator=(Pixbuf &&other);
    const sg_pixbuf *operator->() const { return &m_pixbuf; }
    void alloc(sg_pixbuf_format_t format, int width, int height);
    void calloc(sg_pixbuf_format_t format, int width, int height);
};

struct Texture {
    GLuint tex;
    short iwidth;
    short iheight;
    short twidth;
    short theight;
    float scale[2];

    Texture();
    Texture(const Texture &other) = delete;
    Texture(Texture &&);
    ~Texture();
    Texture &operator=(const Texture &) = delete;
    Texture &operator=(Texture &&other);

    /// Load an image as a 2-dimensional texture.
    bool load(const std::string &path);

    /// Load an image as a 2-dimensional texture.
    bool load(const Pixbuf &image, int width, int height);

    /// Load an image as a 1-dimensional texture.
    bool load_1d(const std::string &path);

    /// Load an image as a 2-dimensional texture.
    bool load_1d(const Pixbuf &image, int width);
};

}
#endif
