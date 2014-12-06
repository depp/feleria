/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "file.hpp"
#include "image.hpp"
#include "sg/entry.h"
#include "sg/error.h"
#include "sg/log.h"
#include "sg/util.h"
#include <cstdlib>
#include <cstring>
namespace Base {

Image::Image() : m_image(nullptr) { }

Image::Image(Image &&other) : m_image(nullptr) {
    std::swap(m_image, other.m_image);
}

Image::~Image() {
    if (m_image)
        m_image->free(m_image);
}

Image &Image::operator=(Image &&other) {
    std::swap(m_image, other.m_image);
    return *this;
}

bool Image::load(const std::string &path) {
    struct sg_error *err = nullptr;
    sg_image *img = sg_image_file(path.data(), path.size(), &err);
    if (!img) {
        sg_logerrf(SG_LOG_ERROR, err,
                   "%s: could not load image", path.c_str());
        sg_error_clear(&err);
        return false;
    }
    if (m_image) {
        m_image->free(m_image);
    }
    m_image = img;
    return true;
}

void Image::draw(Pixbuf &buf, int x, int y) {
    if (!m_image)
        return;
    m_image->draw(m_image, &buf.m_pixbuf, x, y, nullptr);
}

Pixbuf::Pixbuf() {
    m_pixbuf.data = nullptr;
}

Pixbuf::Pixbuf(Pixbuf &&other) {
    m_pixbuf = other.m_pixbuf;
    other.m_pixbuf.data = nullptr;
}

Pixbuf::~Pixbuf() {
    std::free(m_pixbuf.data);
}

Pixbuf &Pixbuf::operator=(Pixbuf &&other) {
    free(m_pixbuf.data);
    m_pixbuf = other.m_pixbuf;
    other.m_pixbuf.data = nullptr;
    return *this;
}

void Pixbuf::alloc(sg_pixbuf_format_t format, int width, int height) {
    std::free(m_pixbuf.data);
    m_pixbuf.data = nullptr;
    int r = sg_pixbuf_alloc(&m_pixbuf, format, width, height, nullptr);
    if (r)
        sg_sys_abort("Pixbuf::alloc");
}

void Pixbuf::calloc(sg_pixbuf_format_t format, int width, int height) {
    std::free(m_pixbuf.data);
    m_pixbuf.data = nullptr;
    int r = sg_pixbuf_calloc(&m_pixbuf, format, width, height, nullptr);
    if (r)
        sg_sys_abort("Pixbuf::calloc");
}

Texture::Texture()
    : tex(0),
      iwidth(0), iheight(0),
      twidth(0), theight(0)
{ }

Texture::Texture(Texture &&other)
    : tex(other.tex),
      iwidth(other.iwidth), iheight(other.iheight),
      twidth(other.twidth), theight(other.theight) {
    scale[0] = other.scale[0];
    scale[1] = other.scale[1];
}

Texture::~Texture() {
    if (tex)
        glDeleteTextures(1, &tex);
}

Texture &Texture::operator=(Texture &&other) {
    if (this == &other)
        return *this;
    if (tex)
        glDeleteTextures(1, &tex);
    tex      = other.tex;
    iwidth   = other.iwidth;
    iheight  = other.iheight;
    twidth   = other.twidth;
    theight  = other.theight;
    scale[0] = other.scale[0];
    scale[1] = other.scale[1];
    other.tex = 0;
    return *this;
}

bool Texture::load(const std::string &path) {
    Image image;
    if (!image.load(path))
        return false;
    int width = sg_round_up_pow2_32(image->width);
    int height = sg_round_up_pow2_32(image->height);
    Pixbuf buf;
    buf.calloc(SG_RGBA, width, height);
    image.draw(buf, 0, 0);
    return load(buf, image->width, image->height);
}

bool Texture::load(const Pixbuf &image, int width, int height) {
    iwidth = width;
    iheight = height;
    twidth = image->width;
    theight = image->height;
    scale[0] = 1.0f / (float) twidth;
    scale[1] = 1.0 / (float) theight;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    sg_pixbuf_texture(const_cast<sg_pixbuf *>(&image.m_pixbuf));
    glBindTexture(GL_TEXTURE_2D, 0);

    sg_opengl_checkerror("Texture::load");
    return true;
}

bool Texture::load_1d(const std::string &path) {
    Image image;
    if (!image.load(path))
        return false;
    int width = sg_round_up_pow2_32(image->width);
    Pixbuf buf;
    buf.calloc(SG_RGBA, width, 1);
    image.draw(buf, 0, 0);
    return load_1d(buf, image->width);
}

bool Texture::load_1d(const Pixbuf &image, int width) {
    iwidth = width;
    iheight = 1;
    twidth = image->width;
    theight = 1;
    scale[0] = (float) (1.0 / twidth);
    scale[1] = 1.0f;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    sg_pixbuf_texture(const_cast<sg_pixbuf *>(&image.m_pixbuf));
    glBindTexture(GL_TEXTURE_1D, 0);

    sg_opengl_checkerror("Texture::load_1d");
    return true;
}

}
