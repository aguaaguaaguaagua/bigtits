#ifndef _TDEF_H_
#define _TDEF_H_

enum kernelType		{Uniform, Gaussian, UserDefined};
enum childType		{LEFT, RIGHT};
enum SpeedUpLevel	{NO_SPEEDUP, MED_SPEEDUP, HIGH_SPEEDUP};
enum ErrorLevel		{EL_OKAY, EL_ERROR, EL_HALT};
enum ErrorType		{NONFATAL, FATAL};
enum imageType      {GRAYSCALE,COLOR};
typedef  unsigned char BYTE;
typedef  unsigned int UINT;
#define min(x,y) (x<y)?x:y
#endif