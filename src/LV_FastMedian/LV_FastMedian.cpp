/**
 * \file    LV_FastMedian.cpp
 * \brief	Implementacja szybkiej mediany oparta o poprzedni projekt.
 * \author  PB
 * \date    2012/09/08
 */

#include "stdafx.h"

#ifndef SAFE_DELETE
	#define SAFE_DELETE(p)       { if(p) { delete[] (p);     (p)=NULL; } }
#endif

/** 
 * Zwraca wartoœæ z tablicy na pozycji [r,k], przy za³ozeniu ¿e tablica jest
 * interpretowana jako 2D
 * \param[in] image		struktura opisuj¹ca obraz
 * \param[in] r			rz¹d
 * \param[in] k			kolumna
 * \return wartoœæ na pozycji tab[r,k]
 * \remarks W obrazie dane uk³adane sa rzedami, zgodnie z C_MatrixContainer
 * 
*/
inline unsigned short getPoint(OBRAZ *image, unsigned int r, unsigned int k)
{
	_ASSERT(r*image->cols+k<image->tabsize);
	return image->tab[r*image->cols+k];
}

/** 
 * Zwraca medianê z wektora wartoœci
 * \param[in] tab		tablica wartoœci
 * \param[in] tabsize		rozmiar tablicy
 * \return wartoœæ mediany
*/
unsigned short getMedian(const unsigned short *tab, unsigned int tabsize)
{
	std::vector<unsigned int> v( tab, tab+tabsize );
	std::nth_element( v.begin(), v.begin()+tabsize/2,v.end() );
	return v[tabsize/2];
}

/** 
 * Kopiuje okno o rozmiarze 2*halfmedian+1 z obrazka wejœciowego do tablicy out.
 * Kopiuje rzêdami jak na obrazie oryginalnym dodatkowow tworzy histogram
 * \param[in] input_image		obraz wejœciowy
 * \param[in] mask				nieparzysty rozmiar maski
 * \param[in] current_row		indeks rzêdu dla którego wyci¹æ okno - indeks w którym jest œrodek okna
 * \param[in] current_col		indeks kolumny dla której wyci¹æ okno - indeks w którym jest œrodek okna
 * \param[out] out				tablica wyjœciowa o rozmiarze maski
 * \param[out] hist				tablica z histogramem okna
 * \remarks Funkcja czyœci tablicê hist. Jeœli okno wystaje poza obszar obrazu to wpisywane s¹ 0 do okna
 * \todo T¹ funkcjê mo¿na przyspieszyæ u¿ywaj¹c kopiowania ca³ych rzêdów !!
*/
void CopyWindow( OBRAZ *input_image, unsigned short mask, unsigned int current_row, unsigned int current_col, unsigned short *out, unsigned int *hist )
{
	int wr,wk;			// indeksy wewn¹trz okna
	unsigned short bok_maski = (mask-1)/2;
	unsigned int l;	// licznik
	memset(hist, 0, GRAYSCALE*sizeof(unsigned int));	// szybkie zerowanie pamiêci histogramu (dla kazdego rzêdu procedura dzia³a od pocz¹tku)
	for (wr = static_cast<int>(current_row)-bok_maski,l=0;wr<static_cast<int>(current_row)+bok_maski+1;wr++)	
		for (wk = static_cast<int>(current_col)-bok_maski;wk<static_cast<int>(current_col)+bok_maski+1;wk++)
			{
				if(wr<0 || wk<0 || wk>=static_cast<int>(input_image->cols) || wr>=static_cast<int>(input_image->rows))	// jeœli okno wystaje poza obraz to wpisywane s¹ w to miejsce zera
					out[l] = 0;
				else
					out[l] = getPoint(input_image,wr,wk);	// kopiowanie danych z obrazu do osobnej tablicy reprezentuj¹cej okno
		 		hist[out[l]]++;	// obliczam histogram
				l++;				
			}
}

/** 
 * Zwraca medianê z wektora reprezentowanego przez jego histogram
 * \param[in] hist		tablica z histogramem 
 * \param[in] tabsize		rozmiar tablicy
 * \return wartoœæ mediany
 * \remarks Na podstawie http://mathematics.blurtit.com/q6132160.html
*/
unsigned short getMedianHist( const unsigned int *hist, unsigned int tabsize )
{
	unsigned int cum_hist[GRAYSCALE]; // histogram kumulacyjny
	unsigned short cum_index[GRAYSCALE];	// indeksy z histo które wchodz¹ w sklad kumulacyjnego (niezerowe wartoœci histogramu)
	unsigned short licznik;
	unsigned short a=0;
	unsigned short M;
	unsigned int prev_cum;

	while(a<GRAYSCALE-1 && 0==hist[a])	// jeœli pierwszy element histo jest 0, to szukamy pierwszego niezerowego, a sprawdzam w zakresie 0-65535 (uwaga na przepe³nienie)
		a++;
	if(a==GRAYSCALE-1)	// to znaczy ¿e w histo s¹ same 0, koñczymy, zwracaj¹c medianê
		return 0;
	
	// jeœli normalny histogram to wycinamy wartoœci niezerowe
	cum_hist[0] = hist[a];	// pierwsza wartoœæ niezerowa
	cum_index[0] = a;	// oraz jej indeks
	licznik = 1;	// od nastêpniej wartoœci bo zerowa ju¿ ustawiona powy¿ej
	// obliczanie kumulacyjnego ale tylko z wartoœci niezerowych
	for(unsigned int b=a+1;b<GRAYSCALE;b++)	// uwaga na przepe³nienie - b nie mo¿e byæ szort
		if (hist[b]>0)
		{
			cum_hist[licznik] = cum_hist[licznik-1] + hist[b]; // niezerowa wartoœæ
			cum_index[licznik] = b; // oraz jej index w histo (jasnoœæ piksela)
			licznik++;
		}
	--licznik;	// indeks ostatniego elementu w cum_hist i cum_indeks
	M = cum_hist[licznik]/2;
	// do którego przedzia³u trafia M?
	for(a=0;a<=licznik;a++)
		if(M<cum_hist[a])
			break;
	if(0==a)	// czyli nale¿y do pierwszego przedzia³u
		prev_cum = 0;
	else
		prev_cum = cum_hist[a-1];

	// mediana
	return cum_index[a] + (M-prev_cum)/hist[ cum_index[a] ];
}

/** 
 * Filtruje obraz median¹
 * Na podstawie Huang, A Fast Two-Dimensional Median Filtering Algorithm.
 * \param[in] image		obraz wejœciowy
 * \param[out] tabout	wskaŸnik na tablice wyjœciow¹ z przefiltrowanym obrazem o rozmiarze obrazu wejœciowego
 * \param[in] mask		rozmiar maski, maska nieparzysta i kwadratowa
 * \remarks Na rogach obrazu pojawiaj¹ siê zera. Pozatym mo¿na procedurê jeszcze przyspieszyæ modyfikuj¹c pierwsz¹ medianê (mo¿e na containerze bêdzie szybsza (getMedian)
 * oraz modyfikuj¹c pobieranie wartoœci okna poprzez kopiowanie ca³ych rzedów na raz (s¹ liniowo w pamiêci).
*/
void FastMedian_Huang(	OBRAZ *image,
						unsigned short *tabout, 
						unsigned short mask)
{
	unsigned int *hist=NULL;				// histogram obszaru filtrowanego
	unsigned short *window = NULL;			// dane z okna maski
	unsigned short mdm;						// wartoœæ mediany w oknie
 	unsigned int lmdm;					// liczba elementów obrazu o wartoœciach mniejszych od mdm
 	unsigned short *left_column = NULL;		// lewa kolumna okna
 	unsigned short *right_column = NULL;	// prawa kolumna okna
 	unsigned int r,k;						// indeksy bierz¹cej pozycji okna na obrazie (œrodka)
 	unsigned short mask_center = (mask+1)/2;// œrodek maski (indeks)
 	unsigned short bok_maski = (mask-1)/2;	// rozmiar boku maski ca³a maska to 2*bok + 1 
 	unsigned int l;							// licznik
	unsigned short picval;					// pomocnicza wartoœæ piksela obrazu
	unsigned int th = (mask*mask)/2;			// parametr pomocniczy
 
 	hist = new unsigned int[GRAYSCALE];		// zak³adam g³êbiê 16 bit
 	left_column = new unsigned short[mask];	// lewa kolumna poprzedniej pozycji maski (maska jest zawsze kwadratowa)
 	right_column = new unsigned short[mask];// prawa kolumna bierzacej maski
 	window = new unsigned short[static_cast<unsigned int>(mask)*mask];
 	/* 
 	 * Przegl¹danie obrazu po rzêdach a procedura szybkiej filtracji po 
 	 * kolumnach. Dla kazdego nowego rzêdu powtarza siê wszystko od pocz¹tku.
 	 */
 	for (r = 0;r<image->rows;r++)	// g³ówna pêtla po rzêdach obrazu
 	{
 		// -------------------- inicjalizacja parametrów dla ka¿dego rzêdu --------------------------
		k = 0;
		CopyWindow(image,mask,r,k,window,hist);	// kopiowanie okna skrajnego lewego dla danego rzêdu
 // 		mdm = getMedian(window,static_cast<unsigned int>(mask)*mask);	// znajdujê medianê z okna
		mdm = getMedianHist(hist,GRAYSCALE);
		tabout[r*image->cols+k] = mdm;	// ustawiam wyjœcie przy za³o¿eniu ¿e tabout jaest taka sama jak tabin
		// obliczanie lmdm
		for(l=0,lmdm=0;l<static_cast<unsigned int>(mask)*mask;l++)
			if(window[l]<mdm)
				lmdm++;
		for (k = 0+1;k<image->cols;k++)	// g³ówna pêtla po kolumnach obrazu, dla pierwszej pozycji k=mask_center obliczane jest osobno
 		{

 			 // modyfikacja histogramu - Na podstawie Huang, A Fast Two-Dimensional Median Filtering Algorithm 

			CopyOneColumn(image,mask,static_cast<int>(r)-bok_maski,static_cast<int>(k)-bok_maski-1,left_column); //pobieranie lewej kolumny poprzedniego (k-1) okna (podajê [r,k] pocz¹tku kolumny	
			CopyOneColumn(image,mask,static_cast<int>(r)-bok_maski,static_cast<int>(k)+bok_maski,right_column);	// prawa kolumna bierz¹cego k okna
			// liczenie mediany
			for(l=0;l<mask;l++)	// po wszystkich wartoœciach kolumny
			{
				picval = left_column[l];
				_ASSERT(hist[picval]>0);	// jeœli =0 to po odjêciu przepe³nienie
				hist[ picval ]--;	// kasowanie lewej kolumny z histogtramu
				if (picval<mdm)
					lmdm--;
				picval = right_column[l];	
				_ASSERT(hist[picval]<65535);	// jeœli =65535 to po odjêciu przepe³nienie
				hist[picval]++;		// dodawanie prawej kolummny
				if (picval<mdm)
					lmdm++;
			}	// koniec pêtli po kolumnach okna
			if (lmdm>th)
				while (lmdm>th)	// zmiana wzglêdem orygina³u !!! FUCK !!
				{
					_ASSERT(mdm>0);
					mdm--;
					_ASSERT(lmdm>=hist[mdm]);
					lmdm-=hist[mdm];
				} 
			else
				while(lmdm+hist[mdm]<=th) {
					lmdm+=hist[mdm];
					_ASSERT(mdm<65535);
					mdm++;
				}		

			tabout[r*image->cols+k] = mdm;	// ustawiam wyjœcie przy za³o¿eniu ¿e tabout jaest taka sama jak tabin
		} // koniec pêtli po kolumnach obrazu
  
 	} // koniec pêtli po rzêdach
 
 	SAFE_DELETE(hist);
 	SAFE_DELETE(left_column);
 	SAFE_DELETE(right_column);
 	SAFE_DELETE(window);
}

/** 
 * kopiuje jedn¹ kolumnê zaczynaj¹c od pozycji poz
 * Na podstawie Huang, A Fast Two-Dimensional Median Filtering Algorithm 
 * \param[in] input_image		obraz wejœciowy
 * \param[in] mask		rozmiar maski, maska nieparzysta i kwadratowa
 * \param[in] r			rz¹d startowy
 * \param[in] k			kolumna startowa	
 * \param[out] out		tablica o rozmiarze mask z kolumn¹	
 * \remarks Procedura dopuszcza ujemne kolumny i rzêdy, co odpowiada sytuacji gdy kolumna nie miesci siê
 * na obrazie, czyli dla berzegów
*/
void CopyOneColumn( OBRAZ *input_image, unsigned short mask, int r, int k, unsigned short *out )
{
	unsigned short a;
	for (a=0;a<mask;a++)
	{
		if(r<0 || r>=static_cast<int>(input_image->rows) || k<0 || k>=static_cast<int>(input_image->cols))	// jeœli rz¹d lub kolumna poza obrazem t owpisywane s¹ 0
			out[a] = 0;
		else
			out[a] = getPoint(input_image,r,k);
		r++;
	}
}

/** 
 * Filtruje obraz median¹ - funkcja exportowalna dl DLL. Zak³ada ¿e obrz jest podawany wierszami w tablicy 1D
 * \param[in] input_image		obraz wejœciowy
 * \param[out] output_image	wskaŸnik na tablice wyjœciow¹ z przefiltrowanym obrazem o rozmiarze obrazu wejœciowego
 * \param[in] nrows		liczba wierszy
 * \param[in] ncols liczba kolumn
 * \remarks Funckja dokonuje transformacji parametrów wejœciowych na format z poprzedniego projektu
 * \warning Zak³ada maskê o rozmiarze 31x31
*/
extern "C" __declspec(dllexport) void med_filt(const UINT16* input_image, UINT16* output_image, UINT16 nrows, UINT16 ncols)
{
	OBRAZ obraz;	// lokalna kopia obrazu wejœciowego (p³ytka)
	unsigned short mask = 31;	// rozmiar maski
	obraz.tab = input_image;
	obraz.rows = nrows;
	obraz.cols = ncols;
	obraz.tabsize = nrows*ncols;
	FastMedian_Huang(&obraz,output_image,mask);
}

/** 
 * Filtruje obraz median¹ - funkcja exportowalna dl DLL. Zak³ada ¿e obrz jest podawany wierszami w tablicy 1D. Dodatkowo pozwala na ustawienie maski
 * \param[in] input_image		obraz wejœciowy
 * \param[out] output_image	wskaŸnik na tablice wyjœciow¹ z przefiltrowanym obrazem o rozmiarze obrazu wejœciowego
 * \param[in] nrows		liczba wierszy
 * \param[in] ncols liczba kolumn
 * \param[in] mask Rozmiar maski
 * \remarks Funckja dokonuje transformacji parametrów wejœciowych na format z poprzedniego projektu
*/
extern "C" __declspec(dllexport) void med_filt_mask(const UINT16* input_image, UINT16* output_image, UINT16 nrows, UINT16 ncols, UINT16 mask)
{
	OBRAZ obraz;	// lokalna kopia obrazu wejœciowego (p³ytka)
	obraz.tab = input_image;
	obraz.rows = nrows;
	obraz.cols = ncols;
	obraz.tabsize = nrows*ncols;
	FastMedian_Huang(&obraz,output_image,mask);
}