#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
using namespace std;


const int WINDOW_WIDTH = 800;       // Windows width
const int WINDOW_HEIGHT = 500;      // Windows height
const int BLOCK_SIZE = 20;          // Each Block's size
int SPEED = 1000;                   // Standard drop speed (lower is faster)
int c_speed = SPEED;                // Current drop speed
int Time_Limit = 200;               // Time limit to end the game
int Time_count_speed = 1000;        // Control the speed of time decreasing

vector<vector<int> > COLOR = {      // Store different colors, corresponding to the different blocks
    {255, 0, 0},      // Red
    {255, 165, 0},    // Orange
    {255, 255, 0},    // Yellow
    {0, 255, 0},      // Green
    {0, 0, 255},      // Deep Blue
    {0, 255, 255},    // Light Blue
    {160, 32, 240},   // Purple
    {0, 0, 0}         // NULL
};

// Used to record each grid's data
struct pos {
    bool filled;        // Whether a block exists on the grid.
    SDL_Rect rec;       // A grid's area
    vector<int> color;  // A grid's color
};

vector<SDL_Rect> M_Block;                                    // Moving Block
vector<SDL_Rect> pM_Block;                                   // Projection of Moving Block
vector<SDL_Rect> N_Block;                                    // Next Block
vector<int> M_color;                                         // Moving Block's color
vector<int> N_color;                                         // Next Block's color
vector<vector<SDL_Rect> > Blocks(28, vector<SDL_Rect>(4));   // All possible Blocks
vector< SDL_Rect > wall_U(WINDOW_WIDTH/BLOCK_SIZE);          // Wall(UP)
vector< SDL_Rect > wall_D(WINDOW_WIDTH/BLOCK_SIZE);          // Wall(Down)
vector< SDL_Rect > wall_L(WINDOW_HEIGHT/BLOCK_SIZE);         // Wall(Left)
vector< SDL_Rect > wall_M(WINDOW_HEIGHT/BLOCK_SIZE);         // Wall(Middle)
vector< SDL_Rect > wall_R(WINDOW_HEIGHT/BLOCK_SIZE);         // Wall(Right)
vector<vector<pos> > T_metrix;                               // The 2D vectors to record each grid

// Initialize the positions of walls, and the gap between each grid point is 2 pixels.
void Build_Wall() {
    wall_U[0] = {1, 1, BLOCK_SIZE-2, BLOCK_SIZE-2};
    for (int i = 20; i < static_cast<int>(wall_U.size()); i++) wall_U[i] = {BLOCK_SIZE*i+1, 1, BLOCK_SIZE-2, BLOCK_SIZE-2};
    for (int i = 0; i < static_cast<int>(wall_D.size()); i++) wall_D[i] = {BLOCK_SIZE*i+1, WINDOW_HEIGHT-BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2};
    for (int i = 1; i < static_cast<int>(wall_L.size())-1; i++) wall_L[i] = {1, BLOCK_SIZE*i+1, BLOCK_SIZE-2, BLOCK_SIZE-2};
    for (int i = 1; i < static_cast<int>(wall_M.size())-1; i++) wall_M[i] = {BLOCK_SIZE*20+1, BLOCK_SIZE*i+1, BLOCK_SIZE-2, BLOCK_SIZE-2};
    for (int i = 1; i < static_cast<int>(wall_R.size())-1; i++) wall_R[i] = {WINDOW_WIDTH-BLOCK_SIZE+1, BLOCK_SIZE*i+1, BLOCK_SIZE-2, BLOCK_SIZE-2};
}

// Initialize the positions of each possible blocks, includes four rotational directions.
void Orig_Blocks() {
    // I-Block (1st direction)
    for (int i = 0; i < 4; i++) {
        Blocks[0][i] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*(4-i)+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    }
    // J, L-Block (1st direction)
    for (int i = 0; i < 3; i++) {
        Blocks[1][i] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*(3-i)+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
        Blocks[2][i] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*(3-i)+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    }
    Blocks[1][3] = { BLOCK_SIZE*9+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[2][3] = { BLOCK_SIZE*11+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // O-Block (1st direction)
    Blocks[3][0] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[3][1] = { BLOCK_SIZE*11+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[3][2] = { BLOCK_SIZE*10+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[3][3] = { BLOCK_SIZE*11+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // S-Block (1st direction)
    Blocks[4][0] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[4][1] = { BLOCK_SIZE*11+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[4][2] = { BLOCK_SIZE*9+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[4][3] = { BLOCK_SIZE*10+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // T-Block (1st direction)
    for (int i = 0; i < 3; i++) {
        Blocks[5][i] = { BLOCK_SIZE*(9+i)+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    }
    Blocks[5][3] = { BLOCK_SIZE*10+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // Z-Block (1st direction)
    Blocks[6][0] = { BLOCK_SIZE*9+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[6][1] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[6][2] = { BLOCK_SIZE*10+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[6][3] = { BLOCK_SIZE*11+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // I-Block (2nd direction)
    for (int i = 0; i < 4; i++) {
        Blocks[7][i] = { BLOCK_SIZE*(8+i)+1, -BLOCK_SIZE*3+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    }
    // J, L-Block (2nd direction)
    for (int i = 0; i < 3; i++) {
        Blocks[8][i] = { BLOCK_SIZE*(11-i)+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
        Blocks[9][i] = { BLOCK_SIZE*(11-i)+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    }
    Blocks[8][3] = { BLOCK_SIZE*9+1, -BLOCK_SIZE*3+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[9][3] = { BLOCK_SIZE*9+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // O-Block (2nd direction)
    Blocks[10][0] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[10][1] = { BLOCK_SIZE*11+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[10][2] = { BLOCK_SIZE*10+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[10][3] = { BLOCK_SIZE*11+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // S-Block (2nd direction)
    Blocks[11][0] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[11][1] = { BLOCK_SIZE*10+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[11][2] = { BLOCK_SIZE*9+1, -BLOCK_SIZE*3+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[11][3] = { BLOCK_SIZE*9+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // T-Block (2nd direction)
    for (int i = 0; i < 3; i++) {
        Blocks[12][i] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*(3-i)+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    }
    Blocks[12][3] = { BLOCK_SIZE*9+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // Z-Block (2nd direction)
    Blocks[13][0] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*3+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[13][1] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[13][2] = { BLOCK_SIZE*9+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[13][3] = { BLOCK_SIZE*9+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // I-Block (3rd direction)
    for (int i = 0; i < 4; i++) {
        Blocks[14][i] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*(4-i)+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    }
    // J, L-Block (3rd direction)
    for (int i = 0; i < 3; i++) {
        Blocks[15][i] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*(1+i)+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
        Blocks[16][i] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*(1+i)+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    }
    Blocks[15][3] = { BLOCK_SIZE*11+1, -BLOCK_SIZE*3+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[16][3] = { BLOCK_SIZE*9+1, -BLOCK_SIZE*3+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // O-Block (3rd direction)
    Blocks[17][0] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[17][1] = { BLOCK_SIZE*11+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[17][2] = { BLOCK_SIZE*10+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[17][3] = { BLOCK_SIZE*11+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // S-Block (3rd direction)
    Blocks[18][0] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[18][1] = { BLOCK_SIZE*11+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[18][2] = { BLOCK_SIZE*9+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[18][3] = { BLOCK_SIZE*10+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // T-Block (3rd direction)
    for (int i = 0; i < 3; i++) {
        Blocks[19][i] = { BLOCK_SIZE*(11-i)+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    }
    Blocks[19][3] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*3+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // Z-Block (3rd direction)
    Blocks[20][0] = { BLOCK_SIZE*9+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[20][1] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[20][2] = { BLOCK_SIZE*10+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[20][3] = { BLOCK_SIZE*11+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // I-Block (4th direction)
    for (int i = 0; i < 4; i++) {
        Blocks[21][i] = { BLOCK_SIZE*(8+i)+1, -BLOCK_SIZE*3+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    }
    // J, L-Block (4th direction)
    for (int i = 0; i < 3; i++) {
        Blocks[22][i] = { BLOCK_SIZE*(9+i)+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
        Blocks[23][i] = { BLOCK_SIZE*(9+i)+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    }
    Blocks[22][3] = { BLOCK_SIZE*11+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[23][3] = { BLOCK_SIZE*11+1, -BLOCK_SIZE*3+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // O-Block (4th direction)
    Blocks[24][0] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[24][1] = { BLOCK_SIZE*11+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[24][2] = { BLOCK_SIZE*10+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[24][3] = { BLOCK_SIZE*11+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // S-Block (4th direction)
    Blocks[25][0] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[25][1] = { BLOCK_SIZE*10+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[25][2] = { BLOCK_SIZE*9+1, -BLOCK_SIZE*3+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[25][3] = { BLOCK_SIZE*9+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // T-Block (4th direction)
    for (int i = 0; i < 3; i++) {
        Blocks[26][i] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*(1+i)+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    }
    Blocks[26][3] = { BLOCK_SIZE*11+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    // Z-Block (4th direction)
    Blocks[27][0] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*3+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[27][1] = { BLOCK_SIZE*10+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[27][2] = { BLOCK_SIZE*9+1, -BLOCK_SIZE*2+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
    Blocks[27][3] = { BLOCK_SIZE*9+1, -BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2 };
}

// To create specified text using the font "font_type".
//     Provide the specified renderer, the text to be rendered,
//     the top-left corner's x and y coordinates of the text, the text size, and the font type.
void Build_text(SDL_Renderer* renderer, string text, int x, int y, int f_size, string font_type) {
    char* s = new char[text.length() + 1];
    char* st = new char[font_type.length() + 1];
    // Used to convert a string to a const char*, then convert to char*
    strcpy(s, text.c_str());
    strcpy(st, font_type.c_str());
    // Initialize the SDL_ttf library
    TTF_Init();
    // Create the font object
    TTF_Font* font = TTF_OpenFont(st, f_size);
    // Specified color of the text (White)
    SDL_Color textColor = {255, 255, 255};
    // A data structure used to represent an image surface.
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, s, textColor);
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    // Convert SDL_Surface into SDL_Texture for GPU processing and rendering on the screen.
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    // Generate text and render it.
    SDL_Rect textRect = { x, y, textWidth, textHeight };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    // Free up memory that is not needed for use.
    TTF_CloseFont(font);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    delete [] s;
    delete [] st;
}


// Creat the specified text aligned to the right.
//     Provide the specified renderer, the text to be rendered,
//     the top-right corner's y coordinates of the text.
void Build_right_text(SDL_Renderer* renderer, string text, int y) {
    char* s = new char[text.length() + 1];
    // Used to convert a string to a const char*, then convert to char*
    strcpy(s, text.c_str());
    // Initialize the SDL_ttf library
    TTF_Init();
    // Create the font object
    TTF_Font* font = TTF_OpenFont("ttf/JetBrainsMono-Medium.ttf", 25);
    // Specified color of the text (White)
    SDL_Color textColor = {255, 255, 255};
    // A data structure used to represent an image surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, s, textColor);
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    // Convert SDL_Surface into SDL_Texture for GPU processing and rendering on the screen.
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    // Generate text and render it, with 730-textWidth to aligned to the right.
    SDL_Rect textRect = { 730-textWidth, y, textWidth, textHeight };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    // Free up memory that is not needed for use.
    TTF_CloseFont(font);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    delete [] s;
}

// Check if every grid in the specified row is filled with blocks.
bool RowFull(vector<vector<pos> > T_metrix, int row) {
    for (int i = 1; i < 20; i++) {
        if (!T_metrix[i][row].filled) return 0;
    }
    return 1;
}

// Swap the Moving Block with the Next Block.
// Swap using the 1st direction each time instead of the current direction.
void Switch_Block(int& r, int& nr) {
    r %= 7; // Using the 1st direction.
    // The difference in the x, y-coordinate between the Moving Block and its initial position.
    int X = M_Block[0].x - Blocks[r][0].x;
    int Y = M_Block[0].y - Blocks[r][0].y;
    // Check if there is enough space available to swap the blocks. Otherwise, return to end the function.
    for (int i = 0; i < 4; i++) {
        if (T_metrix[((Blocks[nr][i].x+X-1)/BLOCK_SIZE)][24-((Blocks[nr][i].y+Y-1)/BLOCK_SIZE)].filled) return;
    }
    // Swap the positions and the color of Moving Block and Next Block.
    for (int i = 0; i < 4; i++) {
        M_Block[i].x = Blocks[nr][i].x+X;
        M_Block[i].y = Blocks[nr][i].y+Y;
        swap(M_color[i], N_color[i]);
        N_Block[i].x = Blocks[r][i].x + 480;
        N_Block[i].y = Blocks[r][i].y + 280;
    };
    // Update the Projection of Moving Block.
    pM_Block = M_Block;
    // Swap the selected 'r' and 'rn'.
    swap(r, nr);
    return;
}

// Swap the Moving Block with its different direction.
void Rotate_Block(int& r) {
    // The difference in the x, y-coordinate between the Moving Block and its initial position.
    int X = M_Block[0].x - Blocks[r][0].x;
    int Y = M_Block[0].y - Blocks[r][0].y;
    // Check if there is enough space available to rotate the blocks. Otherwise, return to end the function.
    for (int i = 0; i < 4; i++) {
        if (T_metrix[(Blocks[(r+7)%28][i].x+X-1)/BLOCK_SIZE][24-(Blocks[(r+7)%28][i].y+Y-1)/BLOCK_SIZE].filled) return;
    }
    // Swap the current direction of Moving Block with the next direction.
    for (int i = 0; i < 4; i++) {
        M_Block[i].x = Blocks[(r+7)%28][i].x+X;
        M_Block[i].y = Blocks[(r+7)%28][i].y+Y;
    }
    // Update the Projection of Moving Block.
    pM_Block = M_Block;
    // Update the current direction of Moving Block.
    r = (r+7)%28;
    return;
}


// MAIN FUNCTION
int main(int argc, char* argv[]) {
    // Using 'srand()' to generate random blocks
    srand(time(NULL));
    // Initialize the coordinates and size of each grid point in T_metrix. (including the range of the walls.)
    for (int i = 0; i < 21; i++) {
        vector<pos> col(29);
        for (int j = 0; j < 29; j++) {
            col[j] = {0, {i*BLOCK_SIZE+1, (24-j)*BLOCK_SIZE+1, BLOCK_SIZE-2, BLOCK_SIZE-2}, COLOR[7]};
        }
        T_metrix.push_back(col);
        vector<pos>().swap(col);
    }
    // Set all grid cells within the walls' range to filled = 1.
    for (int i = 0; i < 29; i++) {
        T_metrix[0][i].filled = T_metrix[20][i].filled = 1;
    }
    for (int i = 1; i < 20; i++) {
        T_metrix[i][0].filled = 1;
    }
    // Initialize the SDL system.
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "Initializing SDL false: " << SDL_GetError() << endl;
        return 1;
    }
    // Initialize the SDL window.
    SDL_Window* window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {  // Check if the window was successfully created.
        cerr << "Creating window false: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }
    // Create an SDL renderer.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {  // Check if the renderer was successfully created.
        cerr << "Creating renderer false: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize the parameters.
    bool quit = 0;                 // Create a flag to control the while loop.
    SDL_Event event;               // Create an event variable to handle SDL events.
    int lastMoveTime1 = 0;         // Used to record the previous moment. (Control the falling speed)
    int lastMoveTime2 = 0;         // Used to record the previous moment. (Control the countdown speed)
    int score = 0;                 // Current score
    int break_color_count = 0;     // Record the degree of color fading when blocks are destroyed.
    bool should_break = 0;         // Determine whether to execute the block-destroying function.
    vector<int> v;                 // Used to store rows with blocks to be destroyed.
    vector<int> pv;                // Temporarily used to store another set of rows to be destroyed.
    bool down_one_step = 0;        // Coincide with the command for shifting downwards by one grid.
    bool END = 0;                  // Determine whether to end the game.
    bool Stop = 0;                 // Determine whether to stop the game.
    float BACKGROUND_COLOR = 0;    // Control the transparency of the background color.
    int r, nr;                     // Represent the indices of the Moving Block and the Next Block.
    vector<int> has_color;         // Ensure blocks and colors don't repeat.

    Build_Wall();
    Orig_Blocks();

    // Generate unique Moving block and Next block without repetition.
    r = rand()%7;
    M_color = COLOR[r];
    has_color.push_back(r);
    M_Block = pM_Block = Blocks[r];
    do {
        nr = rand()%7;
    } while (nr == r);
    has_color.push_back(nr);
    N_color = COLOR[nr];
    N_Block = Blocks[nr];
    for (int i = 0; i < 4; i++) {
        N_Block[i].x += 480;
        N_Block[i].y += 280;
    }

    // The starting page of Tetris game
    while (!quit) {
        Build_text(renderer, "TETRIS GAME", 210, 140, 60, "ttf/JetBrainsMonoNL-ExtraBold.ttf");
        Build_text(renderer, "Select a TIME LIMIT to start:", 237, 270, 20, "ttf/JetBrainsMono-Medium.ttf");
        Build_text(renderer, "Q: 200s   W: 300s   E: 400s", 245, 320, 20, "ttf/JetBrainsMono-Medium.ttf");
        // Display the content rendered by Build_text.
        SDL_RenderPresent(renderer);
        // Fetch events from the event queue. (enter something)
        while (SDL_PollEvent(&event)) {
            // Detecting key press events, used to select the countdown timer's time limit.
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    // If enter "q/Q"
                    case SDLK_q: {
                        Time_Limit = 201;
                        SPEED = 1000;
                        c_speed = SPEED;
                        quit = 1;
                        break;
                    }
                    // If enter "w/W"
                    case SDLK_w: {
                        Time_Limit = 301;
                        SPEED = 900;
                        c_speed = SPEED;
                        quit = 1;
                        break;
                    }
                    // If enter "e/E"
                    case SDLK_e: {
                        Time_Limit = 401;
                        SPEED = 800;
                        c_speed = SPEED;
                        quit = 1;
                        break;
                    }
                }

            }
        }
    }
    // Reset the flag.
    quit = 0;
    // MAIN LOOP
    while (!quit) {
        // Fetch events from the event queue. (enter something)
        while (SDL_PollEvent(&event)) {
            // Upon receiving the quit event, set the quit flag to true then end the game.
            if (event.type == SDL_QUIT) quit = true;
            // Handle keyboard press events to change block positions based on the key pressed.
            // Make the keys inactive when the game ends.
            if ((event.type == SDL_KEYDOWN) && (!END)) {
                switch (event.key.keysym.sym) {
                    // Set the current falling speed to the standard speed.
                    case SDLK_1: {
                        c_speed = SPEED;
                        break;
                    }
                    // Accelerate the falling speed to 70% of the standard speed.
                    case SDLK_2: {
                        c_speed = SPEED*0.7;
                        break;
                    }
                    // Accelerate the falling speed to 50% of the standard speed.
                    case SDLK_3: {
                        c_speed = SPEED*0.5;
                        break;
                    }
                    // Rotate the Moving Block
                    case SDLK_r: {
                        Rotate_Block(r);
                        break;
                    }
                    // Pause the game
                    case SDLK_p: {
                        if (!Stop) {
                            // Set the speed to a maximum value to stop the countdown and falling.
                            c_speed = INT_MAX;
                            Time_count_speed = INT_MAX;
                            Stop = 1;
                            BACKGROUND_COLOR = 150;
                        }
                        else {
                            // Set the speed to standard value.
                            c_speed = SPEED;
                            Time_count_speed = 1000;
                            Stop = 0;
                            BACKGROUND_COLOR = 0;
                        }
                        break;
                    }
                    // Swap the Moving Block with the Next Block.
                    case SDLK_c: {
                        Switch_Block(r, nr);
                        break;
                    }
                    // Use the left arrow key or "a/A" key to move Moving Block one grid to the left.
                    case SDLK_LEFT: case SDLK_a: {
                        // Check if every grid to the left of the Projection of Moving Block is not filled.
                        // Otherwise, keep increasing the height of the Projection of the Moving Block.
                        while(1) {
                            bool empty_left = 1;
                            for (int i = 0; i < 4; i++) {
                                for (int j = ((M_Block[i].y-1)/BLOCK_SIZE); j <= ((pM_Block[i].y-1)/BLOCK_SIZE); j++) {
                                    if (T_metrix[((pM_Block[i].x-1)/BLOCK_SIZE)-1][24-j].filled && (((pM_Block[i].x-1)/BLOCK_SIZE)-1 > 0)) empty_left = 0;
                                }
                            }
                            if (!empty_left) for (int i = 0; i < 4; i++) {pM_Block[i].y -= BLOCK_SIZE;}
                            else break;
                        }
                        // Check if every grid to the left of Moving Block is not filled.
                        // Otherwise, do not move.
                        bool empty_left = 1;
                        for (int i = 0; i < 4; i++) {
                            if (T_metrix[((M_Block[i].x-1)/BLOCK_SIZE)-1][24-((M_Block[i].y-1)/BLOCK_SIZE)].filled) {
                                empty_left = 0;
                                break;
                            }
                        }
                        if (empty_left) for (int i = 0; i < 4; i++) {
                            M_Block[i].x -= BLOCK_SIZE;
                            pM_Block[i].x -= BLOCK_SIZE;
                        }
                        break;
                    }
                    // Use the right arrow key or "d/D" key to move Moving Block one grid to the right.
                    case SDLK_RIGHT: case SDLK_d: {
                        // Check if every grid to the right of the Projection of Moving Block is not filled.
                        // Otherwise, keep increasing the height of the Projection of the Moving Block.
                        while(1) {
                            bool empty_right = 1;
                            for (int i = 0; i < 4; i++) {
                                for (int j = ((M_Block[i].y-1)/BLOCK_SIZE) ; j <= ((pM_Block[i].y-1)/BLOCK_SIZE); j++) {
                                    if (T_metrix[((pM_Block[i].x-1)/BLOCK_SIZE)+1][24-j].filled && (((pM_Block[i].x-1)/BLOCK_SIZE)+1 < 20)) empty_right = 0;
                                }
                            }
                            if (!empty_right) for (int i = 0; i < 4; i++) {pM_Block[i].y -= BLOCK_SIZE;}
                            else break;
                        }
                        // Check if every grid to the right of Moving Block is not filled.
                        // Otherwise, do not move.
                        bool empty_right = 1;
                        for (int i = 0; i < 4; i++) {
                            if (T_metrix[((M_Block[i].x-1)/BLOCK_SIZE)+1][24-((M_Block[i].y-1)/BLOCK_SIZE)].filled) {
                                empty_right = 0;
                                break;
                            }
                        }
                        if (empty_right) {
                            for (int i = 0; i < 4; i++) {
                                M_Block[i].x += BLOCK_SIZE;
                                pM_Block[i].x += BLOCK_SIZE;
                            }
                        }
                        break;
                    }
                    // Drop the Moving Block directly to the bottom.
                    case SDLK_DOWN: case SDLK_b: {
                        c_speed = 0;
                        break;
                    }
                    // Move Moving Block down by one grid.
                    case SDLK_s: {
                        // Used to signal subsequent loops that only one downward movement is needed.
                        down_one_step = 1;
                        c_speed = 0;
                        break;
                    }

                }
            }
        }
        // Check if every grid below the Projection of Moving Block is not filled.
        // Otherwise, do not move.
        bool empty_below = 1;
        for (int i = 0; i < 4; i++) {
            if (T_metrix[((pM_Block[i].x-1)/BLOCK_SIZE)][23-((pM_Block[i].y-1)/BLOCK_SIZE)].filled) {
                empty_below = 0;
                break;
            }
        }
        if (empty_below) {
            for (int i = 0; i < 4; i++) pM_Block[i].y += BLOCK_SIZE;
        }
        // Control the drop of the Moving Block.
        int currentTime1 = SDL_GetTicks();
        if (currentTime1 - lastMoveTime1 >= c_speed) {
            // Check if every grid below the Moving Block is not filled.
            bool empty_below = 1;
            for (int i = 0; i < 4; i++) {
                if (T_metrix[((M_Block[i].x-1)/BLOCK_SIZE)][23-((M_Block[i].y-1)/BLOCK_SIZE)].filled) {
                    empty_below = 0;
                    break;
                }
            }
            if (empty_below) {
                for (int i = 0; i < 4; i++) M_Block[i].y += BLOCK_SIZE;
            }
            // If the grid below is already filled:
            else {
                // Fill the T_metrix grid corresponding to the current position of the Moving Block
                // with the same color as the Moving Block.
                for (int i = 0; i < 4; i++) {
                    int X = ((M_Block[i].x-1)/BLOCK_SIZE);
                    int Y = 24-((M_Block[i].y-1)/BLOCK_SIZE);
                    T_metrix[X][Y].filled = 1;
                    T_metrix[X][Y].color = M_color;
                    // If the previous set of rows is still in the process of being cleared
                    // , and there's a new set of rows to be destroyed
                    // , temporarily store it in the pv vector.
                    if (break_color_count && RowFull(T_metrix, Y)) {
                        if (!count(pv.begin(), pv.end(), Y)) {
                            pv.push_back(Y);
                        }
                    }
                    else if (!count(v.begin(), v.end(), Y)) {
                        if (RowFull(T_metrix, Y)) {
                            v.push_back(Y);
                            should_break = 1;
                        }
                    }
                }
                // Replace the Moving Block and its projection with the Next Block.
                // The newer Next Block must be different from all the previously used Blocks.
                r = nr;
                if (has_color.size() < 7) {
                    do {
                        nr = rand()%7;
                    } while (count(has_color.begin(), has_color.end(), nr));

                }
                else if (has_color.size() == 7) {
                    vector<int>().swap(has_color);
                    nr = rand()%7;
                }
                has_color.push_back(nr);
                M_color = COLOR[r];
                M_Block = Blocks[r];
                pM_Block = M_Block;
                N_color = COLOR[nr];
                N_Block = Blocks[nr];
                for (int i = 0; i < 4; i++) {
                    N_Block[i].x += 480;
                    N_Block[i].y += 280;
                }
                c_speed = SPEED;  // Set the current speed back to the default standard speed.
            }
            // If only one grid downward movement is intended
            // , immediately set the speed back to the standard speed.
            if (down_one_step) {
                down_one_step = 0;
                c_speed = SPEED;
            }
            lastMoveTime1 = currentTime1;
        }
        // Control the speed of the countdown timer.
        int currentTime2 = SDL_GetTicks();
        if (currentTime2 - lastMoveTime2 >= Time_count_speed) {
            // If the time's up, set END to true to end the game, and stop all movement.
            if (Time_Limit == 1) {
                Time_Limit = 0;
                c_speed = INT_MAX;
                END = 1;
            }
            else Time_Limit -= 1;
            lastMoveTime2 = currentTime2;
        }
        // As the score increases, the standard speed gets faster.
        if (score > 500) {
            if (score < 1000) {
                SPEED = 700;
            }
            else if (score < 1500) {
                SPEED = 600;
            }
            else if (score < 2000) {
                SPEED = 400;
            }
            else if (score < 3000) {
                SPEED = 300;
            }
            else if (score < 6000) {
                SPEED = 150;
            }
        }
        // If the should_break flag is true, destroy the rows
        if (should_break) {
            // Once the entire row's transparency reaches 0, proceed to remove the specified row.
            if (break_color_count > 255) {
                should_break = 0;
                break_color_count = 0;
                // Sort the row and then proceed to remove them sequentially.
                sort(v.begin(), v.end());
                for (int i = 0; i < static_cast<int>(v.size()); i++) {
                    for (int k = v[i]-i; k < 25; k++) {
                        for (int j = 1; j < 20; j++) {
                            T_metrix[j][k].filled = T_metrix[j][k+1].filled;
                            T_metrix[j][k].color = T_metrix[j][k+1].color;
                        }
                    }
                }
                // Adjust the rows stored in the pv vector by subtracting the number of rows
                // removed from the v vector to correct their positions.
                for (int i = 0; i < static_cast<int>(pv.size()); i++) {
                    pv[i] -= v.size();
                }
                // Get different score multipliers based on the number of rows cleared at once.
                score += 100 * v.size() * v.size();
                // Clean v vector
                vector<int>().swap(v);
                // If pv vector's size larger than 0, swap v and pv to process row-breaking again.
                if (pv.size()) {
                    swap(v, pv);
                    should_break = 1;
                }
            }
            // Gradually reduce the transparency of the specified row's colors to create a simple fade-out animation.
            else {
                break_color_count++;
                for (int i = 0; i < static_cast<int>(v.size()); i++) {
                    for (int j = 1; j < 20; j++) {
                        if (T_metrix[j][v[i]].color[0] > 0) T_metrix[j][v[i]].color[0]--;
                        if (T_metrix[j][v[i]].color[1] > 0) T_metrix[j][v[i]].color[1]--;
                        if (T_metrix[j][v[i]].color[2] > 0) T_metrix[j][v[i]].color[2]--;
                    }
                }
            }
        }
        // Check the topmost row, and if there are blocks exceeding that position, end the game.
        for (int i = 1; i < 20; i++) {
            if (T_metrix[i][24].filled) {
                c_speed = INT_MAX;
                END = 1;
            }
        }

        // RENDER
        // Clear the renderer and set display colors.
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        // Set SDL_BLENDMODE_NONE to maintain opaque colors consistently.
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        // Render the colors of each grid on T_metrix.
        for (int i = 0; i < 21; i++) {
            for (int j = 0; j < 29; j++) {
                SDL_SetRenderDrawColor(renderer, (T_metrix[i][j].color)[0], (T_metrix[i][j].color)[1], (T_metrix[i][j].color)[2], 255);
                SDL_RenderFillRect(renderer, &T_metrix[i][j].rec);
            }
        }
        // Render all the text on the right-hand panel.
        Build_text(renderer, "TETRIS GAME", 467, 50, 40, "ttf/JetBrainsMonoNL-ExtraBold.ttf");
        Build_text(renderer, "111651070", 540, 100, 20, "ttf/JetBrainsMono-Medium.ttf");
        Build_text(renderer, "SCORE:", 470, 150, 25, "ttf/JetBrainsMono-Medium.ttf");
        Build_right_text(renderer, to_string(score), 150);
        Build_text(renderer, "Next Block:", 470, 240, 25, "ttf/JetBrainsMono-Medium.ttf");
        Build_text(renderer, "Time Left:", 470, 330, 25, "ttf/JetBrainsMono-Medium.ttf");
        Build_right_text(renderer, to_string(Time_Limit), 330);
        // Render Projection of Moving Block and Next Block
        for (int i = 0; i < 4; i++) {
            SDL_SetRenderDrawColor(renderer, M_color[0]/2.5, M_color[1]/2.5, M_color[2]/2.5, 0);
            SDL_RenderFillRect(renderer, &pM_Block[i]);
            SDL_SetRenderDrawColor(renderer, N_color[0], N_color[1], N_color[2], 255);
            SDL_RenderFillRect(renderer, &N_Block[i]);
        }
        // Render Moving Block (to render the Moving Block above the Projection Moving Block layer.)
        for (int i = 0; i < 4; i++) {
            SDL_SetRenderDrawColor(renderer, M_color[0], M_color[1], M_color[2], 255);
            SDL_RenderFillRect(renderer, &M_Block[i]);
        }
        // Render the wall with white color
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < static_cast<int>(wall_U.size()); i++) {
            SDL_RenderFillRect(renderer, &wall_U[i]);
            SDL_RenderFillRect(renderer, &wall_D[i]);
        }
        for (int i = 1; i < static_cast<int>(wall_L.size())-1; i++) {
            SDL_RenderFillRect(renderer, &wall_L[i]);
            SDL_RenderFillRect(renderer, &wall_M[i]);
            SDL_RenderFillRect(renderer, &wall_R[i]);
        }
        // If the game ends, gradually increase the background's opacity to darken the screen.
        // Stop the countdown timer
        if (END) {
            Time_count_speed = INT_MAX;
            BACKGROUND_COLOR += 0.75;
            if (BACKGROUND_COLOR >= 255) {
                quit = 1;
            }
        }
        // Render the background
        SDL_Rect BACK = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        // Set SDL_BLENDMODE_BLEND to make the transparency adjustable.
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        // Decreasing the transparency of the background.
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, BACKGROUND_COLOR);
        SDL_RenderFillRect(renderer, &BACK);
        // Render the text "STOP".
        if (Stop) {
            Build_text(renderer, "STOP", 330, 200, 70, "ttf/JetBrainsMonoNL-ExtraBold.ttf");
        }
        // Update the renderer to display the changes.
        SDL_RenderPresent(renderer);
    }
    // Reset the flag.
    quit = 0;
    // Clear the renderer and set display colors.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    // The ending page of Tetris game.
    while (!quit) {
        // Render all the text on the screen.
        Build_text(renderer, "GAME OVER", 250, 130, 60, "ttf/JetBrainsMonoNL-ExtraBold.ttf");
        Build_text(renderer, "Your Score: " + to_string(score), 255, 210, 30, "ttf/JetBrainsMono-Medium.ttf");
        Build_text(renderer, "Press any key to leave.", 295, 320, 17, "ttf/JetBrainsMono-Medium.ttf");
        SDL_RenderPresent(renderer);
        // Program execution ends when the user presses any key.
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN || event.type == SDL_QUIT) quit = 1;
        }
    }
    // Free up memory that is not needed for use.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

