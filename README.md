# 3D-projection
![](https://github.com/hellux/3d-projection/blob/master/examples/suzanne/suz_render.png "Render of Suzanne")
Experiment to render 3D objects onto a 2D surface. Previewer and renderer written in C. Previewer uses the [SDL2](https://www.libsdl.org/) library to draw the projection in real time. Renderer uses the [libpng](http://www.libpng.org/pub/png/libpng.html) library to produce an output image. Both programs use the [libconfig](http://www.hyperrealm.com/libconfig/) library for handling config files.

#### Previewer
```
Usage: preview [options..]
Options:
    -c <config_path>    specify input config file [default: render.cfg].
    -o <output_path>    specify output config file [default: render.cfg].
    -h                  show this help screen.
```

#### Renderer
```
Usage: render [options..]
Options:
    -c <config_path>    specify config file [default: render.cfg].
    -o <output_path>    specify output image file [default: render.png].
    -h                  show this help screen.
```
