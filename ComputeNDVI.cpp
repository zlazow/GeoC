#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()
#include "cpl_string.h"

int main() {
	//Open File
	GDALDataset  *poDataset;
    GDALAllRegister();
    poDataset = (GDALDataset *) GDALOpen( "GE01.tif", GA_ReadOnly );

    //Create new DataSet/File


    const char *pszFormat = "GTiff";
    GDALDriver *poDriver;
    char **papszMetadata;
    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);

    if( poDriver == NULL )
        exit( 1 );


	GDALDataset *poNDVI_Dataset;
    papszMetadata = poDriver->GetMetadata();
    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) ){
        printf( "Driver %s supports CreateCopy() method.\n", pszFormat );


		poNDVI_Dataset = poDriver->CreateCopy( "NDVI_GEO1.tif", poDataset, FALSE, 
		                                NULL, NULL, NULL );
	}

	GDALRasterBand  *poBand_ndvi;
	poBand_ndvi = poNDVI_Dataset->GetRasterBand(1);
	//poBand_ndvi->Fill(0);


	//Fetch Raster Band
	GDALRasterBand  *poBand_r;
	GDALRasterBand  *poBand_i;


	int             nBlockXSize, nBlockYSize;
	poBand_r = poDataset->GetRasterBand( 3 ); //3 is red
	poBand_i = poDataset->GetRasterBand( 4 ); //4 is NIR
	poBand_r->GetBlockSize( &nBlockXSize, &nBlockYSize );
	poBand_i->GetBlockSize( &nBlockXSize, &nBlockYSize );
	poBand_ndvi->GetBlockSize( &nBlockXSize, &nBlockYSize);
	int nBlocksX = (poBand_r->GetXSize() + nBlockXSize-1)/ nBlockXSize;
	int nBlocksY = (poBand_r->GetYSize() + nBlockYSize-1)/ nBlockYSize;

	//do I ever free these?
	GByte *pRedDataBuf = (GByte *) CPLMalloc(nBlockXSize * nBlockYSize);
	GByte *pNIRDataBuf = (GByte *) CPLMalloc(nBlockXSize * nBlockYSize);
	GByte *pNDVIBuf    = (GByte *) CPLMalloc(nBlockXSize * nBlockYSize);

	for (int iBlockY = 0; iBlockY < nBlocksY ; iBlockY++){
		for (int iBlockX = 0; iBlockX <nBlocksX; iBlockX++){
			float red, nir, ndvi;
			int nValidX, nValidY;
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
					ndvi = 127.5 + ndvi * 127.5;
					//printf("%f, %f, %f ... ", red, nir, ndvi);
					pNDVIBuf[iX + iY * nBlockXSize] = (uint8_t)ndvi;
                }
            }
             //printf("Got here1 %i, %i \n", iBlockX, iBlockY);
             poBand_ndvi->WriteBlock(iBlockX, iBlockY, pNDVIBuf);
             //printf("Got here2\n");
             
		}


	}
	char ** papszArgv[] = {"NDVI_GEO1.tif", -b, -expand};
	GDALTranslateOptionsNew(papszArgv, NULL);
	GDALDatasetH GDALTranslate("NDVI_GEO1.tif", "NDVI_GEO1.tif", -b 1 -expand gray, NULL);
	GDALTranslateOptionsFree();

	//gdal_translate -b 1 -expand gray NDVI_GEO1.tif NDVI_GEO1.tif

	printf("error is in closing or writeout\n");

	//save/write image file to storage
	//NDVI = (NIR-red)/(NIR+red)

	//Close Dataset
	GDALClose(poDataset);
	GDALClose( (GDALDatasetH) poNDVI_Dataset );
	//Exit      
	return 0;
}