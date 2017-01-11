#ifndef SKIP_H
#define SKIP_H

#include<time.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <utility>
#include <stdexcept>

namespace cs540{

template<typename Key_T, typename Mapped_T>
class Map;

template<typename Key_T, typename Mapped_T>
class Node;

template<typename Key_T, typename Mapped_T>
class SkipList;



template<typename Key_T, typename Mapped_T>
class Link {
    friend class SkipList<Key_T,Mapped_T>;
    friend class Map<Key_T,Mapped_T>;
public:
    Link<Key_T,Mapped_T>(Node<Key_T,Mapped_T> *nb):next(nb),prev(nb) {}
    Node<Key_T,Mapped_T> *next, *prev;
private:
};

template<typename Key_T,typename Mapped_T>
class Node {
    friend class SkipList<Key_T,Mapped_T>;
    friend class Map<Key_T,Mapped_T>;
    // All links initialized to point to itself.
public:
    Node<Key_T,Mapped_T>(int h) :_pair(nullptr), levels(h, Link<Key_T,Mapped_T>(this)) {}
    Node<Key_T,Mapped_T>(Key_T k,Mapped_T v, int h):levels(h,Link<Key_T,Mapped_T>(this)){
    	_pair=new std::pair<const Key_T,Mapped_T>(k,v);
    }
    ~Node(){
    	levels.clear();
    	delete _pair;
    }
    int height() const { return levels.size(); }
    std::vector< Link<Key_T,Mapped_T> > levels;
    Key_T getKey(){return this->_pair->first;}
    Mapped_T getVal(){return this->_pair->second;}
    Node<Key_T,Mapped_T> *getnext(){return this->levels[0].next;}
    Node<Key_T,Mapped_T>* getprev(){return this->levels[0].prev;}
    std::pair<const Key_T, Mapped_T> & getpair(){return *_pair;}
	std::pair<const Key_T, Mapped_T> *_pair;
};

template<typename Key_T,typename Mapped_T>
class SkipList{
    friend class Map<Key_T,Mapped_T>;
private:
    Node<Key_T,Mapped_T> *head;
    Node<Key_T,Mapped_T> *tail;
public:
    std::pair<Node<Key_T,Mapped_T>*,bool> insert(Key_T,Mapped_T);
    bool erase(Key_T);
    Node<Key_T,Mapped_T>* search(Key_T) const;
    void printSkipList();
    int max_ht(){return (head->height());}
    SkipList<Key_T,Mapped_T>();
    ~SkipList<Key_T,Mapped_T>();
    size_t size;
};

template<typename Key_T, typename Mapped_T>
SkipList<Key_T,Mapped_T>::SkipList(){
    srand(time(NULL));
    size = 0;
    head= new Node<Key_T,Mapped_T>(1);
    tail= new Node<Key_T,Mapped_T>(1);
    tail->levels[0].prev=head;
    tail->levels[0].next=head;
    head->levels[0].prev=tail;
    head->levels[0].next=tail;
}

template<typename Key_T, typename Mapped_T>
SkipList<Key_T,Mapped_T>::~SkipList(){
    Node<Key_T,Mapped_T> *temp=head;
    Node<Key_T,Mapped_T> *nxt=head;
    while(nxt!=tail){
        temp=nxt;
        nxt=temp->levels[0].next;
        //temp->levels.clear();
        delete temp;
    }
    delete tail;
}

template<typename Key_T, typename Mapped_T>
void SkipList<Key_T,Mapped_T>::printSkipList(){
    Node<Key_T,Mapped_T> *temp;
    for(int i=max_ht()-1;i>=0;i--){
        std::cout<<"Level:"<<i<<std::endl;
        temp=head->levels[i].next;
        while(temp!=tail){
            std::cout<<"("<<temp->_pair->first<<","<<temp->_pair->second<<")"<<std::endl;
            temp=temp->levels[i].next;
        }
        std::cout<<"\n";
    }
}

template<typename Key_T, typename Mapped_T>
Node<Key_T,Mapped_T>* SkipList<Key_T,Mapped_T>::search(Key_T k)const{
	//std::cout<<"\nsearch";
    Node<Key_T,Mapped_T> *temp=head->levels[head->height()-1].next;
    int h=0;

    for(h=head->height()-1;h>=0; h--){
        temp=head->levels[h].next;
        while(temp!=tail && temp->_pair->first < k){
            temp=temp->levels[h].next;
        }
        if(temp!=tail && temp->_pair->first==k)
            break;
    }
    return temp;
}

template<typename Key_T, typename Mapped_T>
std::pair<Node<Key_T,Mapped_T>*,bool> SkipList<Key_T,Mapped_T>::insert(Key_T k,Mapped_T v){
    Node<Key_T,Mapped_T> *temp=head;
    std::pair<Node<Key_T,Mapped_T>*,bool> temp_pair;
    int h=0;

    Node<Key_T,Mapped_T> *newN, *updateN;
    updateN= new Node<Key_T,Mapped_T>(max_ht());
    updateN->levels[0].next=tail;
    updateN->levels[0].prev=head;

    for(h=head->height()-1; h>=0 ; h--){
        temp=head->levels[h].next;
        while(temp!=tail && temp->_pair->first < k){
            temp=temp->levels[h].next;
        }
        updateN->levels[h].next=temp;
        updateN->levels[h].prev=temp->levels[h].prev;
    }
    if(temp!=tail && temp->_pair->first==k){
        //std::cout<<"\nDuplicate "<<k	;
        temp_pair.first=temp;
        temp_pair.second=false;
        delete updateN;
        return temp_pair;
    }
    else{
    	++size;
        int newh=1;
        while(rand()%2!=0) //coin flip
            newh++;

        newN= new Node<Key_T,Mapped_T>(k,v,newh);

        newh=newN->height();
        //if height increases
        if(newh>max_ht()){
        	while(newh!=head->height()){
        		head->levels.push_back(Link<Key_T,Mapped_T>(tail));
        		tail->levels.push_back(Link<Key_T,Mapped_T>(head));
        		head->levels[head->height()-1].next=tail;
        		head->levels[head->height()-1].prev=tail;
        		tail->levels[head->height()-1].next=head;
        		tail->levels[head->height()-1].prev=head;
        	}
            int temph=updateN->height();
            for(int i=temph;i<newh;i++){
                newN->levels[i].next=tail;
                tail->levels[i].prev=newN;
                newN->levels[i].prev=head;
                head->levels[i].next=newN;
            }
        }
        int temph=updateN->height();
        int minh=newh;
        minh=(temph< newh)? temph : newh;
        for(int i=0;i<minh;i++){
            newN->levels[i].next=updateN->levels[i].next;
            updateN->levels[i].next->levels[i].prev=newN;
            newN->levels[i].prev=updateN->levels[i].prev;
            updateN->levels[i].prev->levels[i].next=newN;

        }
        delete updateN;
        temp_pair.first=newN;
        temp_pair.second=true;
        return temp_pair;
    }
}


template<typename Key_T, typename Mapped_T>
bool SkipList<Key_T,Mapped_T>::erase(Key_T k){
    Node<Key_T,Mapped_T> *temp=search(k);
    if(temp!=tail && temp->_pair->first==k){
        int temph=temp->height();
        for(int i=0;i<temph;i++){
            temp->levels[i].prev->levels[i].next=temp->levels[i].next;
            temp->levels[i].next->levels[i].prev=temp->levels[i].prev;
        }
        //temp->levels.clear();
        delete temp;
        --size;
        return true;
    }
    else{
        return false;
    }
}

template <typename Key_T, typename Mapped_T>
class Map{

private:
    SkipList<Key_T,Mapped_T> List;

public:
    typedef std::pair<const Key_T,Mapped_T> ValueType;

    Map(){}
    Map(const Map<Key_T, Mapped_T> &m){
    	for(Map::ConstIterator it=m.begin();it!=m.end();++it){
    	    		this->insert(*it);
    	    	}
    }

    Map(std::initializer_list<std::pair<const Key_T,Mapped_T> > l){
    	typename std::initializer_list<std::pair<const Key_T,Mapped_T> >::iterator it=l.begin();
        for(;it!=l.end();it++){
            this->insert(*it);
        }
    }

    ~Map(){}
    Map &operator=(const Map<Key_T, Mapped_T> &m1){
    	for(Map::ConstIterator it=m1.begin();it!=m1.end();++it){
    		this->insert(*it);
    	}
    	return *this;
    }
    size_t size()const;
    bool empty()const;

    class Iterator{
        friend bool operator==(const Iterator &it1,const Iterator &it2){
            return (it1.node==it2.node);
        }
        friend bool operator!=(const Iterator &it1,const Iterator &it2){
            return !(it1==it2);
        }

    public:
        Iterator(Node<Key_T,Mapped_T>* n):node(n){}
        Iterator():node(0){}
        ~Iterator(){}
        Iterator &operator=(const Iterator &i){
        	//std::cout<<"\niter =";
            node=i.node;
            return *this;
        }
        Iterator &operator++(){
        	//std::cout<<"\niter ++";
            node=node->getnext();
            return *this;
        }
        Iterator operator++(int){
        	//std::cout<<"\niter ++";
            Iterator *it=this;
            node=node->getnext();
            return *it;
        }
        Iterator &operator--(){
        	//std::cout<<"\niter --";
            node=node->getprev();
            return *this;
        }

        Iterator operator--(int){
        	//std::cout<<"\niter --";
            Iterator *it= this;
            node=node->getprev();
            return *it;
        }

        ValueType &operator*()const{
            return (node->getpair());
        }

        ValueType *operator->()const{
            return &(node->getpair());
        }

        Node<Key_T,Mapped_T>* getnode(){
            return node;
        }
    //protected:
        Node<Key_T,Mapped_T>* node;
    };

    class ConstIterator: public Iterator{
        friend bool operator==(const ConstIterator &it1,const ConstIterator &it2){
            return (it1.node==it2.node);
        }
        friend bool operator!=(const ConstIterator &it1,const ConstIterator &it2){
            return !(it1==it2);
        }
    public:
        ConstIterator(Node< Key_T, Mapped_T>* n):Iterator (n){}
        ConstIterator(){}
        ~ConstIterator(){}

        ValueType &operator*()const{
            return *(Iterator::node->_pair);
        }
        ValueType *operator->()const{
            return (Iterator::node->_pair);
        }
    };


    class ReverseIterator:public Iterator{
        friend bool operator==(const ReverseIterator &it1,const ReverseIterator &it2){
            return (it1.node==it2.node);
        }
        friend bool operator!=(const ReverseIterator &it1,const ReverseIterator &it2){
            return !(it1==it2);
        }
    public:
        ReverseIterator(Node<Key_T, Mapped_T>* n):Iterator(n){}
        ReverseIterator(){}
        ~ReverseIterator(){}

        ReverseIterator &operator++(){
            Iterator::node=Iterator::node->getprev();
            return *this;
        }
        ReverseIterator operator++(int){
            ReverseIterator *it=this;
            Iterator::node=Iterator::node->getprev();
            return *it;
        }
        ReverseIterator &operator--(){
        	Iterator::node=Iterator::node->getnext();
            return *this;
        }
        ReverseIterator operator--(int){
            ReverseIterator *it= this;
            Iterator::node=Iterator::node->getnext();
            return *it;
        }
        /*ValueType &operator*()const{
            return (Iterator::node->getpair());
        }
        ValueType *operator->()const{
            return (Iterator::node->getpair());
        }*/
    };


    Iterator begin(){
    	//std::cout<<"\niterator begin";
        return Iterator(List.head->levels[0].next);
    }
    Iterator end(){
    	//std::cout<<"\niterator end";
        return Iterator(List.tail);
    }

    ConstIterator begin()const{
        return ConstIterator(List.head->getnext());
    }
    ConstIterator end()const{
        return ConstIterator(List.tail);
    }


    ReverseIterator rbegin(){
        return ReverseIterator(List.tail->getprev());
    }
    ReverseIterator rend(){
        return ReverseIterator(List.head);
    }

    Iterator find(const Key_T &k){
        Iterator it;
        it.node=List.search(k);
        if(it.node==List.tail || !(it.node->getKey()==k))
            it=this->end();
        return it;
    }

    ConstIterator find(const Key_T &k) const{
        ConstIterator it;
        it.node=List.search(k);
        if(it.node==List.tail || !(it.node->getKey()==k))
            it=this->end();
        return it;
    }

    Mapped_T &at(const Key_T &k){
    	//std::cout<<"\nat";
        Node<Key_T,Mapped_T> *n=List.search(k);
        if(n==List.tail || !((*n)._pair->first==k)){
            throw std::out_of_range("at");
        }
        return (*n)._pair->second;
    }
    const Mapped_T &at(const Key_T &k)const{
    	//std::cout<<"\nconst at";
        const Node<Key_T,Mapped_T> *n=List.search(k);
        if(n==List.tail || !((*n)._pair->first==k)){
            throw std::out_of_range("const at");
        }
        return (*n)._pair->second;
    }
    Mapped_T &operator[](const Key_T &k){
    	//std::cout<<"\noperator[]";
        Node<Key_T,Mapped_T> *n=List.search(k);
        if(n==List.tail || !(n->getKey()==k)){
        	std::pair<Iterator,bool>p=List.insert(k,Mapped_T());
        	return (*(p.first)).second;
        }
        else if(n->getKey()==k){
        	return (*n)._pair->second;
        }
        else
        	throw std::out_of_range("operator []");
    }

    std::pair<Iterator,bool>insert(const ValueType &v){
    	//std::cout<<"\nInsert "<<v.first;
        std::pair<Node<Key_T,Mapped_T>* ,bool>p=List.insert(v.first,v.second);
        return std::pair<Iterator,bool>(Iterator(p.first),p.second);
    }

    template<class IT_T>
    void insert(IT_T range_beg, IT_T range_end){
        for(;range_beg!=range_end;range_beg++){
            List.insert(range_beg->first,range_beg->second);
        }
    }

    void erase(Iterator pos){
        List.erase(pos.getnode()->getKey());
    }
    void erase(const Key_T &k){
        if(!List.erase(k))
            throw std::out_of_range("erase");
    }

    void clear(){
        Iterator it = this->begin();
        for(;it != this->end(); ++it){
            this->erase(it);
        }
        List.head->levels.clear();
        List.tail->levels.clear();
    }
};

template <typename Key_T, typename Mapped_T>
size_t Map<Key_T,Mapped_T>::size()const{
    return this->List.size;
}

template <typename Key_T, typename Mapped_T>
bool Map<Key_T,Mapped_T>::empty()const{
    return (this->List.head->height()==0);
}


template <typename Key_T, typename Mapped_T>
bool operator==(const Map<Key_T,Mapped_T> &m1,const Map<Key_T,Mapped_T> &m2){
	if(m1.size()!=m2.size()){
		return false;
	}
	typename Map<Key_T,Mapped_T>::ConstIterator it1=m1.begin();
	typename Map<Key_T,Mapped_T>::ConstIterator it2=m2.begin();
	for(;it1!=m1.end();++it1,++it2){
		if(!(*it1==*it2))
			return false;
	}
	return true;
}

template <typename Key_T, typename Mapped_T>
 bool operator!=(const Map<Key_T,Mapped_T> &m1,const Map<Key_T,Mapped_T> &m2){
    return !(m1==m2);
}
template <typename Key_T, typename Mapped_T>
bool operator<(const Map<Key_T,Mapped_T> &m1,const Map<Key_T,Mapped_T> &m2){
	typename Map<Key_T,Mapped_T>::ConstIterator it1=m1.begin();
	typename Map<Key_T,Mapped_T>::ConstIterator it2=m2.begin();
	bool isLess=false,isEqual=false;
	while(it1!=m1.end() && it2!=m2.end()){
		if(!(*it1 < *it2))
			return false;
		else if(*it1==*it2 && !isLess)
			isEqual=true;
		else if(*it1 <*it2){
			isLess= true;
			isEqual=false;
		}
		it1++;
		it2++;
	}
	if(!isEqual && isLess)
		return true;
	else if(isEqual && !isLess && m1.size()< m2.size())
		return true;
	return false;
}

}//cs540
#endif // SKIP_H
