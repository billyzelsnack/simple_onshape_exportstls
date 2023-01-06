local vcpkgdir="c:/vcpkg/"

workspace "simple_onshape_exportstls"   

   location "%{_ACTION}"
   configurations { "Debug", "Release" }   

   filter "configurations:Release"
      defines { "DEBUG" }
      optimize "Off"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   project "simple_onshape_exportstls"

      kind "ConsoleApp"
      architecture "x64"   
      libdirs { vcpkgdir.."installed/x64-windows/lib" }

      language "C++"
      cpp_dialect "C++20"
      defines {_CRT_SECURE_NO_WARNINGS}
      includedirs { vcpkgdir.."installed/x64-windows/include" }
      
      files { "src/*.cpp" }
      objdir "%{_ACTION}/%{cfg.buildcfg}/int_x64/"
      targetdir "%{_ACTION}/%{cfg.buildcfg}/out_x64/"
      debugdir  "%{_ACTION}/%{cfg.buildcfg}/out_x64/"
      links { "curlpp", "libcurl", "zlib" }

      postbuildcommands 
      {
         "{COPY} "..vcpkgdir.."installed/x64-windows/bin/curlpp.dll %{cfg.targetdir}",
         "{COPY} "..vcpkgdir.."installed/x64-windows/bin/libcurl.dll %{cfg.targetdir}",
         "{COPY} "..vcpkgdir.."installed/x64-windows/bin/zlib1.dll %{cfg.targetdir}"
      }

