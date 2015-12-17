#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()
#include <opencv2/opencv.hpp>
//#include "opencv2/ml.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

//g++ threshold.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lgdal

int main()
{
    GDALDataset  *poDataset;
    GDALAllRegister();
    poDataset = (GDALDataset *) GDALOpen( "NDVI_GE01.tif", GA_Update );

    GDALRasterBand *poBand;
	int             nBlockXSize, nBlockYSize;
	poBand = poDataset->GetRasterBand( 1 ); //3 is red
	poBand -> GetBlockSize( &nBlockXSize, &nBlockYSize );
	int nBlocksX = (poBand->GetXSize() + nBlockXSize-1)/ nBlockXSize;
	int nBlocksY = (poBand->GetYSize() + nBlockYSize-1)/ nBlockYSize;

    const char *pszFormat = "GTiff";
    GDALDriver *poDriver;
    char **papszMetadata;
    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
    papszMetadata = poDriver->GetMetadata();
    if( poDriver == NULL )
        exit( 1 );


	GDALDataset *edges_Dataset;
	char **papszOptions = NULL;
	papszOptions = CSLSetNameValue( papszOptions, "TILED", "YES" );
	if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) ){
        printf( "Driver %s supports CreateCopy() method.\n", pszFormat );


		edges_Dataset = poDriver->CreateCopy( "edges_GEO1.tif", poDataset, FALSE, 
		                                papszOptions, NULL, NULL );
	}

	//edges_Dataset = poDriver->CreateCopy("edges_GE01.tif", poBand->GetXSize(), poBand->GetYSize(), 1, GDT_Byte, papszMetadata);
	//edges_Dataset = (GDALDataset *) GDALOpen( "edges_GE01.tif", GA_Update );
	GDALRasterBand  *poBand_edges;
	poBand_edges = edges_Dataset->GetRasterBand(1);

	for (int iBlockY = 0; iBlockY < nBlocksY ; iBlockY++){
		for (int iBlockX = 0; iBlockX <nBlocksX; iBlockX++){
			int nValidX, nValidY;

			if( (iBlockX+1) * nBlockXSize > poBand->GetXSize() )
                 nValidX = poBand->GetXSize() - iBlockX * nBlockXSize;
             else
                 nValidX = nBlockXSize;
             if( (iBlockY+1) * nBlockYSize > poBand->GetYSize() )
                 nValidY = poBand->GetYSize() - iBlockY * nBlockYSize;
             else
                 nValidY = nBlockYSize;

            cv::Rect roi(nBlockXSize * iBlockX, nBlockYSize *iBlockY , nValidX, nValidY);
			cv::Mat dst(roi.width, roi.height, CV_8U);
			poBand->RasterIO( GF_Read, roi.x, roi.y, roi.width, roi.height, dst.data,
                roi.width, roi.height, GDT_Byte, 0, 0);
			//cv::threshold(src, dst, 190, 255, cv::THRESH_BINARY_INV);

			cv::vector<cv::vector<cv::Point> > contours;
    		cv::vector<cv::Vec4i> hierarchy;
    		cv::inRange(dst, 140, 165, dst);

			cv::findContours( dst, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

		    cv::drawContours( dst, contours, -1, 255 , -1 ,8, hierarchy, 2); 
		    /*cv::findContours( dst, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

		    cv::drawContours( dst, contours, -1, 255 , -1 ,8, hierarchy, 2);    
		    cv::Mat kern(2,2, CV_8U);
		    cv::erode(dst, dst, kern);	
		    cv::erode(dst, dst, kern);
		    cv::erode(dst, dst, kern);*/
			poBand_edges->RasterIO(GF_Write, roi.x, roi.y, roi.width, roi.height, dst.data,
                roi.width, roi.height, GDT_Byte, 0, 0);
		
		}
		

	}

	GDALClose(poDataset);
	GDALClose( (GDALDatasetH) edges_Dataset );
}