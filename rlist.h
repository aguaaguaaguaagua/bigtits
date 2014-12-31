#pragma once
#ifndef RLIST_H
#define RLIST_H


#include "global.h"
struct REGION {
	int			label;
	int			pointCount;
	int			region;

};

class RegionList {

public:



	RegionList(int, int, int);

		~RegionList( void );

  /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
  /*  Region List Manipulation  */
  /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

                                                                              
	void AddRegion(int, int, int*);

                                  
	void Reset( void );	

  /*/\/\/\/\/\/\/\/\/\/\*/
  /*  Query Region List */
  /*\/\/\/\/\/\/\/\/\/\/*/

                                  
	int	GetNumRegions ( void );

                                                        
	int	GetLabel(int);

                                                          
	int GetRegionCount(int);

                                                            
	int*GetRegionIndeces(int);

private:

  /*/\/\/\/\/\/\/\/\/\/\/\*/
  /*  Class Error Handler */
  /*\/\/\/\/\/\/\/\/\/\/\/*/

	void ErrorHandler(char*, char*, ErrorType);

      
			
	REGION		*regionList;				int			minRegion;

	int			maxRegions;															int			numRegions;															int			freeRegion;														
			
	int			*indexTable;														int			freeBlockLoc;			
			
		int			N;																
		int			L;																
};

#endif