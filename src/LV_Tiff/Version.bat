@echo off
rem Auto versioning
rem this file starts verbuild tool
rem Minor and major versions should be changed here
rem Each assembly has a version number as part of its identity. As such, two assemblies that differ by version number are considered by the runtime to be completely different assemblies.
rem This version number is physically represented as a four-part string with the following format:
rem				<major version>.<minor version>.<build number>.<revision>
rem For example, version 1.5.1254.0 indicates 1 as the major version, 5 as the minor version, 1254 as the build number, and 0 as the revision number. 
rem The version number is stored in the assembly manifest along with other identity information, including the assembly name and public key, as well as information on relationships and identities of other assemblies connected with the application. 
rem When an assembly is built, the development tool records dependency information for each assembly that is referenced in the assembly manifest. 
rem The runtime uses these version numbers, in conjunction with configuration information set by an administrator, an application, or a publisher, to load the proper version of a referenced assembly. 
rem The runtime distinguishes between regular and strong-named assemblies for the purposes of versioning. Version checking only occurs with strong-named assemblies. 
rem For information about specifying version binding policies, see Configuration Files. For information about how the runtime uses version information to find a particular assembly, see How the Runtime Locates Assemblies.

rem USAGE Version.bat <tool_path> <versionfile>
rem path must end with \
set MAJOR_VER=0
set MINOR_VER=10

%1verbuild %2 %MAJOR_VER%.%MINOR_VER%.*.+ -d2013 -xFp -s