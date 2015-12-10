#include "aoi.h"
#include <algorithm>
#include <math.h>

//�ѵ�ͼ�ָ������������, ����߳�������ڵ�������ʵ��Ŀ��Ӱ뾶
void AOI::init(int row, int column, int distance)
{
	m_row=row;
	m_column=column;
	m_distance=distance;
	m_blocks=new MapBlock[row*column];
}

void AOI::enter(int id, int x, int y, int distance)
{
	//���ӷ�Χ���ܴ�������߳�
	if(distance > m_distance)
		return;

	Entity* pEntity=new Entity;
	pEntity->distance=distance;
	pEntity->id=id;
	pEntity->x=x;
	pEntity->y=y;

	//��¼��ʵ��
	m_entities[id]=pEntity;

	//������������������
	MapBlock* mb=getMapBlock(x, y);

	//�����¼��ʵ��
	mb->obj_set.push_back(id);

	//��ÿ����б�
	vector<int> v;
	roundEntitySet(id, v);

	//���������б�������ӽڵ��Ƿ��ܿ����Լ�������ûص�
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

	//��ÿ����б�
	vector<int> v;
	roundEntitySet(id, v);

	//���������б�������ӽڵ��Ƿ��ܿ����Լ�������ûص�
	for(int i=0; i<v.size(); i++)
	{
		int tmp_id=v[i];
		onLeave(tmp_id, id);
	}

	//ʵ����������ɾ����ʵ��
	MapBlock* mb=getMapBlock(entity->x, entity->y);
	std::vector<int>::iterator itor=find(mb->obj_set.begin(), mb->obj_set.end(), id);
	if(itor != mb->obj_set.end())
		mb->obj_set.erase(itor);

	//����ʵ��
	m_entities.erase(entity_itor);
}

void AOI::moveTo(int id, int x, int y)
{
	std::map<int, Entity*>::iterator entity_itor=m_entities.find(id);
	if(entity_itor == m_entities.end())
		return;

	Entity* entity=m_entities[id];

	//��þ�����Ŀ����б�old_set
	vector<int> old_set;
	roundEntitySet(id, old_set);

	//���������Ŀ����б�new_set
	std::vector<int> new_set;
	roundEntitySet(x, y, entity->distance, new_set);

	//delete self
	std::vector<int>::iterator itor=find(new_set.begin(), new_set.end(), id);
	if(itor != new_set.end())
		new_set.erase(itor);

	vector<int> move_set;
	//ͬʱ��old_set��new_set�ڣ������ƶ�
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

	//��old_set�ڣ�������move_set���Ҳ����������뿪
	// leave_set = old_set SUB move_set
	for (int i=0; i<old_set.size(); i++)
	{
		int tmp_id=old_set[i];
		if (find(move_set.begin(), move_set.end(), tmp_id) != move_set.end())
		{
			onLeave(tmp_id, id);
		}
	}

	//��new_set�ڣ�������move_set���Ҳ������������
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

//��ÿ����б�
void AOI::roundEntitySet(int x, int y, int distance, std::vector<int>& v)
{

	//������������������
	int row=y/m_distance;
	int column=x/m_distance;

	//���9������
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

//��ָ�������ڵ�ʵ�弯��׷�ӵ�������
void AOI::appendEntitySet(int x, int y, int distance, int index, std::vector<int>& v)
{
	if(index >= m_row*m_column)
		return;

	MapBlock* mb=&m_blocks[index];
	for(int i=0; i<mb->obj_set.size(); i++)
	{
		int tmp_id=mb->obj_set[i];

		//�����������룬�ж��Ƿ��ڿ��ӷ�Χ��
		std::map<int,Entity*>::iterator itor= m_entities.find(tmp_id);
		if(itor==m_entities.end())
			continue;

		Entity* other_entity=m_entities[tmp_id];
		//����Ϊ������ƽ��Ϊ�������Բ�Ҫ��
		int diff_x=x - other_entity->x;
		int diff_y=y - other_entity->y;
		double diff = sqrt ((double)(diff_x*diff_x + diff_y*diff_y));

		if(diff <= distance)
			v.push_back(tmp_id);
	}
}


MapBlock* AOI::getMapBlock(int x, int y)
{
	//������������������
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