/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "defs.hpp"
#include "system.hpp"
#include "transform.hpp"
#include "color.hpp"
#include "game/game.hpp"
#include "game/person.hpp"
#include "base/image.hpp"
#include <cstring>
namespace Graphics {

namespace {

const bool debug_trace = false;
const float FONT_SIZE = 48.0f;
const float BOX_SIZE = 1024.0f;
const Vec2 TEXT_POS {{ -425.0f, 325.0f }};
const int TEXT_PALETTE[3] = { 21, 23, 8 };

using Base::Orientation;

struct DirectionInfo {
    int index;
    Orientation orient;
};

const DirectionInfo DIRECTION_INFO[4] = {
    { 1, Orientation::FLIP_HORIZONTAL },
    { 2, Orientation::NORMAL },
    { 1, Orientation::NORMAL },
    { 0, Orientation::NORMAL },
};

const int LIGHT_COUNT = 4;
const float LIGHT_DIR[LIGHT_COUNT][3] = {
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },
    {-1.0f, 0.0f, 0.0f }
};
const float LIGHT_COLOR[LIGHT_COUNT][3] = {
    { 0.5f, 0.5f, 0.5f },
    { 1.0f, 0.5f, 0.5f },
    { 0.5f, 1.0f, 0.5f },
    { 0.5f, 0.5f, 1.0f }
};

const float SPRITE_SCALE = 0.2f;

struct FrameData {
    const Game::Game &game;
    int width, height;
    Mat4 projection;
    Mat4 worldview;
    Quat camera_angle;
    float pixscale;

    FrameData(int width, int height, const Game::Game &game);
};

FrameData::FrameData(int width, int height, const Game::Game &game)
    : game(game), width(width), height(height) {
    // Reference aspect ratio.
    const double ref_aspect = 16.0 / 9.0, inv_ref_aspect = 9.0 / 16.0;
    // 35mm equivalent focal length.
    const double focal = 55.0;
    // Width of the subject.
    const double subject_size = 64.0 * 1.4;

    double distance;

    {
        // We pretend that we are using a 16:9 aspect ratio.
        double xratio = 18.0 / focal;
        double yratio = xratio * inv_ref_aspect;
        distance = 0.5 * subject_size / xratio;

        // Then we expand the FOV to match the actual aspect ratio.
        double aspect = (double) width / (double) height;
        if (aspect > ref_aspect) {
            xratio = yratio * aspect;
            pixscale = (float) height * (1.0f / 1080.0f);
        } else {
            yratio = xratio / aspect;
            pixscale = (float) width * (1.0f / 1920.0f);
        }

        projection = Mat4::perspective(
            (float) xratio,
            (float) yratio,
            std::max(1.0f, (float) (distance - 0.5 * subject_size)),
            (float) (distance + 0.5 * subject_size));
    }

    // View angle.
    const double azimuth = 180.0;
    const double elevation = 40.0;

    {
        camera_angle =
            Quat::rotation(
                Vec3{{0.0f, 0.0f, 1.0f}},
                (std::atan(1.0) / 45.0) * (180.0 - azimuth)) *
            Quat::rotation(
                Vec3{{1.0f, 0.0f, 0.0f}},
                (std::atan(1.0) / 45.0) * (90.0 - elevation));
        Vec3 target {{ 0.0f, 0.0f, 2.0f }};
        Vec3 dir = camera_angle.transform(Vec3{{0.0f, 0.0f, 1.0f}});
        Vec3 pos = target + dir * (float) distance;
        worldview = Mat4::rotation(camera_angle.conjugate()) *
            Mat4::translation(-pos);
    }
}

}

// ======================================================================
// User interface
// ======================================================================

class System::SysUi {
private:
    unsigned m_serial;
    bool m_visible;
    Base::Texture m_textbox;
    Base::Program<Shader::Ui> m_prog;
    GLuint m_buffer;
    GLuint m_array;

public:
    SysUi();
    SysUi(const SysUi &) = delete;
    ~SysUi();
    SysUi &operator=(const SysUi &) = delete;

    bool load(const Game::Game &game);
    void draw(const FrameData &f);

public:
    void update(const FrameData &f);
};

System::SysUi::SysUi()
    : m_serial(0xffffffff),
      m_visible(false),
      m_buffer(0),
      m_array(0) {}

System::SysUi::~SysUi() {
    glDeleteBuffers(1, &m_buffer);
    glDeleteVertexArrays(1, &m_buffer);
}

bool System::SysUi::load(const Game::Game &game) {
    (void) &game;
    bool success = true;

    if (!m_textbox.load("image/textbox")) {
        success = false;
    } else {
        glBindTexture(GL_TEXTURE_2D, m_textbox.tex);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (!m_prog.load("ui", "ui")) {
        success = false;
    }
    glDeleteBuffers(1, &m_buffer);
    glDeleteVertexArrays(1, &m_array);

    if (m_prog.is_loaded()) {
        glGenBuffers(1, &m_buffer);
        glGenVertexArrays(1, &m_array);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glBindVertexArray(m_array);
        // glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        if (m_prog->a_vert >= 0) {
            glEnableVertexAttribArray(m_prog->a_vert);
            glVertexAttribPointer(
                m_prog->a_vert, 4, GL_FLOAT, GL_FALSE,
                0, reinterpret_cast<void *>(0));
        }
    }

    sg_opengl_checkerror("SysUi::load");
    return success;
}

void System::SysUi::draw(const Graphics::FrameData &f) {
    if (!m_prog.is_loaded()) {
        return;
    }
    update(f);
    if (!m_visible) {
        return;
    }

    glUseProgram(m_prog.prog());
    glBindVertexArray(m_array);
    glUniform1i(m_prog->u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textbox.tex);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glDepthRange(0.0, 0.0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0, 1.0);

    sg_opengl_checkerror("SysUi::draw");
}

void System::SysUi::update(const Graphics::FrameData &f) {
    const auto &vm = f.game.machine();
    if (m_serial == vm.text_serial()) {
        return;
    }
    m_serial = vm.text_serial();
    m_visible = !f.game.machine().text().empty();
    if (!m_visible) {
        return;
    }

    Vec2 vertscale { 2.0f / (float) f.width, 2.0f / (float) f.height };
    float boxsize = BOX_SIZE * f.pixscale;
    float x0 = -0.5f * boxsize * vertscale[0];
    float x1 = +0.5f * boxsize * vertscale[0];
    float y0 = -1.0f;
    float y1 = -1.0f + vertscale[1] * boxsize * 0.5f;
    float vert[24] = {
        x0, y0, 0.0f, 1.0f,
        x1, y0, 1.0f, 1.0f,
        x0, y1, 0.0f, 0.0f,
        x0, y1, 0.0f, 0.0f,
        x1, y0, 1.0f, 1.0f,
        x1, y1, 1.0f, 0.0f
    };
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    sg_opengl_checkerror("SysUi::update");
}

// ======================================================================
// Text
// ======================================================================

class System::SysText {
private:
    struct Batch {
        int line;
        float vertxform[4];
        int offset;
        int count;
    };

    sg_typeface *m_typeface;
    sg_font *m_font;
    unsigned m_serial;
    std::vector<Batch> m_batch;

    Base::Program<Shader::Text> m_prog;
    GLuint m_buffer;
    GLuint m_array;

public:
    SysText();
    SysText(const SysText &) = delete;
    ~SysText();
    SysText &operator=(const SysText &) = delete;

    bool load(const Game::Game &game);
    void draw(const FrameData &f);

private:
    void update(const FrameData &f);
};

System::SysText::SysText()
    : m_typeface(nullptr),
      m_font(nullptr),
      m_serial(0xffffffff),
      m_buffer(0),
      m_array(0) {}

System::SysText::~SysText() {
    glDeleteBuffers(1, &m_buffer);
    glDeleteVertexArrays(1, &m_array);
}

bool System::SysText::load(const Game::Game &game) {
    (void) &game;
    bool success = true;

    const char *path = "font/Alegreya-Bold";
    auto typeface = sg_typeface_file(path, std::strlen(path), nullptr);
    if (typeface == nullptr) {
        success = false;
    } else {
        if (m_typeface) {
            sg_typeface_decref(m_typeface);
        }
        m_typeface = typeface;
    }

    if (!m_prog.load("text", "text")) {
        success = false;
    }
    glDeleteBuffers(1, &m_buffer);
    glDeleteVertexArrays(1, &m_array);

    if (m_prog.is_loaded()) {
        glGenBuffers(1, &m_buffer);
        glGenVertexArrays(1, &m_array);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glBindVertexArray(m_array);
        if (m_prog->a_vert >= 0) {
            glEnableVertexAttribArray(m_prog->a_vert);
            glVertexAttribPointer(
                m_prog->a_vert, 4, GL_SHORT, GL_FALSE,
                0, reinterpret_cast<void *>(0));
        }
    }

    sg_opengl_checkerror("SysText::load");
    return success;
}

void System::SysText::draw(const Graphics::FrameData &f) {
    if (!m_prog.is_loaded()) {
        return;
    }
    update(f);
    if (m_batch.empty()) {
        return;
    }

    GLuint texture;
    float texscale[2];
    sg_font_gettexture(m_font, &texture, texscale);
    glUseProgram(m_prog.prog());
    glBindVertexArray(m_array);
    glUniform2fv(m_prog->u_texscale, 1, texscale);
    glUniform1i(m_prog->u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glDepthRange(0.0, 0.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    const auto &text = f.game.machine().text();
    for (const auto &batch : m_batch) {
        Color color = Color::palette(TEXT_PALETTE[text[batch.line].state]);
        glUniform4fv(m_prog->u_color, 1, color.v);
        glUniform4fv(m_prog->u_vertxform, 1, batch.vertxform);
        glDrawArrays(GL_TRIANGLES, batch.offset, batch.count);
    }

    glDepthFunc(GL_LESS);
    glDepthRange(0.0, 1.0);
    glDisable(GL_BLEND);

    sg_opengl_checkerror("SysText::draw");
}

struct Batch {
        int line;
        float vertxform[4];
        float color[4];
        int offset;
        int count;
    };

void System::SysText::update(const Graphics::FrameData &f) {
    const auto &vm = f.game.machine();
    if (m_serial == vm.text_serial()) {
        return;
    }
    m_serial = vm.text_serial();
    const auto &text = vm.text();

    bool load_font = !m_font && !text.empty();
    if (load_font) {
        auto font = sg_font_new(
            m_typeface, FONT_SIZE * f.pixscale, nullptr);
        if (m_font) {
            sg_font_decref(m_font);
        }
        m_font = font;
    }

    std::vector<sg_textlayout> layouts;
    layouts.reserve(text.size());
    int vertcount = 0, batchcount = 0;
    for (const auto &line : text) {
        auto flow = sg_textflow_new(nullptr);
        if (!flow) {
            break;
        }
        sg_textflow_setfont(flow, m_font);
        sg_textflow_setwidth(flow, -2.0f * TEXT_POS[0] * f.pixscale);
        sg_textflow_addtext(flow, line.text, std::strlen(line.text));

        sg_textlayout layout;
        int r = sg_textlayout_create(&layout, flow, nullptr);
        sg_textflow_free(flow);
        if (r) {
            break;
        }

        vertcount += layout.vertcount;
        batchcount += layout.batchcount;
        layouts.push_back(layout);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertcount * sizeof(sg_textvert),
        nullptr,
        GL_STATIC_DRAW);
    m_batch.clear();
    int vertoffset = 0;
    if (vertcount > 0) {
        m_batch.reserve(batchcount);
        Vec2 vertscale { 2.0f / f.width, 2.0f / f.height };
        Vec2 pos = TEXT_POS * f.pixscale;
        for (int i = 0, n = (int) layouts.size(); i < n; i++) {
            const auto &layout = layouts[i];
            glBufferSubData(
                GL_ARRAY_BUFFER,
                vertoffset * sizeof(sg_textvert),
                layout.vertcount * sizeof(sg_textvert),
                layout.vert);

            const auto &m = layout.metrics;
            Vec2 lpos = pos;
            lpos[0] -= (float) m.logical.x0;
            lpos[1] -= (float) m.logical.y1;
            float vertxform[4] = {
                vertscale[0],
                vertscale[1],
                lpos[0] * vertscale[0],
                lpos[1] * vertscale[1] - 1.0f
            };

            for (int j = 0, m = layout.batchcount; j < m; j++) {
                const auto &ib = layout.batch[j];
                Batch ob;
                ob.line = i;
                for (int k = 0; k < 4; k++) {
                    ob.vertxform[k] = vertxform[k];
                }
                ob.offset = vertoffset + ib.offset;
                ob.count = ib.count;
                m_batch.push_back(ob);
            }

            pos[1] -= (float) (m.logical.y1 - m.logical.y0);
            vertoffset += layout.vertcount;
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    for (auto &layout : layouts) {
        sg_textlayout_destroy(&layout);
    }

    sg_opengl_checkerror("SysText::update");
}

// ======================================================================
// Game world
// ======================================================================

class System::SysWorld {
private:
    Base::Program<Shader::World> m_prog;
    GLuint m_buffer;
    GLuint m_array;
    GLsizei m_count;

public:
    SysWorld();
    SysWorld(const SysWorld &) = delete;
    ~SysWorld();
    SysWorld &operator=(const SysWorld) = delete;

    bool load(const Game::Game &game);
    void draw(const FrameData &f);
};

System::SysWorld::SysWorld()
    : m_buffer(0),
      m_array(0),
      m_count(0) {}

System::SysWorld::~SysWorld() {
    glDeleteBuffers(1, &m_buffer);
    glDeleteVertexArrays(1, &m_array);
}

bool System::SysWorld::load(const Game::Game &game) {
    bool success = true;
    const auto &w = game.world();
    auto vdata = w.vertex_data();

    if (!m_prog.load("world", "world")) {
        success = false;
    }
    glDeleteBuffers(1, &m_buffer);
    glDeleteVertexArrays(1, &m_array);

    if (m_prog.is_loaded()) {
        glGenBuffers(1, &m_buffer);
        glGenVertexArrays(1, &m_array);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glBindVertexArray(m_array);
        glBufferData(GL_ARRAY_BUFFER,
                     vdata.second, vdata.first, GL_STATIC_DRAW);
        if (m_prog->a_vert >= 0) {
            glEnableVertexAttribArray(m_prog->a_vert);
            glVertexAttribPointer(
                m_prog->a_vert, 4, GL_INT_2_10_10_10_REV, GL_FALSE,
                8, reinterpret_cast<void *>(0));
        }
        if (m_prog->a_normal >= 0) {
            glEnableVertexAttribArray(m_prog->a_normal);
            glVertexAttribPointer(
                m_prog->a_normal, 4, GL_INT_2_10_10_10_REV, GL_TRUE,
                8, reinterpret_cast<void *>(4));
        }
    }

    m_count = (GLsizei) (vdata.second / 8);

    sg_opengl_checkerror("SysWorld::load");
    return success;
}

void System::SysWorld::draw(const Graphics::FrameData &f) {
    const auto &w = f.game.world();

    auto scale = w.vertex_scale();
    Mat4 modelview = f.worldview * Mat4::scale(scale);
    Mat3 normalmat = Mat3::identity();
    Color terrain_color[8] = {
        Color::palette(1),  Color::palette(2),
        Color::palette(7), Color::palette(20),
        Color::palette(3),  Color::palette(3),
        Color::palette(27),  Color::palette(27)
    };
    float height[8] = {
        -5.0f, +8.0f, 2.5f, 7.0f,
        -100.0f, -100.0f, -100.0f, -100.0f
    };
    for (int i = 0; i < 8; i++) {
        terrain_color[i].v[3] = height[i] * (1.0f / scale[2]);
    }

    glUseProgram(m_prog.prog());
    glBindVertexArray(m_array);
    glUniformMatrix4fv(m_prog->u_modelview, 1, GL_FALSE,
                       modelview.data());
    glUniformMatrix4fv(m_prog->u_projection, 1, GL_FALSE,
                       f.projection.data());
    glUniformMatrix3fv(m_prog->u_normalmat, 1, GL_FALSE,
                       normalmat.data());
    glUniform4fv(m_prog->u_terrain_color, 8, &terrain_color[0].v[0]);
    glUniform3fv(m_prog->u_light_dir, LIGHT_COUNT, LIGHT_DIR[0]);
    glUniform3fv(m_prog->u_light_color, LIGHT_COUNT, LIGHT_COLOR[0]);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, m_count);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    sg_opengl_checkerror("SysWorld::draw");
}

// ======================================================================
// Sprites
// ======================================================================

class System::SysSprite {
private:
    Base::Texture m_texture;
    SpriteArray m_sprites;
    int m_util_sprite;

    Base::Program<Shader::Sprite> m_prog;
    GLuint m_buffer;
    GLuint m_array;
    GLsizei m_count;

public:
    SysSprite();
    SysSprite(const SysSprite &) = delete;
    ~SysSprite();
    SysSprite &operator=(const SysSprite &) = delete;

    bool load(const Game::Game &game);
    void draw(const FrameData &f);

private:
    void update(const FrameData &f);
};

System::SysSprite::SysSprite()
    : m_util_sprite(-1),
      m_buffer(0),
      m_array(0),
      m_count(0) {}

System::SysSprite::~SysSprite() {
    glDeleteBuffers(1, &m_buffer);
    glDeleteVertexArrays(1, &m_array);
}

bool System::SysSprite::load(const Game::Game &game) {
    (void) &game;
    bool success = true;

    if (!m_texture.load("image/sprite")) {
        success = false;
    } else {
        glBindTexture(GL_TEXTURE_2D, m_texture.tex);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (!m_prog.load("sprite", "sprite")) {
        success = false;
    }
    glDeleteBuffers(1, &m_buffer);
    glDeleteVertexArrays(1, &m_array);

    if (m_prog.is_loaded()) {
        glGenBuffers(1, &m_buffer);
        glGenVertexArrays(1, &m_array);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glBindVertexArray(m_array);
        if (m_prog->a_vert >= 0) {
            glEnableVertexAttribArray(m_prog->a_vert);
            glVertexAttribPointer(
                m_prog->a_vert, 3, GL_FLOAT, GL_FALSE,
                16, reinterpret_cast<void *>(0));
        }
        if (m_prog->a_texcoord >= 0) {
            glEnableVertexAttribArray(m_prog->a_texcoord);
            glVertexAttribPointer(
                m_prog->a_texcoord, 4, GL_SHORT, GL_FALSE,
                16, reinterpret_cast<void *>(12));
        }
    }

    sg_opengl_checkerror("SysSprite::load");
    return success;
}

void System::SysSprite::draw(const Graphics::FrameData &f) {
    if (!m_prog.is_loaded()) {
        return;
    }
    update(f);
    if (!m_count) {
        return;
    }

    glUseProgram(m_prog.prog());
    glBindVertexArray(m_array);
    glUniformMatrix4fv(m_prog->u_modelview, 1, GL_FALSE,
                       f.worldview.data());
    glUniformMatrix4fv(m_prog->u_projection, 1, GL_FALSE,
                       f.projection.data());
    glUniform2fv(m_prog->u_texscale, 1, m_texture.scale);
    glUniform1i(m_prog->u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture.tex);

    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, m_count);
    glDisable(GL_DEPTH_TEST);

    sg_opengl_checkerror("SysSprite::draw");
}

void System::SysSprite::update(const Graphics::FrameData &f) {
    float frac = f.game.frame_frac();
    Vec3 right =
        f.camera_angle.transform(Vec3{{SPRITE_SCALE, 0.0f, 0.0f}});
    Vec3 up = f.camera_angle.transform(Vec3{{0.0f, SPRITE_SCALE, 0.0f}});

    const auto &sd = f.game.sprites();
    m_sprites.clear();
    for (const auto &person : f.game.person()) {
        auto dir = DIRECTION_INFO[static_cast<int>(person.direction())];
        SpritePart parts[Game::PART_COUNT], *op = parts;
        for (auto part : person.sprite()) {
            *op++ = SpritePart {
                &sd.get_data(part.sprite(), part.frame(), dir.index),
                part.offset()
            };
        }
        m_sprites.add(parts, (int) (op - parts),
                      person.position(frac), right, up, dir.orient);

        if (debug_trace) {
            const auto &w = f.game.world();
            auto pos = person.position(frac);
            Vec2 pos2 {{ pos[0], pos[1] }};
            auto trace = w.edge_distance(pos2, true);
            int frame = trace.first > 0.0f ? 0 : 1;
            parts[0] = SpritePart {
                &sd.get_data(m_util_sprite, frame, 0),
                Vec2::zero()
            };
            m_sprites.add(
                parts, 1,
                w.project(pos2 + trace.first * trace.second),
                right, up, Orientation::NORMAL);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    m_sprites.upload(GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_count = m_sprites.size();
    m_sprites.clear();

    sg_opengl_checkerror("SysSprite::update");
}

// ======================================================================
// System
// ======================================================================

System::System()
    : m_ui(new SysUi),
      m_text(new SysText),
      m_world(new SysWorld),
      m_sprite(new SysSprite) {}

System::~System() {}

#define LOAD(s) \
    do { \
        if (!(m_ ## s)->load(game)) { \
            Log::warn("Could not load graphics subsystem: %s", #s); \
            success = false; \
        } \
    } while (0)

bool System::load(const Game::Game &game) {
    bool success = true;
    sg_opengl_checkerror("System::load");

    {
        // We can't use GEW's version number variables, because we
        // enable glewExperimental, because GLEW doesn't query version
        // numbers correctly in the core profile.
        if (!GLEW_VERSION_3_0) {
            Log::abort("OpenGL version 3.0 or greater is required.");
            return false;
        }
        int major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        int version = (major << 8) | (minor & 0xff);
        // Mesa does not support GLSL > 1.30.
        // OS X does not support GLSL < 1.40 (unless you go back to 1.20).
        if (version >= 0x0301) {
            Base::shader_path = "shader/v140";
        } else {
            Base::shader_path = "shader/v130";
        }
        Log::info("OpenGL %d.%d (shader path: %s)",
                  major, minor, Base::shader_path.c_str());
    }

    LOAD(ui);
    LOAD(text);
    LOAD(world);
    LOAD(sprite);

    return success;
}

void System::draw(int width, int height, const Game::Game &game) {
    sg_opengl_checkerror("System::draw 0");
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.1f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    FrameData f(width, height, game);

    m_ui->draw(f);
    m_text->draw(f);
    m_world->draw(f);
    m_sprite->draw(f);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    sg_opengl_checkerror("System::draw 1");
}

}
