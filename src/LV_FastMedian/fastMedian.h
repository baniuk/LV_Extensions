/**
 * \file    fastMEdian.h
 * \brief	Headers and definitions for LV_Tiff.dll		
 * \details Contains only definitions of methods that are not exported in release but are exported in debug. This file can be linked to TEST in
 * order to perform tests
 * \author  PB
 * \date    2012/09/08
 */

#ifndef fastMedian_h__
#define fastMedian_h__

/** 
 * Struktura opisuj¹ca obraz lub bardziej generalnie obszar pamiêci
 */
struct OBRAZ 
{
	const unsigned short *tab; /** wska¿nik na tablicê z obrazem */
	unsigned int rows; /** iloœæ rzêdów */
	unsigned int cols; /** iloœæ kolumn */
	unsigned int tabsize;	/** iloœæ elementów tablicy = rows*cols */
};

void FastMedian_Huang(	OBRAZ *image,
 						unsigned short *tabout, 
					 	unsigned short mask);

inline unsigned short getPoint(OBRAZ *image, unsigned int r, unsigned int k);
unsigned short getMedian(const unsigned short *tab, unsigned int tabsize);
unsigned short getMedianHist(const unsigned int *hist, unsigned int tabsize);

void CopyWindow(OBRAZ *input_image,
 				unsigned short mask,
				unsigned int current_row,
				unsigned int current_col,
				unsigned short *out,
				unsigned int *hist);
void CopyOneColumn( OBRAZ *input_image, unsigned short mask, int r, int k, unsigned short *out );

/// Number of gray levels in the image
#define GRAYSCALE 65536

#endif // fastMedian_h__

