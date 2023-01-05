
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

* Install https://github.com/microsoft/vcpkg wherever like. For simplicity this project assumes C:\vcpkg
* vcpkg install cpp-httplib:x64-windows
* vcpkg install cppcodec:x64-windows
* vcpkg install curlpp:x64-windows
* vcpkg install nlohmann-json:x64-windows
* Open prj/vs2022/simple_onshape_exportstls.sln in Visual Studio 2022
* Set the active build to Release.
* If needed update "C/C++ -> General -> Additional Include Directories" to match your vcpkg installation.
* If needed update "Linker -> Input -> Additional Library Directories" to match your vcpkg installation.
* If needed update the curlpp.dll, libcurl.dll, and zlib1.dll entries so Visual Studio can find and copy them.
* If you are using onshape API keys for authorization then add ONSHAPE_ACCESS_KEY and ONSHAPE_SECRET_KEY to your environment variables.
* If you are using onshape OAuth for authorization then add ONSHAPE_CLIENT_ID to your environment variables.
* Restart Visual Studio.
* Open main.cpp and in main() choose one of the two authorization options.
* Hit F5 and it should export stls from a hardcoded onshape partstudio document url.
