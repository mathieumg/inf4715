// ============================================================================
// SimpleParser.cpp
//
// History:
//  02.16.01 - Created by Simon Feltman
//
// Copyright �2001, Discreet
// ----------------------------------------------------------------------------
#include <maxscript/maxscript.h>
#include <maxscript/foundation/streams.h>
#include <maxscript/foundation/strings.h>
#include <maxscript/foundation/numbers.h>
#include "iMemStream.h"

#include "assetmanagement\AssetUser.h"
#include "AssetManagement\iassetmanager.h"
using namespace MaxSDK::AssetManagement;

// ============================================================================
#include <maxscript\macros\define_instantiation_functions.h>
	def_visible_primitive_debug_ok	( isSpace,		"isSpace");
	def_visible_primitive_debug_ok	( trimLeft,		"trimLeft");
	def_visible_primitive_debug_ok	( trimRight,	"trimRight");
	def_visible_primitive_debug_ok	( readToken,	"readToken");
	def_visible_primitive_debug_ok	( peekToken,	"peekToken");
	def_visible_primitive_debug_ok	( skipSpace,	"skipSpace");
	def_visible_primitive_debug_ok	( getFileSize,	"getFileSize");

void SkipSpace( CharStream *stream )
{
	MaxSDK::Util::Char c;
	while(!stream->at_eos())
	{
		c = stream->get_char();
		if(IsSpace(c))
			continue;
		else
		{
			if(c != _T('\0'))
				stream->unget_char(c);
			break;
		}
	}
}

// ============================================================================
// <Boolean> isSpace <String>
Value* isSpace_cf(Value** arg_list, int count)
{
	check_arg_count(isSpace, 1, count);
	const TCHAR *str = arg_list[0]->to_string();
	return (IsSpace(str[0]) ? &true_value : &false_value);
}

// ============================================================================
// <String> trimLeft <String> [String trimChars]
Value* trimLeft_cf(Value** arg_list, int count)
{
	if(count < 1 || count > 2)
		check_arg_count(trimLeft, 1, count);

	const TCHAR *trim;
	MaxSDK::Util::CharIterator<const MCHAR> in(arg_list[0]->to_string());
	if(count == 2)
		trim = arg_list[1]->to_string();
	else
		trim = whitespace;

	while(!in.AtEnd() && (*in).IsInCharacterSet(trim) != NULL) ++in;
	return new String(in);
}

// ============================================================================
// <String> trimRight <String> [String trimChars]
Value* trimRight_cf(Value** arg_list, int count)
{
	if(count < 1 || count > 2)
		check_arg_count(trimLeft, 1, count);

	const TCHAR *trim;
	const TCHAR *in = arg_list[0]->to_string();
	if(count == 2)
		trim = arg_list[1]->to_string();
	else
		trim = whitespace;

	TSTR                              str(in);
	int                               index;
	MaxSDK::Util::CharIterator<const MCHAR> it;

	for (index = str.LanguageCharacterCount() - 1; index >= 0; index--)
	{
		it  = str.data();
		it += index;

		if (!(*it).IsInCharacterSet(trim))
		{
			break;
		}
	}

	return new String(str.Substr(0, index + 1));
}

// ============================================================================
// <String> readToken <CharStream inStream>
Value* readToken_cf(Value** arg_list, int count)
{
	check_arg_count(readToken, 1, count);
	if(arg_list[0]->is_kind_of(class_tag(CharStream)))
	{
		CharStream *stream = (CharStream*)arg_list[0];

		MaxSDK::Util::CharAccumulator token;

retry:;
		// Skip leading space
		SkipSpace(stream);
		if(stream->at_eos()) return &undefined;

		// Skip '//' style comments
		MaxSDK::Util::Char c = stream->get_char();
		if(c == _T('/') && !stream->at_eos() && stream->peek_char() == _T('/'))
		{
			while(!stream->at_eos() && !IsNewline(c=stream->get_char())) ;
			goto retry;
		}

		if(c == _T('"'))
		{
			while(!stream->at_eos() && (c=stream->get_char()) != _T('"'))
				token += c;
		}
		else
		{
			do {
				token += c;
			} while(!stream->at_eos() && !IsSpace(c=stream->get_char()));
		}

		return new String(token);
	}
	return &undefined;
}

// ============================================================================
// <String> peekToken <CharStream inStream>
Value* peekToken_cf(Value** arg_list, int count)
{
	check_arg_count(peekToken, 1, count);
	if(arg_list[0]->is_kind_of(class_tag(CharStream)))
	{
		CharStream *stream = (CharStream*)arg_list[0];
		int pos = stream->pos();
		Value *res = readToken_cf(arg_list, count);
		stream->seek(pos);
		return res;
	}
	return &undefined;
}

// ============================================================================
// skipSpace <CharStream inStream>
Value* skipSpace_cf(Value** arg_list, int count)
{
	check_arg_count(skipSpace, 1, count);
	if(arg_list[0]->is_kind_of(class_tag(CharStream)))
	{
		CharStream *stream = (CharStream*)arg_list[0];
		SkipSpace(stream);
	}

	return &ok;
}

// ============================================================================
// Integer64 getFileSize <String fileName>
Value* getFileSize_cf(Value** arg_list, int count)
{
	check_arg_count(getFileSize, 1, count);
	INT64 filesize = 0;

	TSTR  fileName = arg_list[0]->to_filename();
	AssetId assetId;
	if (IAssetManager::GetInstance()->StringToAssetId(fileName, assetId))
		fileName = IAssetManager::GetInstance()->GetAsset(assetId).GetFileName();

	WIN32_FILE_ATTRIBUTE_DATA   fileInfo;
	if (GetFileAttributesEx(fileName, GetFileExInfoStandard, (void*)&fileInfo))
		filesize = (((INT64)fileInfo.nFileSizeHigh) << 32) + fileInfo.nFileSizeLow;
	return Integer64::intern(filesize);
}

