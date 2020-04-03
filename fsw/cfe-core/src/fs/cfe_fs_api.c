/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/*
** File: cfe_fs_api.c
**
** Purpose:  cFE File Services (FS) library API source file
**
** Author:   S.Walling/Microtel
**
** Notes:
**
*/


/*
** Required header files...
*/
#include "private/cfe_private.h"
#include "cfe_fs_priv.h"
#include "cfe_fs.h"
#include "cfe_time.h"
#include "osapi.h"
#include "cfe_psp.h"
#include "cfe_es.h"
#include "cfe_sb_eds.h"
#include "edslib_datatypedb.h"
#include <string.h>


/*
** CFE_FS_ReadHeader() - See API and header file for details
*/
int32 CFE_FS_ReadHeader(CFE_FS_Header_t *Hdr, int32 FileDes)
{
    int32   Result;
    uint8_t LocalBuffer[sizeof(CFE_FS_Header_t)];
    EdsLib_Id_t EdsId;
    EdsLib_DataTypeDB_TypeInfo_t HdrInfo;
    uint32 ExpectedSize;
    
    /*
    ** Ensure that we are at the start of the file...
    */
    Result = OS_lseek(FileDes, 0, OS_SEEK_SET);

    if (Result == OS_SUCCESS)
    {
        EdsId = EDSLIB_MAKE_ID(EDS_INDEX(CFE_FS), CFE_FS_Header_DATADICTIONARY);
        EdsLib_DataTypeDB_GetTypeInfo(CFE_SB_GetEds(), EdsId, &HdrInfo);
        ExpectedSize = (HdrInfo.Size.Bits + 7) / 8;

        /*
        ** Read header structure into callers buffer...
        */
        Result = OS_read(FileDes, LocalBuffer, sizeof(LocalBuffer));
        if (Result == ExpectedSize)
        {
            Result = EdsLib_DataTypeDB_UnpackCompleteObject(CFE_SB_GetEds(), &EdsId, Hdr, LocalBuffer,
                    HdrInfo.Size.Bytes, 8 * sizeof(LocalBuffer));

            if (Result == EDSLIB_SUCCESS)
            {
                Result = HdrInfo.Size.Bytes;
            }
            else
            {
                Result = CFE_FS_BAD_ARGUMENT;
            }
        }
        else
        {
            Result = CFE_FS_BAD_ARGUMENT;
        }
    }
    
    return(Result);

} /* End of CFE_FS_ReadHeader() */

/*
** CFE_FS_InitHeader() - See API and header file for details
*/
void CFE_FS_InitHeader(CFE_FS_Header_t *Hdr, const char *Description, uint32 SubType)
{
   memset(Hdr, 0, sizeof(CFE_FS_Header_t));
   strncpy((char *)Hdr->Description, Description, sizeof(Hdr->Description) - 1);
   Hdr->SubType = SubType;
}

/*
** CFE_FS_WriteHeader() - See API and header file for details
*/
int32 CFE_FS_WriteHeader(int32 FileDes, CFE_FS_Header_t *Hdr)
{
    CFE_TIME_SysTime_t Time;
    int32   Result;
    EdsLib_Id_t                 EdsId;
    EdsLib_DataTypeDB_TypeInfo_t HdrInfo;
    uint32                      ExpectedSize;
    uint8_t LocalBuffer[sizeof(CFE_FS_Header_t)];

    /*
    ** Ensure that we are at the start of the file...
    */
    Result = OS_lseek(FileDes, 0, OS_SEEK_SET);

    if (Result == OS_SUCCESS)
    {
        EdsId = EDSLIB_MAKE_ID(EDS_INDEX(CFE_FS), CFE_FS_Header_DATADICTIONARY);
        EdsLib_DataTypeDB_GetTypeInfo(CFE_SB_GetEds(), EdsId, &HdrInfo);
        ExpectedSize = (HdrInfo.Size.Bits + 7) / 8;

        /*
        ** Fill in the ID fields...
        */
        Hdr->SpacecraftID  = CFE_PSP_GetSpacecraftId();
        Hdr->ProcessorID   = CFE_PSP_GetProcessorId();
        CFE_ES_GetAppID(&Hdr->ApplicationID);

        /* Fill in length field */

        Hdr->Length = ExpectedSize;

        /* put the header, 'cfe1' in hex, in to the content type */
        Hdr->ContentType = CFE_FS_FILE_CONTENT_ID;


        /*
        ** Fill in the timestamp fields...
        */
        Time = CFE_TIME_GetTime();
        Hdr->TimeSeconds = Time.Seconds;
        Hdr->TimeSubSeconds = Time.Subseconds;

        Result = EdsLib_DataTypeDB_PackCompleteObject(CFE_SB_GetEds(), &EdsId, LocalBuffer, Hdr,
                8 * sizeof(LocalBuffer), HdrInfo.Size.Bytes);

        if (Result == EDSLIB_SUCCESS)
        {
            /*
            ** Write header structure...
            */
            Result = OS_write(FileDes, Hdr, ExpectedSize);

            /*
             * Fixup the return status to match what had been done
             * prior to EDS integration.
             */
            if (Result == ExpectedSize)
            {
                Result = HdrInfo.Size.Bytes;
            }
        }
        else
        {
            Result = CFE_FS_BAD_ARGUMENT;
        }
    }

    return(Result);

} /* End of CFE_FS_WriteHeader() */

/*
** CFE_FS_SetTimestamp - See API and header file for details
*/
int32 CFE_FS_SetTimestamp(int32 FileDes, CFE_TIME_SysTime_t NewTimestamp)
{
    int32              Result;
    uint8              OutputBuffer[sizeof(CFE_TIME_SysTime_t)];
    int32              FileOffset = 0;
    EdsLib_Id_t        EdsId;
    EdsLib_DataTypeDB_TypeInfo_t TypeInfo;
    uint32             BlockSize;

    /*
     * FIXME: This is assuming that the field within the on-disk binary/packed object has
     * the same offset as the native memory representation.
     *
     * _THIS IS NOT GUARANTEED_
     *
     * However in the current definition of the FS header it will be always be true
     * so there is no pressing need to modify this at the moment.
     */
    FileOffset = offsetof(CFE_FS_Header_t, TimeSeconds);
    Result = OS_lseek(FileDes, FileOffset, OS_SEEK_SET);
    
    if (Result == FileOffset)
    {
        EdsId = EDSLIB_MAKE_ID(EDS_INDEX(CFE_TIME), CFE_TIME_SysTime_DATADICTIONARY);
        Result = EdsLib_DataTypeDB_PackCompleteObject(CFE_SB_GetEds(), &EdsId, OutputBuffer, &NewTimestamp,
                8 * sizeof(OutputBuffer), sizeof(NewTimestamp));
        if (Result != EDSLIB_SUCCESS ||
                EdsLib_DataTypeDB_GetTypeInfo(CFE_SB_GetEds(), EdsId, &TypeInfo) != EDSLIB_SUCCESS)
        {
            Result = CFE_FS_BAD_ARGUMENT;
        }
        else
        {
            BlockSize = (TypeInfo.Size.Bits + 7) / 8;

            Result = OS_write(FileDes, OutputBuffer, BlockSize);

            /* On a good write, the value returned will equal the number of bytes written */
            if (Result == BlockSize)
            {
                Result = CFE_SUCCESS;
            }
            else
            {
                CFE_ES_WriteToSysLog("CFE_FS:SetTime-Failed to write Seconds (Status=0x%08X)\n", (unsigned int)Result);
            }
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_FS:SetTime-Failed to lseek time fields (Status=0x%08X)\n", (unsigned int)Result);
    }
    
    return(Result);
} /* End of CFE_FS_SetTimestamp() */

/*
** CFE_FS_ExtractFilenameFromPath - See API and header file for details
*/
int32 CFE_FS_ExtractFilenameFromPath(const char *OriginalPath, char *FileNameOnly)
{
   uint32 i,j;
   int    StringLength;
   int    DirMarkIdx;
   int32   ReturnCode;
   
   if ( OriginalPath == NULL || FileNameOnly == NULL )
   {
      ReturnCode = CFE_FS_BAD_ARGUMENT;
   }
   else
   {
      
       /*
       ** Get the string length of the original file path
       */
       StringLength = strlen(OriginalPath);
   
       /*
       ** Extract the filename from the Path
       */
    
       /* 
       ** Find the last '/' Character 
       */
       DirMarkIdx = -1;
       for ( i = 0; i < StringLength; i++ )
       {
          if ( OriginalPath[i] == '/' )
          {
             DirMarkIdx = i;
          }
       }
    
       /*
       ** Verify the filename isn't too long
       */
       if ((StringLength - (DirMarkIdx + 1)) < OS_MAX_PATH_LEN)
       {       
          /* 
          ** Extract the filename portion 
          */
          if ( DirMarkIdx > 0 )
          {    
             /* 
             ** Extract the filename portion 
             */
             j = 0;
             for ( i = DirMarkIdx + 1; i < StringLength; i++ )
             {
                FileNameOnly[j] = OriginalPath[i];
                j++;
             }
             FileNameOnly[j] = '\0';
    
             ReturnCode = CFE_SUCCESS;       
          }
          else
          { 
             ReturnCode = CFE_FS_INVALID_PATH;
          }
       }
       else 
       {
           ReturnCode = CFE_FS_FNAME_TOO_LONG;
       }
    }
   
    return(ReturnCode);
}


/*
** Function: CFE_FS_IsGzFile - See API and header file for details
*/
bool CFE_FS_IsGzFile(const char *FileName)
{
   size_t    StringLength;
   
   if ( FileName == NULL )
   {
      return(false);
   }
   /*
   ** Get the string length of the SourceFile name
   */
   StringLength = strlen(FileName);
   
   /*
   ** The compressed filename must be named "something.gz"
   ** Because of that, the shortest compressed filename is "a.gz"
   ** Check for the length before determining if the file is zipped
   */
   if ( StringLength < 4 )
   {
      return(false);
   }

   /*
   ** if the last 3 characters are ".gz"
   ** Then assume the file is zipped.
   */
   if ( (FileName[StringLength - 3] == '.') &&
        (FileName[StringLength - 2] == 'g') &&
        (FileName[StringLength - 1] == 'z'))
   {
       return(true);       
   }
   else
   {
       return(false);
   }
  
}

/*
** CFE_FS_GetUncompressedFile - See API and header file for details
*/
int32 CFE_FS_GetUncompressedFile(char *OutputNameBuffer, uint32 OutputNameBufferSize,
        const char *GzipFileName, const char *TempDir)
{
    char    FileNameOnly[OS_MAX_PATH_LEN];
    int32   Status;
    int     RequiredLen;    /* using "int" per snprintf API */

    /*
    ** Extract the filename from the path
    */
    Status = CFE_FS_ExtractFilenameFromPath(GzipFileName, FileNameOnly);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("FS_GetUncompressedFile: Unable to extract filename from path: %s.\n",GzipFileName);
    }
    else
    {
        /*
        ** Build up the destination path in the RAM disk
        */
        RequiredLen = snprintf(OutputNameBuffer, OutputNameBufferSize,
                "%s/%s", TempDir, FileNameOnly);

        /*
        ** Remove the ".gz" prefix from the filename
        ** (Any input file name to this function must have a .gz extension,
        ** i.e. it passed the CFE_FS_IsGzFile() test, of which .gz is the only possibility)
        */
        if (RequiredLen >= 3)
        {
            RequiredLen -= 3;
        }

        /*
         * check if LoadNameBuffer got truncated
         * (snprintf returns the _required_ length, whether or not it actually fit)
         */
        if (RequiredLen < 0 || RequiredLen >= OutputNameBufferSize)
        {
            /* Can't include the name string since it could be too long for the message */
            CFE_ES_WriteToSysLog("FS_GetUncompressedFile: Temporary path plus file name length (%d) exceeds max allowed (%lu)\n",
                                 RequiredLen, (unsigned long)(OutputNameBufferSize-1));
            Status = CFE_FS_FNAME_TOO_LONG;
        }
        else
        {
            /*
             * Actually truncate the .gz from the string
             * (had to wait until after length verification)
             */
            OutputNameBuffer[RequiredLen] = 0;

            /*
            ** Decompress the file:
            */
            Status =  CFE_FS_Decompress( GzipFileName, OutputNameBuffer );
            if ( Status != CFE_SUCCESS )
            {
                CFE_ES_WriteToSysLog("FS_GetUncompressedFile: Unable to decompress %s, error=%08lx\n",
                        GzipFileName, (unsigned long)Status);
            }
        }
    }

    return Status;
}

/************************/
/*  End of File Comment */
/************************/
