// // Author: APD team, except where source was noted

// #include "helpers.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>
// #include <pthread.h>

// #define CONTOUR_CONFIG_COUNT    16
// #define FILENAME_MAX_SIZE       50
// #define STEP                    8
// #define SIGMA                   200
// #define RESCALE_X               2048
// #define RESCALE_Y               2048

// #define CLAMP(v, min, max) if(v < min) { v = min; } else if(v > max) { v = max; }

// //definite fct threaduri:
// void *thread_function(void *args);
// struct Arguments {
//     int nr_threads;
//     int id;
//     char input[50];
//     char output[50];
//     ppm_image **contour_map;
//     ppm_image *image;
//     ppm_image *scaled_image;
//     unsigned char **grid;
//     pthread_mutex_t mutex;
// };


// // Creates a map between the binary configuration (e.g. 0110_2) and the corresponding pixels
// // that need to be set on the output image. An array is used for this map since the keys are
// // binary numbers in 0-15. Contour images are located in the './contours' directory.

// //1: paralelizare contour:
// ppm_image **init_contour_map(struct Arguments * arguments) {
//     ppm_image **map = arguments->contour_map;
//     int thread_id = arguments->id;
//     int N = CONTOUR_CONFIG_COUNT;
//     int P = arguments->nr_threads;
//     int start = (int)(thread_id * (double)N / P);
//     int end = (int)((thread_id + 1) * (double)N / P);
//     if (end > N)
//         end = N;

//     for (int i = start; i < end; i++) {
//         char filename[FILENAME_MAX_SIZE];
//         sprintf(filename, "./contours/%d.ppm", i);
//         map[i] = read_ppm(filename);
//     }
//     return map;
// }

// //2:paralelizare rescale:
// ppm_image* rescale_image(struct Arguments *arguments) {
//     ppm_image *image = arguments->image;
//     uint8_t sample[3];

//     // // nu mai alloc memory for image modific img cu ce am pana acum
//     ppm_image *new_image = arguments->scaled_image;

//     //rescalam doar daca este nevoie:
//     if (image->x <= RESCALE_X && image->y <= RESCALE_Y) {
//         return image;
//     }

//     //indici pt paralelizare thr vor calcula pixelii de pe mai multe linii
//     //deodata
//     int thread_id = arguments->id;
//     //numarul de linii in amtrice pixeli = new_iamge->
//     int start = thread_id * new_image->x / arguments->nr_threads;
//     int end = (thread_id + 1) * new_image->x / arguments->nr_threads;
    

//     // use bicubic interpolation for scaling: pentru paralelizare
//     //impart imaginea in bucati fac interpolare pe bucati
//     for (int i = start; i < end; i++) {
//         for (int j = 0; j < new_image->y; j++) {
//             float u = (float)i / (float)(new_image->x - 1);
//             float v = (float)j / (float)(new_image->y - 1);
//             sample_bicubic(image, u, v, sample);

//             new_image->data[i * new_image->y + j].red = sample[0];
//             new_image->data[i * new_image->y + j].green = sample[1];
//             new_image->data[i * new_image->y + j].blue = sample[2];
//         }
//     }
//     return new_image; 
// }

// //3: grid
// unsigned char **sample_grid(struct Arguments* arguments) {
//     ppm_image *image = arguments->scaled_image;
//     int step_x = STEP;
//     int step_y = STEP;
//     int p = image->x / step_x;
//     int q = image->y / step_y;
//     unsigned char **grid = arguments->grid;
//     pthread_mutex_t mutex;

//     //indicii de paralelizare:
//     int thread_id = arguments->id;
//     //numarul de linii in amtrice pixeli = new_iamge->
//     int start = thread_id * p / arguments->nr_threads;
//     int end = (thread_id + 1) * p / arguments->nr_threads;

//     pthread_mutex_lock(&mutex);
//     for (int i = start; i < end; i++) {
//         for (int j = 0; j < q; j++) {
//             ppm_pixel curr_pixel = image->data[i * step_x * image->y + j * step_y];

//             unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

//             if (curr_color > SIGMA) {
//                 grid[i][j] = 0;
//             } else {
//                 grid[i][j] = 1;
//             }
//         }
//     }

    
//     //regiune critica:
//     // last sample points have no neighbors below / to the right, so we use pixels on the
//     // last row / column of the input image for them

//     for (int i = start; i < end; i++) {
//         ppm_pixel curr_pixel = image->data[i * step_x * image->y + image->x - 1];

//         unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

//         if (curr_color > SIGMA) {
//             grid[i][q] = 0;
//         } else {
//             grid[i][q] = 1;
//         }
//     }
	
//     int start_j = thread_id * q / arguments->nr_threads;
//     int end_j = (thread_id + 1) * q / arguments->nr_threads;
//     for (int j = start; j < end; j++) {
//         ppm_pixel curr_pixel = image->data[(image->x - 1) * image->y + j * step_y];

//         unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

//         if (curr_color > SIGMA) {
//             grid[p][j] = 0;
//         } else {
//             grid[p][j] = 1;
//         }
//     }

//     pthread_mutex_unlock(&mutex);
    

//     return grid;
// }

// // Updates a particular section of an image with the corresponding contour pixels.
// // Used to create the complete contour image.
// void update_image(ppm_image *image, ppm_image *contour, int x, int y) {
//     for (int i = 0; i < contour->x; i++) {
//         for (int j = 0; j < contour->y; j++) {
//             int contour_pixel_index = contour->x * i + j;
//             int image_pixel_index = (x + i) * image->y + y + j;

//             image->data[image_pixel_index].red = contour->data[contour_pixel_index].red;
//             image->data[image_pixel_index].green = contour->data[contour_pixel_index].green;
//             image->data[image_pixel_index].blue = contour->data[contour_pixel_index].blue;
//         }
//     }
// }

// // Corresponds to step 2 of the marching squares algorithm, which focuses on identifying the
// // type of contour which corresponds to each subgrid. It determines the binary value of each
// // sample fragment of the original image and replaces the pixels in the original image with
// // the pixels of the corresponding contour image accordingly.
// void march(ppm_image *image, unsigned char **grid, ppm_image **contour_map, int step_x, int step_y) {
//     int p = image->x / step_x;
//     int q = image->y / step_y;

//     for (int i = 0; i < p; i++) {
//         for (int j = 0; j < q; j++) {
//             unsigned char k = 8 * grid[i][j] + 4 * grid[i][j + 1] + 2 * grid[i + 1][j + 1] + 1 * grid[i + 1][j];
//             update_image(image, contour_map[k], i * step_x, j * step_y);
//         }
//     }
// }

// // Calls `free` method on the utilized resources.
// void free_resources(ppm_image *image, ppm_image **contour_map, unsigned char **grid, int step_x) {
//     for (int i = 0; i < CONTOUR_CONFIG_COUNT; i++) {
//         free(contour_map[i]->data);
//         free(contour_map[i]);
//     }
//     free(contour_map);

//     for (int i = 0; i <= image->x / step_x; i++) {
//         free(grid[i]);
//     }
//     free(grid);

//     free(image->data);
//     free(image);
// }



// ////////////////////////////////////////////PARALELIZAREE///////////////////////////////////////
// //e nevoie de o matrice pt fiecare thread ca altfel se suprascrie thread_id ul!!
// void *thread_function(void *arg) {
//     struct Arguments *arguments = (struct Arguments *) arg;
    
//     int thread_id = arguments->id;
//     int nr_threads = arguments->nr_threads;
//     //printf("thread %d: %s, %s, %d", thread_id, arguments->input, arguments->output, nr_threads);
//     //printf("\n");

//     //Pas1: contur
//     arguments->contour_map = init_contour_map(arguments);
//     //Pas2: rescalare imagine:
//     //bariera, apoi next function:(daca e nevoie)
//     arguments->scaled_image = rescale_image(arguments);
//     //Pas3: constructie grid, trb sa am imaginea deja scalata	
//     arguments->grid = sample_grid(arguments);

// 	pthread_exit(NULL);
// }


// int main(int argc, char *argv[]) {

//     if (argc < 4) {
//         fprintf(stderr, "Usage: ./tema1 <in_file> <out_file> <P>\n");
//         return 1;
//     }

//     //init imagine
//     ppm_image *image = read_ppm(argv[1]);

//     ///ce era inainte:
//     // 0. Initialize contour map
//     ppm_image **contour_map = (ppm_image **)malloc(CONTOUR_CONFIG_COUNT * sizeof(ppm_image *));
//     if ( !contour_map ) {
//         fprintf(stderr, "Unable to allocate memory\n");
//         exit(1);
//     }
    
//     // // 1. Rescale the image
//     //aloc pointer care va fi modificat in structura de thr: alocare + init
//     ppm_image *scaled_image = (ppm_image *)malloc(sizeof(ppm_image));
//     if ( !scaled_image ) {
//         fprintf(stderr, "Unable to allocate memory\n");
//         exit(1);
//     }
//     scaled_image->x = RESCALE_X;
//     scaled_image->y = RESCALE_Y;
//     scaled_image->data = (ppm_pixel*)malloc(scaled_image->x * scaled_image->y * sizeof(ppm_pixel));
//     if (!scaled_image) {
//         fprintf(stderr, "Unable to allocate memory\n");
//         exit(1);
//     }
//     if (image->x <= RESCALE_X && image->y <= RESCALE_Y) {
//         scaled_image = image;
//     }

//     // 2. Sample the grid: init aloc grid si step urile
//     int step_x = STEP;
//     int step_y = STEP;
//     int p = image->x / step_x;
//     int q = image->y / step_y;
//     unsigned char **grid = (unsigned char **)malloc((p + 1) * sizeof(unsigned char*));
//     if (!grid) {
//         fprintf(stderr, "Unable to allocate memory\n");
//         exit(1);
//     }
//     for (int i = 0; i <= p; i++) {
//         grid[i] = (unsigned char *)malloc((q + 1) * sizeof(unsigned char));
//         if (!grid[i]) {
//             fprintf(stderr, "Unable to allocate memory\n");
//             exit(1);
//         }
//     }

//     // /////////////////////////////////PARALELIZARE:://////////////////////////////////
//     int P = atoi(argv[3]), i;
//     pthread_t threads[P];
	
//     pthread_mutex_t mutex;
//     if (pthread_mutex_init(&mutex, NULL) != 0) { 
//         printf("\n mutex init has failed\n"); 
//         return 1; 
//     } 
//     //init structura pt fiecare thread 
    
//     for ( i  = 0; i < P; i++ ) {
//         struct Arguments *arguments = (struct Arguments *)malloc(sizeof(struct Arguments));
//         arguments->id = i;
//         memcpy(arguments->input, argv[1], strlen(argv[1]));
//         arguments->input[strlen(argv[1])] = '\0';
//         memcpy(arguments->output, argv[2], strlen(argv[1]));
//         arguments->output[strlen(argv[2])] = '\0';
//         arguments->nr_threads = P;
//         arguments->contour_map = contour_map;
//         arguments->image = image;
//         arguments->scaled_image = scaled_image;
//         arguments->grid = grid;
//         arguments->mutex = mutex;
//         pthread_create(&threads[i], NULL, thread_function, arguments);
//     }
//     for ( i = 0; i < P; i++ ) {
// 		pthread_join(threads[i], NULL);
// 	}
//     /////////////////////functiile ramase neparalal//////////

//     // int step_x = STEP;
//     // int step_y = STEP;
//     // // 2. Sample the grid
//     // unsigned char **grid = sample_grid(scaled_image, step_x, step_y, SIGMA);

//     // 3. March the squares
//     march(scaled_image, grid, contour_map, step_x, step_y);

//     // 4. Write output
//     write_ppm(scaled_image, argv[2]);
//     free_resources(scaled_image, contour_map, grid, step_x);

//     pthread_mutex_destroy(&mutex); 
//     return 0;
// }
