#pragma once

#include "IConVar.h"
#include "IAppSystem.h"

class ConBase;
class ConVar;
class ConCmd;

class ICvar : public IAppSystem
{
public:
	virtual int				AllocateDLLIdentifier() = 0;
	virtual void			RegisterConCommand(ConVar* pCommandBase, int iDefaultValue = 1) = 0;
	virtual void			UnregisterConCommand(ConVar* pCommandBase) = 0;
	virtual void			UnregisterConCommands(int id) = 0;
	virtual const char*		GetCommandLineValue(const char* szVariableName) = 0;
	virtual ConBase*		FindCommandBase(const char* szName) = 0;
	virtual const ConBase*	FindCommandBase(const char* szName) const = 0;
	virtual ConVar*			FindVar(const char* szVariableName) = 0;
	virtual const ConVar*	FindVar(const char* szVariableName) const = 0;
	virtual ConCmd*			FindCommand(const char* szName) = 0;
	virtual const ConCmd*	FindCommand(const char* szName) const = 0;
	virtual void			InstallGlobalChangeCallback(FnChangeCallback_t callback) = 0;
	virtual void			RemoveGlobalChangeCallback(FnChangeCallback_t callback) = 0;
	virtual void			CallGlobalChangeCallbacks(ConVar* pVar, const char* szOldString, float flOldValue) = 0;
	virtual void			InstallConsoleDisplayFunc(void* pDisplayFunc) = 0;
	virtual void			RemoveConsoleDisplayFunc(void* pDisplayFunc) = 0;
	virtual void			ConsoleColorPrintf(const Color& color, const char* pFormat, ...) const = 0;
	virtual void			ConsolePrintf(const char* pFormat, ...) const = 0;
	virtual void			ConsoleDPrintf(const char* pFormat, ...) const = 0;
	virtual void			RevertFlaggedConVars(int nFlag) = 0;
};