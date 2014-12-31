#include "stdafx.h"
#include "RAList.h"

RAList::RAList( void )
{
		label			= -1;
	next			= NULL;

		edgeStrength	= 0;
	edgePixelCount	= 0;
}


RAList::~RAList( void )
{
	}


int RAList::Insert(RAList *entry)
{

			if(!next)
	{
				next		= entry;
		entry->next = NULL;

				return 0;
	}

	
				
		if(next->label > entry->label)
	{
				entry->next	= next;
		next		= entry;

				return 0;
	}

		exists	= 0;
	cur		= next;
	while(cur)
	{
		if(entry->label == cur->label)
		{
						exists = 1;
			break;
		}
		else if((!(cur->next))||(cur->next->label > entry->label))
		{
						entry->next	= cur->next;
			cur->next	= entry;
			break;
		}

				cur = cur->next;
	}

			return (int)(exists);

}



