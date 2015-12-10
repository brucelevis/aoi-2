#include "aoi.h"
#include <algorithm>
#include <math.h>

//把地图分割成正方形网格, 网格边长必须大于等于所有实体的可视半径
void AOI::init(int row, int column, int distance)
{
	m_row=row;
	m_column=column;
	m_distance=distance;
	m_blocks=new MapBlock[row*column];
}

void AOI::enter(int id, int x, int y, int distance)
{
	//可视范围不能大于网格边长
	if(distance > m_distance)
		return;

	Entity* pEntity=new Entity;
	pEntity->distance=distance;
	pEntity->id=id;
	pEntity->x=x;
	pEntity->y=y;

	//记录该实体
	m_entities[id]=pEntity;

	//根据坐标获得所在网格
	MapBlock* mb=getMapBlock(x, y);

	//网格记录新实体
	mb->obj_set.push_back(id);

	//获得可视列表
	vector<int> v;
	roundEntitySet(id, v);

	//遍历可视列表，如果可视节点是否能看到自己，则调用回调
	for(int i=0; i<v.size(); i++)
	{
		int tmp_id=v[i];
		onEnter(tmp_id, id);
		onEnter(id, tmp_id);
	}
}

void AOI::leave(int id)
{
	std::map<int, Entity*>::iterator entity_itor=m_entities.find(id);
	if(entity_itor == m_entities.end())
		return;

	Entity* entity=m_entities[id];

	//获得可视列表
	vector<int> v;
	roundEntitySet(id, v);

	//遍历可视列表，如果可视节点是否能看到自己，则调用回调
	for(int i=0; i<v.size(); i++)
	{
		int tmp_id=v[i];
		onLeave(tmp_id, id);
	}

	//实体所在网格删除该实体
	MapBlock* mb=getMapBlock(entity->x, entity->y);
	std::vector<int>::iterator itor=find(mb->obj_set.begin(), mb->obj_set.end(), id);
	if(itor != mb->obj_set.end())
		mb->obj_set.erase(itor);

	//销毁实体
	m_entities.erase(entity_itor);
}

void AOI::moveTo(int id, int x, int y)
{
	std::map<int, Entity*>::iterator entity_itor=m_entities.find(id);
	if(entity_itor == m_entities.end())
		return;

	Entity* entity=m_entities[id];

	//获得旧坐标的可视列表old_set
	vector<int> old_set;
	roundEntitySet(id, old_set);

	//获得新坐标的可视列表new_set
	std::vector<int> new_set;
	roundEntitySet(x, y, entity->distance, new_set);

	//delete self
	std::vector<int>::iterator itor=find(new_set.begin(), new_set.end(), id);
	if(itor != new_set.end())
		new_set.erase(itor);

	vector<int> move_set;
	//同时在old_set和new_set内，则算移动
	// move_set = old_set MIX new_set
	for (int i=0; i<old_set.size(); i++)
	{
		int tmp_id=old_set[i];
		if (find(new_set.begin(), new_set.end(), tmp_id) != new_set.end())
		{
			onMove(tmp_id, id);
			move_set.push_back(tmp_id);
		}
	}

	//在old_set内，但是在move_set内找不到，则算离开
	// leave_set = old_set SUB move_set
	for (int i=0; i<old_set.size(); i++)
	{
		int tmp_id=old_set[i];
		if (find(move_set.begin(), move_set.end(), tmp_id) != move_set.end())
		{
			onLeave(tmp_id, id);
		}
	}

	//在new_set内，但是在move_set内找不到，则算进入
	// enter_set = new_set SUB move_set
	for (int i=0; i<new_set.size(); i++)
	{
		int tmp_id=new_set[i];
		if (find(move_set.begin(), move_set.end(), tmp_id) != move_set.end())
		{
			onEnter(tmp_id, id);
			onEnter(id, tmp_id);
		}
	}
}

//获得可视列表
void AOI::roundEntitySet(int x, int y, int distance, std::vector<int>& v)
{

	//根据坐标获得所在网格
	int row=y/m_distance;
	int column=x/m_distance;

	//获得9个网格
	int index=row*m_column + column;
	appendEntitySet(x, y, distance, index, v);

	if(column<m_column)
	{
		index=row*m_column+column+1;
		appendEntitySet(x, y, distance, index, v);
	}

	if(column>=1)
	{
		index=row*m_column+column-1;
		appendEntitySet(x, y, distance, index, v);
	}

	if(row>=1)
	{
		index=(row-1)*m_column + column;
		appendEntitySet(x, y, distance, index, v);

		if(column<m_column)
		{
			index=(row-1)*m_column+column+1;
			appendEntitySet(x, y, distance, index, v);
		}

		if(column>=1)
		{
			index=(row-1)*m_column+column-1;
			appendEntitySet(x, y, distance, index, v);
		}
	}

	if(row<m_row)
	{
		index=(row+1)*m_column + column;
		appendEntitySet(x, y, distance, index, v);

		if(column<m_column)
		{
			index=(row+1)*m_column+column+1;
			appendEntitySet(x, y, distance, index, v);
		}

		if(column>=1)
		{
			index=(row+1)*m_column+column-1;
			appendEntitySet(x, y, distance, index, v);
		}
	}
}

void AOI::roundEntitySet(int id, std::vector<int>& v)
{
	Entity* entity=m_entities[id];
	roundEntitySet(entity->x, entity->y, entity->distance, v);

	//delete self
	std::vector<int>::iterator itor=find(v.begin(), v.end(), id);
	if(itor != v.end())
		v.erase(itor);
}

//把指定网格内的实体集合追加到集合内
void AOI::appendEntitySet(int x, int y, int distance, int index, std::vector<int>& v)
{
	if(index >= m_row*m_column)
		return;

	MapBlock* mb=&m_blocks[index];
	for(int i=0; i<mb->obj_set.size(); i++)
	{
		int tmp_id=mb->obj_set[i];

		//计算两点间距离，判断是否在可视范围内
		std::map<int,Entity*>::iterator itor= m_entities.find(tmp_id);
		if(itor==m_entities.end())
			continue;

		Entity* other_entity=m_entities[tmp_id];
		//可能为负数，平方为正，所以不要紧
		int diff_x=x - other_entity->x;
		int diff_y=y - other_entity->y;
		double diff = sqrt ((double)(diff_x*diff_x + diff_y*diff_y));

		if(diff <= distance)
			v.push_back(tmp_id);
	}
}


MapBlock* AOI::getMapBlock(int x, int y)
{
	//根据坐标获得所在网格
	int column=x/m_distance;
	int row=y/m_distance;
	int index=row*m_column + column;
	if(index >= m_row*m_column)
		return NULL;

	MapBlock* mb=&m_blocks[index];
	return mb;
}

void AOI::onEnter(int watcher, int mover)
{
	Entity* watch_entity=m_entities[watcher];
	Entity* mover_entity=m_entities[mover];

	printf("send [%d, %d:%d]\t ENTER msg to obj [%d, %d:%d:%d]\n",
		mover_entity->id, mover_entity->x, mover_entity->y,
		watch_entity->id, watch_entity->x, watch_entity->y, watch_entity->distance);
}

void AOI::onLeave(int watcher, int mover)
{
	Entity* watch_entity=m_entities[watcher];
	Entity* mover_entity=m_entities[mover];

	printf("send [%d, %d:%d]\t leave msg to obj [%d, %d:%d:%d]\n",
		mover_entity->id, mover_entity->x, mover_entity->y,
		watch_entity->id, watch_entity->x, watch_entity->y, watch_entity->distance);
}

void AOI::onMove(int watcher, int mover)
{

	Entity* watch_entity=m_entities[watcher];
	Entity* mover_entity=m_entities[mover];

	printf("send [%d, %d:%d]\t move msg to obj [%d, %d:%d:%d]\n",
		mover_entity->id, mover_entity->x, mover_entity->y,
		watch_entity->id, watch_entity->x, watch_entity->y, watch_entity->distance);
}