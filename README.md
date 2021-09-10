# Wille

A multi-threading server-client framework

## Logging

- `Logger`
  - A list of `LogAppender`: 
  - `LogFormatter`: A default formatter for passing to LogAppender
    - `FormatItem`: Single item of log. Every item of component must inherit it and implement a format method.
    - log: go through all appenders and use each appender's formatter to log.
  - `LogLevel`: Every `Logger` has a default level (DEBUG)
- `LogAppender`: destination of logs.
  - `LogFormatter`: Every `LogAppender` can have its own formatter.
  - `LogLevel`: Every `LogAppender` has a default level (DEBUG)
- `LogLevel`: log levels. DEBUG, INFO, WARN, ERROR, FATAL
- `LogEvent`: log content.
  - `LogLevel`: Every `LogEvent` has a level
