#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()
#include "cpl_string.h"
#include "cpl_port.h"
#include "gdal.h"

int main() {
	//Open File
	GDALDataset  *poDataset;
    GDALAllRegister();
    poDataset = (GDALDataset *) GDALOpen( "GE01.tif", GA_ReadOnly );

    GDALRasterBand  *poBand_r;
	GDALRasterBand  *poBand_i;
	poBand_r = poDataset->GetRasterBand( 3 ); //3 is red
	poBand_i = poDataset->GetRasterBand( 4 ); //4 is NIR

    const char *pszFormat = "GTiff";
    GDALDriver *poDriver;
    char **papszMetadata;
    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);

    if( poDriver == NULL )
        exit( 1 );


	GDALDataset *poNDVI_Dataset;
    papszMetadata = poDriver->GetMetadata();
    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) ){
        printf( "Driver %s supports CreateCopy() method.\n", pszFormat );

		poNDVI_Dataset = poDriver->Create( "NDVI_GE01.tif", poBand_r->GetXSize(), poBand_r->GetYSize(), 1, GDT_Byte, NULL);
		//poNDVI_Dataset = poDriver->CreateCopy( "NDVI_GE01.tif", poDataset, FALSE, NULL, NULL, NULL );
	}

	GDALRasterBand  *poBand_ndvi;

	poBand_ndvi = poNDVI_Dataset->GetRasterBand(1);





	int             nBlockXSize, nBlockYSize;
	poBand_r->GetBlockSize( &nBlockXSize, &nBlockYSize );
	int nBlocksX = (poBand_r->GetXSize() + nBlockXSize-1)/ nBlockXSize;
	int nBlocksY = (poBand_r->GetYSize() + nBlockYSize-1)/ nBlockYSize;
	fprintf(stderr, "nBlockXSize:%i, nBlockYSize:%i, nBlocksX:%i, nBlocksY:%i\n", nBlockXSize, nBlockYSize,nBlocksX,nBlocksY);

	GByte *pRedDataBuf = (GByte *) CPLMalloc(nBlockXSize * nBlockYSize);
	GByte *pNIRDataBuf = (GByte *) CPLMalloc(nBlockXSize * nBlockYSize);
	GByte *pNDVIBuf    = (GByte *) CPLMalloc(nBlockXSize * nBlockYSize);
	int nValidX, nValidY;
	for (int iBlockY = 0; iBlockY < nBlocksY ; iBlockY++){
		for (int iBlockX = 0; iBlockX < nBlocksX; iBlockX++){
			float red, nir, ndvi;

			poBand_r->ReadBlock(iBlockX, iBlockY, pRedDataBuf);
			poBand_i->ReadBlock(iBlockX, iBlockY, pNIRDataBuf);
			
			 if( (iBlockX+1) * nBlockXSize > poBand_r->GetXSize() )
                 nValidX = poBand_r->GetXSize() - iBlockX * nBlockXSize;
             else
                 nValidX = nBlockXSize;
             if( (iBlockY+1) * nBlockYSize > poBand_r->GetYSize() )
                 nValidY = poBand_r->GetYSize() - iBlockY * nBlockYSize;
             else
                 nValidY = nBlockYSize;
			
            for( int iY = 0; iY < nValidY; iY++ ){
                for( int iX = 0; iX < nValidX; iX++ ){
                    red = (float)pRedDataBuf[iX + iY * nBlockXSize];
					nir = (float)pNIRDataBuf[iX + iY * nBlockXSize];
					ndvi = (nir-red)/(nir+red);
					if (ndvi<=0){
						ndvi = 0;
					}
					else
						ndvi = 255*ndvi;
					//printf("%f, %f, %f ... ", red, nir, ndvi);
					pNDVIBuf[iX + iY * nBlockXSize] = (uint8_t)ndvi;
                }
            }
            //printf("Got here1 y:%i, x:%i\n", iBlockY, iBlockX);

            poBand_ndvi->WriteBlock(iBlockX, iBlockY, pNDVIBuf);
            //printf("Got here2 y:%i, x:%i\n", iBlockY, iBlockX);
             
		}


	}
	char **papszOptions = NULL;
	papszOptions = CSLSetNameValue( papszOptions, "TILED", "YES" );
	if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) ){
		poNDVI_Dataset = poDriver->CreateCopy( "NDVI_GE01.tif", poNDVI_Dataset, FALSE, 
		                                papszOptions, NULL, NULL );
	}
	
	//NDVI = (NIR-red)/(NIR+red)

	//Close Dataset
	GDALClose(poDataset);
	GDALClose( (GDALDatasetH) poNDVI_Dataset );
	//Exit      
	return 0;
}
