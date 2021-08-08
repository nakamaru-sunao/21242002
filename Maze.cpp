#include <iostream>
#include <memory>
#include <vector>


//迷路に設置する物体のIDを設定
enum :size_t {
    empty_,//通路
    wall_,//壁
    root_,//ゴールまでの道
};

//迷路のサイズを決めるためのクラスと変数を設定
struct Mazesize
{
    int h;
    int vec_s;
};


//迷路のサイズを保管するvectorを作成
//変化後と変化前のそれぞれを保管できるようにする
using msize_t = std::vector<size_t>;
using vmsize_t = std::vector<msize_t>;

//生成チェック
const bool mapCheck(const vmsize_t& map_) {
    if (map_.size() <= 2 || map_.data()->size() <= 2) return false;
    if ((map_.size() & 1) == 0 || (map_.data()->size() & 1) == 0) return false;
    return true;
}


//穴掘り法での作成
void Dig(vmsize_t& map_, size_t x_, size_t y_, const size_t wall_, const size_t empty_)
{
    if (!mapCheck(map_)) return;//mapcheckの結果を確認する
    int32_t dx, dy;//変化量を設定

    size_t random = size_t(rand()), counter = 0;
    while (counter < 4) {
        switch ((random + counter) & 3)
        {
        case 0:dx = 0; dy = -2; //上下左右に2個進めたらOK
            break;
        case 1:dx = -2; dy = 0;
            break;
        case 2:dx = 0; dy = 2;
            break;
        case 3:dx = 2; dy = 0;
            break;
        default:dx = 0; dy = 0;
            break;
        }
        if (x_ + dx <= 0 || y_ + dy <= 0 || x_ + dx >= map_.size() - 1 || y_ + dy >= map_.data()->size() - 1 || map_[x_ + dx][y_ + dy] == empty_) {
            ++counter;
        }
        else if (map_[x_ + dx][y_ + dy] == wall_) {
            map_[x_ + (dx >> 1)][y_ + (dy >> 1)] = empty_;
            map_[x_ + dx][y_ + dy] = empty_;
            x_ += dx;
            y_ += dy;
            counter = 0;
            random = size_t(rand());
        }
    }
    return;
}

//迷路生成
const size_t MakeLoop(vmsize_t& map_, const size_t wall_, const size_t empty_, std::unique_ptr<size_t[]>& select_x, std::unique_ptr<size_t[]>& select_y)
{
    size_t ii = 0;
    const size_t i_max = map_.size() - 1;
    const size_t j_max = map_.data()->size() - 1;

    for (size_t i = 1; i < i_max; i += 2)
        for (size_t j = 1; j < j_max; j += 2) {
            if (map_[i][j] != empty_) continue;
            if ((i >= 2 && map_[i - 2][j] == wall_) || (j >= 2 && map_[i][j - 2] == wall_)) {
                select_x[ii] = i;
                select_y[ii] = j;
                ++ii;
            }
            else if ((j == map_.data()->size() - 2) && (i == map_.size() - 2)) break;
            else if ((i + 2 < map_.size() && map_[i + 2][j] == wall_) || (j + 2 < map_.data()->size() && map_[i][j + 2] == wall_)) {
                select_x[ii] = i;
                select_y[ii] = j;
                ++ii;
            }
        }
    return ii;
}

void Make(vmsize_t& map_, const size_t wall_, const size_t empty_)
{
    if (map_.size() <= 2 || map_.data()->size() <= 2) return;
    if ((map_.size() & 1) == 0 || (map_.data()->size() & 1) == 0) return;

    map_[1][1] = empty_;

    size_t a, ii;
    std::unique_ptr<size_t[]> select_x(new size_t[map_.size() * map_.data()->size()]);
    std::unique_ptr<size_t[]> select_y(new size_t[map_.size() * map_.data()->size()]);

    //スタートとゴールの座標
    while (true)
    {
        ii = MakeLoop(map_, wall_, empty_, select_x, select_y);
        if (ii == 0) break;
        srand((unsigned int)time(nullptr));
        a = size_t(rand()) % ii;
        Dig(map_, select_x[a], select_y[a], wall_, empty_);
    }
    return;
}


//経路探索
void RootLoop(vmsize_t& map_, const size_t x_, const size_t y_, const size_t empty_, const size_t root_)
{
    //経路探索状況
    static bool b = true;

    map_[x_][y_] = root_;
    if (x_ == map_.size() - 2 && y_ == map_.data()->size() - 2) b = false;

    //上
    if (b && map_[x_][y_ - 1] == empty_) RootLoop(map_, x_, y_ - 1, empty_, root_);
    //下
    if (b && map_[x_][y_ + 1] == empty_) RootLoop(map_, x_, y_ + 1, empty_, root_);
    //左
    if (b && map_[x_ - 1][y_] == empty_) RootLoop(map_, x_ - 1, y_, empty_, root_);
    //右
    if (b && map_[x_ + 1][y_] == empty_) RootLoop(map_, x_ + 1, y_, empty_, root_);

    if (b) map_[x_][y_] = empty_;
    return;
}
void Root(vmsize_t& map_, const size_t empty_, const size_t root_)
{
    if (!mapCheck(map_)) return;//mapcheckを確認
    RootLoop(map_, 1, 1, empty_, root_);//経路を生成
}


//出力
void Output(const vmsize_t& map_, const size_t wall_, const size_t empty_)
{
    if (!mapCheck(map_)) return;//mapcheckを確認
    const size_t i_max = map_.size();
    const size_t j_max = map_.data()->size();
    for (size_t i = 0; i < i_max; ++i) {
        for (size_t j = 0; j < j_max; ++j) {
            if (map_[i][j] == empty_) std::cout << "\x1B[34m□\033[0m";//通路
            else if (map_[i][j] == wall_) std::cout << "■";//壁
            else std::cout << "\x1B[31m・\033[0m";//経路
        }
        std::cout << std::endl;
    }
}





//迷路と経路の生成
int createMaze(const size_t x_, const size_t y_, const size_t empty_, const size_t wall_, const size_t root_) {
    if (x_ <= 2 || y_ <= 2) return 1;
    if ((x_ & 1) == 0 || (y_ & 1) == 0) return 2;

    std::string tmp;
    int a;
    //迷路マップ(全ての位置を壁にする)
    vmsize_t map_{ vmsize_t(y_, msize_t(x_, wall_)) };
    //迷路を生成
    Make(map_, wall_, empty_);
    Output(map_, wall_, empty_);
    
    std::cout << "Please enter a number" << std::endl;

    std::cin >> a;//数字を入力後に経路を表示

    Make(map_, wall_, empty_);
    std::cout << " " << std::endl;
    Root(map_, empty_, root_);
    //出力
    
    Output(map_, wall_, empty_);
    std::cout << "" << std::endl;

    return 0;
}



int main()
{
    std::vector<int> size(2);//入力された座標を保管するvectorを作成
    Mazesize main;

    main.vec_s = 0;//vectorの場所を指定する
    
    
    for (auto i = size.begin(); i != size.end(); ++i)//座標が埋まるまで繰り返す
    {

        std::cout << "size?";
        std::cin >> main.h;//入力された数字を読み込む

        while (true)
        {
            if (main.h % 2 != 0)//奇数かどうかを判定
            {
                size[main.vec_s] = main.h;//奇数の場合はvectorに保管
                main.vec_s = main.vec_s + 1;//保管場所を1つ進める
                break;//whileを抜ける
            }
            else
            {
                std::cout << "Enter an odd number";//奇数を入力するように要求
                std::cin >> main.h;
            }
        }


    }
    
    //迷路の横幅
    size_t size_x = size[0];//入力結果をxとyにそれぞれ入力
    //迷路の縦幅
    size_t size_y = size[1];
    //迷路生成

    createMaze(size_x, size_y, empty_, wall_, root_);//createMazeのクラスに変数を入力



}
