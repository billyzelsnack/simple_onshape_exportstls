
# Simple C++ onshape STL export

A minimal implementation of an onshape STL exporter written in c++. It supports two styles of onshape authorization.

# Requirements
* onshape account
* Windows 10 or 11
* Visual Studio 2022

# Onshape API keys authorization (Option A)
* Go to https://dev-portal.onshape.com/keys
* Click the "Create new API key"
* Copy the "Access key" and add it to your environment variables as "ONSHAPE_ACCESS_KEY"
* Copy the "Secret key" and add it to your environment variables as "ONSHAPE_SECRET_KEY"

# Onshape OAuth authorization (Option B)
* Go to https://dev-portal.onshape.com/oauthApps
* Click the "Create new OAuth application"
* Fill out "Name" with your application name
* Fill out "Primary Format" with a global unique identifier name for your application
* Fill out "Redirect URLs" with http:/localhost:8293. If you change the port in the software you will need to update this as well
* Leave "Admin Team" and "OAuth URL" and "Settings" blank
* In "Permissions" check "Application can read your documents"
* Copy the "OAuth client identifier" and add it to your environment variables as "ONSHAPE_CLIENT_ID"

# Installation
* Install https://premake.github.io/
* Install https://github.com/microsoft/vcpkg wherever you like. For simplicity this project assumes C:\vcpkg
* vcpkg install cpp-httplib:x64-windows
* vcpkg install cppcodec:x64-windows
* vcpkg install curlpp:x64-windows
* vcpkg install nlohmann-json:x64-windows
* Changed the vckgdir variable at the top of premake5.lua to match your vcpkg directory.
* premake5 vs2022
* Open prj/vs2022/simple_onshape_exportstls.sln in Visual Studio 2022
* Set the active build to Release.
* Open main.cpp and in main() choose one of the two authorization options.
* F5 to build and run. The exported stls will be saved to out/vs2022/x64/Release

