/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "NPL"); you may not use this file except in
 * compliance with the NPL.  You may obtain a copy of the NPL at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the NPL is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
 * for the specific language governing rights and limitations under the
 * NPL.
 *
 * The Initial Developer of this code under the NPL is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation.  All Rights
 * Reserved.
 */

/* 
 * nsDiskModule
 *
 * Gagan Saksena 02/02/98
 * 
 */

#include <prtypes.h>
#include <prmem.h>
#include <plstr.h>

#include "nsDiskModule.h"
#include "nsCacheObject.h"
#include "nsCacheManager.h"

#include "mcom_db.h"

//
// Constructor: nsDiskModule
//
nsDiskModule::nsDiskModule(const PRUint32 size):
    nsCacheModule(size),
    m_pDB(0)
{
}

nsDiskModule::~nsDiskModule()
{
    if (m_pDB)
    {
        (*m_pDB->sync)(m_pDB, 0);
        (*m_pDB->close)(m_pDB);
        m_pDB = 0;
    }
}

PRBool nsDiskModule::AddObject(nsCacheObject* io_pObject)
{
    if (!InitDB())
    {   
        // Set some error state TODO
        return PR_FALSE;
    }   

    if (io_pObject && io_pObject->Address())
    {
        static DBT key,data;
        
        io_pObject->Module(nsCacheManager::DISK);

        PR_FREEIF(key.data);
        PR_FREEIF(data.data);

        key.data = (void*)io_pObject->Address(); /* Later on change this to include post data- io_pObject->KeyData() */
        key.size = PL_strlen(io_pObject->Address());

        data.data = io_pObject->Info();
        data.size = io_pObject->InfoSize();

        int status = (*m_pDB->put)(m_pDB, &key, &data, 0);
        if (status == 0)
        {
//            if (m_Sync == EVERYTIME)
                status = (*m_pDB->sync)(m_pDB, 0);
        }
        return (status == 0);
    }
    return PR_FALSE;
}

PRBool nsDiskModule::Contains(nsCacheObject* io_pObject) const
{
    if (!m_pDB)
    {
        nsDiskModule* pThis = (nsDiskModule*) this;
        pThis->InitDB();
    }

    if (!m_pDB || !io_pObject)
        return PR_FALSE;

    nsCacheObject* pTemp = GetObject(io_pObject->Address());
    if (pTemp)
    {
        io_pObject = pTemp;
        return PR_TRUE;
    }
    return PR_FALSE;
}

PRBool nsDiskModule::Contains(const char* i_url) const
{
    if (!m_pDB)
    {
        nsDiskModule* pThis = (nsDiskModule*) this;
        pThis->InitDB();
    }

    if (!m_pDB || !i_url || !*i_url)
        return PR_FALSE;

    DBT key, data;

    key.data = (void*) i_url;
    key.size = PL_strlen(i_url);

    int status = (*m_pDB->get)(m_pDB, &key, &data, 0);

    return (status == 0);

}

nsCacheObject* nsDiskModule::GetObject(const PRUint32 i_index) const
{
    if (!m_pDB)
    {
        nsDiskModule* pThis = (nsDiskModule*) this;
        pThis->InitDB();
    }

    if (!m_pDB)
        return 0;

    return 0;
}

nsCacheObject* nsDiskModule::GetObject(const char* i_url) const
{
    if (!m_pDB)
    {
        nsDiskModule* pThis = (nsDiskModule*) this;
        pThis->InitDB();
    }

    if (!m_pDB || !i_url || !*i_url)
        return 0;

    DBT key, data;

    key.data = (void*) i_url;
    key.size = PL_strlen(i_url);

    if (0 == (*m_pDB->get)(m_pDB, &key, &data, 0))
    {
        nsCacheObject* pTemp = new nsCacheObject();
        pTemp->Info(data.data);
        return pTemp;
    }

    return 0;
}

PRBool nsDiskModule::InitDB(void) 
{

    if (m_pDB)
        return PR_TRUE;

    HASHINFO hash_info = {
        16*1024,  /* bucket size */
        0,        /* fill factor */
        0,        /* number of elements */
        0,        /* bytes to cache */
        0,        /* hash function */
        0};       /* byte order */

    m_pDB = dbopen(
        nsCachePref::DiskCacheDBFilename(), 
        O_RDWR | O_CREAT, 
        0600, 
        DB_HASH, 
        &hash_info);

    if (!m_pDB)
        return PR_FALSE;

    return PR_TRUE;
}
