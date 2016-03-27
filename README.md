# 3D-projection
![](https://github.com/hellux/3d-projection/examples/suzanne/suz_render.png "Render of Suzanne")
Experiment to render 3D objects onto a 2D surface. Previewer and renderer written in C. Previewer uses the SDL library to draw the projection in real time. Renderer uses the libpng library to produce an output image.

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
