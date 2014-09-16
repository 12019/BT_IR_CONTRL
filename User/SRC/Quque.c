#include "myconfig.h"

/*******************************************************************************
* Function Name  : Quque_Configuration
* Description    : Configures the Quque.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitQue(Quque *q)
{
	u16 i;
	q->head = 0;
	q->tail = 0;

	for(i=0;i<MAXOFRXQ;i++)
	{
		q->elem[i] = 0x00;
	}
}
void ResetQue(Quque *q)
{
	q->head = 0;
	q->tail = 0;
}

u8 IsFull(Quque *q)
{
	if(q->head==(q->tail+1)%MAXOFRXQ)  
		return QUEFULL;
	else
		return 0;	
}
	
	
u8 InsertQue(Quque *q,u8 elem)
{
	if(IsFull(q) == QUEFULL) 
		return QUEFULL;
	q->elem[q->tail]=elem;
	q->tail=(q->tail+1)%MAXOFRXQ; 
		return QUESUC;
}
	
u8 IsEmpty(Quque *q)
{
	if(q->head==q->tail)
		return QUEEMP;
	else 
		return QUESUC;
}

u8 OutQueOneByte(Quque *q,u8 *pelem)
{
	if(IsEmpty(q) == QUEEMP)
	{
		return QUEEMP;		
	}
	pelem[0] = q->elem[q->head];
	q->head =(q->head+1)%MAXOFRXQ;
	return QUESUC;
}
	
u8 OutQue(Quque *q,u8 *pelem,u16 len)
{
	u16 i;
	if(IsEmpty(q) == QUEEMP)
	{
		return QUEEMP;		
	}
	if(q->head < q->tail)
	{
		if((q->tail - q->head)<len)
			return QUENOEGH;
		else
		{
			for(i=0;i<len;i++)
			{
				pelem[i] = q->elem[q->head + i];
			}
			q->head =(q->head+len)%MAXOFRXQ;
		}
	}
	else if(q->head > q->tail)
	{
		if((MAXOFRXQ - q->head + q->tail )<len)
			return QUENOEGH;
		else if((MAXOFRXQ - q->head)>len)
		{			
			for(i=0;i<len;i++)
			{
				pelem[i] = q->elem[q->head + i];
			}
			q->head =(q->head+len)%MAXOFRXQ;
		}
		else 
			{
				for(i=0;i<(MAXOFRXQ-q->head);i++)
				{
					pelem[i] = q->elem[q->head + i];
				}
				for(i=(MAXOFRXQ-q->head);i<len;i++ )
				{
					pelem[i] = q->elem[(q->head + i)%MAXOFRXQ];
				}
				q->head =(q->head+len)%MAXOFRXQ;
			}
	}
	return QUESUC;
}

u8 AllOutQue(Quque *q,u8 *pelem)
{
	u16 i;
	if(IsEmpty(q) == QUEEMP)
		return QUEEMP;
	if(q->head > q->tail)
	{
		for(i=0;i<(MAXOFRXQ - q->head + q->tail);i++)
		{
			pelem[i] = q->elem[q->head + i];
		}				
		InitQue(q);
	}
	if(q->head < q->tail)
	{
		for(i=0;i<(q->tail - q->head);i++)
		{
			pelem[i] = q->elem[q->head + i];
		}
		InitQue(q);
	}		
	return QUESUC;
}
	
u16 NumOfQue(Quque *q)
{
	u16 Re;
	if(IsEmpty(q) == QUEEMP)
		Re = 0;
	else if(q->head > q->tail)
	{
		Re = MAXOFRXQ - q->head + q->tail;
	}
	else if(q->head < q->tail)
	{
		Re = q->tail - q->head;
	}
	return Re;
}

