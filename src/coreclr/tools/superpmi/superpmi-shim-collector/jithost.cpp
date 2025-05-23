// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

#include "standardpch.h"
#include "runtimedetails.h"
#include "spmiutil.h"
#include "jithost.h"

// There is a single JitHost object created during the one-time initialization of the JIT (function jitStartup),
// and shared amongst all subsequent compilations. Any calls to the getIntConfigValue/getStringConfigValue
// APIs get recorded in a single, global MethodContext/CompileResult, and are copied to the
// per-compilation MethodContext in the shim implementation of compileMethod (using recGlobalContext()).
// This works because the JIT eagerly asks for all config values once, in the one-time jitStartup
// function. If the JIT were to ask for config values later, during the per-compilation phase,
// they would get recorded here in the global MethodContext, and copied to all subsequent
// compilation MethodContexts. This would be incorrect. A solution would be to use a per-compilation
// MethodContext in addition to the global MethodContext, but we have to allow for multi-threading. That is,
// there could be multiple JIT compilations happening concurrently, so we can't just replace the global
// MethodContext with a per-compilation MethodContext. Perhaps per-compilation MethodContext could be
// stored in a map from OS thread id to MethodContext, and looked up here based on thread id. The host APIs
// have no per-compilation knowledge.

JitHost* g_ourJitHost;

// RecordVariable: return `true` if the given DOTNET variable `key` should be recorded
// in the method context.
bool RecordVariable(const char* key)
{
    // Special cases: we don't want to store some DOTNET variables during
    // collections, typically when they refer to file paths or simply because
    // it does not make sense to replay with it.

    static const char* s_ignoredVars[] = {
        "EnableExtraSuperPmiQueries",
        "JitDisasm",
        "JitDump",
        "JitDisasmWithAlignmentBoundaries",
        "JitDumpASCII",
        "JitHashBreak",
        "JitHashDump",
        "JitHashHalt",
        "JitOrder",
        "JitPrintInlinedMethods",
        "JitPrintDevirtualizedMethods",
        "JitBreak",
        "JitDebugBreak",
        "JitDisasmAssemblies",
        "JitDisasmWithGC",
        "JitDisasmWithDebugInfo",
        "JitDisasmSpilled",
        "JitDumpTier0",
        "JitDumpAtOSROffset",
        "JitDumpInlinePhases",
        "JitEHDump",
        "JitExclude",
        "JitGCDump",
        "JitDebugDump",
        "JitHalt",
        "JitImportBreak",
        "JitInclude",
        "JitLateDisasm",
        "JitUnwindDump",
        "JitDumpFg",
        "JitDumpFgDir",
        "JitDumpFgPhase",
        "JitDumpFgPrePhase",
        "JitDumpFgDot",
        "JitDumpFgEH",
        "JitDumpFgLoops",
        "JitDumpFgConstrained",
        "JitDumpFgBlockID",
        "JitDumpFgBlockFlags",
        "JitDumpFgLoopFlags",
        "JitDumpFgBlockOrder",
        "JITLateDisasmTo",
        "JitDisasmSummary",
        "JitStdOutFile",
        "WriteRichDebugInfoFile",
        "JitFuncInfoLogFile",
        "JitTimeLogCsv",
        "JitMeasureNowayAssertFile",
        "JitInlineDumpData",
        "JitInlineDumpXml",
        "JitInlineDumpXmlFile",
        "JitInlinePolicyDumpXml",
        "JitInlineReplayFile",
        "JitFunctionFile"
        "JitRawHexCode",
        "JitRawHexCodeFile"
    };

    for (const char* ignoredVar : s_ignoredVars)
    {
        if (_stricmp(key, ignoredVar) == 0)
        {
            return false;
        }
    }

    return true;
}

JitHost::JitHost(ICorJitHost* wrappedHost, MethodContext* methodContext) : wrappedHost(wrappedHost), mc(methodContext)
{
}

void* JitHost::allocateMemory(size_t size)
{
    return wrappedHost->allocateMemory(size);
}

void JitHost::freeMemory(void* block)
{
    return wrappedHost->freeMemory(block);
}

int JitHost::getIntConfigValue(const char* key, int defaultValue)
{
    // Special-case handling: don't collect this pseudo-variable, and don't
    // even record that it was called (since it would get recorded into the
    // global state). (See the superpmi.exe tool implementation of JitHost::getIntConfigValue()
    // for the special-case implementation of this.)
    if (strcmp(key, "SuperPMIMethodContextNumber") == 0)
    {
        return defaultValue;
    }

    mc->cr->AddCall("getIntConfigValue");
    int result = wrappedHost->getIntConfigValue(key, defaultValue);

    // The JIT eagerly asks about every config value. If we store all these
    // queries, it takes almost half the MC file space. So only store the
    // non-default answers.
    if (RecordVariable(key) && (result != defaultValue))
    {
        mc->recGetIntConfigValue(key, defaultValue, result);
    }
    return result;
}

const char* JitHost::getStringConfigValue(const char* key)
{
    mc->cr->AddCall("getStringConfigValue");
    const char* result = wrappedHost->getStringConfigValue(key);

    // Don't store null returns, which is the default
    if (RecordVariable(key) && (result != nullptr))
    {
        mc->recGetStringConfigValue(key, result);
    }
    return result;
}

void JitHost::freeStringConfigValue(const char* value)
{
    mc->cr->AddCall("freeStringConfigValue");
    wrappedHost->freeStringConfigValue(value);
}
