#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void processImage(SDL_Surface *image) {
    // dimensions of the image
    int width = image->w;
    int height = image->h;

    // Loop through each pixel in the image
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Get the color of the current pixel
            Uint32 pixel = *((Uint32*)image->pixels + y * width + x);

            // Extract the red, green, and blue components
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);

            // Calculate the negative of the color
            r = 255 - r;
            g = 255 - g;
            b = 255 - b;

            // Set the new color for the pixel
            *((Uint32*)image->pixels + y * width + x) = SDL_MapRGB(image->format, r, g, b);
        }
    }


    printf("Coordinate(x, y)\tOriginal Image\t Negative Image\n");
    printf("--------------------------------------------\n");
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            Uint32 pixel = *((Uint32*)image->pixels + y * width + x);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            printf("(%d, %d)|\t", x, y);
            printf("(%d, %d, %d)|\t ", 255-r, 255-g, 255-b);
            printf("(%d, %d, %d)|\t", r, g, b);
            printf("\n");
        }
    
    }
}
//displayImageProperties that takes image and displays its properties
void displayImageProperties(SDL_Surface *image){
    printf("Image Properties\n");
    printf("--------------------------------------------\n");
    printf("Width: %d\n", image->w);
    printf("Height: %d\n", image->h);
    printf("Bits per pixel: %d\n", image->format->BitsPerPixel);
    printf("Bytes per pixel: %d\n", image->format->BytesPerPixel);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <image_file.jpg>\n", argv[0]);
        return 1;
    }

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL2 initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Load the image
    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image) {
        printf("Failed to load image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, image->w, image->h, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        SDL_FreeSurface(image);
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_FreeSurface(image);
        SDL_Quit();
        return 1;
    }

    // Load and display the image
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);


    displayImageProperties(image);
    // Process the image to get its negative
    processImage(image);

    // Create a texture for the negative image
    SDL_Texture *negativeTexture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, negativeTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Main loop
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(negativeTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(image);
    SDL_Quit();

    return 0;
}
