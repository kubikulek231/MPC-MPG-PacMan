

function(extract_zip zip_path target_dir)
  if (WIN32)
    message(STATUS "Extracting ${zip_path} to ${target_dir}")
    execute_process(
      COMMAND powershell -Command "Expand-Archive -Path '${zip_path}' -DestinationPath '${target_dir}'"
      RESULT_VARIABLE result
    )
    if (NOT "${result}" STREQUAL "0")
      message(FATAL_ERROR "Failed to extract ${zip_path}")
    endif()
  else()
    message(FATAL_ERROR "ZIP extraction is only implemented for Windows with PowerShell")
  endif()
endfunction()