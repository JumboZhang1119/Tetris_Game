# 1. Game Design Overview
For this project, I used C++ as the primary programming structure, combined with the SDL2 library to handle graphics and input/output. The key SDL2 header files utilized are `SDL.h` and `SDL_ttf.h`. The `SDL.h` file is primarily responsible for rendering block graphics and managing input/output through key controls, while `SDL_ttf.h` is used for rendering specified text fonts.

# 2. Rules and Features
- **Tetromino Types and Colors**
  
  > There are seven types of tetrominoes: I, J, L, O, S, T, and Z. Each type has a unique color, and a single type always uses the same color.
- **Randomized Tetromino Drop**

  > The dropping tetrominoes follow a randomized cycle of seven, meaning the same type of tetromino will reappear at least once within 14 drops and can appear up to 2 times within 7 drops.
- **Projection for Placement**

  > Dropping tetrominoes will have a projection, allowing players to determine the best placement position.
- **Grid Dimensions**

  > The playfield is 19 units wide and 24 units tall. The game ends if any tetromino exceeds the 24th row.
- **Difficulty Levels**

  > Players can select one of three difficulty levels on the start screen:
  > - **Q Key**: 200s time limit, with the slowest initial speed.
  > - **W Key**: 300s time limit, with medium initial speed.
  > - **E Key**: 400s time limit, with the fastest initial speed.
- **Tetromino Movement Controls**

  > **Move Left**: Use the left arrow key or the A key to move the tetromino one block to the left.  
  > **Move Right**: Use the right arrow key or the D key to move the tetromino one block to the right.  
  > **Move Down**: Use the S key to shift the tetromino one block downward.  
  > **Hard Drop**: Use the down arrow key or the B key to make the tetromino fall directly to the bottom.
- **Additional Controls**

  > **Pause/Resume**: Use the P key.  
  > **Hold Swap**: Use the C key to swap the current tetromino with the next one.  
  > **Rotate**: Use the R key to rotate the current tetromino 90 degrees clockwise.  
  > **Adjust Drop Speed**: Use the number keys 1, 2, 3 (above the alphabet keys) to switch between drop speeds:  
  > - 1: Standard speed.
  > - 2: 1.4× standard speed.
  > - 3: 2× standard speed.
- **Constraints on Movement**

  > If there isn’t enough space for a swap or rotation, the action will fail.

- **Automatic Drop**

  > When no controls are applied, the tetromino will automatically fall at its designated speed, one row at a time.

- **Speed Scaling Based on Score**

  > As the score increases, the drop speed accelerates, with the following thresholds:
  > - 0–500 points.
  > - 500–1000 points.
  > - 1000–1500 points.
  > - 1500–2000 points.
  > - 2000–3000 points.
  > - Over 3000 points.
- **Scoring System**

  > Clearing multiple rows simultaneously earns varying points. If $n$ rows are cleared at once, the score is calculated as: $100\times n^2$
# 3. Demo Vedio
[Link to the vedio](https://www.youtube.com/watch?v=BvFeU7NTvdc&ab_channel=%E5%BC%B5%E6%9F%8F%E8%B1%90)  

<img width="684" alt="Tetris" src="https://github.com/user-attachments/assets/d83a547e-5161-4788-9e12-fa7f62e3a910">

