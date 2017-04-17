// Materials.h: interface for the Materials class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIALS_H__C8152D99_8176_4CE4_BF86_9D241D3B54ED__INCLUDED_)
#define AFX_MATERIALS_H__C8152D99_8176_4CE4_BF86_9D241D3B54ED__INCLUDED_


struct NewtonWorld;

extern int woodID; 
extern int metalID; 
extern int concreteID; 

void SetupMaterials (NewtonWorld* nWorld);

#endif 
