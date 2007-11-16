/***************************************************************************
 *  Copyright 2007 Huy Phan  <huyphan@playxiangqi.com>                     *
 *                                                                         * 
 *  This file is part of HOXChess.                                         *
 *                                                                         *
 *  HOXChess is free software: you can redistribute it and/or modify       *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  HOXChess is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with HOXChess.  If not, see <http://www.gnu.org/licenses/>.      *
 ***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// Name:            hoxUtility.cpp
// Created:         09/28/2007
//
// Description:     Containing various helper API in the project.
/////////////////////////////////////////////////////////////////////////////

#include "hoxUtility.h"

using namespace hoxUtility;

static wxString gPiecePath = "";

/**
 * Get the full-path of the image of a given piece.
 */
static wxString
_get_piece_image_path( hoxPieceType  type, 
                       hoxPieceColor color )
{
    wxString filename;
    wxChar cColor;

    cColor = ( color == hoxPIECE_COLOR_RED ? 'r' : 'b' );

    switch ( type )
    {
    case hoxPIECE_TYPE_KING:     // King
        filename.sprintf("%s/%cking.png", gPiecePath.c_str(), cColor);
        break;

    case hoxPIECE_TYPE_ADVISOR:  // Advisor
        filename.sprintf("%s/%cadvisor.png", gPiecePath.c_str(), cColor);
        break;

    case hoxPIECE_TYPE_ELEPHANT: // Elephant 
        filename.sprintf("%s/%celephant.png", gPiecePath.c_str(), cColor);
        break;
  
    case hoxPIECE_TYPE_HORSE:  // Horse 
        filename.sprintf("%s/%chorse.png", gPiecePath.c_str(), cColor);
        break;
  
    case hoxPIECE_TYPE_CHARIOT: // Chariot
        filename.sprintf("%s/%cchariot.png", gPiecePath.c_str(), cColor);
        break;
  
    case hoxPIECE_TYPE_CANNON: // Cannon
        filename.sprintf("%s/%ccannon.png", gPiecePath.c_str(), cColor);
        break;
  
    case hoxPIECE_TYPE_PAWN: // Soldier
        filename.sprintf("%s/%cpawn.png", gPiecePath.c_str(), cColor);
        break;

    default:
        wxASSERT_MSG(false, wxString::Format(_("Unknown piece-type: %d"), (int) type));
    } // switch

    return filename;
}


//
// hoxUtility
//

void 
hoxUtility::SetPiecesPath(const wxString& piecesPath)
{
    gPiecePath = piecesPath;
}

hoxResult 
hoxUtility::LoadPieceImage( hoxPieceType  type, 
                            hoxPieceColor color, 
                            wxImage&      image)
{
    const char* FNAME = "hoxUtility::LoadPieceImage";
    wxString filename;

    filename = _get_piece_image_path( type, color );
    if ( ! image.LoadFile(filename, wxBITMAP_TYPE_PNG) ) 
    {
        wxLogError("%s: Failed to load piece-image from path [%s].", FNAME, filename.c_str());
        return hoxRESULT_ERR;
    }

    return hoxRESULT_OK;
}

void
hoxUtility::FreeNetworkTableInfoList( hoxNetworkTableInfoList& tableList )
{
    for ( hoxNetworkTableInfoList::iterator it = tableList.begin(); 
                                            it != tableList.end(); ++it )
    {
        delete (*it);
    }
}

wxString 
hoxUtility::GenerateRandomString()
{
    ::srand( ::time(NULL) );
    int someNumber = ::rand();

    return wxString("SomeString") << someNumber;
}

/**
 * Convert a given request-type to a (human-readable) string.
 */
const wxString 
hoxUtility::RequestTypeToString( const hoxRequestType requestType )
{
    switch( requestType )
    {
        case hoxREQUEST_TYPE_UNKNOWN:     return "UNKNOWN";

        case hoxREQUEST_TYPE_ACCEPT:      return "ACCEPT";
        case hoxREQUEST_TYPE_PLAYER_DATA: return "PLAYER_DATA";
        case hoxREQUEST_TYPE_CONNECT:     return "CONNECT";
        case hoxREQUEST_TYPE_DISCONNECT:  return "DISCONNECT";
        case hoxREQUEST_TYPE_SHUTDOWN:    return "SHUTDOWN";
        case hoxREQUEST_TYPE_POLL:        return "POLL";
        case hoxREQUEST_TYPE_MOVE:        return "MOVE";
        case hoxREQUEST_TYPE_LIST:        return "LIST";
        case hoxREQUEST_TYPE_NEW:         return "NEW";
        case hoxREQUEST_TYPE_JOIN:        return "JOIN";
        case hoxREQUEST_TYPE_LEAVE:       return "LEAVE";
        case hoxREQUEST_TYPE_OUT_DATA:    return "OUT_DATA";
        case hoxREQUEST_TYPE_WALL_MSG:    return "WALL_MSG";

        default:                          return "UNKNOWN";
    }
}

/**
 * Convert a given (human-readable) string to a request-type.
 */
hoxRequestType
hoxUtility::StringToRequestType( const wxString& input )
{
    if ( input == "UNKNOWN" )     return hoxREQUEST_TYPE_UNKNOWN;

    if ( input == "ACCEPT" )      return hoxREQUEST_TYPE_ACCEPT;
    if ( input == "PLAYER_DATA" ) return hoxREQUEST_TYPE_PLAYER_DATA;
    if ( input == "CONNECT" )     return hoxREQUEST_TYPE_CONNECT;
    if ( input == "DISCONNECT" )  return hoxREQUEST_TYPE_DISCONNECT;
    if ( input == "SHUTDOWN" )    return hoxREQUEST_TYPE_SHUTDOWN;
    if ( input == "POLL" )        return hoxREQUEST_TYPE_POLL;
    if ( input == "MOVE" )        return hoxREQUEST_TYPE_MOVE;
    if ( input == "LIST" )        return hoxREQUEST_TYPE_LIST;
    if ( input == "NEW" )         return hoxREQUEST_TYPE_NEW;
    if ( input == "JOIN" )        return hoxREQUEST_TYPE_JOIN;
    if ( input == "LEAVE" )       return hoxREQUEST_TYPE_LEAVE;
    if ( input == "OUT_DATA" )    return hoxREQUEST_TYPE_OUT_DATA;
    if ( input == "WALL_MSG" )    return hoxREQUEST_TYPE_WALL_MSG;

    return hoxREQUEST_TYPE_UNKNOWN;
}

// ----------------------------------------------------------------------------
// hoxURI
// ----------------------------------------------------------------------------

/* static */
wxString 
hoxURI::Escape_String(const wxString& str)
{
    /*
     * @see Uniform Resource Identifier (URI): Generic Syntax
     *          http://www.ietf.org/rfc/rfc3986.txt
     */

    wxString new_uri;
    char     c;

    for (size_t i = 0; i < str.length(); ++i)
    {
        c = str[i];
        if ( ::isspace( c ) )
            wxURI::Escape( new_uri, c );
        else
            new_uri += c;
    }

    return new_uri;
}

/**
 * Parse a given string of the format "hostname:port" into a host-name
 * and a port.
 *
 * @return true if everything is fine. Otherwise, return false.
 */
bool 
hoxUtility::ParseServerAddress( const wxString&   input,
                                hoxServerAddress& serverAddress )
{
    const char SEPARATOR = ':';

    serverAddress.name = input.BeforeFirst( SEPARATOR );
    if ( serverAddress.name.empty() )
        return false;

    wxString portStr = input.AfterFirst( SEPARATOR );
    if ( !portStr.empty() )
    {
        long longVal;
        if ( !portStr.ToLong( &longVal ) )
            return false;

        serverAddress.port = (int) longVal;
    }
    
    return true;
}

/************************* END OF FILE ***************************************/