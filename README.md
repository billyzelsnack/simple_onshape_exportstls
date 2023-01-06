
# Simple C++ onshape STL export

A minimal implementation of an onshape STL exporter written in c++. It supports two options for onshape authorization.

# Requirements
* onshape account
* Windows 10 or 11
* Visual Studio 2022 (others might work)

# Onshape API keys authorization (Option A)
* Go to https://dev-portal.onshape.com/keys
* Click the "Create new API key"
* Copy the "Access key" and add it to your environment variables as "ONSHAPE_ACCESS_KEY"
* Copy the "Secret key" and add it to your environment variables as "ONSHAPE_SECRET_KEY"
* Restart Visual Studio if it is already running

# Onshape OAuth authorization (Option B)
* Go to https://dev-portal.onshape.com/oauthApps
* Click the "Create new OAuth application"
* Fill out "Name" with your application name
* Fill out "Primary Format" with a global unique identifier name for your application
* Fill out "Redirect URLs" with http:/localhost:8293. If you change the port in the software you will need to update this as well
* Leave "Admin Team" and "OAuth URL" and "Settings" blank
* In "Permissions" check "Application can read your documents"
* Copy the "OAuth client identifier" and add it to your environment variables as "ONSHAPE_CLIENT_ID"
* Restart Visual Studio if it is already running

# Installation
* Install https://premake.github.io/
* Install https://github.com/microsoft/vcpkg
* vcpkg install cpp-httplib:x64-windows
* vcpkg install cppcodec:x64-windows
* vcpkg install curlpp:x64-windows
* vcpkg install nlohmann-json:x64-windows
* Change the vckgdir variable at the top of premake5.lua to match your vcpkg directory.
* In the directory where premake5.lua is located run: premake5 vs2022
* Open vs2022/simple_onshape_exportstls.sln in Visual Studio 2022
* Open main.cpp and in the main() choose one of the two authorization options.
* F5 to build and run. The exported stls will be saved to either vs2022/Debug/out_x64 or vs2022/Release/out_x64

