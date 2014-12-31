
#ifndef RALIST_H
#define RALIST_H


#include "global.h"
class RAList {
public:
	int		label;
	float	edgeStrength;
	int		edgePixelCount;
	RAList	*next;
	RAList( void );
	~RAList( void );
	int Insert(RAList*);
private:
	RAList	*cur, *prev;
	unsigned char exists;
};
#endif