setx /m PATH ^"^
%1\%2\quartus\bin64;^
%1\%2\quartus\bin64\cygwin\bin;^
%1\%2\nios2eds\bin;^
%1\%2\nios2eds\bin\gnu\H-x86_64-mingw32\bin;^
%1\%2\nios2eds\sdk2\bin;^
%PATH%"

set "nios_shell_path=%1/%2/18.1/nios2eds"
set "nios_shell_path=%nios_shell_path:\=/%"
setx /m NIOS_SHELL "%nios_shell_path%"
set "altera_path=%1/%2/nios2eds/sdk2/bin"
set "altera_path=%altera_path:\=/%"
setx /m ALTERA_BIN "%altera_path%"
setx /m CYGWIN "nodosfilewarning"
