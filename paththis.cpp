#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <tuple>
#include <cmath>

using namespace std;

#define MINE 1
#define OPP 0
#define NONE -1
#define MIN_REC 5

typedef pair<int, int>          coord;
typedef set<pair<int, coord> >  army;

coord   &operator+=(coord &lhs, const coord &rhs)
{
	lhs.first += rhs.first;
	lhs.second += rhs.second;
	return lhs;
}

coord   operator+(const coord &lhs, const coord &rhs)
{
	coord   newCoord(lhs);
	return newCoord += rhs;
}

coord   &operator-=(coord &lhs, const coord &rhs)
{
	lhs.first -= rhs.first;
	lhs.second -= rhs.second;
	return lhs;
}

coord   operator-(const coord &lhs, const coord &rhs)
{
	coord   newCoord(lhs);
	return newCoord -= rhs;
}

int ManhattanNorm(const coord &a, const coord &b)
{
	return (abs(a.first - b.first) + abs(a.second - b.second));
}

void    spawn(int nb, int x, int y)
{
	std::cout << "SPAWN " << nb << ' ' << x << ' ' << y << ';';
}

void    spawn(int nb, coord c)
{
	spawn(nb, c.first, c.second);
}

void    move(int nb, int xFrom, int yFrom, int xTo, int yTo)
{
	std::cout << "MOVE " << nb << ' ' << xFrom << ' ' << yFrom << ' ' << xTo << ' ' << yTo << ';';
}

void    move(int nb, coord cFrom, coord cTo)
{
	move(nb, cFrom.first, cFrom.second, cTo.first, cTo.second);
}

void    build(int x, int y)
{
	std::cout << "BUILD " << x << ' ' << y << ';';
}

void    build(coord c)
{
	build(c.first, c.second);
}

ostream &operator<<(ostream &o, const coord &c)
{
	o << c.first << "|" << c.second;
	return o;
}

ostream &operator<<(ostream &o, const pair<int, coord> &p)
{
	o << p.first << "->" << p.second;
	return o;
}

ostream &operator<<(ostream &o, const army &a)
{
	for (army::const_iterator it = a.begin(); it != a.end(); ++it)
		o << *it << '\n';
	return o;
}

class Node
{

	Node() : _value(0), _cost(0), _parent(0)
	{}

public :

	std::pair<int, int>	_pos;
	int					_value;
	int 				_cost;
	Node				*_parent;

	Node(coord pos, Node *parent, coord goal) : _pos(pos), _parent(parent)
	{
		if (parent)
			_cost = parent->_cost + 1;
		else
			_cost = 0;
		_value = _cost + ManhattanNorm(_pos, goal);
	}
	Node(coord pos, Node *parent, int heuristicValue) : _pos(pos), _parent(parent)
	{
		if (parent)
			_cost = parent->_cost + 1;
		else
			_cost = 0;
		_value = _cost + heuristicValue;
	}
	Node(const Node &other) : _pos(other._pos), _value(other._value), _cost(other._cost), _parent(other._parent)
	{}
	~Node() {}
	Node &operator=(const Node &other)
	{
		_pos = other._pos;
		_value = other._value;
		_cost = other._cost;
		_parent = other._parent;
		return (*this);
	}
};

typedef std::multimap<int, Node*>				openType;
typedef std::map<std::pair<int, int>, Node*>	closeType;


class AStar
{
	//Coplien not to use
	AStar() {}
	AStar(const AStar &other) {}
	AStar &operator&(const AStar &other) {return *this;}
	//End of Coplien not to use

	//Member's data
	openType								_open;
	closeType 								_close;
	std::set<coord>							_secureFreeTool;
	std::set<coord>                         _zone;
	coord									_start;
	coord									_end;
	bool									_endReached;
	Node									*_current;

	//Private tool functions

	void _checkSide(const coord &move)
	{
		coord	newCoord = _current->_pos + move;
		set<coord>::const_iterator  zoneIt = _zone.find(newCoord);
		int newVal = ManhattanNorm(_end, newCoord);
		if (zoneIt != _zone.end()) // check if I'm out of border + if the height is too big
		{
			//	std::cout << newVal << '\n';
			closeType::iterator	found = _close.find(_current->_pos);
			if (found != _close.end())
			{
				if (found->second->_value > newVal + 1 + _current->_cost)
				{
					Node	*newNode = new Node(newCoord, _current, newVal);
					_open.insert(std::make_pair(newNode->_value, newNode));
					_close.erase(found);
				}
			}
			else
			{
				Node	*newNode = new Node(_current->_pos + move, _current, newVal);
				_open.insert(std::make_pair(newNode->_value, newNode));
			}
		}
	}

public :

	AStar(const set<coord> &zone, const coord &start, const coord &end) : _start(start), _end(end), _zone(zone),
																		  _endReached(false)
	{
		Node *newNode = new Node(_start, NULL, end);
		_open.insert(std::make_pair(newNode->_value, newNode));
		_current = _open.begin()->second;
		while (!_open.empty() && !_endReached)
		{
			_checkSide(std::make_pair(-1, 0));	//LEFT
			_checkSide(std::make_pair(0, -1));	//UP
			_checkSide(std::make_pair(0, 1));	//DOWN
			_checkSide(std::make_pair(1, 0));	//RIGHT
			std::pair<closeType::iterator, bool> res = _close.insert(std::make_pair(_open.begin()->second->_pos, _open.begin()->second));
			/*if (!res.second)
			{
				std::cerr << "error\n";
			}*/
			_open.erase(_open.begin());
			if (!ManhattanNorm(_current->_pos, end))
				_endReached = true;
			if (!_open.empty() && !_endReached)
				_current = _open.begin()->second;
		}
	}

	~AStar ()
	{
		for (openType::iterator it = _open.begin(); it != _open.end(); ++it)
		{
			_secureFreeTool.insert(it->second->_pos);
			delete (it->second);
		}
		for (closeType::iterator it = _close.begin(); it != _close.end(); ++it)
		{
			if (_secureFreeTool.find(it->first) == _secureFreeTool.end())
				delete (it->second);
		}
	}

	pair<int, coord> stepCount()	const
	{
		int 	stepCount = 0;
		Node	*idx = _current;
		Node    *nextMove;
		if (!_endReached)
			return make_pair(-1, coord(0, 0));
		while (idx->_parent)
		{
			nextMove = idx;
			idx = idx->_parent;
			++stepCount;
		}
		return make_pair(stepCount, nextMove->_pos);
	}

};

void parseLine(const std::string &s, coord &c)
{
	int idx = s.find(',') + 1;
	c.first = atoi(&s[0]);
	c.second = atoi(&s[idx]);
}

int main(int ac, char **av)
{
	if (ac != 2)	return 1;
	ifstream input(av[1]);
	if (input.fail()) return 2;

	std::string line;
	std::set<coord>	coords;
	coord	pouet;
	while (!input.eof())
	{
		getline(input, line);
		parseLine(line, pouet);
		coords.insert(pouet);
	}
	AStar	test(coords, coord(0, 0), coord(5, 5));
	std::cout << test.stepCount().first << ' ' << test.stepCount().second << '\n';
	std::cout << test.stepCount().first << ' ' << test.stepCount().second << '\n';
	return 0;
}