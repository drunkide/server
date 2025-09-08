@%~dp0tools\pour_wrapper_windows --chdir "%~dp0." --dont-print-commands --script "%0" %* && exit /B 0 || exit /B 1
pour.require("mingw32-8.1.0")
pour.run('go', table.unpack(arg))
