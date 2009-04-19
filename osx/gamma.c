//***********************
/*#include "gamma.h"

CTabHandle 	pFadeCTable;	
CTabHandle 	pOrigCTable;	

void FadeDevice( GDHandle screen, short fadeCmd );
void FadeOutDevice( RGBColor *fadeValues );
void FadeInDevice( RGBColor *fadeValues );
void BlackoutDevice( void );

//***********************
void DoFade( void )
{
	long		finalTicks;
	
	FadeDevice( GetMainDevice(), kFadeOut );
	Delay(60, &finalTicks);	
	FadeDevice( GetMainDevice(),  kFadeIn );
}

//***********************
void FadeDevice( 	GDHandle fadeGDevice, 
				short    fadeState )
{
	GDHandle 		oldDev;
	RGBColor			fadeValues[kMaxColors];
	short			index;
	
	// must be an indexed device
	if ( ( **fadeGDevice).gdType == directType )
		return;

	oldDev = GetGDevice();
	SetGDevice( fadeGDevice );
	
	// copy color table to manipulate and restore later
	pFadeCTable = ( *(*fadeGDevice)->gdPMap )->pmTable;
	pOrigCTable = ( *(*fadeGDevice)->gdPMap )->pmTable;
	HandToHand( ( Handle* )&pOrigCTable );
	
	HLock( ( Handle )pFadeCTable );
	HLock( ( Handle )pOrigCTable );

	// calculate the fade values for the RGB components for
	// each entry in the color table
	for (index = 0; index < ( *pFadeCTable )->ctSize; index++)
	{
		fadeValues[index].red   = (*pFadeCTable)->ctTable[index].rgb.red   / 100;
		fadeValues[index].green = (*pFadeCTable)->ctTable[index].rgb.green / 100;
		fadeValues[index].blue  = (*pFadeCTable)->ctTable[index].rgb.blue  / 100;
	}

	if ( fadeState == kFadeOut )
		FadeOutDevice( fadeValues );
	else
		FadeInDevice(  fadeValues );
	
	//restore the clut to original value
	for ( index = 0; index < ( *pFadeCTable )->ctSize; index++ )
	{
		(*pFadeCTable)->ctTable[index].rgb.red   = 
				( *pOrigCTable )->ctTable[index].rgb.red;
		(*pFadeCTable)->ctTable[index].rgb.green = 
				( *pOrigCTable )->ctTable[index].rgb.green;
		(*pFadeCTable)->ctTable[index].rgb.blue  = 
				( *pOrigCTable )->ctTable[index].rgb.blue;
	}
	
	// color table changed, must reset ctseed
	(*pFadeCTable )->ctSeed = GetCTSeed();	
	MakeITable( nil, nil, 0 );				
	if (  noErr != QDError() )
		return;
	
	HUnlock( ( Handle )pFadeCTable );
	HUnlock( ( Handle )pOrigCTable );

	SetGDevice( oldDev );
	DisposeHandle( ( Handle )pOrigCTable );
}

//***********************
void FadeOutDevice( RGBColor *fadeValues)
{
	short 	index, percentFade;
	
	for ( percentFade = 100; percentFade >= 0; percentFade-- )
	{
		for (index = 0; index < ( *pFadeCTable )->ctSize; index++)
		{
			if ((*pFadeCTable)->ctTable[index].rgb.red   > fadeValues[index].red)
				(*pFadeCTable)->ctTable[index].rgb.red    -= fadeValues[index].red;

			if ((*pFadeCTable)->ctTable[index].rgb.green > fadeValues[index].green)
				(*pFadeCTable)->ctTable[index].rgb.green -= fadeValues[index].green;

			if ((*pFadeCTable)->ctTable[index].rgb.blue  > fadeValues[index].blue)
				(*pFadeCTable)->ctTable[index].rgb.blue   -= fadeValues[index].blue;
		}
		SetEntries( 0, ( *pFadeCTable )->ctSize - 1, (*pFadeCTable)->ctTable );
	}
	BlackoutDevice();
}

//***********************
void FadeInDevice( RGBColor *fadeValues )
{
	short 	index, speedIndex;
	
	BlackoutDevice();
	
	for (speedIndex = 0; speedIndex < 100; speedIndex++)
	{
		for (index = 0; index < ( *pFadeCTable )->ctSize; index++)
		{
			if ((*pFadeCTable)->ctTable[index].rgb.red   <
 					(*pOrigCTable)->ctTable[index].rgb.red)
				(*pFadeCTable)->ctTable[index].rgb.red   += fadeValues[index].red;

			if ((*pFadeCTable)->ctTable[index].rgb.green < 
					(*pOrigCTable)->ctTable[index].rgb.green)
				(*pFadeCTable)->ctTable[index].rgb.green += fadeValues[index].green;

			if ((*pFadeCTable)->ctTable[index].rgb.blue  < 
					(*pOrigCTable)->ctTable[index].rgb.blue)
				(*pFadeCTable)->ctTable[index].rgb.blue  += fadeValues[index].blue;
		}
		SetEntries( 0, ( *pFadeCTable )->ctSize - 1, ( *pFadeCTable )->ctTable );
	}
}

//***********************
void BlackoutDevice(void)
{
	short 	index;
	
	for ( index = 0; index < ( *pFadeCTable )->ctSize; index++ )
	{
		( *pFadeCTable )->ctTable[index].rgb.red   = 0;
		( *pFadeCTable )->ctTable[index].rgb.green = 0;
		( *pFadeCTable )->ctTable[index].rgb.blue  = 0;
	}
	SetEntries( 0, ( *pFadeCTable )->ctSize - 1, ( *pFadeCTable )->ctTable );
}
*/
// File "gamma.c" - Source for Altering the Gamma Tables of GDevices
//   Last updated 11/9/93, MJS
 
// * ****************************************************************************** *
//
//	This is the Source Code for the Gamma Utils Library file. Use this to build
//		new functionality into the library or make an A4-based library. 
//	See the header file "gamma.h" for much more information. -- MJS
//
// * ****************************************************************************** *

#include "gamma.h"

long			gammaUtilsInstalled;
globalGammasHdl	gammaTables;

// * ****************************************************************************** *
// * ****************************************************************************** *
void FadeDevice( 	GDHandle fadeGDevice, 
				short    fadeState )
{
	short i;
	if (fadeState == kFadeIn) {
		for (i = 0; i <= 100; i+= 2)
			DoOneGammaFade(fadeGDevice,i);
		}
		else {		
			for (i = 100; i >= 0; i-= 2)
				DoOneGammaFade(fadeGDevice,i);
			}
}

// Let's be optimistic for now and assume it is. If it breaks later, I'll try to work out how to do it properly.
Boolean IsGammaAvailable() {
/*	GDHandle theGDevice;

	if (NGetTrapAddress(kGetDeviceListTrapNum, ToolTrap) ==
			NGetTrapAddress(_Unimplemented, ToolTrap)) return(0);
	
	for(theGDevice = GetDeviceList(); theGDevice; theGDevice = GetNextDevice(theGDevice))
		if (TestDeviceAttribute(theGDevice, screenDevice) && 
				TestDeviceAttribute(theGDevice, noDriver)) return(0);
*/
	return(-1);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

Boolean IsOneGammaAvailable(GDHandle theGDevice) {
	/*
	if (NGetTrapAddress(kGetDeviceListTrapNum, ToolTrap) ==
			NGetTrapAddress(_Unimplemented, ToolTrap)) return(0);
	
	if (TestDeviceAttribute(theGDevice, screenDevice) && 
			TestDeviceAttribute(theGDevice, noDriver)) return(0);
	*/
	return(-1);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

OSErr SetupGammaTools() {
	short errorCold=0;
	globalGammasHdl tempHdl;
	GammaTblPtr	masterGTable;
	GDHandle theGDevice;

	if (gammaUtilsInstalled == kGammaUtilsSig) return(-1);
	
	gammaTables = 0;
	gammaUtilsInstalled = kGammaUtilsSig;
	for(theGDevice = GetDeviceList(); theGDevice; theGDevice = GetNextDevice(theGDevice)) {
		if ((errorCold = GetDevGammaTable(theGDevice, &masterGTable)) != 0) return(errorCold);
		
		tempHdl = (globalGammasHdl) NewHandle(sizeof(globalGammas));
		if (tempHdl == 0) return(errorCold = MemError());
		
		(*tempHdl)->size = sizeof(GammaTbl) + masterGTable->gFormulaSize +
				(masterGTable->gChanCnt * masterGTable->gDataCnt * masterGTable->gDataWidth / 8);
		(*tempHdl)->dataOffset = masterGTable->gFormulaSize;
		(*tempHdl)->theGDevice = theGDevice;
		
		(*tempHdl)->saved = (GammaTblHandle) NewHandle((*tempHdl)->size);
		if ((*tempHdl)->saved == 0) return(errorCold = MemError());
		(*tempHdl)->hacked = (GammaTblHandle) NewHandle((*tempHdl)->size);
		if ((*tempHdl)->hacked == 0) return(errorCold = MemError());
	
		BlockMove((Ptr) masterGTable, (Ptr) *(*tempHdl)->saved, (*tempHdl)->size);
		
		(*tempHdl)->next = gammaTables;
		gammaTables = tempHdl;
		}

	return(0);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

OSErr DoGammaFade(short percent) {
	short errorCold=0;
	register long size, i, theNum;
	globalGammasHdl tempHdl;
	unsigned char *dataPtr;

	if (gammaUtilsInstalled != kGammaUtilsSig) return(-1); 
	
	for(tempHdl = gammaTables; tempHdl; tempHdl = (*tempHdl)->next) {
	
		BlockMove((Ptr) *(*tempHdl)->saved, (Ptr) *(*tempHdl)->hacked, (*tempHdl)->size);
		dataPtr = (unsigned char *) (*(*tempHdl)->hacked)->gFormulaData + (*tempHdl)->dataOffset;
		size = (*(*tempHdl)->hacked)->gChanCnt * (*(*tempHdl)->hacked)->gDataCnt;
		
		for(i=0; i < size; i++) {
			theNum = dataPtr[i];
			theNum = (theNum * percent) / 100;
			dataPtr[i] = theNum;
			}
		
		if ((errorCold = SetDevGammaTable((*tempHdl)->theGDevice, (*tempHdl)->hacked)) != 0)
			return(errorCold);
		}
		
	return(0);
	}

void do_nothing()
{
}

// * ****************************************************************************** *
// * ****************************************************************************** *

OSErr DoOneGammaFade(GDHandle theGDevice, short percent) {
	short errorCold=0;
	register long size, i, theNum;
	globalGammasHdl tempHdl;
	unsigned char *dataPtr;

	if (gammaUtilsInstalled != kGammaUtilsSig) return(-1); 
	for(tempHdl = gammaTables; tempHdl && (theGDevice != (*tempHdl)->theGDevice);
			tempHdl = (*tempHdl)->next) do_nothing();
		
	BlockMove((Ptr) *(*tempHdl)->saved, (Ptr) *(*tempHdl)->hacked, (*tempHdl)->size);
	dataPtr = (unsigned char *) (*(*tempHdl)->hacked)->gFormulaData + (*tempHdl)->dataOffset;
	size = (*(*tempHdl)->hacked)->gChanCnt * (*(*tempHdl)->hacked)->gDataCnt;
	
	for(i=0; i < size; i++) {
		theNum = dataPtr[i];
		theNum = (theNum * percent) / 100;
		dataPtr[i] = theNum;
		}
	
	errorCold = SetDevGammaTable((*tempHdl)->theGDevice, (*tempHdl)->hacked);
	
	return(errorCold);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

OSErr DisposeGammaTools() {
	globalGammasHdl tempHdl, nextHdl;

	if (gammaUtilsInstalled != kGammaUtilsSig) return(-1); 
	for(tempHdl = gammaTables; tempHdl; tempHdl = nextHdl) {
		nextHdl = (*tempHdl)->next;
		DisposeHandle((Handle) (*tempHdl)->saved);
		DisposeHandle((Handle) (*tempHdl)->hacked);
		DisposeHandle((Handle) tempHdl);
		}
		
	gammaUtilsInstalled = 0;
	return(0);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

OSErr GetDevGammaTable(GDHandle theGDevice, GammaTblPtr *theTable) {
	short errorCold=0;
	CntrlParam  *myCPB;
 
	((long *) theTable)[0] = 0;
 
	if (IsOneGammaAvailable(theGDevice) == 0) return(-1);
			
	if ((myCPB = (CntrlParam *) NewPtrClear(sizeof(CntrlParam))) == 0) return(MemError());
	myCPB->csCode = cscGetGamma;
	myCPB->ioCRefNum = (*theGDevice)->gdRefNum;
	*(GammaTblPtr **) myCPB->csParam = theTable;
	errorCold = PBStatus((ParmBlkPtr) myCPB, 0);

	DisposePtr((Ptr) myCPB);
	return(errorCold);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

OSErr SetDevGammaTable(GDHandle theGDevice, GammaTblPtr *theTable) {
	CntrlParam *myCPB;
	short errorCold=0;
	CTabHandle cTab;
	GDHandle saveGDevice;
 
	if (IsOneGammaAvailable(theGDevice) == 0) return(-1);

	if ((myCPB = (CntrlParam *) NewPtrClear(sizeof(CntrlParam))) == 0) return(MemError());
	myCPB->csCode = cscSetGamma;
	myCPB->ioCRefNum = (*theGDevice)->gdRefNum;
	*(GammaTblPtr **) myCPB->csParam = theTable;
	errorCold = PBControl((ParmBlkPtr) myCPB, 0);
 
	if (errorCold == 0) {
		saveGDevice = GetGDevice();
		SetGDevice(theGDevice);
 		cTab = (*(*theGDevice)->gdPMap)->pmTable;
		SetEntries (0, (*cTab)->ctSize, (*cTab)->ctTable);
		SetGDevice(saveGDevice);
		}

	DisposePtr((Ptr) myCPB);
	return (errorCold);
	}
