#ifndef __TASK_QUEUE__
#define __TASK_QUEUE__

#include <iostream>

template <class T, int MaxSize>
class CQueue
{
public:
	inline CQueue(void)
	{
		memset(m_queue, 0, MaxSize);
		front = 0;
		rear = 0;
		count = 0;
	}

	inline CQueue(const CQueue &item)
	{
		count = item.Size();
		for (int i = 0; i < MaxSize; ++i)
		{
			m_queue[i] = item.m_queue[i];
		}
	}

	CQueue &operator=(const CQueue &item)
	{
		count = item.Size();
		for (int i = 0; i < MaxSize; ++i)
		{
			m_queue[i] = item.m_queue[i];
		}
		return *this;
	}

	CQueue &operator()(const CQueue &item)
	{
		count = item.Size();
		for (int i = 0; i < MaxSize; ++i)
		{
			m_queue[i] = item.m_queue[i];
		}
		return *this;
	}

	~CQueue(void)
	{

	}
public:
	//入队
	bool Push(const T& item)                                         
	{
		if (IsFull())
		{
			std::cout << "Queue is full" << std::endl;
			return false;
		}
		m_queue[rear] = item;
		rear = (rear + 1) % MaxSize;
		++count;
		return true;
	}

	//出队并返回元素
	T Pop(void)
	{
		/*if (IsEmpty())
		{
			std::cout << "Queue is empty" << std::endl;
			return NULL;
		}*/
		T temp = m_queue[front];
		//memset((void *)m_queue[front], 0, sizeof(m_queue[front]));
		front = (front + 1) % MaxSize;
		--count;
		return temp;
	}

	//返回队头元素
	T QueueFront(void) const
	{
		/*if (IsEmpty())
		{
			std::cout << "Queue is empty" << std::endl;
			return NULL;
		}*/
		return m_queue[front];
	}

	//清空队列
	inline void Clear(void)
	{
		memset(m_queue, 0, MaxSize);
		front = 0;
		rear = 0;
		count = 0;
	}

    //判断队列空
	inline bool IsEmpty(void) const
	{
		return count == 0;
	}

	//判断队列满
	inline bool IsFull(void) const
	{
		return count == MaxSize;
	}

	//返回队列大小
	inline int Size(void) const
	{
		return count;
	}

	//比较两个栈元素是否相等
	bool Compare(CQueue &queue)
	{
		if (count != queue.Size())
		{
			return false;
		}
		while (1)
		{
			if (IsEmpty())  break;
			if (Pop() != queue.Pop())
			{
				return false;
			}
		}
		return true;
	}

public:
	T m_queue[MaxSize];
	int front, rear, count;
};
#endif // !__TASK_QUEUE__
