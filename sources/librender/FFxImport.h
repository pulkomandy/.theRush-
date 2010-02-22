///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : FFxImport.h
// Creation : 28/06/2007
// Author : Cedric Guillemet
// Description :
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
class FFxImport
{
public:
	FFxImport();
	virtual ~FFxImport();
	bool importCgfx(ZFx* fx, CGcontext aContext, const char* aFileName, const char** args);
	bool importEffect( ZFx* fx, CGeffect effect, const char* aFileName);

protected:
	void importCgStateAssignment(ZFx* fx, FFxPass *pPass, CGstateassignment sa, CGbool unabridged);
	FFxProgram* importCgProgram(const char* aFileName, CGprogram program);
};


