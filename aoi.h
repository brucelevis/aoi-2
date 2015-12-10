
/************************************************************************/
/*
aoi: area of interest, �����еĿ��ӷ�Χ���� 
1������
http://www.cnblogs.com/sniperHW/archive/2012/09/29/2707953.html

2��˫����
http://www.codedump.info/?p=388


�������õ������񷨡�


http://blog.codingnow.com/2009/09/aoi_watchtower.html

��2d����ô���
�����2D��������Ϊһ����ά����������ʱ������ʹ�ã���Ϊ���Ը���һ����������������귽�����������ǵĸ��ӵ��±귶Χ��
������һ��AOI�ƶ�ʱ�����Ҹ��Ƿ�Χ�ڵĶ��������ס�����������������������ӵĴ�С��AOI�Ĵ�С

����߳�Ҫ��

��6�����������ǿ�������ֻ��ѯ����3�����ӣ����������Ļ���6���εı߳�������ڵ���2���Ĺ۲�뾶��3�����ӵ������=18 * 3^(1/2) * r^2
����������������������ӱ߳����ڵ���2���Ĺ۲�뾶����Ҫ��ѯ����4�����ӣ��������=16 * r^2
�����ӱ߳����ڹ۲�뾶������Ҫ��ѯ����9�����ӣ������=9 * r^2����һ����С�����С�������Եõ���С�Ĳ�ѯ��������ǻ����Ӵ洢�ռ��Լ���������hash�Ŀ�����δ�ػ��㡣

���������񣬲�ѯ����3�����ӣ� ����߳����ڵ���2���Ĺ۲�뾶
���������񣬲�ѯ����4�����ӣ�����߳����ڵ���2���Ĺ۲�뾶(Ϊʲôֻ��Ҫ����4�����ӣ� ��Ϊ�߳�Ϊ�۲�뾶2�����۲��������ֻ�Ḳ�ǵ��ĸ����ӣ�
����������ʱ��ֻ�Ḳ�ǽ����ܱߵ�4������)
���������񣬲�ѯ����9�����ӣ�����߳����ڵ���1���Ĺ۲�뾶(���ʵ���������м䣬���п��ܸ���9������)


���·�������9�����ӵ�����������
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
	int distance;							//ʵ����Ӱ뾶��ÿ��ʵ��ɲ���ͬ�����ǲ��ܴ�������߳�
};

//����
struct MapBlock
{
	std::vector<int> obj_set;		//�������ڵ�����ʵ��
};

class AOI
{
public:

	//�ѵ�ͼ�ָ�ɶ��ж��е�����������, ����߳�������ڵ���ʵ��Ŀ��Ӱ뾶
	void init(int row, int column, int distance);
	void enter(int id, int x, int y, int distance);
	void leave(int id);
	void moveTo(int id, int x, int y);

	//��ÿ����б�
	void roundEntitySet(int x, int y, int distance, std::vector<int>& v);
	void roundEntitySet(int id, std::vector<int>& v);
private:
	//��ָ�������ڵ�ʵ�弯��׷�ӵ�������
	void appendEntitySet(int x, int y, int distance, int index, std::vector<int>& v);
	MapBlock* getMapBlock(int x, int y);

	void onEnter(int watcher, int mover);
	void onLeave(int watcher, int mover);
	void onMove(int watcher, int mover);
private:
	int m_row;								//����
	int m_column;							//����
	int m_distance;							//����������߳�
	MapBlock* m_blocks;						//��������
	std::map<int, Entity*> m_entities;		//��ų���������ʵ��
};
