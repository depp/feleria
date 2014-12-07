/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_SPRITE_HPP
#define LD_GRAPHICS_SPRITE_HPP
#include "defs.hpp"
#include "base/array.hpp"
#include "base/file.hpp"
#include "base/orientation.hpp"
#include "base/image.hpp"
#include "game/sprite.hpp"
#include <vector>
struct sg_sprite;
namespace Graphics {

class SpriteSheet {
private:
    struct GroupInfo;

    Base::Data m_data;
    std::vector<GroupInfo> m_group;
    Base::Texture m_texture;

public:
    SpriteSheet();
    SpriteSheet(const SpriteSheet &) = delete;
    ~SpriteSheet();
    SpriteSheet &operator=(const SpriteSheet &) = delete;

    /// Load the sprite data.
    bool load();

    /// Get sprite info.
    const struct sg_sprite &get(Game::Sprite sprite, int nx, int ny) const;

    /// Get the sprite texture.
    const Base::Texture &texture() const { return m_texture; }
};

/// A part of a composite sprite.
struct SpritePart {
    const sg_sprite *sprite;
    Vec2 offset;
};

// Array of sprite rectangles with texture coordinates.
// Draw with GL_TRIANGLES.
class SpriteArray {
private:
    struct Vertex {
        Vec3 pos;
        short tx, ty;
    };

    Base::Array<Vertex> m_array;

public:
    SpriteArray();
    SpriteArray(const SpriteArray &other) = delete;
    SpriteArray(SpriteArray &&other);
    ~SpriteArray();
    SpriteArray &operator=(const SpriteArray &other) = delete;
    SpriteArray &operator=(SpriteArray &&other);

    /// Clear the array.
    void clear();
    /// Add sprites at the given location.
    void add(const SpritePart *parts, int count,
             Vec3 pos, Vec3 right, Vec3 up,
             Base::Orientation orient);
    /// Upload the array data.
    void upload(GLuint usage);
    /// Bind the OpenGL attribute.
    void set_attrib(GLint attrib);
    /// Get the number of vertexes.
    unsigned size() const { return m_array.size(); }
    /// Determine whether the array is empty.
    bool empty() const { return m_array.empty(); }
};

}
#endif
