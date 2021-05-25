# 五子棋AI

## 对象

- 电脑 MAX

- 玩家 MIN

- 棋盘局面

  

## 基本框架

- 状态`State`：包括**棋盘局面**和该轮**出招的游戏者**

- 评价函数

  - $e(s)>0$ 表示对MAX有利，$e(s)<0$ 表示对MIN有利。$|e(s)|$ 越大越有利。
  - 故可定义 $e(s)$ 为 $\text{Score MAX} - \text{Score MIN}$。
  - <img src="https://kimlongli.github.io/uploads/score.jpg" alt="img" style="zoom:35%;" />
  - 分为**单点**评分和**全局**评分

- 后继函数：生成下一步

  - 只需考虑已有棋子相邻（横纵对角）的点
  - 利用**单点**评分进行粗略**排序**，大幅度提升alpha-beta剪枝效率

- 终止测试：判断游戏是否结束

  

## 需要实现的函数

- `void terminate(State s, int lastMove)`

- `int evaluate_global(State s)`, `int evaluate_point(State s)`

- `vector<State> make_successors(State s, int player)`

- Minimax算法 + alpha-beta剪枝

  - 搜索的最大深度`lim` (迭代加深)

  ```cpp
  int max_value(int depth, State s, int alpha, int beta){ // 当前节点的alpha beta
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
