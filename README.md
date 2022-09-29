# Connect-Five-In-A-Row Game AI

## Basic Components

- Computer - `MAX`

- Gamer - `MIN`

- State of the chess board $S$

- The evaluation $e$ of a state $S$

  - $e(S)>0$ indicates that `MAX` (i.e. computer) is having the advantage, and $e(S)<0$ indicates that `MIN` (i.e. the gamer) is having the advantage. A larger magnitude represents a larger advantage.
  - We can define $e(S)=\text{Score MAX} - \text{Score MIN}$.
  - <img src="https://kimlongli.github.io/uploads/score.jpg" alt="img" style="zoom:35%;" />
  - The score of a point (local score) vs the score of the whole board (global score).
  
- Successor move

  - Suffices to consider neighbouring locations of the existing chesses on the board.
  - Sort each candidate move using their local scores. This improves the efficiency of alpha-beta pruning.
  
- Termination of the game


## Search Algorithms

- Minimax + Alpha-beta Pruning

  - `lim` is the maximum height of the search tree allowed - for adjusting game difficulty
  ```cpp
  int max_value(int depth, State s, int alpha, int beta){ // alpha, beta bound for the current node
      if (terminate(s) || depth >= lim) return evaluate(s);
      v = -INF;
      vector<State> succ = make_succesors(s, player^1);
      for (State nxt : succ){
          v = max(v, min_value(depth+1, nxt, alpha, beta));
          if (v >= beta) return v;
          alpha = max(alpha, v);
      }
      return v;
  }
  
  int min_value(int depth, State s, int alpha, int beta){
      if (terminate(s)) return evaluate(s);
      v = INF;
      vector<State> succ = make_succesors(s, player^1);
      for (State nxt : succ){
          v = min(v, max_value(depth+1, nxt, alpha, beta));
          if (v <= alpha) return v;
          beta = min(beta, v);
      }
      return v;
  }
  ```
  
