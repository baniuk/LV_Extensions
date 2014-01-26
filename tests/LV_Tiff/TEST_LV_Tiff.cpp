/**
 * \file    TEST_LV_Tiff.cpp
 * \brief	Tests of public and privat efunctions in LV_Tiff.dll
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
/// \copydoc ::Tiff_GetImage
typedef BYTE (*p_Tiff_GetImage)(char*, UINT16*); 
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
	p_Tiff_GetImage Tiff_GetImage;	// pointer to function from DLL
	p_WarnHandler WarnHandler; // pointer to function from DLL
	p_ErrorHandler ErrorHandler; // pointer to function from DLL
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
		Tiff_GetImage = (p_Tiff_GetImage)GetProcAddress(hinstLib, "Tiff_GetImage"); 
		if(Tiff_GetImage==NULL)
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
	 bool ex = false;	// no exception
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
 * \test Tiff_GetImage
 * Load test image to user's buffor
 * Expects:
 * -# Proper initialization of DLL in fixture class
 * -# Tiff_GetParams returns OK
 * -# Tiff_GetImage returns OK
 */ 
TEST_F(DLL_Tests,Tiff_GetImage)
{
	ASSERT_FALSE(init_error); // expect no error during initialization ( SetUp() )
	BYTE err;	// error returned from procedure
	UINT16 width, height;	// tiff size
	err = Tiff_GetParams("../../../../tests/LV_Tiff/data/test_4800x2000.tif",&height, &width);
	ASSERT_EQ(OK,err);		// expect that returned size is correct
	UINT16* image = new UINT16[width*height];
	err = Tiff_GetImage("../../../../tests/LV_Tiff/data/test_4800x2000.tif",image);
	EXPECT_EQ(OK,err);		// expect OK from procedure
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
 * \test Tiff_Unsupported_GetImage
 * Load unsupported test image to user's buffor.
 * Expects:
 * -# Proper initialization of DLL in fixture class
 * -# Tiff_GetParams returns error
 * -# Expects that this is FILE_READ_ERROR
 */ 
TEST_F(DLL_Tests,Tiff_Unsupported_GetImage)
{
	ASSERT_FALSE(init_error); // expect no error during initialization ( SetUp() )
	BYTE err;	// error returned from procedure
	UINT16 width, height;	// tiff size
	err = Tiff_GetParams("../../../../tests/LV_Tiff/data/unsupported.tif",&height, &width);
	EXPECT_EQ(FILE_READ_ERROR,err);		// expect that returned size is correct
	UINT16* image = new UINT16[1];	// dummy initialziation
	err = Tiff_GetImage("../../../../tests/LV_Tiff/data/unsupported.tif",image);
	EXPECT_EQ(FILE_READ_ERROR,err);		// expect FILE_READ_ERROR from procedure
	delete[] image;
}