/***************************************************************************
 *  Copyright 2007, 2008 Huy Phan  <huyphan@playxiangqi.com>               *
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
// Name:            hoxLocalPlayer.cpp
// Created:         10/28/2007
//
// Description:     The LOCAL Player.
/////////////////////////////////////////////////////////////////////////////

#include "hoxLocalPlayer.h"
#include "hoxNetworkAPI.h"
#include "hoxUtility.h"
#include "hoxSite.h"

DEFINE_EVENT_TYPE(hoxEVT_CONNECTION_RESPONSE)

IMPLEMENT_ABSTRACT_CLASS(hoxLocalPlayer, hoxPlayer)

BEGIN_EVENT_TABLE(hoxLocalPlayer, hoxPlayer)
    EVT_COMMAND(wxID_ANY, hoxEVT_CONNECTION_RESPONSE, hoxLocalPlayer::OnConnectionResponse)
END_EVENT_TABLE()

//-----------------------------------------------------------------------------
// hoxLocalPlayer
//-----------------------------------------------------------------------------

hoxLocalPlayer::hoxLocalPlayer()
{
    wxFAIL_MSG( "This default constructor is never meant to be used." );
}

hoxLocalPlayer::hoxLocalPlayer( const wxString& name,
                                hoxPlayerType   type,
                                int             score )
            : hoxPlayer( name, type, score )
{ 
    const char* FNAME = "hoxLocalPlayer::hoxLocalPlayer";
    wxLogDebug("%s: ENTER.", FNAME);
}

hoxLocalPlayer::~hoxLocalPlayer() 
{
    const char* FNAME = "hoxLocalPlayer::~hoxLocalPlayer";
    wxLogDebug("%s: ENTER.", FNAME);
}

void 
hoxLocalPlayer::OnClose_FromTable( wxCommandEvent&  event )
{
    const char* FNAME = "hoxLocalPlayer::OnClose_FromTable";

    wxLogDebug("%s: ENTER.", FNAME);

    const wxString tableId  = event.GetString();
    this->LeaveNetworkTable( tableId, this );

    this->hoxPlayer::OnClose_FromTable( event );
}

hoxResult 
hoxLocalPlayer::ConnectToNetworkServer( wxEvtHandler* sender )
{
    this->StartConnection();

    hoxRequest* request = new hoxRequest( hoxREQUEST_LOGIN, sender );
	request->parameters["pid"] = this->GetName();
	request->parameters["password"] = this->GetPassword();
    this->AddRequestToConnection( request );

    return hoxRESULT_OK;
}

hoxResult 
hoxLocalPlayer::DisconnectFromNetworkServer( wxEvtHandler* sender )
{
    hoxRequest* request = new hoxRequest( hoxREQUEST_LOGOUT, sender );
	request->parameters["pid"] = this->GetName();
	this->AddRequestToConnection( request );

    return hoxRESULT_OK;
}

hoxResult 
hoxLocalPlayer::QueryForNetworkTables( wxEvtHandler* sender )
{
    hoxRequest* request = new hoxRequest( hoxREQUEST_LIST, sender );
	request->parameters["pid"] = this->GetName();
    this->AddRequestToConnection( request );

    return hoxRESULT_OK;
}

hoxResult 
hoxLocalPlayer::JoinNetworkTable( const wxString& tableId,
                                  wxEvtHandler*   sender )
{
	/* Check if this Player is already AT the Table. */
	bool hasRole = this->HasRoleAtTable( tableId );

    hoxRequest* request = new hoxRequest( hoxREQUEST_JOIN, sender );
	request->parameters["pid"] = this->GetName();
	request->parameters["tid"] = tableId;
    request->parameters["color"] = hoxUtility::ColorToString( hoxCOLOR_NONE ); // Observer.
	request->parameters["joined"] = hasRole ? "1" : "0";
    this->AddRequestToConnection( request );

    return hoxRESULT_OK;
}

hoxResult 
hoxLocalPlayer::OpenNewNetworkTable( wxEvtHandler*   sender )
{
    hoxRequest* request = new hoxRequest( hoxREQUEST_NEW, sender );
	request->parameters["pid"] = this->GetName();
	request->parameters["itimes"] = "1500/300/20"; // TODO: Hard-coded initial times.
    this->AddRequestToConnection( request );

    return hoxRESULT_OK;
}

hoxResult 
hoxLocalPlayer::LeaveNetworkTable( const wxString& tableId,
                                   wxEvtHandler*   sender )
{
    hoxRequest* request = new hoxRequest( hoxREQUEST_LEAVE, sender );
	request->parameters["pid"] = this->GetName();
	request->parameters["tid"] = tableId;
    this->AddRequestToConnection( request );

    return hoxRESULT_OK;
}

void 
hoxLocalPlayer::OnConnectionResponse( wxCommandEvent& event )
{
    const char* FNAME = "hoxLocalPlayer::OnConnectionResponse";
    hoxResult     result;
    int           returnCode = -1;
    wxString      returnMsg;

    wxLogDebug("%s: ENTER.", FNAME);

    hoxResponse* response_raw = wx_reinterpret_cast(hoxResponse*, event.GetEventObject());
    std::auto_ptr<hoxResponse> response( response_raw ); // take care memory leak!

    /* Make a note to 'self' that one request has been serviced. */
    DecrementOutstandingRequests();

    if ( response->sender && response->sender != this )
    {
		if ( response->type == hoxREQUEST_LOGIN )
		{
            result = this->HandleResponseEvent_Connect(event);
			if ( result != hoxRESULT_OK )
			{
				wxLogDebug("%s: *** WARN *** Failed to handle CONNECT's response [%s].", 
					FNAME, response->content.c_str());
				response->code = result;
			}
        }
		else if ( response->type == hoxREQUEST_LIST )
		{
			hoxNetworkTableInfoList* pTableList = new hoxNetworkTableInfoList;
			result = hoxNetworkAPI::ParseNetworkTables( response->content,
														*pTableList );
			if ( result != hoxRESULT_OK )
			{
				wxLogDebug("%s: *** WARN *** Failed to parse LIST's response [%s].", 
					FNAME, response->content.c_str());
				response->code = result;
			}
			response->eventObject = pTableList;
		}
		else if ( response->type == hoxREQUEST_JOIN )
		{
			std::auto_ptr<hoxNetworkTableInfo> pTableInfo( new hoxNetworkTableInfo() );
			result = hoxNetworkAPI::ParseJoinNetworkTable( response->content,
														   *pTableInfo );
			if ( result != hoxRESULT_OK )
			{
				wxLogDebug("%s: *** WARN *** Failed to parse JOIN's response [%s].", 
					FNAME, response->content.c_str());
				response->code = result;
			}
			hoxRemoteSite* remoteSite = static_cast<hoxRemoteSite*>( this->GetSite() );
			remoteSite->JoinExistingTable( *pTableInfo );
			return;
		}
		else if ( response->type == hoxREQUEST_NEW )
		{
			std::auto_ptr<hoxNetworkTableInfo> pTableInfo( new hoxNetworkTableInfo() );
			result = hoxNetworkAPI::ParseNewNetworkTable( response->content,
														  *pTableInfo );
			if ( result != hoxRESULT_OK )
			{
				wxLogDebug("%s: *** WARN *** Failed to parse NEW's response [%s].", 
					FNAME, response->content.c_str());
				response->code = result;
                return;
			}
			hoxRemoteSite* remoteSite = static_cast<hoxRemoteSite*>( this->GetSite() );
			remoteSite->JoinNewTable( *pTableInfo );
			return;
		}
		
        wxEvtHandler* sender = response->sender;
        response.release();
        wxPostEvent( sender, event );
        return;
    }

    if ( response->type == hoxREQUEST_OUT_DATA )
    {
		wxLogDebug("%s: [%s] 's response received. END.", 
			FNAME, hoxUtility::RequestTypeToString(response->type).c_str());
        return;
    }

    /* Parse the response */
    result = hoxNetworkAPI::ParseSimpleResponse( response->content,
                                                 returnCode,
                                                 returnMsg );
    if ( result != hoxRESULT_OK || returnCode != 0 )
    {
        wxLogDebug("%s: *** WARN *** Failed to parse the response. [%d] [%s]", 
            FNAME,  returnCode, returnMsg.c_str());
        return;
    }

    wxLogDebug("%s: The response is OK.", FNAME);
}

hoxResult 
hoxLocalPlayer::HandleResponseEvent_Connect( wxCommandEvent& event )
{
    const char* FNAME = "hoxLocalPlayer::HandleResponseEvent_Connect";
    hoxResult   result;
    int         returnCode = -1;
    wxString    returnMsg;
	wxString    sessionId;
	int         nScore = 0;

    wxLogDebug("%s: ENTER.", FNAME);

    hoxResponse* response = wx_reinterpret_cast(hoxResponse*, event.GetEventObject());

    result = hoxNetworkAPI::ParseConnectResponse( response->content,
                                                  returnCode,
                                                  returnMsg,
												  sessionId,
												  nScore );
    if ( result != hoxRESULT_OK || returnCode != 0 )
    {
        wxLogDebug("%s: *** WARN *** Connection ERROR. Error = [%d: %d].", 
            FNAME, result, returnCode);
        return hoxRESULT_ERR;
    }

	m_sessionId = sessionId; // Extract the session-Id.
	this->SetScore( nScore );
	wxLogDebug("%s: Connection OK. Session-Id = [%s].", FNAME, m_sessionId.c_str());

	/* Return the error-message to the default (parent) handler. */
	response->content = returnMsg;

    return hoxRESULT_OK;
}

/************************* END OF FILE ***************************************/
