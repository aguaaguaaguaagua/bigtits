#pragma once


#ifndef msImageProcessor_H
#define msImageProcessor_H


#include	"meanshiftFilter.h"
#include	"rlist.h"
#include	"RAList.h"
 #define	TOTAL_ITERATIONS	14
#define BIG_NUM				0xffffffff  
#define NODE_MULTIPLE		10

const double Xn			= 0.95050;
const double Yn			= 1.00000;
const double Zn			= 1.08870;
    const double Un_prime	= 0.19784977571475;
const double Vn_prime	= 0.46834507665248;
const double Lt			= 0.008856;

const double XYZ[3][3] = {	{  0.4125,  0.3576,  0.1804 },
							{  0.2125,  0.7154,  0.0721 },
							{  0.0193,  0.1192,  0.9502 }	};

const double RGB[3][3] = {	{  3.2405, -1.5371, -0.4985 },
							{ -0.9693,  1.8760,  0.0416 },
							{  0.0556, -0.2040,  1.0573 }	};

  typedef unsigned char byte;
  //my toy
  class countsList
  {
  public:
	  countsList():index(0),counts(0){};
	countsList(UINT _index,int _counts):index(_index),counts(_counts){};
   UINT index;
   int counts;
   bool operator > (const countsList& right)
	{
		  return counts>right.counts;
	};
   bool operator < (const countsList& right)
	{
		  return counts<right.counts;
	};
  };
  class msImageProcessor: public MeanShift {

public:

  /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
  /* Class Constructor and Destructor */
  /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

  msImageProcessor( void );           ~msImageProcessor( void );          
 /*/\/\/\/\/\/\/\/\/\/\/\/\/\*/
 /* Input Image Declaration  */
 /*\/\/\/\/\/\/\/\/\/\/\/\/\/*/

                                                                                                                                                                            
  void DefineImage(byte*,imageType, int, int);
  void DefineBgImage(byte*, imageType , int , int );


 /*/\/\/\/\/\/\*/
 /* Weight Map */
 /*\/\/\/\/\/\/*/

                                                                                                                                                                    
  void SetWeightMap(float*, float);

                                                                                                
  void RemoveWeightMap(void);
                                                                                                                                             
  void Filter(int, float, SpeedUpLevel);
                                                                                                                                                        
  void FuseRegions(float, int);
                                                                                                                                                                                                 
  void Segment(int, float, int, SpeedUpLevel);

                                                                                                                                
  void RGBtoLUV(byte*, float*);

  void RGB2LAB(const BYTE& r, const BYTE& g, const BYTE& b, float& lval, float& aval, float& bval);

                                                                                                                                
  void LUVtoRGB(float*, byte*);

  void LAB2RGB(const float& lval, const float& aval, const float& bval, BYTE& r, BYTE& g, BYTE& b);
                                                                                                                         
  void GetRawData(float*);

                                                                                                                                
  void GetResults(byte*);

                                                                                                    
  RegionList *GetBoundaries( void );

                                                                                                                                                                                                                                                                
  int GetRegions(int**, float**, int**);
  //added method
  void GetModeCounts(vector<countsList>&);
 
  int GetLabels(int* lab);  
  void SetSpeedThreshold(float);
private:


	void NonOptimizedFilter(float, float);         void NewNonOptimizedFilter(float, float);
	void OptimizedFilter1(float, float);                 void NewOptimizedFilter1(float, float);
	void OptimizedFilter2(float, float);               void NewOptimizedFilter2(float, float);

	void Connect( void );    
	void Fill(int, int);      


	void BuildRAM( void );    
	void DestroyRAM( void );    
	void TransitiveClosure( void );  
	void ComputeEdgeStrengths( void );    
  	void Prune(int);      
	void DefineBoundaries( void );      
  	bool InWindow(int, int);      
	float SqDistance(int, int);  
	void InitializeOutput( void );    
	void DestroyOutput( void );    
            
               
  	RegionList		*regionList;  
  	int				regionCount;    
  	int				neigh[8];

  	int				*indexTable;  
    float				*LUV_data;     
	float           LUV_treshold;          

               
  	float			*msRawData;    
  	int				*labels;      
	float			*modes;  
	int				*modePointCounts;    
               
  	RAList			*raList;    
  	RAList			*freeRAList;    
	RAList			*raPool;    
               
  	float			epsilon;  
  	unsigned char	*visitTable;    
               
  	float			rR2;    
   float speedThreshold;   };

#endif