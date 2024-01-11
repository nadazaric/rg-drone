// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <GL/glew.h>
// #include <GLFW/glfw3.h>
// #include "texture_helper.h"
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"
//
// unsigned loadImgToTex(const char* filePath) {
//     int TextureWidth;
//     int TextureHeight;
//     int TextureChannels;
//     unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
//     if (ImageData != NULL)
//     {
//         //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
//         stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);
//
//         // Provjerava koji je format boja ucitane slike
//         GLint InternalFormat = -1;
//         switch (TextureChannels) {
//         case 1: InternalFormat = GL_RED; break;
//         case 3: InternalFormat = GL_RGB; break;
//         case 4: InternalFormat = GL_RGBA; break;
//         default: InternalFormat = GL_RGB; break;
//         }
//
//         unsigned int Texture;
//         glGenTextures(1, &Texture);
//         glBindTexture(GL_TEXTURE_2D, Texture);
//         glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
//         glBindTexture(GL_TEXTURE_2D, 0);
//         std::cout << "Textura procitana sa putanje: " << filePath << std::endl;
//         // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
//         stbi_image_free(ImageData);
//         return Texture;
//     }
//     else
//     {
//         std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
//         stbi_image_free(ImageData);
//         return 0;
//     }
// }