#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()

///Library/Frameworks/GDAL.framework/Versions/1.11/Headers/
///Library/Frameworks?/GDAL.framework/unix/bin
//sudo g++ -I/Library/Frameworks/GDAL.framework/Versions/1.11/Headers -L/Library/Frameworks/GDAL.framework/unix/lib -lgdal metadata.cpp

/* 
result: main.o
	g++ main.o -L/ -lgdal -o result
main.o: main.cpp main.h
	g++ -c main.cpp -I/ -o main.o

     poBand->GetBlockSize( &nXBlockSize, &nYBlockSize );
     int nXBlocks = (poBand->GetXSize() + nXBlockSize - 1) / nXBlockSize;
     int nYBlocks = (poBand->GetYSize() + nYBlockSize - 1) / nYBlockSize;
     GByte *pabyData = (GByte *) CPLMalloc(nXBlockSize * nYBlockSize);
     for( int iYBlock = 0; iYBlock < nYBlocks; iYBlock++ )
     {
         for( int iXBlock = 0; iXBlock < nXBlocks; iXBlock++ )
         {
             int        nXValid, nYValid;
             poBand->ReadBlock( iXBlock, iYBlock, pabyData );
             // Compute the portion of the block that is valid
             // for partial edge blocks.
             if( (iXBlock+1) * nXBlockSize > poBand->GetXSize() )
                 nXValid = poBand->GetXSize() - iXBlock * nXBlockSize;
             else
                 nXValid = nXBlockSize;
             if( (iYBlock+1) * nYBlockSize > poBand->GetYSize() )
                 nYValid = poBand->GetYSize() - iYBlock * nYBlockSize;
             else
                 nYValid = nYBlockSize;
             // Collect the histogram counts.
             for( int iY = 0; iY < nYValid; iY++ )
             {
                 for( int iX = 0; iX < nXValid; iX++ )
                 {
                     panHistogram[pabyData[iX + iY * nXBlockSize]] += 1;
                 }
             }
         }
     }
 }

CPLErr GetHistogram( GDALRasterBand *poBand, GUIntBig *panHistogram )
 {
     memset( panHistogram, 0, sizeof(GUIntBig) * 256 );
     CPLAssert( poBand->GetRasterDataType() == GDT_Byte );
     int nXBlockSize, nYBlockSize;
     poBand->GetBlockSize( &nXBlockSize, &nYBlockSize );
     int nXBlocks = (poBand->GetXSize() + nXBlockSize - 1) / nXBlockSize;
     int nYBlocks = (poBand->GetYSize() + nYBlockSize - 1) / nYBlockSize;
     GByte *pabyData = (GByte *) CPLMalloc(nXBlockSize * nYBlockSize);
     for( int iYBlock = 0; iYBlock < nYBlocks; iYBlock++ )
     {
         for( int iXBlock = 0; iXBlock < nXBlocks; iXBlock++ )
         {
             int        nXValid, nYValid;
             poBand->ReadBlock( iXBlock, iYBlock, pabyData );
             // Compute the portion of the block that is valid
             // for partial edge blocks.
             if( (iXBlock+1) * nXBlockSize > poBand->GetXSize() )
                 nXValid = poBand->GetXSize() - iXBlock * nXBlockSize;
             else
                 nXValid = nXBlockSize;
             if( (iYBlock+1) * nYBlockSize > poBand->GetYSize() )
                 nYValid = poBand->GetYSize() - iYBlock * nYBlockSize;
             else
                 nYValid = nYBlockSize;
             // Collect the histogram counts.
             for( int iY = 0; iY < nYValid; iY++ )
             {
                 for( int iX = 0; iX < nXValid; iX++ )
                 {
                     panHistogram[pabyData[iX + iY * nXBlockSize]] += 1;
                 }
             }
         }
     }
 }
*/


int main()
{
    GDALDataset  *poDataset;
    GDALAllRegister();
    poDataset = (GDALDataset *) GDALOpen( "GE01.tif", GA_ReadOnly );
    printf("Working! \n");

    if( poDataset != NULL ){
    	//Get Dataset Information
        double adfGeoTransform[6];

		printf( "Driver: %s/%s\n", poDataset->GetDriver()->GetDescription(), poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );

		printf( "Size is %dx%dx%d\n", poDataset->GetRasterXSize(), poDataset->GetRasterYSize(), poDataset->GetRasterCount() );

		if( poDataset->GetProjectionRef()  != NULL )
    		printf( "Projection is `%s'\n", poDataset->GetProjectionRef() );

		if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None ){
    		printf( "Origin = (%.6f,%.6f)\n",
            adfGeoTransform[0], adfGeoTransform[3] );
    		printf( "Pixel Size = (%.6f,%.6f)\n",
            adfGeoTransform[1], adfGeoTransform[5] );
		}

		//Fetch Raster Band
		GDALRasterBand  *poBand;
		int             nBlockXSize, nBlockYSize;
		int             bGotMin, bGotMax;
		double          adfMinMax[2];
		poBand = poDataset->GetRasterBand( 1 );
		poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
		printf( "Block=%dx%d Type=%s, ColorInterp=%s\n",
		        nBlockXSize, nBlockYSize,
		        GDALGetDataTypeName(poBand->GetRasterDataType()),
		        GDALGetColorInterpretationName(
		            poBand->GetColorInterpretation()) );
		adfMinMax[0] = poBand->GetMinimum( &bGotMin );
		adfMinMax[1] = poBand->GetMaximum( &bGotMax );
		if( ! (bGotMin && bGotMax) )
		    GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
		printf( "Min=%.3fd, Max=%.3f\n", adfMinMax[0], adfMinMax[1] );
		if( poBand->GetOverviewCount() > 0 )
		    printf( "Band has %d overviews.\n", poBand->GetOverviewCount() );
		if( poBand->GetColorTable() != NULL )
		    printf( "Band has a color table with %d entries.\n", 
		             poBand->GetColorTable()->GetColorEntryCount() );

		//Close Dataset
		GDALClose(poDataset);
		//Exit      
		return 0;
	}


}