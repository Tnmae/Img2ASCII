#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define PIXEL_SIZE 10

int main(int argc, char *argv[]) {

  if (argc == 1) {
    std::cerr << "enter path of image to turn into ascii" << std::endl;
    return EXIT_FAILURE;
  }

  if (argc == 2) {
    std::cout << "you can also add another arguement ranging from 1-3, which "
                 "specifies the level of detail for your ascii art, by default "
                 "1 is chosen\n";
  }

  const char *path = (const char *)argv[1];

  int width, height, channels;

  unsigned char *img = stbi_load(path, &width, &height, &channels, 0);
  if (img == NULL) {
    std::cout << "error loading image\n";
    return EXIT_FAILURE;
  }

  std::cout << "width and height of image is " << width << " " << height
            << " respectively\n";
  std::cout << "and the no of channels is " << channels << "\n";
  size_t img_size = width * height * channels;
  unsigned int gray_channels = (channels == 4) ? 2 : 1;
  size_t gray_img_size = width * height * gray_channels;
  unsigned char *gray_img = (unsigned char *)malloc(gray_img_size);
  unsigned char *inv_img = (unsigned char *)malloc(img_size);
  if (gray_img == NULL) {
    std::cout << "error alloating space for gray image\n";
    return EXIT_FAILURE;
  }

  for (unsigned char *p = img, *pg = gray_img, *ip = inv_img;
       p != img + img_size;
       p += channels, pg += gray_channels, ip += channels) {
    *(pg) = (unsigned int)((*p + *(p + 1) + *(p + 2)) / 3.0);
    *(ip) = (255 - *(p));
    *(ip + 1) = (255 - *(p + 1));
    *(ip + 2) = (255 - *(p + 2));
    if (channels == 4) {
      *(pg + 1) = *(p + 3);
      *(ip + 3) = *(p + 3);
    }
  }

  stbi_write_png("gray.png", width, height, gray_channels, gray_img,
                 width * gray_channels);

  stbi_write_png("invImg.png", width, height, channels, inv_img,
                 width * channels);

  unsigned int pixel_channel = (gray_channels == 2) ? 2 : 1;
  size_t pixel_img_size = width * height * pixel_channel;
  unsigned char *pixel_img = (unsigned char *)(malloc(pixel_img_size));
  if (pixel_img == NULL) {
    std::cout << "error allocating space for pixelated image\n";
  }

  //@NOTE: this is the logic for pixelating an image
  unsigned int pixel_size = PIXEL_SIZE;
  unsigned char *pp = pixel_img;
  int counter = 0;
  unsigned char *pp1 = pixel_img;

  unsigned char *gp = gray_img;
  unsigned int lum = 0;

  for (int i = 0; i < height; i += pixel_size) {
    unsigned int avg = 0;
    for (int j = 0; j < width; j += pixel_size) {
      for (int l = j; l < pixel_size + j; l++) {
        for (int m = i; m < pixel_size + i; m++) {
          lum += (unsigned int)*(gp + l + m * pixel_channel * width);
        }
      }
      avg = (unsigned int)lum / (pixel_size * pixel_size);
      for (int l = j; l < pixel_size + j; l++) {
        for (int m = i; m < pixel_size + i; m++) {
          *(pp + l + m * pixel_channel * width) = avg;
        }
      }
      lum = 0;
    }
  }

  //@NOTE:This is the logic for mapping each pixel of pixel_size*pixel_size area
  // to its corresponding brightness intensity from the array

  std::string luminosity = "@%#*+=-:. "; // default luminosity

  if (argc == 3) {
    if (strcmp(argv[2], "1") == 0) {
      luminosity = "@%#*+=-:. ";
    } else if (strcmp(argv[2], "2") == 0) {
      luminosity = "Ñ@#W$9876543210?!abc;:+=-,._ ";
    } else if (strcmp(argv[2], "3") == 0) {
      luminosity = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/"
                   "\\|()1{}[]?-_+~i!lI;:,\"^`. ";
    }
  }
  std::cout << luminosity.length() << "\n";
  std::string inputString;
  unsigned int len = luminosity.length();
  double intensity;
  for (int i = 0; i < height; i += PIXEL_SIZE) {
    for (int j = 0; j < width; j += PIXEL_SIZE) {
      unsigned char *pp = (pixel_img + j + i * pixel_channel * width);
      intensity = (double)((*(pp) / 255.0) * len);
      std::cout << luminosity[len - intensity];
      inputString += luminosity[len - intensity];
    }
    std::cout << "\n";
    inputString += "\n";
  }

  std::ofstream file("asciiArt.txt");
  file << inputString;
  file.close();
  // std::cout << (luminosity) << std::endl;  prints whole string
  // std::cout << (luminosity[0]) << std::endl; prints first character of the
  // string
  // std::cout << *(luminosity) << std::endl;  prints first character of the
  // string

  stbi_write_png("gray_pixelated.png", width, height, pixel_channel, pixel_img,
                 width * pixel_channel);

  stbi_image_free(img);
  stbi_image_free(gray_img);
  stbi_image_free(inv_img);
  stbi_image_free(pixel_img);
  std::cout << __cplusplus << std::endl;
  return EXIT_SUCCESS;
}
