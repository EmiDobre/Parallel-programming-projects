#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define CONTOUR_CONFIG_COUNT    16
#define FILENAME_MAX_SIZE       50
#define STEP                    8
#define SIGMA                   200
#define RESCALE_X               2048
#define RESCALE_Y               2048

#define CLAMP(v, min, max) if(v < min) { v = min; } else if(v > max) { v = max; }

void *thread_function(void *args);
struct Arguments {
    int nr_threads;
    int id;
    char input[50];
    ppm_image **contour_map;
    ppm_image *image;
    ppm_image *scaled_image;
};

//1: paralelizare contour:
ppm_image **init_contour_map(struct Arguments * arguments) {

    ppm_image **map = arguments->contour_map;
    int thread_id = arguments->id;
    int N = CONTOUR_CONFIG_COUNT;
    int P = arguments->nr_threads;
    int start = (int)(thread_id * (double)N / P);
    int end = (int)((thread_id + 1) * (double)N / P);
    if (end > N)
        end = N;

    for (int i = start; i < end; i++) {
        char filename[FILENAME_MAX_SIZE];
        sprintf(filename, "./contours/%d.ppm", i);
        map[i] = read_ppm(filename);
    }
    return map;
}

//2: paralelizare rescale:
ppm_image* rescale_image(struct Arguments *arguments) {

    ppm_image *image = arguments->image;
    uint8_t sample[3];
    ppm_image *new_image = arguments->scaled_image;

    //rescalam doar daca este nevoie:
    if (image->x <= RESCALE_X && image->y <= RESCALE_Y) {
        return image;
    }

    //numarul de linii in amtrice pixeli = new_iamge->
    int thread_id = arguments->id;
    int start = thread_id * new_image->x / arguments->nr_threads;
    int end = (thread_id + 1) * new_image->x / arguments->nr_threads;
    
    //impart imaginea in bucati fac interpolare pe bucati
    //fiecare thread modifica mai multe linii 
    for (int i = start; i < end; i++) {
        for (int j = 0; j < new_image->y; j++) {
            float u = (float)i / (float)(new_image->x - 1);
            float v = (float)j / (float)(new_image->y - 1);
            sample_bicubic(image, u, v, sample);

            new_image->data[i * new_image->y + j].red = sample[0];
            new_image->data[i * new_image->y + j].green = sample[1];
            new_image->data[i * new_image->y + j].blue = sample[2];
        }
    }
    return new_image; 
}

//3: grid
unsigned char **sample_grid(ppm_image *image, int step_x, int step_y, unsigned char sigma) {

    int p = image->x / step_x;
    int q = image->y / step_y;

    unsigned char **grid = (unsigned char **)malloc((p + 1) * sizeof(unsigned char*));
    if (!grid) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    for (int i = 0; i <= p; i++) {
        grid[i] = (unsigned char *)malloc((q + 1) * sizeof(unsigned char));
        if (!grid[i]) {
            fprintf(stderr, "Unable to allocate memory\n");
            exit(1);
        }
    }

    for (int i = 0; i < p; i++) {
        for (int j = 0; j < q; j++) {
            ppm_pixel curr_pixel = image->data[i * step_x * image->y + j * step_y];

            unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

            if (curr_color > sigma) {
                grid[i][j] = 0;
            } else {
                grid[i][j] = 1;
            }
        }
    }

    for (int i = 0; i < p; i++) {
        ppm_pixel curr_pixel = image->data[i * step_x * image->y + image->x - 1];

        unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

        if (curr_color > sigma) {
            grid[i][q] = 0;
        } else {
            grid[i][q] = 1;
        }
    }
    for (int j = 0; j < q; j++) {
        ppm_pixel curr_pixel = image->data[(image->x - 1) * image->y + j * step_y];

        unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

        if (curr_color > sigma) {
            grid[p][j] = 0;
        } else {
            grid[p][j] = 1;
        }
    }

    return grid;
}


//update sectiune particulara din imagine corespunzatori pixelilor de contur
//-sevential-
void update_image(ppm_image *image, ppm_image *contour, int x, int y) {

    for (int i = 0; i < contour->x; i++) {
        for (int j = 0; j < contour->y; j++) {
            int contour_pixel_index = contour->x * i + j;
            int image_pixel_index = (x + i) * image->y + y + j;

            image->data[image_pixel_index].red = contour->data[contour_pixel_index].red;
            image->data[image_pixel_index].green = contour->data[contour_pixel_index].green;
            image->data[image_pixel_index].blue = contour->data[contour_pixel_index].blue;
        }
    }
}

// pasul 2 algoritm - secvential - secvential -
void march(ppm_image *image, unsigned char **grid, ppm_image **contour_map, int step_x, int step_y) {

    int p = image->x / step_x;
    int q = image->y / step_y;

    for (int i = 0; i < p; i++) {
        for (int j = 0; j < q; j++) {
            unsigned char k = 8 * grid[i][j] + 4 * grid[i][j + 1] + 2 * grid[i + 1][j + 1] + 1 * grid[i + 1][j];
            update_image(image, contour_map[k], i * step_x, j * step_y);
        }
    }
}

// dezalocare resurse: - secvential -
void free_resources(ppm_image *image, ppm_image **contour_map, unsigned char **grid, int step_x) {

    for (int i = 0; i < CONTOUR_CONFIG_COUNT; i++) {
        free(contour_map[i]->data);
        free(contour_map[i]);
    }
    free(contour_map);

    for (int i = 0; i <= image->x / step_x; i++) {
        free(grid[i]);
    }
    free(grid);

    free(image->data);
    free(image);
}

//paralelizez primii 2 pasi:
void *thread_function(void *arg) {

    struct Arguments *arguments = (struct Arguments *) arg;
    //Pas1: contur:
    arguments->contour_map = init_contour_map(arguments);
    //Pas2: rescalare imagine:
    arguments->scaled_image = rescale_image(arguments);
	pthread_exit(NULL);
}


int main(int argc, char *argv[]) {

    if (argc < 4) {
        fprintf(stderr, "Usage: ./tema1 <in_file> <out_file> <P>\n");
        return 1;
    }

    //init imagine
    ppm_image *image = read_ppm(argv[1]);

    // 0. alocare contour map
    ppm_image **contour_map = (ppm_image **)malloc(CONTOUR_CONFIG_COUNT * sizeof(ppm_image *));
    if ( !contour_map ) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    
    // // 1. alocare imagine de scalat
    //aloc pointer care va fi modificat in structura de thread
    ppm_image *scaled_image = (ppm_image *)malloc(sizeof(ppm_image));
    if ( !scaled_image ) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    scaled_image->x = RESCALE_X;
    scaled_image->y = RESCALE_Y;
    scaled_image->data = (ppm_pixel*)malloc(scaled_image->x * scaled_image->y * sizeof(ppm_pixel));
    if (!scaled_image) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    if (image->x <= RESCALE_X && image->y <= RESCALE_Y) {
        scaled_image = image;
    }

    // creare threaduri:
    int P = atoi(argv[3]), i;
    pthread_t threads[P];
    
    for ( i  = 0; i < P; i++ ) {
        struct Arguments *arguments = (struct Arguments *)malloc(sizeof(struct Arguments));
        arguments->id = i;
        memcpy(arguments->input, argv[1], strlen(argv[1]));
        arguments->input[strlen(argv[1])] = '\0';
        arguments->nr_threads = P;
        arguments->contour_map = contour_map;
        arguments->image = image;
        arguments->scaled_image = scaled_image;
        pthread_create(&threads[i], NULL, thread_function, arguments);
    }
    for ( i = 0; i < P; i++ ) {
		pthread_join(threads[i], NULL);
	}

    // continuare apel functii in mod secvential:
    int step_x = STEP;
    int step_y = STEP;

    // 2. gridul:
    unsigned char **grid = sample_grid(scaled_image, step_x, step_y, SIGMA);

    // 3. march squares
    march(scaled_image, grid, contour_map, step_x, step_y);

    // 4. scriere output:
    write_ppm(scaled_image, argv[2]);

    // 5. dezalocare:
    free_resources(scaled_image, contour_map, grid, step_x);
    return 0;
}
