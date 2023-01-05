local vcpkgdir="c:/vcpkg/"

workspace "simple_onshape_exportstls"
   configurations { "Debug", "Release" }
   location "prj/vs2022"
   architecture "x64"

includedirs { vcpkgdir.."installed/x64-windows/include" }
libdirs { vcpkgdir.."installed/x64-windows/lib" }

 project "simple_onshape_exportstls"
   kind "ConsoleApp"
   language "C++"
   defines {_CRT_SECURE_NO_WARNINGS}
   files { "src/*.cpp" }
   location "prj/vs2022"
   objdir "int/vs2022/x64/%{cfg.buildcfg}"
   targetdir "out/vs2022/x64/%{cfg.buildcfg}"
   links { "curlpp", "libcurl", "zlib" }
   postbuildcommands {
      "{COPY} "..vcpkgdir.."installed/x64-windows/bin/curlpp.dll %{cfg.targetdir}",
      "{COPY} "..vcpkgdir.."installed/x64-windows/bin/libcurl.dll %{cfg.targetdir}",
      "{COPY} "..vcpkgdir.."installed/x64-windows/bin/zlib1.dll %{cfg.targetdir}"
    }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"