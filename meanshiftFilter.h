#ifndef _MEANSHIFTFILTER_H_
#define _MEANSHIFTFILTER_H_
#include "global.h"


#define	PROMPT

#define PROGRESS_RATE	100

#define	SWAP(d_a, d_b) temp=(d_a);(d_a)=(d_b);(d_b)=temp;

struct tree {
  float *x;
  tree  *right;
  tree  *left;
  tree  *parent;
};

   struct userWeightFunct {
   
  double			*w;
  double			halfWindow;
  int				sampleNumber;
  int				subspace;
  userWeightFunct	*next;

};

  struct ClassStateStruct {
	bool	KERNEL_DEFINED;
	bool	INPUT_DEFINED;
	bool	LATTICE_DEFINED;
	bool	OUTPUT_DEFINED;
};

  
   const double	EPSILON		= 0.015;			  const double	MU				= 0.05;		  const double	TC_DIST_FACTOR	= 0.5;		  											  const double	SQ_TC_DFACTOR	= 0.0625;	  const int		LIMIT           = 100;		  
   const int		GAUSS_NUM_ELS   = 16;		  const double	GAUSS_LIMIT     = 2.9;		  const double	GAUSS_INCREMENT = GAUSS_LIMIT*GAUSS_LIMIT/GAUSS_NUM_ELS;
											  
   const double	DELTA           = 0.00001;	  
  class MeanShift {

 public:

  /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
  /* Class Constructor and Destructor */
  /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

  MeanShift( void );    ~MeanShift( void );   
  /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
  /* Creation/Initialization of Mean Shift Kernel */
  /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

                                                                                                                                                                                        
  void  DefineKernel(kernelType*, float*, int*, int);

                                                                                                                                                                                                                                                                            
  void AddWeightFunction(double g(double), float, int, int);

                                                                        
  void ClearWeightFunctions( void );

  /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
  /* Input Data Set Declaration */
  /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

                                                                                                                                                            
  void	DefineInput(float*, int, int);

                                                                                                                                                        
  void	DefineLInput(float*, int, int, int);

 /*/\/\/\/\/\/\/\/\/\/\/\*/
 /*  Lattice Weight Map  */
 /*\/\/\/\/\/\/\/\/\/\/\/*/

                                                                                                                                                                                
  void SetLatticeWeightMap(float*);

                                                                                                
  void RemoveLatticeWeightMap(void);

  /*/\/\/\/\/\/\/\/\/\/\/\/\*/
  /* Mean Shift Operations  */
  /*\/\/\/\/\/\/\/\/\/\/\/\/*/

                                                                                                                                            
  void	msVector(double*, double*);

                                                                                                                                                                        
  void	latticeMSVector(double*, double*);

                                                                                                                                            
  void FindMode(double*, double*);

                                                                                                                                                                        
  void FindLMode(double*, double*);


                                            
  char			*ErrorMessage;

                                                                
  ErrorLevel	ErrorStatus;

 protected:

            
     /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
     /* Mean Shift: Using kd-Tree  */
     /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

               
   void MSVector      (double*, double*);                                                                           				                                               /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
     /*  Mean Shift: Using Lattice */
     /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

               
   void	LatticeMSVector     (double*, double*);			  														  
               
   void	OptLatticeMSVector     (double*, double*);        														  														  														  
    /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
	/* Kernel-Input Data Consistency  */
    /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

               
   void classConsistencyCheck(int, bool);				  														  														  
   	 /*/\/\/\/\/\/\/\/\/\/\/\*/
     /* Class Error Handler  */
	 /*\/\/\/\/\/\/\/\/\/\/\/*/

                    
   void ErrorHandler(char*, char*, char*);				  

            
               
  
               
	int				L, N, kp, *P;						  

               
	  	float			*data;								  														  														  														  														  
               
	  	int				height, width;						  
               
	float			*h;									  
	float			*offset;							                                                            
               
	unsigned char	*modeTable;							  														  														  														  														  														  
	int				*pointList;							  														  														  
	int				pointCount;							  

                         
	float			*weightMap;							  														  
	bool			weightMapDefined;					  														  
               
	ClassStateStruct	class_state;					  														  
 private:

            
	 /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
	 /* Kernel Creation/Manipulation */
	 /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

   void	generateLookupTable ( void );                    
   void	DestroyKernel       ( void );                    

	 /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
	 /* Input Data Initialization/Destruction  */
	 /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

   void	CreateBST		( void );						  
   void	InitializeInput	(float*);						  
   void	ResetInput		( void );						  														  
     /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
     /* k-dimensional Binary Search Tree */
     /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

	     tree  *BuildKDTree (tree*,  int, int, tree* );                                                                   
   void  QuickMedian (tree*, int, int, int );                                                                                                                                                                                                                                                                                                                                                                         
     /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
     /* Mean Shift: Using kd-Tree  */
     /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

   void uniformSearch (tree*, int, double*, double*);                                                                                                                                                                                     
   void generalSearch (tree*, int, double*, double*);                                                                                                                                                                                     
     /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
     /*  Mean Shift: Using Lattice */
     /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

   void	uniformLSearch	 (double *, double *);			  														  														  
   void optUniformLSearch(double *, double *);			  														  														  														  
   void generalLSearch	 (double *, double *);			  														  														  
   void optGeneralLSearch(double *, double *);			  														  														  														  

            
               
	kernelType		*kernel;							  
	double			**w;								  
	double			*increment;							  
	bool			uniformKernel;						  	
	userWeightFunct	*head, *cur;						     

               
	  	tree			*root;								  
	tree			*forest;							  
	float			*range;								  														                                                            
                    
	double			*uv;								  														  
	double			wsum;								  
               
	  	int				LowerBoundX, UpperBoundX;			  														  
	int				LowerBoundY, UpperBoundY;			  														  
};

#endif
