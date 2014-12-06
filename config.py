#!/usr/bin/env python3
# Copyright 2014 Dietrich Epp.
import sys
from os.path import join, dirname
sys.path.append(join(dirname(__file__), 'sglib', 'script'))
import sglib

src = sglib.SourceList(base=__file__, path='src')

src.add(sources='''
main.cpp
''')

src.add(path='base', sources='''
array.hpp
chunk.cpp
chunk.hpp
file.cpp
file.hpp
image.cpp
image.hpp
log.cpp
log.hpp
mat.cpp
mat.hpp
orientation.cpp
orientation.hpp
quat.cpp
random.cpp
random.hpp
shader.cpp
shader.hpp
vec.hpp
''')

src.add(path='data', sources='''
sprite.array.hpp
sprite.enum.hpp
''')

src.add(path='game', sources='''
color.cpp
color.hpp
control.cpp
control.hpp
defs.hpp
game.cpp
game.hpp
move.cpp
move.hpp
''')

src.add(path='graphics', sources='''
defs.hpp
shader.cpp
shader.hpp
sprite.cpp
sprite.hpp
system.cpp
system.hpp
''')

def configure(build, sgmod):
    mod = build.target.module()
    mod.add_sources(
        src.sources,
        {'private': [
            build.target.module()
            .add_header_path(sglib._base(__file__, 'src')),
            sgmod]})
    return mod

app = sglib.App(
    name='Legend of Feleria',
    datapath=sglib._base(__file__, 'data'),
    email='depp@zdome.net',
    uri='http://www.moria.us/ludumdare/ld31/',
    copyright='Copyright © 2014 Dietrich Epp',
    identifier='us.moria.feleria',
    uuid='f6b6beed-78fb-4019-a56e-b41ea0a5b5bc',
    sources=src,
    configure=configure,
    icon=None,
)

if __name__ == '__main__':
    app.run()
