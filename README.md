# Interactive Mandelbrot set explorer

## Build

The repository includes solution files for Visual Studio. The provided files expect the following modules to be installed.

```bash
mandel\
  res\
  src\
  vendor\
    glm # from https://github.com/g-truc/glm
    imgui # from https://github.com/ocornut/imgui/wiki
    rapidjson # from https://rapidjson.org/
    stb\
      stb_image_write.h # from https://github.com/nothings/stb/blob/master/stb_image_write.h
  depdendencies\
    glew\ # from https://github.com/nigels-com/glew/blob/master/README.md
      include\
        GL\
      lib\
        Release\
    SDL2\ # from https://www.libsdl.org/
      include\
      lib\
```

## Zoom movies
Movie settings will generate .jpg frames. These can be converted with ffmpeg e.g:

```bash
ffmpeg -r 25 -i <mandel path>\movie\frame%04d.jpg -c:v libx264 -preset slow -crf 18 <mandel path>\movie\zoom.mp4
```
[Example movie](https://drive.google.com/file/d/1AMVITnDhYlRfh6JMHcJb_2EYI1koQUDb/preview)
