//	$Id$
/*!	@file
	@brief タグファイル作成ダイアログボックス

	@author MIK
	$Revision$
*/
/*
	Copyright (C) 2003, MIK

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose, 
	including commercial applications, and to alter it and redistribute it 
	freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented;
		   you must not claim that you wrote the original software.
		   If you use this software in a product, an acknowledgment
		   in the product documentation would be appreciated but is
		   not required.

		2. Altered source versions must be plainly marked as such, 
		   and must not be misrepresented as being the original software.

		3. This notice may not be removed or altered from any source
		   distribution.
*/

#include <string.h>
#include "sakura_rc.h"
#include "CDlgTagsMake.h"
#include "etc_uty.h"
#include "debug.h"
#include "CEditDoc.h"
#include "global.h"
#include "funccode.h"
#include "mymessage.h"

#include "sakura.hh"
const DWORD p_helpids[] = {	//13700
	IDC_EDIT_TAG_MAKE_FOLDER,	HIDC_EDIT_TAG_MAKE_FOLDER,	//タグ作成フォルダ
	IDC_BUTTON_TAG_MAKE_REF,	HIDC_BUTTON_TAG_MAKE_REF,	//参照
	IDC_EDIT_TAG_MAKE_CMDLINE,	HIDC_EDIT_TAG_MAKE_CMDLINE,	//コマンドライン
	IDC_CHECK_TAG_MAKE_RECURSE,	HIDC_CHECK_TAG_MAKE_RECURSE,	//サブフォルダも対象
	IDOK,						HIDC_TAG_MAKE_IDOK,
	IDCANCEL,					HIDC_TAG_MAKE_IDCANCEL,
	IDC_BUTTON_HELP,			HIDC_BUTTON_TAG_MAKE_HELP,
//	IDC_STATIC,						-1,
	0, 0
};

CDlgTagsMake::CDlgTagsMake()
{
	strcpy( m_szPath, "" );
	strcpy( m_szTagsCmdLine, "" );
	m_nTagsOpt = 0;
	return;
}

/* モーダルダイアログの表示 */
int CDlgTagsMake::DoModal(
	HINSTANCE	hInstance,
	HWND		hwndParent,
	LPARAM		lParam,
	const char*	pszPath		//パス
)
{
	strcpy( m_szPath, pszPath );

	return CDialog::DoModal( hInstance, hwndParent, IDD_TAG_MAKE, lParam );
}

BOOL CDlgTagsMake::OnBnClicked( int wID )
{
	switch( wID )
	{
	case IDC_BUTTON_HELP:
		/* ヘルプ */
		::WinHelp( m_hWnd, m_szHelpFile, HELP_CONTEXT, ::FuncID_To_HelpContextID(F_TAGS_MAKE) );
		return TRUE;

	case IDC_BUTTON_TAG_MAKE_REF:	/* 参照 */
		SelectFolder( m_hWnd );
		return TRUE;

	case IDOK:
		/* ダイアログデータの取得 */
		::EndDialog( m_hWnd, GetData() );
		return TRUE;

	case IDCANCEL:
		::EndDialog( m_hWnd, FALSE );
		return TRUE;

	}

	/* 基底クラスメンバ */
	return CDialog::OnBnClicked( wID );
}

/*!
	フォルダを選択する
	
	@param hwndDlg [in] ダイアログボックスのウィンドウハンドル
*/
void CDlgTagsMake::SelectFolder( HWND hwndDlg )
{
	char	szPath[_MAX_PATH + 1];

	/* フォルダ */
	::GetDlgItemText( hwndDlg, IDC_EDIT_TAG_MAKE_FOLDER, szPath, _MAX_PATH );

	if( SelectDir( hwndDlg, "タグ作成フォルダの選択", szPath, szPath ) )
	{
		//末尾に\\マークを追加する．
		int pos = strlen( szPath );
		if( pos > 0 && szPath[ pos - 1 ] != '\\' )
		{
			szPath[ pos     ] = '\\';
			szPath[ pos + 1 ] = '\0';
		}

		::SetDlgItemText( hwndDlg, IDC_EDIT_TAG_MAKE_FOLDER, szPath );
	}
}

/* ダイアログデータの設定 */
void CDlgTagsMake::SetData( void )
{
	//作成フォルダ
	::SendMessage( ::GetDlgItem( m_hWnd, IDC_EDIT_TAG_MAKE_FOLDER ), CB_LIMITTEXT, (WPARAM)sizeof( m_szPath ), 0 );
	::SetDlgItemText( m_hWnd, IDC_EDIT_TAG_MAKE_FOLDER, m_szPath );

	//オプション
	m_nTagsOpt = m_pShareData->m_nTagsOpt;
	if( m_nTagsOpt & 0x0001 ) ::CheckDlgButton( m_hWnd, IDC_CHECK_TAG_MAKE_RECURSE, TRUE );

	//コマンドライン
	::SendMessage( ::GetDlgItem( m_hWnd, IDC_EDIT_TAG_MAKE_CMDLINE ), CB_LIMITTEXT, (WPARAM)sizeof( m_pShareData->m_szTagsCmdLine ), 0 );
	strcpy( m_szTagsCmdLine, m_pShareData->m_szTagsCmdLine );
	::SetDlgItemText( m_hWnd, IDC_EDIT_TAG_MAKE_CMDLINE, m_pShareData->m_szTagsCmdLine );

	return;
}

/* ダイアログデータの取得 */
/* TRUE==正常  FALSE==入力エラー */
int CDlgTagsMake::GetData( void )
{
	//フォルダ
	::GetDlgItemText( m_hWnd, IDC_EDIT_TAG_MAKE_FOLDER, m_szPath, sizeof( m_szPath ) );
	int length = strlen( m_szPath );
	if( length > 0 )
	{
		if( m_szPath[ length - 1 ] != '\\' ) strcat( m_szPath, "\\" );
	}

	//CTAGSオプション
	m_nTagsOpt = 0;
	if( ::IsDlgButtonChecked( m_hWnd, IDC_CHECK_TAG_MAKE_RECURSE ) == BST_CHECKED ) m_nTagsOpt |= 0x0001;
	m_pShareData->m_nTagsOpt = m_nTagsOpt;

	//コマンドライン
	::GetDlgItemText( m_hWnd, IDC_EDIT_TAG_MAKE_CMDLINE, m_szTagsCmdLine, sizeof( m_szTagsCmdLine ) );
	strcpy( m_pShareData->m_szTagsCmdLine, m_szTagsCmdLine );

	return TRUE;
}

LPVOID CDlgTagsMake::GetHelpIdTable( void )
{
	return (LPVOID)p_helpids;
}

/*[EOF]*/
