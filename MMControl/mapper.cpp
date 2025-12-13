/* SmallSoft Multimedia Control Panel
 * Copyright (c) 2025, SmallSoft (https://github.com/smallsoft-rus/) 
 * License: BSD 2.0 */
#include "fil_data.h"
#include "dshow.h"
#define SAFE_RELEASE(x) { if (x) { x->Release(); x = NULL; } }

HRESULT EnumerateFilters(IEnumMoniker **ppEnum);
UINT mapper_SetMerit(IPropertyBag *pPropBag, DWORD dwMerit,BYTE* pdata,UINT c);

IFilterMapper2 *m_pMapper = NULL;
HRESULT GetMerit(IPropertyBag *pPropBag, DWORD *pdwMerit);

DWORD mapper_GetFilterMerit(TCHAR* name) 
{
    HRESULT hr=S_OK;
    int nFilters = 0;    

	IEnumMoniker *pEnum = NULL;
    IMoniker *pMoniker=0;

    hr = CoCreateInstance(CLSID_FilterMapper2, 
    NULL, CLSCTX_INPROC, IID_IFilterMapper2, 
    (void **) &m_pMapper);

	if (FAILED(hr)) return hr;

    // Enumerate filters based on the current dialog box selections.
	hr = EnumerateFilters(&pEnum);

    if (FAILED(hr)) return 0;
	
	// Reset the enumerator.
    hr = pEnum->Reset();  // Not strictly necessary but can't hurt.

	// Go through each moniker in the collection.
    while((hr = pEnum->Next(1, &pMoniker, NULL)) == S_OK)
    {
        IPropertyBag *pPropBag = NULL;
        DWORD dwMerit;
        VARIANT varName;
        
		VariantInit(&varName);
		
		// Initialize variant types 
		varName.vt = VT_BSTR;
        
        // Get a property bag for this moniker.
        hr = pMoniker->BindToStorage(
			0, 0, IID_IPropertyBag, (void **)&pPropBag);

	    // Read the filter name from the property bag
        if (SUCCEEDED(hr))
		{
		    hr = pPropBag->Read(L"FriendlyName", &varName, 0);
		}

        if(lstrcmp(varName.bstrVal,name)!=0){
				SysFreeString(varName.bstrVal);
				SAFE_RELEASE(pPropBag);
				SAFE_RELEASE(pMoniker);
				continue;
		}
        // Read the Merit value from the property bag
		dwMerit=0;
		if (SUCCEEDED(hr))
		{
	        hr = GetMerit(pPropBag, &dwMerit);
		}
		   
        // Clean up.
        SysFreeString(varName.bstrVal);
        SAFE_RELEASE(pPropBag);
        SAFE_RELEASE(pMoniker);
		// Clean up enumerator
		SAFE_RELEASE(pEnum);
		SAFE_RELEASE(m_pMapper);
		return dwMerit;
		
    }   

    // Clean up enumerator
    SAFE_RELEASE(pEnum);
	SAFE_RELEASE(m_pMapper);
	return 0;
}

UINT mapper_FilterMeritBinaryData(const TCHAR* name,DWORD merit,BYTE* pdata,UINT c) 
{
    HRESULT hr=S_OK;
    int nFilters = 0;    
	UINT size=0;
	IEnumMoniker *pEnum = NULL;
    IMoniker *pMoniker=0;

    hr = CoCreateInstance(CLSID_FilterMapper2, 
    NULL, CLSCTX_INPROC, IID_IFilterMapper2, 
    (void **) &m_pMapper);

	if (FAILED(hr))
	{
    return hr;
	}

    // Enumerate filters based on the current dialog box selections.
	hr = EnumerateFilters(&pEnum);
    if (FAILED(hr))
    {
        return 0;
    }
	
	// Reset the enumerator.
    hr = pEnum->Reset();  // Not strictly necessary but can't hurt.

	// Go through each moniker in the collection.
    while((hr = pEnum->Next(1, &pMoniker, NULL)) == S_OK)
    {
        IPropertyBag *pPropBag = NULL;
        DWORD dwMerit;
        VARIANT varName;
        
		VariantInit(&varName);
		
		// Initialize variant types 
		varName.vt = VT_BSTR;
        
        // Get a property bag for this moniker.
        hr = pMoniker->BindToStorage(
			0, 0, IID_IPropertyBag, (void **)&pPropBag);

	    // Read the filter name from the property bag
        if (SUCCEEDED(hr))
		{
		    hr = pPropBag->Read(L"FriendlyName", &varName, 0);
		}

        if(lstrcmp(varName.bstrVal,name)!=0){
				SysFreeString(varName.bstrVal);
				SAFE_RELEASE(pPropBag);
				SAFE_RELEASE(pMoniker);
				continue;
		}

        // Read the Merit value from the property bag
		dwMerit=0;
		if (SUCCEEDED(hr))
		{
	        size = mapper_SetMerit(pPropBag,dwMerit,pdata,c);
		}
		   
        // Clean up.
        
        SysFreeString(varName.bstrVal);
        SAFE_RELEASE(pPropBag);
        SAFE_RELEASE(pMoniker);
		// Clean up enumerator
		SAFE_RELEASE(pEnum);
		SAFE_RELEASE(m_pMapper);
		return size;
    }

    // Clean up enumerator
    SAFE_RELEASE(pEnum);
	SAFE_RELEASE(m_pMapper);
	
	return size;
}

///////////////////////////////////////////////////////////////////////
// Name: EnumerateFilters
// Desc: Enumerates the filters that match the user's search criteria.
///////////////////////////////////////////////////////////////////////

HRESULT EnumerateFilters(IEnumMoniker **ppEnum)
{
	HRESULT hr;

	//assert(ppEnum);

	hr = m_pMapper->EnumMatchingFilters(
            ppEnum,
            0,                  // Reserved
            FALSE,        // Use exact match?
            1,            // Minimum merit
            FALSE,
            NULL,       // Number of major type/subtype pairs for input
            NULL,         // Array of major type/subtype pairs for input
            NULL,               // Input medium
            NULL,             // Input pin category
            FALSE,        // Must be a renderer?
            FALSE,
            0,      // Number of major type/subtype pairs for output
            0,        // Array of major type/subtype pairs for output
            NULL,               // Output medium
            NULL);           // Output pin category

	return hr;
}

HRESULT GetMerit(IPropertyBag *pPropBag, DWORD *pdwMerit)
{
    HRESULT hr;
    IAMFilterData *pData = NULL;

    VARIANT varFilData={0};
    varFilData.vt = VT_UI1 | VT_ARRAY;
    varFilData.parray = 0;     // docs say to zero this
    BYTE *pbFilterData = 0;    // 0 if not read
    DWORD dwcbFilterDAta = 0;  // 0 if not read

    // Read compressed filter data from the property bag with a variant
    hr = pPropBag->Read(L"FilterData", &varFilData, 0);
    if (SUCCEEDED(hr))
    {
        //assert(varFilData.vt == (VT_UI1 | VT_ARRAY));
        dwcbFilterDAta = varFilData.parray->rgsabound[0].cElements;
		
        // Access the filter data
        hr = SafeArrayAccessData(varFilData.parray, (void **)&pbFilterData);
    }

    // Get the IAMFilterData interface for parsing the filter data
	if (SUCCEEDED(hr))
	{
	    hr = m_pMapper->QueryInterface(__uuidof(IAMFilterData), (void **)&pData);
	}

    if (SUCCEEDED(hr))
    {
        BYTE *pb=0;

        // Use a helper method to parse the binary filter data.  Pass in
        // the pointer to the filter data, its size, and a buffer to fill with
        // the resulting data.  The "pb" buffer is allocated with CoTaskMemAlloc,
        // so it must be correspondingly freed by the caller.
        hr = pData->ParseFilterData(pbFilterData, dwcbFilterDAta, &pb);
        if(SUCCEEDED(hr))
        {
            REGFILTER2 *pFil = ((REGFILTER2 **)pb)[0];

            // Assign the merit value from the REGFILTER2 structure
            if (pFil)
			{
                *pdwMerit = pFil->dwMerit;

                 // Free the memory allocated by ParseFilterData
                 CoTaskMemFree(pFil);
			}
        }
    }

	if (pbFilterData)
	{
		SafeArrayUnaccessData(varFilData.parray);
		VariantClear(&varFilData);
	}

	// Release the IAMFilterData interface
	SAFE_RELEASE(pData);

    return hr;
}

UINT mapper_SetMerit(IPropertyBag *pPropBag, DWORD dwMerit,BYTE* pdata,UINT c)
{
    HRESULT hr;
    IAMFilterData *pData = NULL;

    VARIANT varFilData={0};
    varFilData.vt = VT_UI1 | VT_ARRAY;
    varFilData.parray = 0;     // docs say to zero this
    BYTE *pbFilterData = 0;    // 0 if not read
    DWORD dwcbFilterDAta = 0;  // 0 if not read
	DWORD dw=0;
	BYTE* pointer=0;DWORD size=0;

    // Read compressed filter data from the property bag with a variant
    hr = pPropBag->Read(L"FilterData", &varFilData, 0);
    if (SUCCEEDED(hr))
    {
        //assert(varFilData.vt == (VT_UI1 | VT_ARRAY));
        dwcbFilterDAta = varFilData.parray->rgsabound[0].cElements;
		
        // Access the filter data
        hr = SafeArrayAccessData(varFilData.parray, (void **)&pbFilterData);
    }

    // Get the IAMFilterData interface for parsing the filter data
	if (SUCCEEDED(hr))
	{
	    hr = m_pMapper->QueryInterface(__uuidof(IAMFilterData), (void **)&pData);
	}

    if (SUCCEEDED(hr))
    {
        BYTE *pb=0;

        // Use a helper method to parse the binary filter data.  Pass in
        // the pointer to the filter data, its size, and a buffer to fill with
        // the resulting data.  The "pb" buffer is allocated with CoTaskMemAlloc,
        // so it must be correspondingly freed by the caller.
        hr = pData->ParseFilterData(pbFilterData, dwcbFilterDAta, &pb);
        if(SUCCEEDED(hr))
        {
            REGFILTER2 *pFil = ((REGFILTER2 **)pb)[0];
            // Assign the merit value from the REGFILTER2 structure
            if (pFil)
			{
                dw = pFil->dwMerit;
				pFil->dwMerit=dwMerit;//change merit
				hr=pData->CreateFilterData(pFil,&pointer,&size);
				
				if(SUCCEEDED(hr)&&size!=0){
					if(size>c)size=c;
					memcpy(pdata,pointer,size);
				}
                 // Free the memory allocated by ParseFilterData
                 CoTaskMemFree(pFil);
			}
        }
    }

	if (pbFilterData)
	{
		SafeArrayUnaccessData(varFilData.parray);
		VariantClear(&varFilData);
	}

	// Release the IAMFilterData interface
	SAFE_RELEASE(pData);

    return size;
}
