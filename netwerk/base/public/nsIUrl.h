/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 4 -*-
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

#ifndef nsIUrl_h___
#define nsIUrl_h___

#include "nsISupports.h"
#include "nsIConnectionGroup.h"
#include "nscore.h"

#undef GetPort  // Windows (sigh)

#define NS_IURL_IID                                  \
{ /* 82c1b000-ea35-11d2-931b-00104ba0fd40 */         \
    0x82c1b000,                                      \
    0xea35,                                          \
    0x11d2,                                          \
    {0x93, 0x1b, 0x00, 0x10, 0x4b, 0xa0, 0xfd, 0x40} \
}

/**
 * The nsIUrl class is an interface to the URL behaviour for parsing
 * portions out of a URI. This follows Tim Berners-Lee's URI spec at-
 * 
 *   http://www.w3.org/Addressing/URI/URI_Overview.html
 * 
 * For the purpose of this class, here is the most elaborate form of a URI
 * and its corresponding parts-
 * 
 *   ftp://username:password@hostname:portnumber/pathname
 *   \ /   \               / \      / \        /\       /
 *    -     ---------------   ------   --------  -------
 *    |            |             |        |         |
 *    |            |             |        |        Path
 *    |            |             |       Port         
 *    |            |            Host
 *    |         PreHost            
 *    Scheme
 * 
 * Note that this class does not assume knowledge of search/query portions 
 * embedded within the path portion of the URI.
 * 
 * This class pretty much "final" and there shouldn't be anything added.
 * If you do feel something belongs here, please do send me a mail. Thanks!
 */

class nsIUrl : public nsISupports {
public:
    NS_DEFINE_STATIC_IID_ACCESSOR(NS_IURL_IID);

    /**
     * Parses a URL spec (a string) relative to a base URL.
     * Any defaults not specified in the spec (because it is a relative
     * spec) are pulled from the base. The baseUrl can be null in which
     * case the spec must be an absolute URL.
     */
    NS_IMETHOD Init(const char* spec, nsIUrl* baseUrl) = 0;

    /**
     * The Scheme is the protocol that this URI refers to. 
     */
    NS_IMETHOD GetScheme(const char* *result) = 0;
    NS_IMETHOD SetScheme(const char* scheme) = 0;

    /**
     * The PreHost portion includes elements like the optional 
     * username:password, or maybe other scheme specific items. 
     */
    NS_IMETHOD GetPreHost(const char* *result) = 0;
    NS_IMETHOD SetPreHost(const char* preHost) = 0;

    /**
     * The Host is the internet domain name to which this URI refers. 
     * Note that it could be an IP address as well. 
     */
    NS_IMETHOD GetHost(const char* *result) = 0;
    NS_IMETHOD SetHost(const char* host) = 0;

    /**
     * A return value of -1 indicates that no port value is set and the 
     * implementor of the specific scheme will use its default port. 
     * Similarly setting a value of -1 indicates that the default is to be used.
     * Thus as an example-
     *   for HTTP, Port 80 is same as a return value of -1. 
     * However after setting a port (even if its default), the port number will
     * appear in the ToString function.
    */
    NS_IMETHOD GetPort(PRInt32 *result) = 0;
    NS_IMETHOD SetPort(PRInt32 port) = 0;

    /** 
     * Note that the path includes the leading '/' Thus if no path is 
     * available the GetPath will return a "/" 
     * For SetPath if none is provided, one would be prefixed to the path. 
     */
    NS_IMETHOD GetPath(const char* *result) = 0;
    NS_IMETHOD SetPath(const char* path) = 0;

    // Other utility functions
    /**
     * Note that this comparison is only on char* level. Use 
     * the scheme specific URI to do a more thorough check. For example--
     * in HTTP-
     *    http://foo.com:80 == http://foo.com
     * but this function through nsIURI alone will not return equality
     * for this case.
     * @return NS_OK if equal
     * @return NS_COMFALSE if not equal
     */
    NS_IMETHOD Equals(nsIUrl* other) = 0;

    /**
     * Makes a copy of the URL.
     */
    NS_IMETHOD Clone(nsIUrl* *result) = 0;

    /**
     * Writes a string representation of the URI. 
     * Free string with delete[].
     */
    NS_IMETHOD ToNewCString(char* *uriString) = 0;

};

////////////////////////////////////////////////////////////////////////////////

/**
 * Protocol writers can obtain a default nsIUrl implementation by calling the
 * component manager with NS_STANDARDURL_CID. The implementation returned will 
 * only implement the set of accessors specified by nsIUrl. After obtaining the
 * instance from the component manager, the Init routine must be called on it
 * to initialize it from the user's URL spec. 
 */

#define NS_STANDARDURL_CID                           \
{ /* 46fc2a26-ff66-11d2-8ccb-0060b0fc14a3 */         \
    0x46fc2a26,                                      \
    0xff66,                                          \
    0x11d2,                                          \
    {0x8c, 0xcb, 0x00, 0x60, 0xb0, 0xfc, 0x14, 0xa3} \
}

////////////////////////////////////////////////////////////////////////////////

extern nsresult
NS_NewURL(nsIUrl** aInstancePtrResult, const char *aSpec, nsIUrl* aBaseUrl);

extern nsresult
NS_NewConnection(nsIUrl* url,
                 nsISupports* eventSink,
                 nsIConnectionGroup* group,
                 nsIProtocolConnection* *result);

////////////////////////////////////////////////////////////////////////////////

#endif /* nsIIUrl_h___ */
