# Graph utilities

This repositiory contains some utilities for graph processing.

## Compilation
Run the `make` command.

## Execution
Run the `gutil` binary in the `bin` folder.

In particular, to convert between two different graph formats, run, for
example:

```
gutil convert input.nde output.oly
```

This command will convert a graph from nde-format to IOI-format. File
format is automatically detected by the extension of the file.
