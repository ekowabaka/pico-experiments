from PIL import Image
import sys

with open(sys.argv[2], 'w') as f:
    with Image.open(sys.argv[1]) as img:

        f.write('#define IMG_WIDTH {0}\n'.format(img.width))
        f.write('#define IMG_HEIGHT {0}\n'.format(img.height))
        f.write('const uint32_t img_data[] = {\n')

        width, height = img.size
        for y in range(height):
            for x in range(width):
                z = img.getpixel((x, y))
                f.write('   0x{0:06x},\n'.format(z[0] | (z[1] << 8) | (z[2] << 16)))
                
        f.write('};\n')


