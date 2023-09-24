#include <stdio.h>
#include <stdlib.h>

//Krishna Pai 2020300046
//DIP Lab Experiment 1
#pragma pack(2)
typedef struct {
    unsigned short type;
    unsigned int size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int offset;
} BMPFileHeader;

typedef struct {
    unsigned int size;
    int width;
    int height;
    unsigned short planes;
    unsigned short bitsPerPixel;
    unsigned int compression;
    unsigned int imageSize;
    int xPixelsPerMeter;
    int yPixelsPerMeter;
    unsigned int colorsUsed;
    unsigned int colorsImportant;
} BMPImageHeader;

typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} Pixel;

void writeBMP(const char* filename, BMPImageHeader* imageHeader, Pixel** pixels) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    BMPFileHeader fileHeader = {
        .type = 0x4D42,
        .size = sizeof(BMPFileHeader) + sizeof(BMPImageHeader) + imageHeader->imageSize,
        .reserved1 = 0,
        .reserved2 = 0,
        .offset = sizeof(BMPFileHeader) + sizeof(BMPImageHeader)
    };

    // Write file header
    fwrite(&fileHeader, sizeof(BMPFileHeader), 1, file);

    // Write image header
    fwrite(imageHeader, sizeof(BMPImageHeader), 1, file);

    int height = abs(imageHeader->height);
    int width = imageHeader->width;
    for (int i =  0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fwrite(&pixels[i][j], sizeof(Pixel), 1, file);
        }
    }

    fclose(file);
}


//display file Print the required parameters and check bytes required to store header and BMPimage information.
void displayFileHeader(BMPFileHeader* fileHeader) {
    printf("-------------------------------------------------------------------------------\n");
    printf("File Header:\n");
    printf("Type: 0x%X\n", fileHeader->type);
    printf("Size: %u bytes\n", fileHeader->size);
    printf("Reserved1: %u\n", fileHeader->reserved1);
    printf("Reserved2: %u\n", fileHeader->reserved2);
    printf("Offset: %u bytes\n", fileHeader->offset);
}

//display image properties
void displayImageProperties(BMPImageHeader* imageHeader) {
    // printf("-------------------------------------------------------------------------------\n");
    printf("Image Properties:\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("Image size: %d x %d pixels\n", imageHeader->width, imageHeader->height);
    printf("Bits per pixel: %d\n", imageHeader->bitsPerPixel);
    printf("Image size: %d bytes\n", imageHeader->imageSize);
}



void displayRGBValues(Pixel** pixels, int start, int end) {
    printf(" RGB Values: for portion (%d, %d) to (%d, %d)\n", start, start, end, end);
    printf("-------------------------------------------------------------------------------\n");
    // printing grid RGB values
    for (int i = start; i < end; i++) {
        for (int j = start; j < end; j++) {
            printf("(%d, %d, %d) ", pixels[i][j].red, pixels[i][j].green, pixels[i][j].blue);
        }
        printf("\n");
    }
}

void readBMP(const char* filename, BMPImageHeader* imageHeader, Pixel*** pixels) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    BMPFileHeader fileHeader;
    fread(&fileHeader, sizeof(BMPFileHeader), 1, file);
    fread(imageHeader, sizeof(BMPImageHeader), 1, file);

    displayFileHeader(&fileHeader);
    displayImageProperties(imageHeader);

    if (fileHeader.type != 0x4D42 || imageHeader->bitsPerPixel != 24) {
        fprintf(stderr, "Not a valid 24-bit BMP file\n");
        fclose(file);
        return;
    }

    int width = imageHeader->width;
    int height = imageHeader->height;

    *pixels = (Pixel**)malloc(height * sizeof(Pixel*));

    for (int i = 0; i < height; i++) {
        (*pixels)[i] = (Pixel*)malloc(width * sizeof(Pixel));
        fread((*pixels)[i], sizeof(Pixel), width, file);
    }

    fclose(file);
}

void processAndSaveImages(const char* inputFilename, const char* outputGrayscaleFilename, const char* outputNegativeFilename) {
    BMPImageHeader imageHeader;
    Pixel** pixels;

    // Read BMP file and retrieve image details
    readBMP(inputFilename, &imageHeader, &pixels);
    // Create a new Pixel** array and copy the values from the original array
    Pixel** pixels2 = (Pixel**)malloc(abs(imageHeader.height) * sizeof(Pixel*));
    for (int i = 0; i < abs(imageHeader.height); i++) {
        pixels2[i] = (Pixel*)malloc(imageHeader.width * sizeof(Pixel));
        for (int j = 0; j < imageHeader.width; j++) {
            pixels2[i][j] = pixels[i][j];
        }
    }
    // Display the original BMP image
    printf("Original BMP Image:");
    displayRGBValues(pixels, 100, 105);

    // Generate and save the negative image

    printf("-------------------------------------------------------------------------------\n");
    printf("Saving Negative Image...\n");
    BMPImageHeader negativeImageHeader = imageHeader;
    negativeImageHeader.bitsPerPixel = 24;
    negativeImageHeader.imageSize = imageHeader.width * imageHeader.height * sizeof(Pixel);
    for (int i = 0; i < abs(imageHeader.height); i++) {
        for (int j = 0; j < imageHeader.width; j++) {
            pixels[i][j].red = 255 - pixels[i][j].red;
            pixels[i][j].green = 255 - pixels[i][j].green;
            pixels[i][j].blue = 255 - pixels[i][j].blue;
        }
    }
    writeBMP(outputNegativeFilename, &negativeImageHeader, pixels);

    // Display the negative image
    printf("Negative Image:");
    displayRGBValues(pixels, 100, 105);

    // Generate and save the grayscale image
    printf("-------------------------------------------------------------------------------\n");
    printf("Saving Grayscale Image...\n");
    BMPImageHeader grayscaleImageHeader = imageHeader;
    grayscaleImageHeader.bitsPerPixel = 24;
    grayscaleImageHeader.imageSize = imageHeader.width * imageHeader.height * sizeof(Pixel);
    for (int i = 0; i < abs(imageHeader.height); i++) {
        for (int j = 0; j < imageHeader.width; j++) {
            // unsigned char gray = (pixels[i][j].red + pixels[i][j].green + pixels[i][j].blue) / 3;
            unsigned char gray = (pixels2[i][j].red + pixels2[i][j].green + pixels2[i][j].blue) / 3;
            pixels[i][j].red =gray;
            pixels[i][j].green = gray;
            pixels[i][j].blue = gray;

        }
    }
    writeBMP(outputGrayscaleFilename, &grayscaleImageHeader, pixels);

    // Display the grayscale image
    printf("Grayscale Image:");
    displayRGBValues(pixels, 100, 105);

    // Free allocated memory
    for (int i = 0; i < abs(imageHeader.height); i++) {
        free(pixels[i]);
    }
    free(pixels);
}

int main() {
    const char* inputFilename = "yourphoto.bmp";
    const char* grayscaleOutputFilename = "final_grayscale.bmp";
    const char* negativeOutputFilename = "final_negative.bmp";

    processAndSaveImages(inputFilename, grayscaleOutputFilename, negativeOutputFilename);

    return 0;
}
