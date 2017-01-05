#
# kservice_desktop_to_json(target desktopfile)
#
# This macro uses desktoptojson to generate a json file from a plugin
# description in a .desktop file. The generated file can be compiled
# into the plugin using the K_PLUGIN_FACTORY_WITH_JSON (cpp) macro.
#
# Example:
#
#  kservice_desktop_to_json(plasma_engine_time plasma-dataengine-time.desktop)

function(kservice_desktop_to_json)
  message(AUTHOR_WARNING "kservice_desktop_to_json() is deprecated, use kcoreaddons_desktop_to_json instead")
  if(${ARGC} EQUAL 1)
    message(FATAL_ERROR "Calling kservice_desktop_to_json with only one argument is no longer possible. The code should be changed to pass the target which depends on the json file as first argument.")
  endif()
  set(target ${ARGV0})
  set(desktop ${ARGV1})
  kcoreaddons_desktop_to_json(${target} ${desktop} COMPAT_MODE)
endfunction()
