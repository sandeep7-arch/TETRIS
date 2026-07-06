#include <cstdint>
#include <iostream>
#include <cstdlib>
#include <random>
#include <raylib.h>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <functional>
#include <clocale>
#include <cmath>

using namespace std::chrono_literals;

const int tile = 32; //for rendering

const int boardWidth = 10*tile; //for rendering
const int boardHeight = 20*tile;
  



 const Color tetrisColors[9] = {
    { 0,   0,   0,   255 }, // Index 0: Empty space / Background (Black)
    { 128, 0,   128, 255 }, // Index 1: T-Piece (Standard Purple)
    { 0,   255, 0,   255 }, // Index 2: S-Piece (Standard Green)
    { 255, 0,   0,   255 }, // Index 3: Z-Piece (Standard Red)
    { 0,   255, 255, 255 }, // Index 4: I-Piece (Standard Cyan)
    { 255, 255, 0,   255 }, // Index 5: O-Piece (Standard Yellow)
    { 255, 127, 0,   255 }, // Index 6: L-Piece (Standard Orange)
    { 0,   0,   255, 255 }, // Index 7: J-Piece (Standard Blue)
    { 120, 120, 120, 255 }  // Index 8: Locked / Ghost Blocks (Gray)
};

const uint16_t tetrimino[7][4] = {    
    {0x4640, 0x0E40, 0x4C40, 0x4E00}, // T Shape
    {0x8C40, 0x6C00, 0x8C40, 0x6C00}, // S Shape
    {0x4C80, 0xC600, 0x4C80, 0xC600}, // Z Shape
    {0x4444, 0x0F00, 0x4444, 0x0F00}, // I Shape 
    {0xCC00, 0xCC00, 0xCC00, 0xCC00}, // O Shape 
    {0x4460, 0x0E80, 0xC440, 0x2E00}, // L Shape
    {0x44C0, 0x8E00, 0x6440, 0x0E20}  // J Shape 

};

class tetrisboard {
    public:
        static constexpr uint16_t h = 20;
        uint16_t grid[h]; //backend of tetris grid is a array of 20 ,  16bit integers(only 10 bits are used)
        int current_score = 0;
        int x = 5;
        int y = 0;
        uint16_t bx = 4; //for block coordinates
        uint16_t by = 4; 

        tetrisboard(){
            current_score = 0;

            for(uint16_t i = 0 ; i < h ; i++){
                    grid[i] = 0;
        }
}
};

class block_randomizer {  //modern 7 bag algo
    private:
        const std::vector<uint16_t> base = {0 , 1 , 2 , 3 , 4 , 5 , 6};
        std::vector<uint16_t> bag;
        std::mt19937 rng;
    public:
        block_randomizer(){
            std::random_device rd;
            rng.seed(rd());
        }

        uint16_t get_nxt_block(){
            if(bag.empty()){
                bag = base;
                std::shuffle(bag.begin() , bag.end() , rng);
            }

            int next_block = bag.back();
            bag.pop_back();
            return next_block;
        }
        
        uint16_t get_nxtx2_block(){ //to get see next block

        if(bag.empty()){
            bag = base;
            std::shuffle(bag.begin() , bag.end() , rng);
        }

            return bag.back();
        }
};

bool can_movefit(uint16_t s , uint16_t r , uint16_t bx , uint16_t by , int x , int y , uint16_t (&grid)[20]){ //to check if a block can fit
    
    for(uint16_t i = 0 ; i < by ; i++) {
        for(uint16_t j = 0 ; j < bx ; j++){
            int bit = (tetrimino[s][r] >> (15 - (i * 4 + j))) & 1;
            if(bit==1 && !((x+j)<=9 && (x+j) >=0 && (y+i)>=0 && (y+i) <= 19)) return false; 
            if(bit && (grid[y+i] & (1 << (x+j)))) return false; 

        }
    }
    return true;

}

bool active_draw(uint16_t r , uint16_t c , uint16_t s , uint16_t ro , uint16_t bx , uint16_t by , int x , int y){ //to draw the active falling piece
    uint16_t  bitgrid = tetrimino[s][ro];
    for(uint16_t i = 0 ; i < by ; i++) {
        for(uint16_t j = 0 ; j < bx ; j++){
            int bit = (tetrimino[s][ro] >> (15 - (i * 4 + j))) & 1;
            if(bit && (r-y == i && c-x == j)) return true;
}
} 
   return false;
}


void draw_grid_mirror(uint16_t (&grid)[20] , int x , int y , int board_x , int board_y ,  uint16_t bx , uint16_t by ,  uint16_t s , uint16_t ro) {

    DrawRectangleLines(board_x , board_y , boardWidth , boardHeight , WHITE);

   for (int r = 0; r < 20; r++) {
      

        for (int c = 0; c < 10 ; c++) {
            // Screen mapping math: column index multiplied by 2 for square block scaling
            int screen_x = board_x + (c * tile); 
            int screen_y = r*tile + board_y;
            DrawRectangleLines(screen_x, screen_y , tile , tile , DARKGRAY);
        if (grid[r] & (1 << c))
        {
          DrawRectangle(screen_x , screen_y , tile , tile , tetrisColors[8]); 
        }
        else if (active_draw(r , c , s , ro , bx , by , x , y))
        {
          DrawRectangle(screen_x , screen_y , tile , tile , tetrisColors[s+1]); 
        }
        }

         }

   
}

void draw_score(int score , int level , int Uix){

        DrawText(
    TextFormat("Score: %i", score),
    Uix,
    240,
    30,
    WHITE
);

        DrawText(
    TextFormat("Level: %i", level),
    Uix,
    280,
    30,
    WHITE
);
}






 
  
   
    




void draw_next_block(std::function<uint16_t()> get_nxtx2_piece , int prev_x , int prev_y){
    uint16_t s = get_nxtx2_piece();

    for(int r = 0 ; r < 4 ; r++){
        for(int c = 0 ; c < 4 ; c++){
              int screen_x = prev_x + (c * tile); 
            int screen_y = r*tile + prev_y;
            DrawRectangleLines(screen_x, screen_y, tile , tile , DARKGRAY);
      if((tetrimino[s][0]&(1<<(15-4*r-c)))){
      DrawRectangle(screen_x , screen_y , tile , tile , tetrisColors[s+1]);               
                   } 


        }


    }
}

void draw_title(){
    DrawText("TETRIS" , 30 , 340 , 100 , RED);
}




int main(){

    InitWindow(1500, 800, "Tetris");
    SetTargetFPS(60);
    std::setlocale(LC_ALL, "");

    
    


    tetrisboard board;
    block_randomizer randomizer;

    int x = board.x;
    int y = board.y;
    int board_x = (GetScreenWidth()-boardWidth)/2; 
    int board_y = (GetScreenHeight()-boardHeight)/2;
    const int previewX = board_x + boardWidth + 60;
    const int previewY = board_y;
    const int Uix = board_x + boardWidth + 60;

    uint16_t s = randomizer.get_nxt_block();
    uint16_t r = 0;
    int inc_lines = 0;
    int cur_lev = 1;
    double down_drop = std::pow((0.8f - ((cur_lev-1)*0.007f)) , cur_lev-1); // this is gravity
    double gravity_timer = 0;// it is a counter to make the block fall




    //gameloop
    bool game_over = false; 
    while(!WindowShouldClose() && !game_over){
    

    
        
            if(IsKeyPressed(KEY_LEFT) && can_movefit(s , (r)%4 , board.bx , board.by , x-1 , y , board.grid)) x -= 1;
            if(IsKeyPressed(KEY_RIGHT) && can_movefit(s , (r)%4 , board.bx , board.by , x+1 , y , board.grid)) x += 1;
            if(IsKeyPressed(KEY_RIGHT_SHIFT) && can_movefit(s , (r+1)%4 , board.bx , board.by , x , y , board.grid)) r += 1;
            if(IsKeyPressed(KEY_DOWN) && can_movefit(s , (r)%4 , board.bx , board.by , x , y+1 , board.grid)) y += 1;
            if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_DOWN) && can_movefit(s , (r)%4 , board.bx , board.by , x , y+1 , board.grid)) y += 1;
            if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_LEFT) && can_movefit(s , (r)%4 , board.bx , board.by , x-1 , y , board.grid)) x -= 1;
            if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_RIGHT) && can_movefit(s , (r)%4 , board.bx , board.by , x+1 , y , board.grid)) x += 1;
            
            
   
    gravity_timer += GetFrameTime();

    if(gravity_timer>down_drop){
    gravity_timer -= down_drop; // for accurate time measures
    if(can_movefit(s , (r)%4 , board.bx , board.by , x , y+1 , board.grid)) y += 1;
    else{
        //block locker
            board.current_score += 25;//score for every block placed
           for(int i = 0 ; i  < board.by ; i++){
                for(int j = 0 ; j < board.bx ; j++){
                    int bit = (tetrimino[s][r%4] >> (15 - (i * 4 + j))) & 1;
                    if(bit==1) board.grid[y+i] |= (1<<(x+j));

                }
           }
              //line checker
        int l = 0;
        for(int i = 0 ; i < 20 ; i++){
            if(board.grid[i] == 1023){
                l++;
                inc_lines++;
                for(int k = i ; k > 0 ; k--) {
                board.grid[k] = board.grid[k-1];
                }
                board.grid[0] = 0;
            }
        if(l) {
            
        board.current_score += l*l*100;// exponential score for hitting lines
       if(inc_lines==10) {
          
         inc_lines = 0;
         cur_lev++;
         down_drop = std::pow((0.8f - ((cur_lev-1)*0.007f)) , cur_lev-1);
        }
        }
        }
        r = 0;
        s = randomizer.get_nxt_block();
        x = board.x;
        y = board.y;
        gravity_timer = 0;
        game_over = !(can_movefit(s , (r)%4 , board.bx , board.by , x , y , board.grid)); 
        
 
    }


    
    }
    BeginDrawing();
    ClearBackground(BLACK);
    draw_title();
    draw_grid_mirror(board.grid , x , y , board_x , board_y ,  board.bx , board.by , s , r%4);
    draw_score(board.current_score , cur_lev , Uix);
    
    draw_next_block([&randomizer](){return randomizer.get_nxtx2_block(); } , previewX , previewY);
    EndDrawing();
    
}
    CloseWindow();
      std::cout<<"Game Over!!"<<"\n";
    std::cout<<"SCORE : "<<board.current_score;
    
return 0;
}
