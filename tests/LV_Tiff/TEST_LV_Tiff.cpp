/**
 * \file    TEST_LV_Tiff.cpp
 * \brief	Tests of public and private functions in LV_Tiff.dll
 * \pre libtiff3.dll and other dependencies must be on path
 * \pre EXPORTTESTING macro must defined in DLL project for testing provate functions
 * \author  PB
 * \date    2014/01/22
 * \related TIFFException.cpp
 */

#include "stdafx.h"
#include "error_codes.h"
#include "TIFFException.h"

using namespace std;

/// \copydoc ::Tiff_GetParams
typedef BYTE (*p_Tiff_GetParams)(char*, UINT16*, UINT16*); 
/// \copydoc ::Tiff_ReadImage
typedef BYTE (*p_Tiff_ReadImage)(char*, UINT16*); 
/// \copydoc ::Tiff_WriteImage
typedef BYTE (*p_Tiff_WriteImage)(char*, UINT16*,UINT16,UINT16); 
/// \copydoc ::WarnHandler
typedef void (*p_WarnHandler)(const char*, const char*, va_list);
/// \copydoc ::ErrorHandler
typedef void (*p_ErrorHandler)(const char*, const char*, va_list);

int _tmain(int argc, _TCHAR* argv[])
{
	int ret = 0;
	::testing::InitGoogleTest(&argc, argv);
	ret = RUN_ALL_TESTS();
	return ret;
}

/**
 * \brief Test fixture class
 * \details Load and free relevant library before every test
 * \warning libtiff3.dll and other dependencies must be on path
 */ 
class DLL_Tests : public ::testing::Test {
protected:
	BOOL init_error;					// possible error in initialization process accessed by particular tests
	HINSTANCE hinstLib; 
	p_Tiff_GetParams Tiff_GetParams;	// pointer to function from DLL
	p_Tiff_ReadImage Tiff_ReadImage;	// pointer to function from DLL
	p_Tiff_WriteImage Tiff_WriteImage; // pointer to function from DLL
	p_WarnHandler WarnHandler; // pointer to function from DLL
	p_ErrorHandler ErrorHandler; // pointer to function from DLL
	
	/**
	* Initializes test environment
	*
	* Load libray and get pointers to exported functions.
	* \warning may not work when compiled as release because some methods are not exported in release
	*/
	virtual void SetUp()
	{
		init_error = FALSE;	// no error
		hinstLib = LoadLibrary(TEXT("../../../../bin/LV_Tiff_Debug.dll")); 
		if(hinstLib==NULL)
		{
			cerr << "Error in LoadLibrary" << endl << "Make sure that libtiff3.dll and other dependencies in on path" << endl;
			init_error = TRUE;
			return;
		}
		// funkcje
		Tiff_GetParams = (p_Tiff_GetParams)GetProcAddress(hinstLib, "Tiff_GetParams"); 
		if(Tiff_GetParams==NULL)
		{
			cerr << "Error in GetProcAddress" << endl;
			init_error = TRUE;
			return;
		}
		Tiff_ReadImage = (p_Tiff_ReadImage)GetProcAddress(hinstLib, "Tiff_ReadImage"); 
		if(Tiff_ReadImage==NULL)
		{
			cerr << "Error in GetProcAddress" << endl;
			init_error = TRUE;
			return;
		}
		Tiff_WriteImage = (p_Tiff_WriteImage)GetProcAddress(hinstLib, "Tiff_WriteImage"); 
		if(Tiff_ReadImage==NULL)
		{
			cerr << "Error in GetProcAddress" << endl;
			init_error = TRUE;
			return;
		}
		WarnHandler = (p_WarnHandler)GetProcAddress(hinstLib, "WarnHandler"); 
		if(WarnHandler==NULL)
		{
			cerr << "Error in GetProcAddress" << endl;
			init_error = TRUE;
			return;
		}
		ErrorHandler = (p_ErrorHandler)GetProcAddress(hinstLib, "ErrorHandler"); 
		if(ErrorHandler==NULL)
		{
			cerr << "Error in GetProcAddress" << endl;
			init_error = TRUE;
			return;
		}
	}

	virtual void TearDown()
	{
		FreeLibrary(hinstLib);
	}

	// Objects declared here can be used by all tests in the test case for Foo.
};

/**
 * \test WarnHandler
 * Test of function for processing of warnings. Result must be visible in log of LV_Tiff
 * \pre Requires EXPORTTESTING macro and will work only for debug configs
 */
TEST_F(DLL_Tests,WarnHandler)
 {
	 ASSERT_FALSE(init_error); // expect no error during initialization ( SetUp() )
	 WarnHandler("Title ","%s","message");
	 // see log to check if it is ok
 }

/**
 * \test ErrorHandler
 * Test of function for processing of errors. Result must be visible in log of LV_Tiff
 * \pre Requires EXPORTTESTING macro and will work only for debug configs
 * 
 * Expects:
 * -# ErrorHandler throws exception
 */
TEST_F(DLL_Tests,ErrorHandler)
 {
	 ASSERT_FALSE(init_error); // expect no error during initialization ( SetUp() )
	 try
	 {
		 ErrorHandler("Error ","%s","Test message");	// call error with message, exception should be thrown
	 }
	 catch(TIFFException& e)
	 {
		 EXPECT_STREQ("Test message", e.what());	// the same message as ErrorHandler("Error ","%s","Test message");
	 }
 }

/**
 * \test Tiff_GetParams
 * Load test image and returns its size
 * Expects:
 * -# Proper initialization of DLL in fixture class
 * -# Tiff_GetParams returns OK
 * -# Image size [2000 4800]
 */ 
TEST_F(DLL_Tests,Tiff_GetParams)
{
	ASSERT_FALSE(init_error); // expect no error during initialization ( SetUp() )
	BYTE err;	// error returned from procedure
	UINT16 width, height;	// tiff size
	err = Tiff_GetParams("../../../../tests/LV_Tiff/data/test_4800x2000.tif",&height, &width);
	EXPECT_EQ(OK,err);		// expect OK from procedure
	EXPECT_EQ(2000, height);
	EXPECT_EQ(4800, width);	
}

/**
 * \test Tiff_ReadImage
 * Load test image to user's buffor
 * Expects:
 * -# Proper initialization of DLL in fixture class
 * -# Tiff_GetParams returns OK
 * -# Tiff_ReadImage returns OK
 */ 
TEST_F(DLL_Tests,Tiff_ReadImage)
{
	ASSERT_FALSE(init_error); // expect no error during initialization ( SetUp() )
	BYTE err;	// error returned from procedure
	UINT16 width, height;	// tiff size
	err = Tiff_GetParams("../../../../tests/LV_Tiff/data/test_4800x2000.tif",&height, &width);
	ASSERT_EQ(OK,err);		// expect that returned size is correct
	UINT16* image = new UINT16[width*height];
	err = Tiff_ReadImage("../../../../tests/LV_Tiff/data/test_4800x2000.tif",image);
	ASSERT_EQ(OK,err);		// expect OK from procedure
	// temporary copy to matrix
	C_Matrix_Container tmpImage;
	tmpImage.AllocateData(height,width);
	for(UINT32 i=0;i< (UINT32)width*height;++i)
		tmpImage.data[i] = (double)image[i];
	C_DumpAll dump("../../../../tests/LV_Tiff/data/test_out.dat");
	dump.AddEntry(&tmpImage,"loadedTiff");
	delete[] image;
}

/**
 * \test Tiff_Unsupported_ReadImage
 * Load unsupported test image to user's buffor.
 * Expects:
 * -# Proper initialization of DLL in fixture class
 * -# Tiff_GetParams returns error
 * -# Expects that this is FILE_READ_ERROR
 */ 
TEST_F(DLL_Tests,Tiff_Unsupported_ReadImage)
{
	ASSERT_FALSE(init_error); // expect no error during initialization ( SetUp() )
	BYTE err;	// error returned from procedure
	UINT16 width, height;	// tiff size
	err = Tiff_GetParams("../../../../tests/LV_Tiff/data/unsupported.tif",&height, &width);
	EXPECT_EQ(FILE_READ_ERROR,err);		// expect that returned size is correct
	UINT16* image = new UINT16[1];	// dummy initialziation
	err = Tiff_ReadImage("../../../../tests/LV_Tiff/data/unsupported.tif",image);
	EXPECT_EQ(FILE_READ_ERROR,err);		// expect FILE_READ_ERROR from procedure
	delete[] image;
}

/**
 * \test Tiff_Nonexistent_ReadImage
 * Reads image that do not exist
 * Expects:
 * -# Proper initialization of DLL in fixture class
 * -# Tiff_GetParams returns error
 * -# Expects that this is FILE_READ_ERROR
 */ 
TEST_F(DLL_Tests,Tiff_Nonexistent_ReadImage)
{
	ASSERT_FALSE(init_error); // expect no error during initialization ( SetUp() )
	BYTE err;	// error returned from procedure
	UINT16 width, height;	// tiff size
	err = Tiff_GetParams("../../../../tests/LV_Tiff/data/no.tif",&height, &width);
	EXPECT_EQ(FILE_READ_ERROR,err);		// expect that returned size is correct
	UINT16* image = new UINT16[1];	// dummy initialziation
	err = Tiff_ReadImage("../../../../tests/LV_Tiff/data/no.tif",image);
	EXPECT_EQ(FILE_READ_ERROR,err);		// expect FILE_READ_ERROR from procedure
	delete[] image;
}

/**
 * \test Tiff_Simple_WriteImage
 * Writes chessboard to file
 * Expects:
 * -# Proper initialization of DLL in fixture class
 */ 
TEST_F(DLL_Tests,Tiff_Simple_WriteImage)
{
	ASSERT_FALSE(init_error); // expect no error during initialization ( SetUp() )
	UINT16 r,rows = 1024;
	UINT16 k,cols  = 1024;
	UINT32 l = 0;
	BYTE err;
	// mem alloc
	UINT16* image = new UINT16[rows*cols];
	for(r=0;r<rows;++r)
		for(k=0;k<cols;++k)
			if(k%2==0)
				image[l++] = 0;
			else
				image[l++] = 1;
	err = Tiff_WriteImage("../../../../tests/LV_Tiff/data/out.tif",image,rows,cols);
	EXPECT_EQ(OK,err);
	delete[] image;
}

/**
 * \test Tiff_WriteImage
 * Load supported tiff and writes it to other file
 * Expects:
 * -# Proper initialization of DLL in fixture class
 * -# working Tiff_GetParams
 * -# working Tiff_ReadImage
 */ 
TEST_F(DLL_Tests,Tiff_WriteImage)
{
	ASSERT_FALSE(init_error); // expect no error during initialization ( SetUp() )
	BYTE err;	// error returned from procedure
	UINT16 cols, rows, w;	// tiff size and counters
	err = Tiff_GetParams("../../../../tests/LV_Tiff/data/test_4800x2000.tif",&rows, &cols);
	ASSERT_EQ(OK,err);		// expect that returned size is correct
	UINT16* image = new UINT16[cols*rows];
	err = Tiff_ReadImage("../../../../tests/LV_Tiff/data/test_4800x2000.tif",image);
	ASSERT_EQ(OK,err);		// expect OK from procedure
	for(w = 0; w < cols; w++)
		image[cols*rows/2+w] = 5000;
	err = Tiff_WriteImage("../../../../tests/LV_Tiff/data/out_simple.tif",image,rows,cols);
	EXPECT_EQ(OK,err);
	delete[] image;
}

/**
 * \test Tiff_Nonexistent_WriteImage
 * Write image on location that do not exist
 * Expects:
 * -# Proper initialization of DLL in fixture class
 * -# Expects that this is FILE_READ_ERROR
 */ 
TEST_F(DLL_Tests,Tiff_Nonexistent_WriteImage)
{
	ASSERT_FALSE(init_error); // expect no error during initialization ( SetUp() )
	BYTE err = OK;	// error returned from procedure
	UINT16* image = new UINT16[100*100];	// dummy initialziation
	err = Tiff_WriteImage("../../../../tests/LV_Tiff/data_no/no.tif",image,100,100);
	EXPECT_EQ(FILE_READ_ERROR,err);		// expect FILE_READ_ERROR from procedure
	delete[] image;
}