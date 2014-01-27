// TEST_LV_FastMedian.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

typedef void (*p_LV_MedFilt31)(UINT16*, UINT16*, UINT16, UINT16); 

int _tmain(int argc, _TCHAR* argv[])
{
	int ret = 0;
	::testing::InitGoogleTest(&argc, argv);
	ret = RUN_ALL_TESTS();
	return ret;
}

class DLL_Tests : public ::testing::Test {
protected:
	BOOL init_error;
	HINSTANCE hinstLib; 
	p_LV_MedFilt31 LV_MedFilt31; 
	virtual void SetUp()
	{
		init_error = FALSE;	// no error
		hinstLib = LoadLibrary(TEXT("../../../../bin/LV_FastMedian_Debug.dll")); 
		if(hinstLib==NULL)
		{
			cerr << "Error in LoadLibrary" << endl;
			init_error = TRUE;
		}
		else
			cout << "Library loaded" << endl;
		// funkcje
		LV_MedFilt31 = (p_LV_MedFilt31)GetProcAddress(hinstLib, "LV_MedFilt31"); 
		if(LV_MedFilt31==NULL)
		{
			cerr << "Error in GetProcAddress" << endl;
			init_error = TRUE;
		}
		else
			cout << "Addres of p_LV_MedFilt31: " << LV_MedFilt31 << endl;
	}

	virtual void TearDown()
	{
		FreeLibrary(hinstLib);
	}

	// Objects declared here can be used by all tests in the test case for Foo.
};

TEST_F(DLL_Tests,FastMedianTest)
{
	EXPECT_FALSE(init_error); // expect no error during initialization
	unsigned short *input_image;	// input image
	unsigned short *output_image;
	// Przygotowanie danych dla funkcji
	// Tymczasowe readbinary i konwersja do formatu dlla
	C_MATRIX_LOAD(tmp_input_image,"../../../../tests/LV_FastMedian/data/testimag1.dat"); // load test file
	tmp_input_image.Normalize(0,1);	// normalization
	input_image = new unsigned short[tmp_input_image.GetNumofElements()];
	if(input_image==NULL)
	{
		cerr << "Error in memory allocation" << endl;
		init_error = TRUE;
	}
	for(unsigned int a=0;a<tmp_input_image.GetNumofElements();a++)
		input_image[a] = static_cast<unsigned short>(floor(65535*tmp_input_image.data[a]+0.5));
	output_image = new unsigned short[tmp_input_image.GetNumofElements()];
	if(output_image==NULL)
	{
		cerr << "Error in memory allocation" << endl;
		init_error = TRUE;
	}

	// wywo³anie funkcji
	LV_MedFilt31(input_image,output_image,tmp_input_image._rows,tmp_input_image._cols);
	// nagranie wyjœcia
	C_DumpAll dump("../../../../tests/LV_FastMedian/data/test_out.dat");
	C_Matrix_Container out;
	out.AllocateData(tmp_input_image._rows,tmp_input_image._cols);
	for(unsigned int a=0;a<tmp_input_image.GetNumofElements();a++)
		out.data[a] = static_cast<double>(output_image[a]);
	dump.AddEntry(&out,"outimage");
	dump.AddEntry(&tmp_input_image,"inimage");
	
	SAFE_DELETE(input_image);
	SAFE_DELETE(output_image);

	cout << "Results in /data/test_out.dat" << endl;
	EXPECT_EQ(1,1);
}