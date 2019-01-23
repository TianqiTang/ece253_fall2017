/*
 * my_container.h
 *
 *  Created on: 2017Äê12ÔÂ11ÈÕ
 *      Author: hasee
 */

#ifndef SRC_MY_CONTAINER_H_
#define SRC_MY_CONTAINER_H_
#include <initializer_list>

using idx_type = unsigned int;

template <idx_type _size, typename T>
class my_ring_buffer{
public:
	T data[_size];
	idx_type head ,tail; // 0 ... _size-1
	idx_type count;

	my_ring_buffer():head(0),tail(0),count(0){}
	my_ring_buffer(const std::initializer_list<T> &list):head(0),tail(0),count(0){
		for(auto &item : list)
			push(item);
	}

	bool full(){
		return  (tail==head+1) || (head == _size-1 && tail ==0);
	}
	bool empty(){
		return head==tail;
	}
	void push(const T &item){
		if(full())
			return ; //return '' if fifo is full means failed

		++count;
		data[head] = item;
		++ head;
		if(head == _size) head =0;
	}
	T& push_fast(){
		if(full())
			return data[0]; //avoid it by use bigger size
		++count;
		T &value = data[head];
		++ head;
		if(head == _size) head =0;
		return value;
	}


	// The pop function didn't do any 'destruction'
	T& pop(){
		if(empty())
			return data[0]; // some how it won't run
		-- count;
		T & value = data[tail];
		++tail;
		if(tail == _size) tail =0;
		return value;
	}

	void clear(){
		tail = head = 0;
		count =0;
	}

	class iter{
	public:
		my_ring_buffer &me;
		idx_type idx;

		iter(my_ring_buffer &me,idx_type idx):me(me),idx(idx){}

		T& operator *(){
			return me.data[idx];
		}
		iter& operator ++(){
			++idx;
			if(idx == _size) idx =0;
			return *this;
		}
		bool operator !=(const iter& rh){
			return idx != rh.idx;
		}

	};

	iter begin(){
		return iter(*this,tail);
	}
	iter end(){
		return iter(*this,head);
	}
};

template <idx_type _size, typename T>
void clear_inactive(my_ring_buffer<_size,T> & buffer){
	for(T& item:buffer)
	{
		if(item.active == false)
		{
			buffer.pop();
		}
		else
		{
			break;
		}
	}
}

#endif /* SRC_MY_CONTAINER_H_ */
