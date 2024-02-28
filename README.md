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
  dependencies\
    glew\ # from https://github.com/nigels-com/glew/blob/master/README.md
      include\
        GL\
      lib\
        Release\
    SDL2\ # from https://www.libsdl.org/
      include\
      lib\
```

## Controls

### Keyboard

<kbd>&larr;</kbd><kbd>&uarr;</kbd><kbd>&darr;</kbd><kbd>&rarr;</kbd> Move centre 1 pixel

<kbd>&larr;</kbd><kbd>&uarr;</kbd><kbd>&darr;</kbd><kbd>&rarr;</kbd> + <kbd>SHIFT</kbd> Move centre 10 pixels

<kbd>&larr;</kbd><kbd>&rarr;</kbd> + <kbd>CTRL</kbd> Change base hue of colouring

<kbd>&uarr;</kbd><kbd>&darr;</kbd> + <kbd>CTRL</kbd> Change rate at which colours change with iterations

<kbd>PAGE &uarr;</kbd><kbd>PAGE &darr;</kbd> Change zoom level

<kbd>PAGE &uarr;</kbd><kbd>PAGE &darr;</kbd> + <kbd>CTRL</kbd> Change iteration threshold

### Mouse

<kbd>LEFT</kbd> or <kbd>RIGHT</kbd> Drag centre

<kbd>LEFT</kbd> or <kbd>RIGHT</kbd> + <kbd>CTRL</kbd> Drag to change colouring. Horizontal changes base hue. Vertical motion changes the rate of colour change.

<kbd>LEFT</kbd>x2 Zoom in and centre on mouse location

<kbd>RIGHT</kbd>x2 Zoom out and centre on mouse location

<kbd>WHEEL</kbd> Change zoom level and centre on mouse location

<kbd>WHEEL</kbd> + <kbd>CTRL</kbd> Change iteration threshold

### Basic Menu
<kbd>Threshold</kbd> Iteration threshold slider

<kbd>Centre</kbd> Manually enter centre coordinates

<kbd>Zoom</kbd> Slider

<kbd>Base Hue</kbd> Base hue slider

<kbd>hueScale</kbd> Colour rate change slider

<kbd>Settings file</kbd> Filename to <kbd>Load</kbd> or <kbd>Save</kbd> settings in JSON format. Settings files can be manually edited with care.

### Advanced Menu

Various settings which affect how sensitivity and limits of the controls.

The image is calculated in batches to prevent the application being unresponsive or OpenGL timing out. The batch and duration settings affect how the application tries to find the appropriate batch size to stay responsive.

Quad (128-bit) precision is much slower and a special factor is included to try to adjust the batches for this. Quad zoom level determines at what point the calculations switch between the Double (64-bit) and Quad (128-bit) shaders.

## Zoom movies

Use the <kbd>Movie</kbd> menu to set the start and end parameters and number of frames. Start and end parameters can be entered manually or captured with the <kbd>Set Start</kbd> and <kbd>Set End</kbd> buttons from the currently displayed view. Preview with Play option or Frame slider.

<kbd>Bounce</kbd> option will zoom back out from start after the end has been reached. <kbd>Bounce</kbd>+<kbd>Loop</kbd> will keep zooming in and out.


<kbd>Generate frame files</kbd> will disable the menus and controls and generate .jpg frames. These can be converted into a video format with ffmpeg e.g:

```bash
ffmpeg -r 25 -i <mandel path>\movie\frame%04d.jpg -c:v libx264 -preset slow -crf 18 <mandel path>\movie\zoom.mp4
```
[Example movie](https://drive.google.com/file/d/1AMVITnDhYlRfh6JMHcJb_2EYI1koQUDb/preview)
