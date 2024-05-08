void Log(const char *format, ...)
{  
  char message[1024];
  va_list args;
  va_start(args, format);
  if (args != 0) vsnprintf(message, sizeof(message), format, args);  
  va_end(args);
  fprintf_s(stdout, message, args);
}

void Error(const char *format, ...)
{  
  LPSTR windowsErrorBuffer = NULL;
  size_t windowsErrorSize = 0;
  DWORD errorMessageID = GetLastError();
  if (errorMessageID != 0)
  {
    windowsErrorSize = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      errorMessageID,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPSTR)&windowsErrorBuffer,
      0,
      NULL
    );
    
    if (windowsErrorSize == 0)
    {
      errorMessageID = GetLastError();
      fprintf_s(stderr, "Can't retrieve Windows error message (0x%x)\n", errorMessageID);
      return;
    }
  }
  
  char message[3*KB];
  va_list args;
  va_start(args, format);
  if (args != 0) vsnprintf(message, sizeof(message), format, args);  
  va_end(args);
  
  if (windowsErrorBuffer != NULL)
  {
    strncat_s(message, sizeof(message), ": ", 2);
    strncat_s(message, sizeof(message), windowsErrorBuffer, _TRUNCATE);
    strncat_s(message, sizeof(message), "\n", 1);
    LocalFree(windowsErrorBuffer);
  }
  
  fprintf_s(stderr, "%s", message);
}

#define CheckIf(condition, message, ...) \
  if (!(condition)) { Error(message, __VA_ARGS__); return 1; }
