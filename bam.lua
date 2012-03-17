local settings = NewSettings() -- {}
	
--[[settings.debug = 0
settings.optimize = 0
settings._is_settingsobject = true
settings.invoke_count = 0

-- SetCommonSettings(settings)
settings.config_name = ""
settings.config_ext  = ""
settings.labelprefix = ""

-- add all tools
for _, tool in pairs(_bam_tools) do
	tool(settings)
end

TableLock(settings)]]

settings.cc.includes:Add("include")
settings.cc.flags:Add( "-Wall", "-Werror", "-Wstrict-aliasing=2" )

--local output_func    = function(settings, path) return PathJoin( 'local', PathFilename(PathBase(path)) .. settings.config_ext ) end
--settings.cc.Output   = output_func
--settings.lib.Output  = output_func
--settings.dll.Output  = output_func
--settings.link.Output = output_func

local objs  = Compile( settings, 'src/getopt.c' )
local lib   = StaticLibrary( settings, 'getopt', objs )

settings.link.libs:Add( 'gtest', 'pthread' )
local test_objs  = Compile( settings, 'test/getopt_tests.cpp' )
local tests      = Link( settings, 'getopt_tests', test_objs, lib )