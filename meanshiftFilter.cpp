
#include "stdafx.h"
#include "meanshiftFilter.h"



MeanShift::MeanShift( void )
{
	
	  	P							= NULL;
	L							= 0;
	N							= 0;
	kp							= 0;
	
	  	data						= NULL;
	
	  	root						= NULL;
	forest						= NULL;
	range						= NULL;
	
	  	height						= 0;
	width						= 0;
	
	  	h							= NULL;
	kernel						= NULL;
	w							= NULL;
	offset						= NULL;
	increment					= NULL;
	uniformKernel				= false;
	
	  	head						= cur	= NULL;
	
	  	uv							= NULL;

	  	weightMap					= NULL;

	  	weightMapDefined			= false;
	
	  	ErrorMessage				= new char [256];
	
	  	ErrorStatus					= EL_OKAY;
	
	  	class_state.INPUT_DEFINED	= false;
	class_state.KERNEL_DEFINED	= false;
	class_state.LATTICE_DEFINED	= false;
	class_state.OUTPUT_DEFINED	= false;
	
}


MeanShift::~MeanShift( void )
{
	delete [] ErrorMessage;
   if (weightMap)
   {
      delete [] weightMap;
   }

	ClearWeightFunctions();
	
	  	DestroyKernel();
	
	  	ResetInput();
	
}


void MeanShift::DefineKernel(kernelType *kernel_, float *h_, int *P_, int kp_)
{
	
	  	int i, kN;
	
	  	if(kp)
		DestroyKernel();
	
	  	if((kp = kp_) <= 0)
	{
		ErrorHandler("MeanShift", "CreateKernel", "Subspace count (kp) is zero or negative.");
		return;
	}
	
	  	if((!(P = new int [kp]))||(!(h = new float [kp]))||(!(kernel = new kernelType [kp]))||
		(!(offset = new float [kp]))||(!(increment = new double [kp])))
	{
		ErrorHandler("MeanShift", "CreateKernel", "Not enough memory available to create kernel.");
		return;
	}
	
	  	  	kN = 0;
	for(i = 0; i < kp; i++)
	{
		if((h[i] = h_[i]) <= 0)
		{
			ErrorHandler("MeanShift", "CreateKernel", "Negative or zero valued bandwidths are prohibited.");
			return;
		}
		if((P[i] = P_[i]) <= 0)
		{
			ErrorHandler("MeanShift", "CreateKernel", "Negative or zero valued subspace dimensions are prohibited.");
			return;
		}
		kernel[i] = kernel_[i];
		kN	   += P[i];
	}
	
	  	if((!(range = new float [2*kN]))||(!(uv = new double [kN])))
	{
		ErrorHandler("MeanShift", "CreateKernel", "Not enough memory available to create kernel.");
		return;
	}

	generateLookupTable();
	
	  	if(ErrorStatus == EL_ERROR)
		return;
	
	  	class_state.KERNEL_DEFINED	= true;
	
	  	return;
	
}


void MeanShift::AddWeightFunction(double g(double), float halfWindow, int sampleNumber, int subspace)
{
	
	
	int   i;
	double increment;

	cur = head;
	while((cur)&&(cur->subspace != subspace))
		cur = cur->next;
	
	  	  	if(cur)
		delete cur->w;
	else
    {
		cur       = new userWeightFunct;
		cur->next = head;
		head      = cur;
    }
	
	  	increment = halfWindow/(double)(sampleNumber);
	
	cur->w = new double [sampleNumber+1];
	for(i = 0; i <= sampleNumber; i++)
		cur->w[i] = g((double)(i*increment));
	
	  	cur->halfWindow   = halfWindow;
	cur->sampleNumber = sampleNumber;
	cur->subspace     = subspace;
	
	  	return;
	
}



void MeanShift::ClearWeightFunctions( void )
{
	
	while(head)
    {
		delete head->w;
		cur  = head;
		head = head->next;
		delete cur;
    }
	
}


void MeanShift::DefineInput(float *x, int L_, int N_)
{
	
	
	  	  	if((class_state.INPUT_DEFINED)||(class_state.LATTICE_DEFINED))
		ResetInput();
	
	  	if(!x)
	{
		ErrorHandler("MeanShift", "UploadInput", "Input data set is NULL.");
		return;
	}
	
	  	if(((L = L_) <= 0)||((N = N_) <= 0))
	{
		ErrorHandler("MeanShift", "UploadInput", "Input data set has negative or zero length or dimension.");
		return;
	}
	
	  	if(!(data = new float [L*N]))
	{
		ErrorHandler("MeanShift", "UploadInput", "Not enough memory.");
		return;
	}

	InitializeInput(x);
	
	  	if(ErrorStatus == EL_ERROR)
		return;

	CreateBST();
	
	  	class_state.INPUT_DEFINED	= true;
	class_state.LATTICE_DEFINED	= false;
	class_state.OUTPUT_DEFINED	= false;
	
	  	return;
	
}



void MeanShift::DefineLInput(float *x, int ht, int wt, int N_)
{
	
	  	  	if((class_state.INPUT_DEFINED)||(class_state.LATTICE_DEFINED))
		ResetInput();
	
	  	if(((height	= ht) <= 0)||((width	= wt) <= 0))
	{
		ErrorHandler("MeanShift", "DefineLInput", "Lattice defined using zero or negative height and/or width.");
		return;
	}
	
	  	if((N = N_) <= 0)
	{
		ErrorHandler("MeanShift", "DefineInput", "Input defined using zero or negative dimension.");
		return;
	}
	
	  	  	L		= height*width;
	
	  	  	  	InitializeInput(x);
	
	  	if(ErrorStatus == EL_ERROR)
		return;

	  	if(!(weightMap = new float [L]))
	{
		ErrorHandler("MeanShift", "InitializeInput", "Not enough memory.");
		return;
	}

	  	memset(weightMap, 0, L*(sizeof(float)));
	
	  	  	class_state.LATTICE_DEFINED	= true;
	class_state.INPUT_DEFINED	= false;
	class_state.OUTPUT_DEFINED	= false;
	
	  	return;
	
}



void MeanShift::SetLatticeWeightMap(float *wm)
{
	  	if(!wm)
	{
		ErrorHandler("MeanShift", "SetWeightMap", "Specified weight map is NULL.");
		return;
	}

	  	int i;
	for(i = 0; i < L; i++)
		weightMap[i] = wm[i];

	  	weightMapDefined	= true;

	  	return;

}



void MeanShift::RemoveLatticeWeightMap(void)
{

	  	  	if(weightMapDefined)
	{
		  		memset(weightMap, 0, L*sizeof(float));

		  		  		weightMapDefined	= false;
	}

	  	return;

}


void MeanShift::msVector(double *Mh, double *yk)
{
	
	  	if((!Mh)||(!yk))
	{
		ErrorHandler("MeanShift", "msVector", "Invalid argument(s) passed to this method.");
		return;
	}
	
	  	  	  	classConsistencyCheck(N, false);
	
	  	  	MSVector(Mh, yk);
	
	  	return;
	
}


void MeanShift::latticeMSVector(double *Mh, double *yk)
{
	
	  	if((!Mh)||(!yk))
	{
		ErrorHandler("MeanShift", "lmsVector", "Invalid argument(s) passed to this method.");
		return;
	}
	
	  	  	  	classConsistencyCheck(N+2, true);
	
	  	  	LatticeMSVector(Mh, yk);
	
	  	return;
	
}

/*******************************************************/

void MeanShift::FindMode(double *mode, double *yk)
{
	
	  	if((!mode)||(!yk))
	{
		ErrorHandler("MeanShift", "FindMode", "Invalid argument(s) passed to this method.");
		return;
	}
	
	  	  	  	classConsistencyCheck(N, false);
	
	  	double	*Mh	= new double [N];
	
	  	int i;
	for(i = 0; i < N; i++)
		mode[i] = yk[i];
	
	  	MSVector(Mh, yk);
	
	  	double mvAbs = 0;
	for(i = 0; i < N; i++)
		mvAbs	+= Mh[i]*Mh[i];
	
	  	int iterationCount = 1;
	while((mvAbs >= EPSILON)&&(iterationCount < LIMIT))
	{
		  		for(i = 0; i < N; i++)
			mode[i]	+= Mh[i];
		
		  		  		  		MSVector(Mh, mode);
		
		  		mvAbs = 0;
		for(i = 0; i < N; i++)
			mvAbs	+= Mh[i]*Mh[i];
		
		  		iterationCount++;
		
	}
	
	  	for(i = 0; i < N; i++)
		mode[i]	+= Mh[i];
	
	  	delete [] Mh;
	
	  	return;
	
}

/*******************************************************/
/*Find Lattice Mode                                    */
/*******************************************************/
/*Calculates the mode of a specified data point yk,    */
/*assuming that the data set exhists on a height x     */
/*width two dimensional lattice.                       */
/*******************************************************/
/*Pre:                                                 */
/*      - a kernel has been created                    */
/*      - a data set has been uploaded                 */
/*      - the height and width of the lattice has been */
/*        specified using method DefineLattice()       */
/*      - mode is the N dimensional mode of the N-dim- */
/*        ensional data point yk                       */
/*Post:                                                */
/*      - the mode of yk has been calculated and       */
/*        stored in mode.                              */
/*      - mode was calculated using the defined input  */
/*        lattice.                                     */
/*******************************************************/

void MeanShift::FindLMode(double *mode, double *yk)
{
	
	  	if((!mode)||(!yk))
	{
		ErrorHandler("MeanShift", "FindLMode", "Invalid argument(s) passed to this method.");
		return;
	}
	
	  	if(!height)
	{
		ErrorHandler("MeanShift", "FindLMode", "Lattice height and width is undefined.");
		return;
	}
	
	  	  	  	classConsistencyCheck(N+2, true);
	
	  	int gridN = N+2;
	
	  	double	*Mh	= new double [gridN];
	
	  	int i;
	for(i = 0; i < gridN; i++)
		mode[i] = yk[i];
	
	  	LatticeMSVector(Mh, mode);
	
	  	double mvAbs = 0;
	for(i = 0; i < gridN; i++)
		mvAbs	+= Mh[i]*Mh[i];
	
	  	int iterationCount = 1;
	while((mvAbs >= EPSILON)&&(iterationCount < LIMIT))
	{
		  		for(i = 0; i < gridN; i++)
			mode[i]	+= Mh[i];
		
		  		  		  		LatticeMSVector(Mh, mode);
		
		  		mvAbs = 0;
		for(i = 0; i < gridN; i++)
			mvAbs	+= Mh[i]*Mh[i];
		
		  		iterationCount++;
		
	}
	
	  	for(i = 0; i < gridN; i++)
		mode[i]	+= Mh[i];
	
	  	delete [] Mh;
	
	  	return;
	
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    PROTECTED METHODS    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

  /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
  /* Mean Shift: Using kd-Tree  */
  /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

/*******************************************************/
/*Mean Shift Vector                                    */
/*******************************************************/
/*Computes the mean shift vector at a window location  */
/*yk using input data set x using a custom, user defin-*/
/*ed kernel.                                           */
/*******************************************************/
/*Pre:                                                 */
/*      - input data has been uploaded into the private*/
/*        data members of the MeanShift class          */
/*      - a window center yk has been defined          */
/*      - uniformKernel indicates the which type of    */
/*        kernel to be used by this procedure: uniform */
/*        or general                                   */
/*Post:                                                */
/*      - the mean shift vector calculated at yk       */
/*        using a either a custom, user defined kernel */
/*        or a uniform kernel is returned              */
/*******************************************************/

void MeanShift::MSVector(double *Mh_ptr, double *yk_ptr)
{
	
	  	int i,j;
	
	  	for(i = 0; i < N; i++)
		Mh_ptr[i] = 0;
	
	  	  	wsum = 0;
	
	  	
	int s = 0;
	
	  	  	  	if(uniformKernel)
    {
		for(i = 0; i < kp; i++)
		{
			for(j = 0; j < P[i]; j++)
			{
				range[2*(s+j)  ] = (float)(yk_ptr[s+j] - h[i]);
				range[2*(s+j)+1] = (float)(yk_ptr[s+j] + h[i]);
			}
			s += P[i];
		}
    }
	else
    {
		for(i = 0; i < kp; i++)
		{
			for(j = 0; j < P[i]; j++)
			{
				range[2*(s+j)  ] = (float)(yk_ptr[s+j] - h[i]*float(sqrt(offset[i])));
				range[2*(s+j)+1] = (float)(yk_ptr[s+j] + h[i]*float(sqrt(offset[i])));
			}
			s += P[i];
		}
    }
	
	  	  	  	  	  	if(uniformKernel)
		uniformSearch(root, 0, Mh_ptr, yk_ptr);
	else
		generalSearch(root, 0, Mh_ptr, yk_ptr);
	
	  	for(i = 0; i < N; i++)
    {
		
		  		Mh_ptr[i] /= wsum;
		
		  		Mh_ptr[i] -= yk_ptr[i];
		
    }
	
	  	return;
	
}

  /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
  /*  Mean Shift: Using Lattice */
  /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

/*******************************************************/
/*Lattice Mean Shift Vector                            */
/*******************************************************/
/*Computes the mean shift vector at a specfied window  */
/*yk using the lattice data structure.                 */
/*******************************************************/
/*Pre:                                                 */
/*      - Mh_ptr and yh_ptr are arrays of doubles con- */
/*        aining N+2 elements                          */
/*      - Mh_ptr is the mean shift vector calculated   */
/*        at window center yk_ptr                      */
/*Post:                                                */
/*      - the mean shift vector at the window center   */
/*        pointed to by yk_ptr has been calculated and */
/*        stored in the memory location pointed to by  */
/*        Mh_ptr                                       */
/*******************************************************/

void MeanShift::LatticeMSVector(double *Mh_ptr, double *yk_ptr)
{
	
	  	register int i;
	for(i = 0; i < N+2; i++)
		Mh_ptr[i] = 0;
	
	  	wsum = 0;
	
	  	  	  	  	if(uniformKernel)
		uniformLSearch(Mh_ptr, yk_ptr);
	else
		generalLSearch(Mh_ptr, yk_ptr);
	
	  	  	  	
	if (wsum > 0)
	{
		for(i = 0; i < N+2; i++)
			Mh_ptr[i] = Mh_ptr[i]/wsum - yk_ptr[i];
	}
	else
	{
		for(i = 0; i < N+2; i++)
			Mh_ptr[i] = 0;
	}
	
	  	return;
	
}

/*******************************************************/
/*Optimized Lattice Mean Shift Vector                  */
/*******************************************************/
/*Computes the mean shift vector at a specfied window  */
/*yk using the lattice data structure. Also the points */
/*that lie within the window are stored into the basin */
/*of attraction structure used by the optimized mean   */
/*shift algorithms.									   */
/*******************************************************/
/*Pre:                                                 */
/*      - Mh_ptr and yh_ptr are arrays of doubles con- */
/*        aining N+2 elements                          */
/*      - Mh_ptr is the mean shift vector calculated   */
/*        at window center yk_ptr                      */
/*Post:                                                */
/*      - the mean shift vector at the window center   */
/*        pointed to by yk_ptr has been calculated and */
/*        stored in the memory location pointed to by  */
/*        Mh_ptr                                       */
/*      - the data points lying within h of of yk_ptr  */
/*        have been stored into the basin of attract-  */
/*        ion data structure.                          */
/*******************************************************/

void MeanShift::OptLatticeMSVector(double *Mh_ptr, double *yk_ptr)
{
	
	  	register int i;
	for(i = 0; i < N+2; i++)
		Mh_ptr[i] = 0;
	
	  	wsum = 0;
	
	  	  	  	  	if(uniformKernel)
		optUniformLSearch(Mh_ptr, yk_ptr);
	else
		optGeneralLSearch(Mh_ptr, yk_ptr);
	
	  	  	  	
   if (wsum > 0)
   {
	   for(i = 0; i < N+2; i++)
   		Mh_ptr[i] = Mh_ptr[i]/wsum - yk_ptr[i];
   } else
   {
      for (i=0; i< N+2; i++)
         Mh_ptr[i] = 0;
   }
	
	  	return;
	
}

  /*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
  /*** Kernel-Input Data Consistency  ***/
  /*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/

/*******************************************************/
/*Class Consistency Check                              */
/*******************************************************/
/*Checks the state of the class prior to the applicat- */
/*ion of mean shift.                                   */
/*******************************************************/
/*Pre:                                                 */
/*      - iN is the specified dimension of the input,  */
/*        iN = N for a general input data set, iN = N  */
/*        + 2 for a input set defined using a lattice  */
/*Post:                                                */
/*      - if the kernel has not been created, an input */
/*        has not been defined and/or the specified    */
/*        input dimension (iN) does not match that of  */
/*        the kernel a fatal error is flagged.         */
/*******************************************************/

void MeanShift::classConsistencyCheck(int iN, bool usingLattice)
{
	
	  	if(class_state.KERNEL_DEFINED == false)
	{
		ErrorHandler("MeanShift", "classConsistencyCheck", "Kernel not created.");
		return;
	}
	
	  	if((class_state.INPUT_DEFINED == false)&&(!usingLattice))
	{
		ErrorHandler("MeanShift", "classConsistencyCheck", "No input data specified.");
		return;
	}
	
	  	if((class_state.LATTICE_DEFINED == false)&&(usingLattice))
	{
		ErrorHandler("MeanShift", "classConsistencyCheck", "Latice not created.");
		return;
	}
	
	  	  	
	  	int i, kN	= 0;
	for(i = 0; i < kp; i++)
		kN	+= P[i];
	
	  	if(iN != kN)
	{
		ErrorHandler("MeanShift", "classConsitencyCheck", "Kernel dimension does not match defined input data dimension.");
		return;
	}
	
	  	return;
	
}


void MeanShift::ErrorHandler(char *className, char *methodName, char* errmsg)
{
	
	strcpy(ErrorMessage, className);
	strcat(ErrorMessage, "::");
	strcat(ErrorMessage, methodName);
	strcat(ErrorMessage, " Error: ");
	
	  	strcat(ErrorMessage, errmsg);
	
	  	ErrorStatus = EL_ERROR;
	
	
}


void MeanShift::generateLookupTable( void )
{
	
	  	int i,j;
	
	  	w = new double*[kp];
	
	  	  	
	  	uniformKernel = true;
	
	for(i = 0; i < kp; i++)
    {
		switch(kernel[i])
		{
			  			  			  			  		case Uniform:
			
			w[i] = NULL;    			
			offset[i] =    1;    			
			increment[i] =    1;    			break;
			
			  		case Gaussian:
			
			  			uniformKernel = false;
			
			  			  			
			  			w[i] = new double [GAUSS_NUM_ELS+1];
			
			for(j = 0; j <= GAUSS_NUM_ELS; j++)
				w[i][j] = exp(-j*GAUSS_INCREMENT/2);
			
			  			offset [i] = (float)(GAUSS_LIMIT*GAUSS_LIMIT);
			increment[i] = GAUSS_INCREMENT;
			
			  			break;
			
			  		case UserDefined:
			
			  			uniformKernel = false;
			
			  			  			cur = head;
			while((cur)&&(cur->subspace != (i+1)))
				cur = cur->next;
			
			  			  			if(cur == NULL)
			{
				fprintf(stderr, "\ngenerateLookupTable Fatal Error: User defined kernel for subspace %d undefined.\n\nAborting Program.\n\n", i+1);
				exit(1);
			}
			
			  			w[i] = new double [cur->sampleNumber+1];
			for(j = 0; j <= cur->sampleNumber; j++)
				w[i][j] = cur->w[j];
			
			  			offset   [i] = (float)(cur->halfWindow);
			increment[i] = cur->halfWindow/(float)(cur->sampleNumber);
			
			  			break;
			
		default:
			
			ErrorHandler("MeanShift", "generateLookupTable", "Unknown kernel type.");
			
		}
		
    }
}

void MeanShift::DestroyKernel( void )
{
	
	  	if(kernel)	delete	[] kernel;
	if     (h)	delete	[] h;
	if     (P)	delete	[] P;
	if (range)	delete	[] range;

   if (uv) delete [] uv;
   if(increment) delete [] increment;
   if (offset) delete [] offset;
   
   if (kp>0)
   {
      if (w)
      {
         int i;
         for (i=0; i<kp; i++)
            delete [] w[i];
         delete [] w;
      }
      w = NULL;
   }
   
	  	kp		= 0;
	kernel	= NULL;
	h		= NULL;
	P		= NULL;
	range	= NULL;

   increment = NULL;
   uv = NULL;
   offset = NULL;
	
	  	return;
	
}

void MeanShift::CreateBST( void )
{
	
	  	
	  	forest = new tree[L];
	
	  	  	int i;
	for(i = 0; i < L; i++)
    {
		forest[i].x      = &data[i*N];
		forest[i].right  = NULL;
		forest[i].left   = NULL;
		forest[i].parent = NULL;
    }
	
	  	  	  	
	root = BuildKDTree(forest, L, 0, NULL);
	
	  	return;
	
}



void MeanShift::InitializeInput(float *x)
{
	
	  	if(!(data = new float [L*N]))
	{
		ErrorHandler("MeanShift", "InitializeInput", "Not enough memory.");
		return;
	}
	
	  	int i;
	for(i = 0; i < L*N; i++)
		data[i]	= x[i];
	
	  	return;
	
}


void MeanShift::ResetInput( void )
{
	
	  	if(data)	delete [] data;
	if(forest)	delete [] forest;
	
	  	data	= NULL;
	forest	= NULL;
	root	= NULL;
	L		= 0;
	N		= 0;
	width	= 0;
	height	= 0;
	
	  	  	  	class_state.INPUT_DEFINED	= class_state.LATTICE_DEFINED = false;
	
}


tree *MeanShift::BuildKDTree(tree *subset, int length, int d, tree* parent)
{
	
	  	  	  	  	  	  	  	if(length == 1)
	{
		subset->parent = parent;
		return subset;
	}
	else if(length > 1)
    {
		
		  		QuickMedian(subset, 0, length-1, d);
		
		  		  		  		  		  		int median            = length/2;
		subset[median].parent = parent;
		subset[median].left   = BuildKDTree(subset           , median         , (d+1)%N, &subset[median]);
		subset[median].right  = BuildKDTree(&subset[median+1], length-median-1, (d+1)%N, &subset[median]);
		
		  		return &subset[median];
		
    }
	else
		return NULL;
	
	  	
}

void MeanShift::QuickMedian(tree *arr, int left, int right, int d)
{
	unsigned long k;
	unsigned long n;
	float* a;
	float* temp;
	n = right-left+1;
	k = n/2 + 1;
	unsigned long i, ir, j, l, mid;
	
	l = 1;
	ir = n;
	for (;;)
	{
		if (ir <= l+1)
		{
			if (ir == l+1 && arr[ir-1].x[d] < arr[l-1].x[d])
			{
				SWAP(arr[l-1].x, arr[ir-1].x)
			}
			return;
		} else
		{
			mid = (l+ir) >> 1;
			SWAP(arr[mid-1].x, arr[l+1-1].x)
				if (arr[l-1].x[d] > arr[ir-1].x[d])
				{
					SWAP(arr[l-1].x, arr[ir-1].x)
				}
				if (arr[l+1-1].x[d] > arr[ir-1].x[d])
				{
					SWAP(arr[l+1-1].x, arr[ir-1].x)
				}
				if (arr[l-1].x[d] > arr[l+1-1].x[d])
				{
					SWAP(arr[l-1].x, arr[l+1-1].x)
				}
				i = l+1;
				j = ir;
				a = arr[l+1-1].x;
				for (;;) {
					do i++; while (arr[i-1].x[d] < a[d]);
					do j--; while (arr[j-1].x[d] > a[d]);
					if (j<i) break;
					SWAP(arr[i-1].x, arr[j-1].x)
				}
				arr[l+1-1].x = arr[j-1].x;
				arr[j-1].x = a;
				if (j>=k) ir = j-1;
				if (j<=k) l = i;
		}
	}
}


void MeanShift::uniformSearch(tree *gt, int gd, double *Mh_ptr, double *yk_ptr)
{
	tree* c_t;
	int c_d;
	int i;
	int actionType;
	
	c_t = gt;
	c_d = gd;
	actionType = 0;
	
	double el, diff;
	int k, j, s;
	
	while (c_t != NULL)
	{
		switch(actionType) {
		case 0:   			if ((c_t->x[c_d] > range[2*c_d]) && ((c_t->left) != NULL))
			{
				c_t = c_t->left;
				c_d = (c_d+1)%N;
			} else
			{
				actionType = 1; 
			}
			break;
		case 1:   			
			for(i = 0; i < N; i++)
			{
				if((c_t->x[i] < range[2*i])||(c_t->x[i] > range[2*i+1]))
					break;
			}
			
			if(i == N)
			{
				
				  				
				  				  				diff = 0;
				j = 0;
				s = 0;
				while((diff < 1.0)&&(j < kp))   				{
					
					  					diff  = 0;
					for(k = 0; k < P[j]; k++)
					{
						el = (c_t->x[s+k] - yk_ptr[s+k])/h[j];
						diff += el*el;
					}
					
					s += P[j];                          					j++;
					
				}
				
				if(diff < 1.0)
				{
					wsum += 1;
					for(j = 0; j < N; j++)
						Mh_ptr[j] += c_t->x[j];
				}
				
			}
			if ((c_t->x[c_d] < range[2*c_d+1]) && ((c_t->right) != NULL))
			{
				c_t = c_t->right;
				c_d = (c_d+1)%N;
				actionType = 0;
			} else
			{
				actionType = 2;
			}
			break;
		case 2:   			c_d = (c_d+N-1)%N;
			
			if (c_t->parent == NULL)
			{
				c_t = NULL;
				break;
			}
			
			if (c_t->parent->left == c_t)
				actionType = 1;
			else
				actionType = 2;
			c_t = c_t->parent;
			break;
		}
	}
}


void MeanShift::generalSearch(tree *gt, int gd, double *Mh_ptr, double *yk_ptr)
{
	tree* c_t;
	int c_d;
	int i;
	int actionType;
	
	c_t = gt;
	c_d = gd;
	actionType = 0;
	
	double el, diff, u, tw, y0, y1;
	int k, j, s, x0, x1;
	
	while (c_t != NULL)
	{
		switch(actionType) {
		case 0:   			if ((c_t->x[c_d] > range[2*c_d]) && ((c_t->left) != NULL))
			{
				c_t = c_t->left;
				c_d = (c_d+1)%N;
			} else
			{
				actionType = 1; 
			}
			break;
		case 1:   			
			for(i = 0; i < N; i++)
			{
				if((c_t->x[i] < range[2*i])||(c_t->x[i] > range[2*i+1]))
					break;
			}
			
			if(i == N)
			{
				
				  				
				  				  				s = 0;
				for(j = 0; j < kp; j++)
				{
					
					  					diff  = 0;
					for(k = 0; k < P[j]; k++)
					{
						el = (c_t->x[s+k] - yk_ptr[s+k])/h[j];
						diff += uv[s+k] = el*el;        						if(diff >= offset[j])           							break;
					}
					
					if(diff >= offset[j])               						break;
					
					s += P[j];                          					
				}
				
				  				  				if(j == kp) j--;
				if(diff < offset[j])
				{
					
					  					tw = 1;
					
					  					  					s = 0;
					for(j = 0; j < kp; j++)
					{
						if(kernel[j])   						{
							  							u = 0;
							for(k = 0; k < P[j]; k++)
								u += uv[s+k];
							
							  							  							
							  							  							
							  							  							x0 = (int)(u/increment[j]);
							x1 = x0+1;
							
							  							y0 = w[j][x0];
							y1 = w[j][x1];
							
							  							tw *= (((double)(x1)*increment[j] - u)*y0+(u - (double)(x0)*increment[j])*y1)/(double)(x1*increment[j] - x0*increment[j]);
							
						}
						s += P[j];                                 					}
					
					  					for(j = 0; j < N; j++)
						Mh_ptr[j] += tw*c_t->x[j];
					
					  					wsum += tw;
					
				}
			}
			if ((c_t->x[c_d] < range[2*c_d+1]) && ((c_t->right) != NULL))
			{
				c_t = c_t->right;
				c_d = (c_d+1)%N;
				actionType = 0;
			} else
			{
				actionType = 2;
			}
			break;
		case 2:   			c_d = (c_d+N-1)%N;
			
			if (c_t->parent == NULL)
			{
				c_t = NULL;
				break;
			}
			
			if (c_t->parent->left == c_t)
				actionType = 1;
			else
				actionType = 2;
			c_t = c_t->parent;
			break;
      }
   }
}


void MeanShift::uniformLSearch(double *Mh_ptr, double *yk_ptr)
{
	
	  	register int	i, j, k;
	int				s, p, dataPoint, lN;
	double			diff, el, dx, dy, tx, weight;
	
	  	lN	= N + 2;
	
	  	
	  	  	  	tx = yk_ptr[0] - h[0] + DELTA + 0.99;
	if (tx < 0)
		LowerBoundX = 0;
	else
		LowerBoundX = (int) tx;
	tx = yk_ptr[1] - h[0] + DELTA + 0.99;
	if (tx < 0)
		LowerBoundY = 0;
	else
		LowerBoundY = (int) tx;
	tx = yk_ptr[0] + h[0] - DELTA;
	if (tx >= width)
		UpperBoundX = width-1;
	else
		UpperBoundX = (int) tx;
	tx = yk_ptr[1] + h[0] - DELTA;
	if (tx >= height)
		UpperBoundY = height - 1;
	else
		UpperBoundY = (int) tx;
	
	  	for(i = LowerBoundY; i <= UpperBoundY; i++)
		for(j = LowerBoundX; j <= UpperBoundX; j++)
		{
			
			  			dataPoint = N*(i*width+j);
			
			  			k		= 1;
			s		= 0;
			dx      = j - yk_ptr[0];
			dy      = i - yk_ptr[1];
			diff	= (dx*dx+dy*dy)/(h[0]*h[0]);
			while((diff < 1.0)&&(k != kp))   			{
				  				diff = 0;
				for(p = 0; p < P[k]; p++)
				{
					el    = (data[dataPoint+p+s]-yk_ptr[p+s+2])/h[k];               
					if((!p)&&(yk_ptr[2] > 80))
						diff += 4*el*el;
					else
						diff += el*el;
				}
				
				  				s += P[k];
				k++;
			}
			
			  			if(diff < 1.0)
			{
				weight = 1 - weightMap[i*width+j];
				Mh_ptr[0] += weight*j;
				Mh_ptr[1] += weight*i;
				for(k = 2; k < lN; k++)
					Mh_ptr[k] += weight*data[dataPoint+k-2];
				wsum += weight;
			}
			  		}
		  		return;
		
}


void MeanShift::optUniformLSearch(double *Mh_ptr, double *yk_ptr)
{
	
	  	register int	i, j, k;
	int				s, p, dataPoint, pointIndx, lN;
	double			diff, el, dx, dy, tx, weight;
	
	  	lN	= N + 2;
	
	  	
	  	  	  	tx = yk_ptr[0] - h[0] + DELTA + 0.99;
	if (tx < 0)
		LowerBoundX = 0;
	else
		LowerBoundX = (int) tx;
	tx = yk_ptr[1] - h[0] + DELTA + 0.99;
	if (tx < 0)
		LowerBoundY = 0;
	else
		LowerBoundY = (int) tx;
	tx = yk_ptr[0] + h[0] - DELTA;
	if (tx >= width)
		UpperBoundX = width-1;
	else
		UpperBoundX = (int) tx;
	tx = yk_ptr[1] + h[0] - DELTA;
	if (tx >= height)
		UpperBoundY = height - 1;
	else
		UpperBoundY = (int) tx;
	
	  	for(i = LowerBoundY; i <= UpperBoundY; i++)
		for(j = LowerBoundX; j <= UpperBoundX; j++)
		{
			
			  			pointIndx	= i*width+j;
			dataPoint	= N*(pointIndx);
			
			  			k		= 1;
			s		= 0;
			dx      = j - yk_ptr[0];
			dy      = i - yk_ptr[1];
			diff	= (dx*dx+dy*dy)/(h[0]*h[0]);
			while((diff < 1.0)&&(k != kp))   			{
				  				diff = 0;
				for(p = 0; p < P[k]; p++)
				{
					el    = (data[dataPoint+p+s]-yk_ptr[p+s+2])/h[k];
					if((!p)&&(yk_ptr[2] > 80))
						diff += 4*el*el;
					else               
					   diff += el*el;
				}
				
				  				s += P[k];
				k++;
			}
			
			  			if(diff < 1.0)
			{
				weight = 1 - weightMap[i*width+j];
				Mh_ptr[0] += weight*j;
				Mh_ptr[1] += weight*i;
				for(k = 2; k < lN; k++)
					Mh_ptr[k] += weight*data[dataPoint+k-2];
				wsum += weight;
				
				              if (diff < 0.5)
            {
				   if(modeTable[pointIndx] == 0)
				   {
   					pointList[pointCount++]	= pointIndx;
					   modeTable[pointIndx]	= 2;
				   }
            }
				
			}
			
			  			
		}
		
		  		return;
		
}



void MeanShift::generalLSearch(double *Mh_ptr, double *yk_ptr)
{
	
	  	register int i, j, k;
	int			 s, p, dataPoint, lN, x0, x1;
	double		 diff, el, dx, dy, tw, u, y0, y1, tx;
	
	  	lN	= N + 2;
	
	  	
	  	  	  	tx = yk_ptr[0] - h[0] + DELTA + 0.99;
	if (tx < 0)
		LowerBoundX = 0;
	else
		LowerBoundX = (int) tx;
	tx = yk_ptr[1] - h[0] + DELTA + 0.99;
	if (tx < 0)
		LowerBoundY = 0;
	else
		LowerBoundY = (int) tx;
	tx = yk_ptr[0] + h[0] - DELTA;
	if (tx >= width)
		UpperBoundX = width-1;
	else
		UpperBoundX = (int) tx;
	tx = yk_ptr[1] + h[0] - DELTA;
	if (tx >= height)
		UpperBoundY = height - 1;
	else
		UpperBoundY = (int) tx;
	
	  	for(i = LowerBoundY; i <= UpperBoundY; i++)
		for(j = LowerBoundX; j <= UpperBoundX; j++)
		{
			
			  			dataPoint = N*(i*width+j);
			
			  			k		= 1;
			s		= 0;
			dx      = j - yk_ptr[0];
			dy      = i - yk_ptr[1];
			uv[0]	= (dx*dx)/(h[0]*h[0]);
			uv[1]	= (dy*dy)/(h[0]*h[0]);
			diff	= uv[0] + uv[1];
			while((diff < offset[k-1])&&(k != kp))   			{
				  				diff = 0;
				for(p = 0; p < P[k]; p++)
				{
					el    = (data[dataPoint+p+s]-yk_ptr[p+s+2])/h[k];
					diff += uv[p+s+2] = el*el;
				}
				
				  				s += P[k];
				k++;
			}
			
			  			if(diff < offset[k-1])
			{
				
				  				tw = 1;
				
				  				  				s = 0;
				for(k = 0; k < kp; k++)
				{
					if(kernel[k])   					{
						  						u = 0;
						for(p = 0; p < P[k]; p++)
							u += uv[s+p];
						
						  						  						
						  						  						
						  						  						x0 = (int)(u/increment[k]);
						x1 = x0+1;
						
						  						y0 = w[k][x0];
						y1 = w[k][x1];
						
						  						tw *= (((double)(x1)*increment[k] - u)*y0+(u - (double)(x0)*increment[k])*y1)/(double)(x1*increment[k] - x0*increment[k]);
						
					}
					s += P[k];                                 				}
				
				  				Mh_ptr[0]	+= tw*j;
				Mh_ptr[1]	+= tw*i;
				for(k = 0; k < N; k++)
					Mh_ptr[k+2] += tw*data[dataPoint+k];
				
				  				wsum += tw;
				
			}
			
			  			
		}
		
		  		return;
		
}


void MeanShift::optGeneralLSearch(double *Mh_ptr, double *yk_ptr)
{
	
	  	register int	i, j, k;
	int				s, p, dataPoint, pointIndx, lN, x0, x1;
	double			diff, el, dx, dy, tw, u, y0, y1, tx;
	
	  	lN	= N + 2;
	
	  	
	  	  	  	tx = yk_ptr[0] - h[0] + DELTA + 0.99;
	if (tx < 0)
		LowerBoundX = 0;
	else
		LowerBoundX = (int) tx;
	tx = yk_ptr[1] - h[0] + DELTA + 0.99;
	if (tx < 0)
		LowerBoundY = 0;
	else
		LowerBoundY = (int) tx;
	tx = yk_ptr[0] + h[0] - DELTA;
	if (tx >= width)
		UpperBoundX = width-1;
	else
		UpperBoundX = (int) tx;
	tx = yk_ptr[1] + h[0] - DELTA;
	if (tx >= height)
		UpperBoundY = height - 1;
	else
		UpperBoundY = (int) tx;
	
	  	for(i = LowerBoundY; i <= UpperBoundY; i++)
		for(j = LowerBoundX; j <= UpperBoundX; j++)
		{
			
			  			pointIndx	= i*width+j;
			dataPoint = N*(i*width+j);
			
			  			k		= 1;
			s		= 0;
			dx      = j - yk_ptr[0];
			dy      = i - yk_ptr[1];
			uv[0]	= (dx*dx)/(h[0]*h[0]);
			uv[1]	= (dy*dy)/(h[0]*h[0]);
			diff	= uv[0] + uv[1];
			while((diff < offset[k-1])&&(k != kp))   			{
				  				diff = 0;
				for(p = 0; p < P[k]; p++)
				{
					el    = (data[dataPoint+p+s]-yk_ptr[p+s+2])/h[k];
					diff += uv[p+s+2] = el*el;
				}
				
				  				s += P[k];
				k++;
			}
			
			  			if(diff < offset[k-1])
			{
				
				  				tw = 1;
				
				  				  				s = 0;
				for(k = 0; k < kp; k++)
				{
					if(kernel[k])   					{
						  						u = 0;
						for(p = 0; p < P[k]; p++)
							u += uv[s+p];
						
						  						  						
						  						  						
						  						  						x0 = (int)(u/increment[k]);
						x1 = x0+1;
						
						  						y0 = w[k][x0];
						y1 = w[k][x1];
						
						  						tw *= (((double)(x1)*increment[k] - u)*y0+(u - (double)(x0)*increment[k])*y1)/(double)(x1*increment[k] - x0*increment[k]);
						
					}
					s += P[k];                                 				}
				
				  				Mh_ptr[0]	+= tw*j;
				Mh_ptr[1]	+= tw*i;
				for(k = 0; k < N; k++)
					Mh_ptr[k+2] += tw*data[dataPoint+k];
				
				  				wsum += tw;
				
				  				if(modeTable[pointIndx] == 0)
				{
					pointList[pointCount++]	= pointIndx;
					modeTable[pointIndx]	= 2;
				}
				
			}
			
			  			
		}
		
		  		return;
		
}
