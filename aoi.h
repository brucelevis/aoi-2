
/************************************************************************/
/*
aoi: area of interest, 场景中的可视范围管理。 
1，网格法
http://www.cnblogs.com/sniperHW/archive/2012/09/29/2707953.html

2，双链表法
http://www.codedump.info/?p=388


本例子用的是网格法。


http://blog.codingnow.com/2009/09/aoi_watchtower.html

用2d网格好处：
如果是2D的网格，作为一个二维数组来看待时很容易使用，因为可以根据一个矩形两个点的坐标方便地算出所覆盖的格子的下标范围。
这样当一个AOI移动时，查找覆盖范围内的对象会很容易。而且这样可以任意调整格子的大小和AOI的大小

网格边长要求：

用6边形型网格是可以做到只查询相邻3个格子，不过那样的话，6边形的边长必须大于等于2倍的观察半径，3个格子的总面积=18 * 3^(1/2) * r^2
如果采用正方形网格，若格子边长大于等于2倍的观察半径，需要查询相邻4个格子，则总面积=16 * r^2
若格子边长等于观察半径，则需要查询相邻9个格子，总面积=9 * r^2。进一部缩小网格大小，还可以得到更小的查询面积，但是会增加存储空间以及对象重新hash的开销，未必划算。

六边形网格，查询相邻3个格子， 必须边长大于等于2倍的观察半径
正方形网格，查询相邻4个格子，必须边长大于等于2倍的观察半径(为什么只需要相邻4个格子， 因为边长为观察半径2倍，观察区域最大只会覆盖到四个格子，
在网格角落的时候，只会覆盖角落周边的4个格子)
正方形网格，查询相邻9个格子，必须边长大于等于1倍的观察半径(如果实体在网格中间，则有可能覆盖9个格子)


以下方法是用9个格子的正方形网格
*/
/************************************************************************/
#include <vector>
#include <map>
using namespace std;

struct Entity
{
	int id;
	int x;
	int y;
	int distance;							//实体可视半径，每个实体可不相同，但是不能大于网格边长
};

//网格
struct MapBlock
{
	std::vector<int> obj_set;		//该网格内的所有实体
};

class AOI
{
public:

	//把地图分割成多行多列的正方形网格, 网格边长必须大于等于实体的可视半径
	void init(int row, int column, int distance);
	void enter(int id, int x, int y, int distance);
	void leave(int id);
	void moveTo(int id, int x, int y);

	//获得可视列表
	void roundEntitySet(int x, int y, int distance, std::vector<int>& v);
	void roundEntitySet(int id, std::vector<int>& v);
private:
	//把指定网格内的实体集合追加到集合内
	void appendEntitySet(int x, int y, int distance, int index, std::vector<int>& v);
	MapBlock* getMapBlock(int x, int y);

	void onEnter(int watcher, int mover);
	void onLeave(int watcher, int mover);
	void onMove(int watcher, int mover);
private:
	int m_row;								//行数
	int m_column;							//列数
	int m_distance;							//正方形网格边长
	MapBlock* m_blocks;						//所有网格
	std::map<int, Entity*> m_entities;		//存放场景内所有实体
};
